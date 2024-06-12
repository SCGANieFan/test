#if 1
#include<new>
#include"Algo.AudioSamlpes.h"
#include"Ape.Inner.decoder.h"
#include"Ape.Inner.demuxer.h"
#include"Ape.h"
using namespace Algo;
using namespace Audio;

typedef enum {
	APE_MODE_DMUX = 0,
	APE_MODE_DEC,
}ApeModeInner;

typedef struct {
	union {
		struct {
			b1 isInited;
		};
		u32 flags;
	};
	ApeModeInner mode;
	void* codec;
}ApeState;


EXTERNC {
	int32_t Ape_GetSize(ApeMode mode)
	{
		int32_t size = sizeof(ApeState);		
		if (mode < APE_INIT_MODE_DEC_USE_CONTEXT){//demux
			size += sizeof(ApeDemuxer);
		}
		else if (mode < APE_INIT_MODE_MAX){//decode
			size += sizeof(ApeDecoder);
		}
		return size;
	}

	int32_t Ape_Init(void* pStateIn, ApeInitParam* sampleParam)
	{
#if 1
		//check
		if (!pStateIn
			|| !sampleParam
			|| !sampleParam->basePorting)
			return APERET_FAIL;
		ApeState* pState = (ApeState*)pStateIn;
		ALGO_MEM_SET(pState, 0, Ape_GetSize(sampleParam->mode));

		pState->codec = (u8*)pState + sizeof(ApeState);
		if (sampleParam->mode < APE_INIT_MODE_DEC_USE_CONTEXT){//demux
			pState->mode = ApeModeInner::APE_MODE_DMUX;
			ApeDemuxer* pDemux = new(pState->codec)ApeDemuxer();
			switch (sampleParam->mode)
			{
			case ApeMode::APE_INIT_MODE_DMUX_USE_BUFFER:
				pState->isInited = true;
				return pDemux->InitWithBuffer(sampleParam->basePorting, (u8*)sampleParam->bufferInit.buffer, (i32*)sampleParam->bufferInit.size);
			case ApeMode::APE_INIT_MODE_DMUX:
				pState->isInited = false;
				return pDemux->InitWithNoBuffer(sampleParam->basePorting);
			}
		}
		else if (sampleParam->mode < APE_INIT_MODE_MAX){//decode
			pState->mode = ApeModeInner::APE_MODE_DEC;
			ApeDecoder* pDec = new(pState->codec)ApeDecoder();
			switch (sampleParam->mode)
			{
			case ApeMode::APE_INIT_MODE_DEC_USE_CONTEXT:
				pState->isInited = true;
				return pDec->InitWithContext(sampleParam->basePorting, (ApeContext_t*)sampleParam->contextInit.context);
			case ApeMode::APE_INIT_MODE_DEC_USE_BUFFER:
				pState->isInited = true;
				return pDec->InitWithBuffer(sampleParam->basePorting, (u8*)sampleParam->bufferInit.buffer, (i32*)sampleParam->bufferInit.size);
			case ApeMode::APE_INIT_MODE_DEC:
				pState->isInited = false;
				return pDec->Init(sampleParam->basePorting);
			}
		}
		else{
			return APERET_FAIL;
		}
#endif
	}

	int32_t Ape_Set(void* pStateIn, ApeSetChhoose_e choose, void** val)
	{
		//check
		if (!pStateIn
			|| choose >= APESET_MAX)
			return APERET_FAIL;
		ApeState* pState = (ApeState*)pStateIn;
		if (pState->isInited == false)
			return APERET_FAIL;

		if (pState->mode == ApeModeInner::APE_MODE_DMUX) {
			ApeDemuxer* pDemux = (ApeDemuxer*)pState->codec;
			switch (choose)
			{
#if 0
			case APEGET_CHOOSE_CONTEXT:
				val[0] = &pDemux->context;
			default:
				break;
#endif
			}
		}
		else if (pState->mode == ApeModeInner::APE_MODE_DEC) {
			ApeDecoder* pDec = (ApeDecoder*)pState->codec;
			switch (choose)
			{
			case APESET_DEC_HAS_IN_CACHE:
				pDec->haveInCache= (b1)(u32)val[0]; break;
			case APESET_DEC_START_NEW_FRAME:
				pDec->StartNewFrame((u32)val[0]); break;
			default:
				break;
			}
		}
		return APERET_SUCCESS;
	}

	int32_t Ape_Get(void* pStateIn, ApeGetChhoose_e choose, void** val)
	{
		//check
		if (!pStateIn
			|| choose >= APEGET_MAX
			|| !val)
			return APERET_FAIL;
		
		ApeState* pState = (ApeState*)pStateIn;
		if (pState->isInited == false)
			return APERET_FAIL;

		if (pState->mode == ApeModeInner::APE_MODE_DMUX) {
			ApeDemuxer* pDemux = (ApeDemuxer*)pState->codec;
			switch (choose)
			{
			case APEGET_CONTEXT:
				val[0] = &pDemux->context;
			case APEGET_FRAME_SEEK_TABLE:
				((FrameSeekTable*)val[0])->seektable = pDemux->context.GetSeekTable();
				((FrameSeekTable*)val[0])->totalframes= pDemux->context.GetHeader()->totalframes;
			default:
				break;
			}
		}
		else if (pState->mode == ApeModeInner::APE_MODE_DEC) {
			ApeDecoder* pDec = (ApeDecoder*)pState->codec;
			switch (choose)
			{
			case APEGET_CONTEXT:
				val[0] = &pDec->context;
			case APEGET_FRAME_SEEK_TABLE:
				((FrameSeekTable*)val[0])->seektable = pDec->context.GetSeekTable();
				((FrameSeekTable*)val[0])->totalframes = pDec->context.GetHeader()->totalframes;
			default:
				break;
			}

		}
		return APERET_SUCCESS;
	}

	int32_t Ape_Run(void* pStateIn, uint8_t* in, int32_t inLen, uint8_t* out, int32_t* outLen)
	{
#if 1
		//check
		if (!pStateIn
			||!out
			||!outLen)
			return APERET_FAIL;
		ApeState* pState = (ApeState*)pStateIn;
		if (pState->mode == ApeModeInner::APE_MODE_DMUX)
		{
			ApeDemuxer* pDemux = (ApeDemuxer*)pState->codec;
			if (pState->isInited)
				return pDemux->Run(in, inLen, out, outLen);
			else
			{
				ApeInitParam initParam;
				ALGO_MEM_SET(&initParam, 0, sizeof(initParam));
				initParam.basePorting = pDemux->GetMM()->GetBasePorting();
				initParam.mode = ApeMode::APE_INIT_MODE_DMUX_USE_BUFFER;				
				initParam.bufferInit.buffer = in;
				i32 inLenUsed = inLen;
				initParam.bufferInit.size = &inLenUsed;
				int32_t ret = Ape_Init(pState, &initParam);
				inLen -= inLenUsed;
				in += inLenUsed;
				return pDemux->Run(in, inLen, out, outLen);
			}
		}
		else if (pState->mode == ApeModeInner::APE_MODE_DEC)
		{
			ApeDecoder* pDec = (ApeDecoder*)pState->codec;
			if (pState->isInited)
				return pDec->Run(in, inLen, out, outLen);
			else
			{
				ApeInitParam initParam;
				ALGO_MEM_SET(&initParam, 0, sizeof(initParam));
				initParam.mode = ApeMode::APE_INIT_MODE_DEC_USE_BUFFER;
				initParam.basePorting = pDec->GetMM()->GetBasePorting();
				initParam.bufferInit.buffer = in;
				i32 inLenUsed = inLen;
				initParam.bufferInit.size = &inLenUsed;
				int32_t ret = Ape_Init(pState, &initParam);
				inLen -= inLenUsed;
				in += inLenUsed;
				return pDec->Run(in, inLen, out, outLen);
			}
		}

#endif
	}

	int32_t Ape_DeInit(void* pStateIn)
	{
		if (!pStateIn)
			return APERET_FAIL;
		ApeState* pState = (ApeState*)pStateIn;
		if (pState->mode== ApeModeInner::APE_MODE_DEC){
			((ApeDecoder*)pState->codec)->DeInitInner();
		}
	}
}

#endif