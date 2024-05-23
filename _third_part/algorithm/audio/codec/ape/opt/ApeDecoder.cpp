#include"ApePrive.h"
#include<string.h>


/***************************************************************
***************************** inner ****************************
****************************************************************/
static inline uint32_t ApeGetApeDecoderSize()
{
	return APE_ALIGN_8(sizeof(ApeDecoder));
}

static inline uint32_t ApeGetNNFilterSize(uint16_t compressLevel)
{
	int32_t sizeNNFilter = 0;
	int32_t fSet = compressLevel / 1000 - 1;
	for (uint16_t ch = 0; ch < APE_MAX_CHANNELS; ch++)
	{
		for (uint16_t i = 0; i < APE_FILTER_LEVELS; i++)
		{
			if (!ape_filter_orders[fSet][i])
				break;
			sizeNNFilter += APE_ALIGN_8((ape_filter_orders[fSet][i]+APE_NN_FILTER_ROLL_BUFFER_EXTRA_LENGTH) * sizeof(int16_t)) * 3 * APE_MAX_CHANNELS;
		}
	}

	return sizeNNFilter;
}


static APE_RET_t mono_decoder_process(ApeDecoder* dec, uint16_t blocksMax, uint16_t* blocksDecoded, void* bufferRead) {

	//0.check
	//
	//1.init	
	int32_t* ch0, * ch1;
	int32_t Y;

	ch0 = dec->ch0;
	ch1 = dec->ch1;
	if (dec->frameFlags & APE_FRAMECODE_STEREO_SILENCE)
	{
		*blocksDecoded = dec->blocksMax;
		memset(ch0, 0, sizeof(*ch0) * (*blocksDecoded));
		memset(ch1, 0, sizeof(*ch1) * (*blocksDecoded));
	}
	else
	{
		*blocksDecoded = 0;
		while (*blocksDecoded < blocksMax)
		{
			if (GetBufferRemByte(dec->bufferRead) < dec->exitByte)
				return APE_RET_NOT_ENOUGH_INPUT;

			//2.entropy decode
			if (!ApeEntropyDecode(&dec->riceY, &dec->rc, dec->bufferRead, &Y, dec->context->descriptor.fileversion))
				return APE_RET_ENTROPY_DECODE_ERROR;

			//3.NN filter
			ApeNNFilter(dec->filters[0], &Y, dec->context->descriptor.fileversion, dec->fSet);

			//4.apply filter
			ApePredictionFilterMono(&dec->prediction, &Y);

			*ch0++ = Y;
			(*blocksDecoded)++;
		}
		if (dec->context->header.channels == 2)
		{
			//APE_FRAMECODE_PSEUDO_STEREO
			memcpy(dec->ch1, dec->ch0, sizeof(*ch0) * (*blocksDecoded));
		}
	}
	return APE_RET_SUCCESS;
}

static APE_RET_t stereo_decoder_process(ApeDecoder* dec, uint16_t blocksMax, uint16_t* blocksDecoded, void* bufferRead) {

	//0.check
#if 0
	if(GetBufferRemByte(dec->bufferRead) < dec->exitByte)
		return APE_RET_NOT_ENOUGH_INPUT;
#endif
	//1.init
	int32_t *ch0, *ch1;
	int32_t Y, X;
	int32_t L, R;

	ch0 = dec->ch0;
	ch1 = dec->ch1;
	if (dec->frameFlags & APE_FRAMECODE_STEREO_SILENCE)
	{
		*blocksDecoded = dec->blocksMax;
		memset(ch0, 0, sizeof(*ch0) * (*blocksDecoded));
		memset(ch1, 0, sizeof(*ch1) * (*blocksDecoded));
	}
	else
	{
		*blocksDecoded = 0;
		while (*blocksDecoded < blocksMax)
		{
			if (GetBufferRemByte(dec->bufferRead) < dec->exitByte)
				return APE_RET_NOT_ENOUGH_INPUT;
			//2.entropy decode
			if (!ApeEntropyDecode(&dec->riceY, &dec->rc, dec->bufferRead, &Y, dec->context->descriptor.fileversion))
				return APE_RET_ENTROPY_DECODE_ERROR;
			if (!ApeEntropyDecode(&dec->riceX, &dec->rc, dec->bufferRead, &X, dec->context->descriptor.fileversion))
				return APE_RET_ENTROPY_DECODE_ERROR;

			//3.NN filter
			ApeNNFilter(dec->filters[0], &Y, dec->context->descriptor.fileversion, dec->fSet);
			ApeNNFilter(dec->filters[1], &X, dec->context->descriptor.fileversion, dec->fSet);

			//4.apply filter
			ApePredictionFilterStero(&dec->prediction, &Y, &X);

#if 1
			//5.Y,X -> L,R
#if 1
			L = X - (Y / 2);
			R = L + Y;
			*ch0++ = L;
			*ch1++ = R;
#else
			* ch0++ = X - (Y >> 1);//L
			*ch1++ = X + (Y >> 1);//R
#endif
#else
			*ch0++ = X;
			*ch1++ = Y;
#endif

			(*blocksDecoded)++;
		}
	}
	return APE_RET_SUCCESS;
}

static bool ApeGetFrameInfoFromContext(ApeContext* context, ApeFrame* frame, uint32_t currentframe)
{
	// ApeDescriptor* descriptor = &context->descriptor;
	ApeHeader* header = &context->header;

	if (currentframe > header->totalframes - 1)
		return false;

	ApeFrame* frame_current = frame;
	if (currentframe == header->totalframes - 1) {
		frame_current->pos = context->seektable[currentframe];
		frame_current->nblocks = header->finalframeblocks;		
		frame_current->skip = (frame_current->pos - context->firstframe) & 3;
	}
	else if (currentframe == 0) {
		frame_current->pos = context->firstframe;
		frame_current->nblocks = header->blocksperframe;
		frame_current->skip = 0;
	}
	else {
		frame_current->pos = context->seektable[currentframe];
		frame_current->nblocks = header->blocksperframe;
		frame_current->skip = (frame_current->pos - context->firstframe) & 3;
	}

	if (frame_current->skip) {
		frame_current->pos -= frame_current->skip;
	}
	return true;
}

void ApeDecodeOut(uint8_t* dst, int32_t dstByte, int32_t* src0, int32_t* src1, int32_t srcValidByte, int32_t channel, int32_t loopNum)
{
	//(uint8_t * dst, int32_t dstByte, int32_t * src0, int32_t * src1, int32_t srcValidByte, int32_t channel, int32_t loopNum)
	int32_t* pSrc0 = (int32_t*)src0;
	int32_t* pSrc1 = (int32_t*)src1;
	int16_t* pDst = (int16_t*)dst;
	switch (srcValidByte)
	{
		case 1:
			if (channel == 1)
			{
				for (int32_t i = 0; i < loopNum; i++)
				{
					*pDst++ = (int16_t)((*pSrc0++) << 8);
				}
			}
			else
			{

				for (int32_t i = 0; i < loopNum; i++)
				{
					*pDst++ = (int16_t)((*pSrc0++) << 8);
					*pDst++ = (int16_t)((*pSrc1++) << 8);
				}
			}
			break;
		case 2:
			if (channel == 1)
			{
				for (int32_t i = 0; i < loopNum; i++)
				{
					*pDst++ = (int16_t)(*pSrc0++);
				}
			}
			else
			{

				for (int32_t i = 0; i < loopNum; i++)
				{
					*pDst++ = (int16_t)(*pSrc0++);
					*pDst++ = (int16_t)(*pSrc1++);
				}
			}
			break;
		case 3:
			if (channel == 1)
			{
				for (int32_t i = 0; i < loopNum; i++)
				{
					*pDst++ = (int16_t)((*pSrc0++) >> 8);
					
				}
			}
			else
			{

				for (int32_t i = 0; i < loopNum; i++)
				{
					*pDst++ = (int16_t)((*pSrc0++) >> 8);
					*pDst++ = (int16_t)((*pSrc1++) >> 8);
				}
			}
			break;
	}
}

static APE_RET_t ApeDecoderStartFrame(void* decoder, uint8_t* in, uint32_t inSize, uint32_t* inUsed)
{
	//check
	if (inSize < 10)
		return APE_RET_NOT_ENOUGH_INPUT;
	uint8_t* pIn = in;
	ApeDecoder* dec = (ApeDecoder*)decoder;
	ApeContext* context = dec->context;
	ApeDescriptor* descriptor = &context->descriptor;
	// ApeHeader* header = &context->header;
	ApeFrame* frame = &dec->frame;
	ApeGetFrameInfoFromContext(context, frame, dec->currentframe);

	//int skipLen = frame->pos - context->bitStreamReadedIndex;
	int skipLen = 0;
	SetBufferIn(dec->bufferRead, pIn, inSize);
	BufferRead(dec->bufferRead, skipLen);
	dec->CRC = BufferRead(dec->bufferRead, 4);
	//printf("[%d] CRC:%x\n", dec->currentframe, dec->CRC);
	dec->frameFlags = 0;
	if ((descriptor->fileversion > 3820)) {
		if (dec->CRC & 0x80000000)
		{
			dec->frameFlags = BufferRead(dec->bufferRead, 4);
		}
		dec->CRC &= ~0x80000000;
	}
	//rice
	dec->riceX.k = 10;
	dec->riceX.ksum = 16384;//(1 << dec->riceX.k) * 16;
	dec->riceY.k = 10;
	dec->riceY.ksum = 16384;//(1 << dec->riceY.k) * 16;

	//range coder
	BufferRead(dec->bufferRead, 1);// ignore the first byte... (slows compression too much to not output this dummy byte)
	dec->rc.buffer = BufferRead(dec->bufferRead, 1);
	dec->rc.low = dec->rc.buffer >> 1;
	dec->rc.range = (uint32_t)1 << 7;

	//NN filter
	for (uint16_t ch = 0; ch < APE_MAX_CHANNELS; ch++)
	{
		for (int i = 0; i < APE_FILTER_LEVELS; i++) {
			if (!ape_filter_orders[dec->fSet][i])
				break;

			uint32_t lenBytes = APE_ALIGN_8((ape_filter_orders[dec->fSet][i] + APE_NN_FILTER_ROLL_BUFFER_EXTRA_LENGTH) * sizeof(int16_t));
			uint32_t orderLenBytes = ape_filter_orders[dec->fSet][i] * sizeof(int16_t);
			ApeRollBufferInit(&dec->filters[ch][i].coeffs, (uint8_t*)dec->filterbuf[ch][i], lenBytes, orderLenBytes);
			ApeRollBufferInit(&dec->filters[ch][i].delay, (uint8_t*)dec->filterbuf[ch][i] + lenBytes, lenBytes, orderLenBytes);
			ApeRollBufferInit(&dec->filters[ch][i].adaptCoeffs, (uint8_t*)dec->filterbuf[ch][i] + lenBytes * 2, lenBytes, orderLenBytes);

			memset(dec->filterbuf[ch][i], 0, lenBytes * 3);
			dec->filters[ch][i].avg = 0;
		}
	}

	//prediction
	for (uint16_t ch = 0; ch < APE_MAX_CHANNELS; ch++)
	{
		memset(&dec->prediction.predictor[ch], 0, sizeof(ApePredictor));
		memcpy(dec->prediction.predictor[ch].coeffsA, initial_coeffs, sizeof(initial_coeffs));
		dec->prediction.xOld = 0;
	}


	dec->currentframe++;

	* inUsed = GetBufferUsed(dec->bufferRead);
	dec->context->bitStreamReadedIndex += GetByteUsed(dec->bufferRead);
	dec->exitByte = 16 * APE_MAX_CHANNELS;
	if(dec->frameFlags & APE_FRAMECODE_MONO_SILENCE
		|| dec->frameFlags & APE_FRAMECODE_STEREO_SILENCE)
		dec->exitByte = 32;

	return APE_RET_SUCCESS;

}

static APE_RET_t ApeDecoderEndFrame(void* decoder, uint8_t* in, uint32_t inSize, uint32_t* inUsed)
{
	//check
	if (decoder == NULL
		|| in == NULL
		|| inSize < 1)
		return APE_RET_INPUT_ERROR;

	ApeDecoder* dec = (ApeDecoder*)decoder;
	SetBufferIn(dec->bufferRead, in, inSize);
	// normalize
	while (dec->rc.range <= ((1u << (32 - 1)) >> 8))
	{
		BufferRead(dec->bufferRead, 1);
		dec->rc.range <<= 8;
		if (dec->rc.range == 0)
			break;
	}
	// used to back-pedal the last two bytes out
	// this should never have been a problem because we've outputted and normalized beforehand
	// but stopped doing it as of 3.96 in case it accounted for rare decompression failures
	if (dec->context->descriptor.fileversion <= 3950)
		BufferRead(dec->bufferRead, -2);
	if (inUsed)
		*inUsed = GetBufferUsed(dec->bufferRead);
	dec->context->bitStreamReadedIndex += GetByteUsed(dec->bufferRead);
	return APE_RET_SUCCESS; // end of life!
}

static APE_RET_t ApeDecodeBlocks(void* decoder, uint8_t* in, uint32_t inSize, uint32_t* inUsed, uint8_t* out, uint32_t* outSize)
{
	//check

	//init
	ApeDecoder* dec = (ApeDecoder*)decoder;
	ApeContext* context = dec->context;
	APE_RET_t ret;
	
	SetBufferIn(dec->bufferRead, in, inSize);

	//
	if ((context->header.channels == 1)
		|| (dec->frameFlags & APE_FRAMECODE_PSEUDO_STEREO))
	{
		ret = mono_decoder_process(dec, dec->blocksMax, &dec->blocksDecoded, dec->bufferRead);
	}
	else
	{
		ret = stereo_decoder_process(dec, dec->blocksMax, &dec->blocksDecoded, dec->bufferRead);
	}

	uint16_t sampleByteIn = dec->context->header.bps>>3;
	uint16_t sampleByteOut = 16>>3;
	ApeDecodeOut(out, sampleByteOut, dec->ch0, dec->ch1, sampleByteIn, dec->context->header.channels, dec->blocksDecoded);

	*inUsed = GetBufferUsed(dec->bufferRead);
	*outSize = dec->context->header.channels * sampleByteOut * dec->blocksDecoded;
	dec->context->bitStreamReadedIndex += GetByteUsed(dec->bufferRead);

	if (ret == APE_RET_ENTROPY_DECODE_ERROR)
	{
		return APE_RET_ENTROPY_DECODE_ERROR;
	}
	return APE_RET_SUCCESS;
}

/***************************************************************
*************************** interface **************************
****************************************************************/
EXTERNC{

//get
uint32_t ApeGetDecoderSize(uint16_t compressLevel,uint32_t seektablelength, uint32_t wavheaderlength)
{
	return ApeGetApeDecoderSize() + ApeGetContextSize(seektablelength,wavheaderlength) + ApeGetNNFilterSize(compressLevel) + GetBufferReadSize();
}

uint16_t ApeGetDecoderChannels(void* decoder)
{
	return ((ApeDecoder*)decoder)->context->header.channels;
}

uint32_t ApeGetDecoderSampleRate(void* decoder)
{
	return ((ApeDecoder*)decoder)->context->header.samplerate;
}

uint16_t ApeGetDecoderSampleBits(void* decoder)
{
	return ((ApeDecoder*)decoder)->context->header.bps;
}

uint32_t ApeGetDecoderNblocks(void* decoder)
{
	return ((ApeDecoder*)decoder)->frame.nblocks;
}

uint16_t ApeGetDecodedBlocks(void* decoder)
{
	return ((ApeDecoder*)decoder)->blocksDecoded;
}

//init
APE_RET_t ApeDecoderInit(void* decoder, void* contextIn)
{
	//check
	if (!decoder || !contextIn)
		return APE_RET_INPUT_ERROR;
	
	//memset decoder
	int decoderSize = ApeGetDecoderSize(((ApeContext*)contextIn)->header.compressiontype, ((ApeContext*)contextIn)->descriptor.seektablelength, ((ApeContext*)contextIn)->descriptor.wavheaderlength);
	if (decoderSize < 1)
		return APE_RET_INPUT_ERROR;
	memset(decoder, 0, decoderSize);

	ApeDecoder* dec = (ApeDecoder*)decoder;
	uint32_t offset = 0;
	uint32_t size = 0;

	//ApeContext
	offset = sizeof(ApeDecoder);
	dec->context = (ApeContext*)((uint8_t*)dec + offset);
	ApeContext* context = dec->context;
	
	//copy context
	int contextSize = ApeGetContextSize(((ApeContext*)contextIn)->descriptor.seektablelength, ((ApeContext*)contextIn)->descriptor.wavheaderlength);
	if (contextSize < 1)
		return APE_RET_INPUT_ERROR;
	memcpy(context, contextIn, contextSize);
	context->seektable = (uint32_t*)((uint8_t*)context + APE_ALIGN_8(sizeof(ApeContext)));
	context->waveHeader = (uint32_t*)((uint8_t*)context + APE_ALIGN_8(sizeof(ApeContext)) + APE_ALIGN_8(context->descriptor.seektablelength));

	//clear NN Filter
	offset = sizeof(ApeDecoder) + contextSize;
	dec->fSet = context->header.compressiontype / 1000 - 1;
	for (uint16_t ch = 0; ch < APE_MAX_CHANNELS; ch++)
	{
		for (int i = 0; i < APE_FILTER_LEVELS; i++) {
			if (!ape_filter_orders[dec->fSet][i])
				break;
			offset += size;
			dec->filterbuf[ch][i] = (int16_t*)((uint8_t*)decoder + offset);			
			size = APE_ALIGN_8(((ape_filter_orders[dec->fSet][i]+APE_NN_FILTER_ROLL_BUFFER_EXTRA_LENGTH) * sizeof(int16_t))) * 3;
			memset(dec->filterbuf[ch][i], 0, size);
		}
	}
	//buffer read
	offset = sizeof(ApeDecoder) + contextSize + ApeGetNNFilterSize(context->header.compressiontype);
	dec->bufferRead = (void*)((uint8_t*)decoder + offset);
	BufferReadInit(dec->bufferRead);

	//dec->inBuffTemp.bufferSize = IN_BUFF_TEMP_SIZE;
	dec->blocksMax = APE_BLOCKS_MAX;

	//dec->currentframe = 0;
	dec->isStartFrame = true;	
	
	return APE_RET_SUCCESS;
}

//static int num;
APE_RET_t ApeDecode(void* decoder, uint8_t* in, int32_t inSize, int32_t* inUsed, uint8_t* out, int32_t* outSize)
{
	//check
	if (decoder == NULL
		|| in == NULL
		|| inSize < 1
		|| out == NULL
		|| outSize == NULL)
	{
		*inUsed = 0;
		if (outSize)
			*outSize = 0;
		//APE_RET_INPUT_ERROR;
		return APE_RET_CONTINUE;
	}
	ApeDecoder* dec = (ApeDecoder*)decoder;
	int32_t inOffset = 0;
	int32_t outSizeMax = *outSize;
	int32_t outOffset = 0;
	uint32_t bufferUsed = 0;

	//loop
	while (1)
	{
		//start frame
		if (dec->isStartFrame == true) {
			APE_RET_t startRet = ApeDecoderStartFrame(dec, in + inOffset, inSize - inOffset, &bufferUsed);
			if (startRet != APE_RET_SUCCESS)
				break;
			dec->isStartFrame = false;
			inOffset += bufferUsed;
			dec->blocksMax = APE_BLOCKS_MAX;
		}

		if (dec->frame.nblocks - dec->blocksUsed < APE_BLOCKS_MAX) {
			dec->blocksMax = dec->frame.nblocks - dec->blocksUsed;
			if (dec->currentframe == dec->context->header.totalframes)
				dec->exitByte = 4;
		}
		if ((inSize - inOffset) < dec->exitByte)
		{
			break;
		}
		if (dec->blocksMax > 0)
		{
			if ((outOffset + dec->blocksMax * 2 * dec->context->header.channels) >= outSizeMax)
				//APE_RET_NOT_ENOUGH_INPUT
				break;
			
			uint32_t outLen = 0;
			APE_RET_t ret = ApeDecodeBlocks(dec, in + inOffset, inSize - inOffset, &bufferUsed, out + outOffset, &outLen);
			//printf("[%d] iSize:%d,bufferUsed:%d,outLenSum:%d,blocksUsedNow:%d,ret:%d\n",
			//	num++, inSize - inOffset, bufferUsed, outOffset + outLen, ApeGetDecodedBlocks(dec), ret);			
			if (outLen < 1)
			{
				break;
			}

			inOffset += bufferUsed;
			dec->blocksUsed += ApeGetDecodedBlocks(dec);
			outOffset += outLen;
		}
		//end frame
		if (dec->blocksUsed == dec->frame.nblocks) {
			if (dec->currentframe == (dec->context->header.totalframes))
			{
				inOffset = inSize;
				break;
			}

			APE_RET_t endRet = ApeDecoderEndFrame(dec, in + inOffset, inSize - inOffset, &bufferUsed);
			if (endRet != APE_RET_SUCCESS)
			{
				break;
			}
			dec->blocksUsed = 0;
			dec->isStartFrame = true;
			inOffset += bufferUsed;
		}
	}
	*outSize = outOffset;
	if (inUsed)
		*inUsed = inOffset;
#if 0
	if (	(dec->currentframe == (dec->context->header.totalframes) 
			&&	(dec->blocksUsed == dec->frame.nblocks))
		||  dec->notEnoughInputAcc > 10)
		return APE_RET_FINISH;
#else
	if (dec->currentframe == (dec->context->header.totalframes)
		&& (dec->blocksUsed == dec->frame.nblocks))
		return APE_RET_FINISH;
#endif
	return APE_RET_CONTINUE;
}
}




















