#if 1
#include"Algo.AudioSamlpes.h"
//#include"ApeCom.h"
//#include"ApeComInner.h"

#include"Ape.Inner.type.h"
#include"Ape.Inner.func.h"
#include"ApeOri.h"

using namespace Algo;
using namespace Audio;


EXTERNC {
	int32_t Ape_GetStateSize()
	{
		return sizeof(ApeDecoder);
	}

	int32_t Ape_StateInit(void* pMusicPlcStateIn, ApeInitParam* sampleParam)
	{
#if 1
		//check
		if (!pMusicPlcStateIn
			|| !sampleParam
			|| !sampleParam->basePorting)
			return APERET_FAIL;
		ApeDecoder* pDec = (ApeDecoder*)pMusicPlcStateIn;

		switch (sampleParam->mode)
		{
		case ApeInitMode::APE_INIT_MODE_DMUX_USE_BUFFER:
			pDec->mode = ApeMode::APE_MODE_DMUX;
			return Ape_StateInitWithBuffer(pDec, sampleParam->basePorting, (u8*)sampleParam->bufferInit.buffer, (i32*)sampleParam->bufferInit.size);
		case ApeInitMode::APE_INIT_MODE_DMUX_USE_NO_PARAM:
			ALGO_MEM_SET(pDec, 0, sizeof(ApeDecoder));
			pDec->mode = ApeMode::APE_MODE_DMUX;
			pDec->basePorting = sampleParam->basePorting;
			pDec->isInited = false;
			return APERET_SUCCESS;
		case ApeInitMode::APE_INIT_MODE_DEC_USE_CONTEXT:
			pDec->mode = ApeMode::APE_MODE_DEC;
			return Ape_StateInitWithContext(pDec, sampleParam->basePorting, (ApeContext*)sampleParam->contextInit.context);
		case ApeInitMode::APE_INIT_MODE_DEC_USE_BUFFER:
			pDec->mode = ApeMode::APE_MODE_DEC;
			return Ape_StateInitWithBuffer(pDec, sampleParam->basePorting, (u8*)sampleParam->bufferInit.buffer, (i32*)sampleParam->bufferInit.size);
		case ApeInitMode::APE_INIT_MODE_DEC_USE_NO_PARAM:
			ALGO_MEM_SET(pDec, 0, sizeof(ApeDecoder));
			pDec->mode = ApeMode::APE_MODE_DEC;
			pDec->basePorting = sampleParam->basePorting;
			pDec->isInited = false;
			return APERET_SUCCESS;
		default:
			return APERET_FAIL;
			break;
		}
#if 0
		if (sampleParam->mode==)
		{
			return Ape_StateInitWithContext(pDec, (ApeContext*)sampleParam->contextInit.context);
		}
		else if (sampleParam->bufferInit.buffer)
		{
			return Ape_StateInitWithBuffer(pDec, (u8*)sampleParam->bufferInit.buffer, (i32*)sampleParam->bufferInit.size);
		}
		else if (sampleParam->NoParamInit.isNoParam)
		{
			ALGO_MEM_SET(pDec, 0, sizeof(ApeDecoder));
			pDec->basePorting = sampleParam->basePorting;
			pDec->isInited = false;
			return APERET_SUCCESS;
		}
		else
		{
			return APERET_FAIL;
		}
#endif
		return APERET_SUCCESS;
#endif
	}

	int32_t Ape_Set(void* pMusicPlcStateIn, ApeSetChhoose_e choose, void** val)
	{
		//check
		if (!pMusicPlcStateIn
			|| choose >= APESET_CHOOSE_MAX
			|| !val)
			return APERET_FAIL;
		ApeDecoder* pDec = (ApeDecoder*)pMusicPlcStateIn;
		if (pDec->isInited == false)
			return APERET_FAIL;

		switch (choose)
		{
#if 0
		case AUDIO_SPEED_CONTROL_SET_CHOOSE_SPEEDQ8:
			pState->speed = (f32)(u32)val[0] / (1 << 8);
#endif
		default:
			break;
		}
		return APERET_SUCCESS;
	}

	int32_t Ape_Get(void* pMusicPlcStateIn, ApeGetChhoose_e choose, void** val)
	{
		//check
		if (!pMusicPlcStateIn
			|| choose >= APEGET_CHOOSE_MAX
			|| !val)
			return APERET_FAIL;
		ApeDecoder* pDec = (ApeDecoder*)pMusicPlcStateIn;
		if (pDec->isInited == false)
			return APERET_FAIL;
		
		switch (choose)
		{
#if 1
		case APEGET_CHOOSE_CONTEXT:
			val[0] = &pDec->context;
#endif
		default:
			break;
		}
		return APERET_SUCCESS;
	}

	int32_t Ape_Run(void* pMusicPlcStateIn, uint8_t* in, int32_t inLen, uint8_t* out, int32_t* outLen)
	{
#if 1
		//check
		if (!pMusicPlcStateIn
			||!out
			||!outLen)
			return APERET_FAIL;
		ApeDecoder* pDec = (ApeDecoder*)pMusicPlcStateIn;
		if(pDec->isInited)
			return Ape_RunInner(pDec, in, inLen, out, outLen);
		else
		{
			if(!pDec->basePorting)
				return APERET_FAIL;
			//AlgoBasePorting* basePorting = pDec->basePorting;
			ApeInitParam initParam;
			ALGO_MEM_SET(&initParam, 0, sizeof(initParam));
			initParam.basePorting = pDec->basePorting;
			if(pDec->mode==ApeMode::APE_MODE_DMUX)
				initParam.mode = ApeInitMode::APE_INIT_MODE_DMUX_USE_BUFFER;
			else if (pDec->mode == ApeMode::APE_MODE_DEC)
				initParam.mode = ApeInitMode::APE_INIT_MODE_DEC_USE_BUFFER;
			initParam.bufferInit.buffer = in;
			i32 inLenUsed = inLen;
			initParam.bufferInit.size = &inLenUsed;
			Ape_StateInit(pDec, &initParam);
			inLen -= inLenUsed;
			in += inLenUsed;
			return Ape_RunInner(pDec, in, inLen, out, outLen);
		}
#endif
	}

	int32_t Ape_StateDeInit(void* pMusicPlcStateIn)
	{
		if (!pMusicPlcStateIn)
			return APERET_FAIL;
		ApeDecoder* pDec = (ApeDecoder*)pMusicPlcStateIn;
		return Ape_StateDeInitInner(pDec);
	}
}

#endif