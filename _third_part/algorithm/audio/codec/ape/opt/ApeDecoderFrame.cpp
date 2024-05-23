#include"ApePrive.h"
#include<string.h>


//interface
EXTERNC {

	EXTERNC uint32_t ApeGetDecoderSize(uint16_t compressLevel, uint32_t seektablelength, uint32_t wavheaderlength);
	int32_t ApeDecodeFrameGetSize(void* contextIn)
	{
#if 0
		return 0;
#else
		ApeContext* pContext = (ApeContext*)contextIn;
		return ApeGetDecoderSize(pContext->header.compressiontype, pContext->descriptor.seektablelength, pContext->descriptor.wavheaderlength);
#endif
	}

	EXTERNC APE_RET_t ApeDecoderInit(void* decoder, void* context);
	APE_RET_t ApeDecodeFrameInit(void* hd, void* contextIn)
	{
#if 0
		//check
		if (!hd || !contextIn)
			return APE_RET_INPUT_ERROR;

		//memset hd
		int decoderSize = ApeDecodeFrameGetSize(contextIn);
		if (decoderSize < 1)
			return APE_RET_INPUT_ERROR;
		memset(hd, 0, decoderSize);

		ApeDecoder* dec = (ApeDecoder*)hd;
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
				dec->filterbuf[ch][i] = (int16_t*)((uint8_t*)hd + offset);
				size = APE_ALIGN_8(((ape_filter_orders[dec->fSet][i] + APE_NN_FILTER_ROLL_BUFFER_EXTRA_LENGTH) * sizeof(int16_t))) * 3;
				memset(dec->filterbuf[ch][i], 0, size);
			}
		}
		//buffer read
		offset = sizeof(ApeDecoder) + contextSize + ApeGetNNFilterSize(context->header.compressiontype);
		dec->bufferRead = (void*)((uint8_t*)hd + offset);
		BufferReadInit(dec->bufferRead);

		//dec->inBuffTemp.bufferSize = IN_BUFF_TEMP_SIZE;
		dec->blocksMax = APE_BLOCKS_MAX;

		//dec->currentframe = 0;
		dec->isStartFrame = true;

		return APE_RET_SUCCESS;
#else
		return ApeDecoderInit(hd, contextIn);
#endif
	}

	APE_RET_t ApeDecodeFrameStart(void* hd, int32_t currentframe)
	{
		//check
		if (!hd) {
			return APE_RET_INPUT_ERROR;
		}
		ApeDecoder* dec = (ApeDecoder*)hd;
		BufferReadInit(dec->bufferRead);
		dec->currentframe = currentframe - 1;
		dec->isStartFrame = true;
	}

	EXTERNC APE_RET_t ApeDecode(void* decoder, uint8_t* in, int32_t inSize, int32_t* inUsed, uint8_t* out, int32_t* outSize);
	APE_RET_t ApeDecodeFrameRun(void* hd, uint8_t* in, int32_t inSize, int32_t* inUsed, uint8_t* out, int32_t* outSize)
	{
#if 0
		//check
		if (hd == NULL
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
		ApeDecoder* dec = (ApeDecoder*)hd;
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
		if ((dec->currentframe == (dec->context->header.totalframes)
			&& (dec->blocksUsed == dec->frame.nblocks))
			|| dec->notEnoughInputAcc > 10)
			return APE_RET_FINISH;
#else
		if (dec->currentframe == (dec->context->header.totalframes)
			&& (dec->blocksUsed == dec->frame.nblocks))
			return APE_RET_FINISH;
#endif
		return APE_RET_CONTINUE;
#else
		return ApeDecode(hd, in, inSize, inUsed, out, outSize);
#endif
	}
	APE_RET_t ApeDecodeFrameEnd(void* hd)
	{
		return 0;
	}
	APE_RET_t ApeDecodeFrameDeInit(void* hd)
	{
		return 0;
	}
}