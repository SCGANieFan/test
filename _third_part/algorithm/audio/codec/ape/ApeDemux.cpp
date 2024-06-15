#if 1
#include"Algo.AudioSamlpes.h"
#include"Ape.Inner.decoder.h"
#include"Ape.Inner.demuxer.h"
#include"ApeDemux.h"
using namespace Algo;
using namespace Audio;



typedef struct {
	ApeDemuxer demux;
	union {
		struct {
			b1 isInited;
		};
		u32 flags;
	};
}ApeDemuxState;


EXTERNC {
	int32_t ApeDemux_GetSize()
	{
		return sizeof(ApeDemuxState);
	}

	int32_t ApeDemux_Init(void* pStateIn, ApeDemuxInitParam_t* sampleParam)
	{
		//check
		if (!pStateIn
			|| !sampleParam
			|| !sampleParam->basePorting
			|| sampleParam->mode>= APE_DEMUX_INIT_MODE_MAX)
			return APERET_FAIL;
		ApeDemuxState* pState = (ApeDemuxState*)pStateIn;
		ALGO_MEM_SET(pState, 0, ApeDemux_GetSize());
		ApeDemuxer* pDemux = &pState->demux;
		i32 ret;
		switch (sampleParam->mode)
		{
		case ApeDemuxInitMode_e::APE_DEMUX_INIT_MODE_DMUX_USE_BUFFER:
			ret=pDemux->InitWithBuffer(sampleParam->basePorting, (u8*)sampleParam->bufferInit_t.buffer, (i32*)sampleParam->bufferInit_t.size);
			if(ret== APERET_SUCCESS)
				pState->isInited = true;
			return ret;
		case ApeDemuxInitMode_e::APE_DEMUX_INIT_MODE_DMUX:
			pState->isInited = false;
			return pDemux->InitWithNoBuffer(sampleParam->basePorting);
		}
	}

	int32_t ApeDemux_Set(void* pStateIn, ApeDemuxSet_e choose, void* val)
	{
		//check
		if (!pStateIn
			|| choose >= APE_DEMUX_SET_MAX)
			return APERET_FAIL;
		ApeDemuxState* pState = (ApeDemuxState*)pStateIn;
		if (pState->isInited == false)
			return APERET_FAIL;
		ApeDemuxer* pDemux = &pState->demux;
		switch (choose)
		{
#if 0
		case APEGET_CHOOSE_CONTEXT:
			val[0] = &pDemux->context;
		default:
			break;
#endif
		}
		return APERET_SUCCESS;
	}

	int32_t ApeDemux_Get(void* pStateIn, ApeDemuxGet_e choose, void* val)
	{
		//check
		if (!pStateIn
			|| choose >= APE_DEMUX_GET_MAX
			|| !val)
			return APERET_FAIL;
		
		ApeDemuxState* pState = (ApeDemuxState*)pStateIn;
		if (pState->isInited == false)
			return APERET_FAIL;

		ApeDemuxer* pDemux = &pState->demux;
		switch (choose)
		{
		case APE_DEMUX_GET_HEADE:
			*(void**)val = pDemux->context.GetContext();
			break;
		case APE_DEMUX_GET_START_POS_FROM_FRAME:
			*(u32*)val = pDemux->context.GetStartPosFromFrame(*(u32*)val);
			break;
		case APE_DEMUX_GET_FRAME_INFO_FROM_POS:
			//*(u32*)val = pDemux->context.GetStartPosFromFrame(*(u32*)val);
			break;
		case APE_DEMUX_GET_RATE:
			*(u32*)val = pDemux->context.GetHeader()->samplerate;
			break;
		case APE_DEMUX_GET_CH:
			*(u32*)val = pDemux->context.GetHeader()->channels;
			break;
		case APE_DEMUX_GET_WIDTH:
			*(u32*)val = pDemux->context.GetHeader()->bps>>3;			
			break;
		default:
			break;
		}
		return APERET_SUCCESS;
	}

	int32_t ApeDemux_Run(void* pStateIn, uint8_t* in, int32_t inLen)
	{
		//check
		if (!pStateIn)
			return APERET_FAIL;
		ApeDemuxState* pState = (ApeDemuxState*)pStateIn;

		ApeDemuxer* pDemux = &pState->demux;
		if (pState->isInited)
			return pDemux->Run(in, inLen);
		else
		{
			ApeDemuxInitParam_t initParam;
			ALGO_MEM_SET(&initParam, 0, sizeof(initParam));
			initParam.basePorting = pDemux->GetMM()->GetBasePorting();
			initParam.mode = ApeDemuxInitMode_e::APE_DEMUX_INIT_MODE_DMUX_USE_BUFFER;
			initParam.bufferInit_t.buffer = in;
			i32 inLenUsed = inLen;
			initParam.bufferInit_t.size = &inLenUsed;
			int32_t ret = ApeDemux_Init(pState, &initParam);
			inLen -= inLenUsed;
			in += inLenUsed;
			return pDemux->Run(in, inLen);
		}
	}

	int32_t ApeDemux_DeInit(void* pStateIn)
	{
		if (!pStateIn)
			return APERET_FAIL;
		ApeDemuxState* pState = (ApeDemuxState*)pStateIn;
		pState->demux.DeInit();
		return APERET_SUCCESS;
	}
}

#endif