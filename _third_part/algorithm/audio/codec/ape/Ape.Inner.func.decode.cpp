#include"Algo.Printer.h"
#include"ApeOri.h"
#include"Ape.Inner.func.h"

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

static APE_RET_t ApeDecoderStartFrame(void* decoder, uint8_t* in, uint32_t inLen, uint32_t* inUsed)
{
	//check
	if (inLen < 10)
		return APE_RET_NOT_ENOUGH_INPUT;
	uint8_t* pIn = in;
	ApeDecoder* pDec = (ApeDecoder*)decoder;
	ApeContext* context = &pDec->context;
	ApeDescriptor* descriptor = &context->descriptor;
	// ApeHeader* header = &context.header;
	ApeFrame* frame = &pDec->frame;
	ApeGetFrameInfoFromContext(context, frame, pDec->currentframe);

	//int skipLen = frame->pos - context.bitStreamReadedIndex;
	int skipLen = 0;
	pDec->bufferRead.SetBufferIn(pIn, inLen);
	pDec->bufferRead.Read(skipLen);
	pDec->CRC = pDec->bufferRead.Read(4);
	//printf("[%d] CRC:%x\n", pDec->currentframe, pDec->CRC);
	pDec->frameFlags = 0;
	if ((descriptor->fileversion > 3820)) {
		if (pDec->CRC & 0x80000000)
		{
			pDec->frameFlags = pDec->bufferRead.Read(4);
		}
		pDec->CRC &= ~0x80000000;
	}
	//rice
	pDec->riceX.k = 10;
	pDec->riceX.ksum = 16384;//(1 << pDec->riceX.k) * 16;
	pDec->riceY.k = 10;
	pDec->riceY.ksum = 16384;//(1 << pDec->riceY.k) * 16;

	//range coder
	pDec->bufferRead.Read(1);
	pDec->rc.buffer = pDec->bufferRead.Read(1);
	pDec->rc.low = pDec->rc.buffer >> 1;
	pDec->rc.range = (uint32_t)1 << 7;

	//NN filter
	for (uint16_t ch = 0; ch < APE_MAX_CHANNELS; ch++)
	{
		for (int i = 0; i < APE_FILTER_LEVELS; i++) {
			if (!ape_filter_orders[pDec->fSet][i])
				break;

			uint32_t lenBytes = (ape_filter_orders[pDec->fSet][i] + APE_NN_FILTER_ROLL_BUFFER_EXTRA_LENGTH) * sizeof(int16_t);
			uint32_t orderLenBytes = ape_filter_orders[pDec->fSet][i] * sizeof(int16_t);
			ApeRollBufferInit(&pDec->filters[ch][i].coeffs, (uint8_t*)pDec->filterbuf[ch][i], lenBytes, orderLenBytes);
			ApeRollBufferInit(&pDec->filters[ch][i].delay, (uint8_t*)pDec->filterbuf[ch][i] + lenBytes, lenBytes, orderLenBytes);
			ApeRollBufferInit(&pDec->filters[ch][i].adaptCoeffs, (uint8_t*)pDec->filterbuf[ch][i] + lenBytes * 2, lenBytes, orderLenBytes);

			memset(pDec->filterbuf[ch][i], 0, lenBytes * 3);
			pDec->filters[ch][i].avg = 0;
		}
	}

	//prediction
	for (uint16_t ch = 0; ch < APE_MAX_CHANNELS; ch++)
	{
		memset(&pDec->prediction.predictor[ch], 0, sizeof(ApePredictor));
		memcpy(pDec->prediction.predictor[ch].coeffsA, initial_coeffs, sizeof(initial_coeffs));
		pDec->prediction.xOld = 0;
	}


	pDec->currentframe++;
	
	*inUsed = pDec->bufferRead.GetBufferUsed();
	pDec->context.bitStreamReadedIndex += pDec->bufferRead.GetByteUsed();
	pDec->exitByte = 16 * APE_MAX_CHANNELS;
	if (pDec->frameFlags & APE_FRAMECODE_MONO_SILENCE
		|| pDec->frameFlags & APE_FRAMECODE_STEREO_SILENCE)
		pDec->exitByte = 32;

	return APE_RET_SUCCESS;

}




static APE_RET_t ApeDecoderEndFrame(void* decoder, uint8_t* in, uint32_t inSize, uint32_t* inUsed)
{
	//check
	if (decoder == NULL
		|| in == NULL
		|| inSize < 1)
		return APE_RET_INPUT_ERROR;

	ApeDecoder* pDec = (ApeDecoder*)decoder;
	pDec->bufferRead.SetBufferIn(in, inSize);
	// normalize
	while (pDec->rc.range <= ((1u << (32 - 1)) >> 8))
	{
		pDec->bufferRead.Read(1);
		pDec->rc.range <<= 8;
		if (pDec->rc.range == 0)
			break;
	}
	// used to back-pedal the last two bytes out
	// this should never have been a problem because we've outputted and normalized beforehand
	// but stopped doing it as of 3.96 in case it accounted for rare decompression failures
	if (pDec->context.descriptor.fileversion <= 3950)
		pDec->bufferRead.Read(-2);
	if (inUsed)
		*inUsed = pDec->bufferRead.GetBufferUsed();
	pDec->context.bitStreamReadedIndex += pDec->bufferRead.GetByteUsed();
	return APE_RET_SUCCESS; // end of life!
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
			if (dec->bufferRead.GetBufferRemByte() < dec->exitByte)
				return APE_RET_NOT_ENOUGH_INPUT;

			//2.entropy decode
			if (!ApeEntropyDecode(&dec->riceY, &dec->rc, &dec->bufferRead, &Y, dec->context.descriptor.fileversion))
				return APE_RET_ENTROPY_DECODE_ERROR;

			//3.NN filter
			ApeNNFilter(dec->filters[0], &Y, dec->context.descriptor.fileversion, dec->fSet);

			//4.apply filter
			ApePredictionFilterMono(&dec->prediction, &Y);

			*ch0++ = Y;
			(*blocksDecoded)++;
		}
		if (dec->context.header.channels == 2)
		{
			//APE_FRAMECODE_PSEUDO_STEREO
			memcpy(dec->ch1, dec->ch0, sizeof(*ch0) * (*blocksDecoded));
		}
	}
	return APE_RET_SUCCESS;
}
i32 frames = 0;
static APE_RET_t stereo_decoder_process(ApeDecoder* dec, uint16_t blocksMax, uint16_t* blocksDecoded, void* bufferRead) {

	//0.check
#if 0
	if (GetBufferRemByte(dec->bufferRead) < dec->exitByte)
		return APE_RET_NOT_ENOUGH_INPUT;
#endif
	//1.init
	int32_t* ch0, * ch1;
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
			
			if (dec->bufferRead.GetBufferRemByte() < dec->exitByte)
				return APE_RET_NOT_ENOUGH_INPUT;

			//2.entropy decode
			if (!ApeEntropyDecode(&dec->riceY, &dec->rc, &dec->bufferRead, &Y, dec->context.descriptor.fileversion))
				return APE_RET_ENTROPY_DECODE_ERROR;
			if (!ApeEntropyDecode(&dec->riceX, &dec->rc, &dec->bufferRead, &X, dec->context.descriptor.fileversion))
				return APE_RET_ENTROPY_DECODE_ERROR;

#if 0
			frames++;
			//ALGO_PRINT("[%d] nY:%x,\tnX:%x", frames, Y, X);
			if (frames == 8859)
				int a = 1;

#endif			

			//3.NN filter
			ApeNNFilter(dec->filters[0], &Y, dec->context.descriptor.fileversion, dec->fSet);
			ApeNNFilter(dec->filters[1], &X, dec->context.descriptor.fileversion, dec->fSet);

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
			* ch0++ = X;
			*ch1++ = Y;
#endif

			(*blocksDecoded)++;
		}
	}
	return APE_RET_SUCCESS;
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

static APE_RET_t ApeDecodeBlocks(ApeDecoder* decoder, uint8_t* in, uint32_t inSize, uint32_t* inUsed, uint8_t* out, uint32_t* outSize)
{
	//check

	//init
	ApeDecoder* pDec = decoder;
	ApeContext* context = &pDec->context;
	APE_RET_t ret;

	//SetBufferIn(pDec->bufferRead, in, inSize);
	pDec->bufferRead.SetBufferIn(in, inSize);
	//
	if ((context->header.channels == 1)
		|| (pDec->frameFlags & APE_FRAMECODE_PSEUDO_STEREO))
	{
		ret = mono_decoder_process(pDec, pDec->blocksMax, &pDec->blocksDecoded, &pDec->bufferRead);
	}
	else
	{
		ret = stereo_decoder_process(pDec, pDec->blocksMax, &pDec->blocksDecoded, &pDec->bufferRead);
	}

	uint16_t sampleByteIn = context->header.bps >> 3;
	uint16_t sampleByteOut = 16 >> 3;
	ApeDecodeOut(out, sampleByteOut, pDec->ch0, pDec->ch1, sampleByteIn, context->header.channels, pDec->blocksDecoded);

	*inUsed = pDec->bufferRead.GetBufferUsed();
	*outSize = context->header.channels * sampleByteOut * pDec->blocksDecoded;
	context->bitStreamReadedIndex += pDec->bufferRead.GetByteUsed();

	if (ret == APE_RET_ENTROPY_DECODE_ERROR)
	{
		return APE_RET_ENTROPY_DECODE_ERROR;
	}
	return APE_RET_SUCCESS;
}

STATIC i32 Ape_Decode(ApeDecoder* pMusicPlcStateIn, uint8_t* in, int32_t inLen, uint8_t* out, int32_t* outLen)
{

	ApeDecoder* pDec = pMusicPlcStateIn;

	int32_t inOffset = 0;
	int32_t outSizeMax = *outLen;
	int32_t outOffset = 0;
	uint32_t bufferUsed = 0;

	//loop
	while (1)
	{
		//in cache
		i32 used;
		pDec->inCache.AppendFully(in + inOffset, inLen - inOffset,&used);
		inOffset += used;

		if (pDec->currentframe != pDec->context.header.totalframes)
		{
			if (pDec->inCache.GetSize() < 10)
				break;
		}
		else
		{
			if (pDec->inCache.GetSize() < 4)
				break;
		}
			
			
		//start frame
		if (pDec->isStartFrame == true) {
			
			APE_RET_t startRet = ApeDecoderStartFrame(pDec, pDec->inCache.GetData(), pDec->inCache.GetSize(), &bufferUsed);
			if (startRet != APE_RET_SUCCESS)
				break;
			pDec->isStartFrame = false;
			pDec->inCache.Used(bufferUsed);
			pDec->blocksMax = APE_BLOCKS_MAX;
		}

		if (pDec->frame.nblocks - pDec->blocksUsed < APE_BLOCKS_MAX) {
			pDec->blocksMax = pDec->frame.nblocks - pDec->blocksUsed;
			if (pDec->currentframe == pDec->context.header.totalframes)
				pDec->exitByte = 4;
		}
		if (pDec->blocksMax > 0)
		{
			if ((outOffset + pDec->blocksMax * 2 * pDec->context.header.channels) >= outSizeMax)
				break;

			uint32_t outLen = 0;
			APE_RET_t ret = ApeDecodeBlocks(pDec, pDec->inCache.GetData(), pDec->inCache.GetSize(), &bufferUsed, out + outOffset, &outLen);
			pDec->inCache.Used(bufferUsed);
			//printf("[%d] iSize:%d,bufferUsed:%d,outLenSum:%d,blocksUsedNow:%d,ret:%d\n",
			//	num++, inLen - inOffset, bufferUsed, outOffset + outLen, ApeGetDecodedBlocks(pDec), ret);			
			if (outLen < 1)
			{
				break;
			}
			pDec->blocksUsed += pDec->blocksDecoded;
			outOffset += outLen;
		}
		//end frame
		if (pDec->blocksUsed == pDec->frame.nblocks) {
#if 0
			if (pDec->currentframe == (pDec->context.header.totalframes))
			{
				inOffset = inLen;
				break;
			}
#endif
			APE_RET_t endRet = ApeDecoderEndFrame(pDec, pDec->inCache.GetData(), pDec->inCache.GetSize(), &bufferUsed);
			pDec->inCache.Used(bufferUsed);
			if (endRet != APE_RET_SUCCESS)
			{
				break;
			}
			pDec->blocksUsed = 0;
			pDec->isStartFrame = true;
		}
	}
	pDec->inCache.ClearUsed();
	*outLen = outOffset;
	if (pDec->currentframe == (pDec->context.header.totalframes)
		&& (pDec->blocksUsed == pDec->frame.nblocks))
		return APE_RET_FINISH;
	return APE_RET_SUCCESS;
}











i32 Ape_RunInner(ApeDecoder* pMusicPlcStateIn, uint8_t* in, int32_t inLen, uint8_t* out, int32_t* outLen)
{
	//ALGO_PRINT();
	ApeDecoder* pDec= pMusicPlcStateIn;

	if (pDec->mode == ApeMode::APE_MODE_DEC)
	{
		return Ape_Decode(pDec, in, inLen, out, outLen);

	}


	return APERET_SUCCESS;
}
