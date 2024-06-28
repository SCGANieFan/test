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
			|| !sampleParam->basePorting)
			return APERET_FAIL;
		ApeDemuxState* pState = (ApeDemuxState*)pStateIn;
		ALGO_MEM_SET(pState, 0, ApeDemux_GetSize());
		ApeDemuxer* pDemux = &pState->demux;
		i32 ret;
#if 0
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
#else
		pState->isInited = true;
		return pDemux->InitWithNoBuffer(sampleParam->basePorting);
#endif
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
#if 1
		case APE_DEMUX_SET_SEEK_TABLE:
		{
			void** pVal = (void**)val;
			pDemux->seekTable.UpdataSeekTable((i32*)pVal[0], (i32)(pVal[1]));
		}
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
		b1 ret;
		switch (choose)
		{
		case APE_DEMUX_GET_HEADE:
			*(u32*)val = (u32)pDemux->_isDemuxHeadSuccess;
			break;
		case APE_DEMUX_GET_START_POS_FROM_FRAME:
			ret = pDemux->seekTable.GetStartPosFromFrame(*(u32*)val, (u32*)val);
			if (!ret)return APERET_FAIL;
			break;
		case APE_DEMUX_GET_START_SKIP:
			ret = pDemux->seekTable.GetStartSkip(*(u32*)val, (u32*)val);
			if (!ret)return APERET_FAIL;
			break;
		case APE_DEMUX_GET_FRAME_INFO_FROM_POS:
			//*(u32*)val = pDemux->context.GetStartPosFromFrame(*(u32*)val);
			break;
#if 0
		case APE_DEMUX_GET_RATE:
			*(u32*)val = pDemux->context.GetHeader()->samplerate;
			break;
		case APE_DEMUX_GET_CH:
			*(u32*)val = pDemux->context.GetHeader()->channels;
			break;
		case APE_DEMUX_GET_WIDTH:
			*(u32*)val = pDemux->context.GetHeader()->bps>>3;			
			break;
#else
		case APE_DEMUX_GET_AUDIO_INFO:
		{
			void** pVal = (void**)val;
			*(u32*)pVal[0] = pDemux->context.GetHeader()->samplerate;
			*(u32*)pVal[1] = pDemux->context.GetHeader()->channels;
			*(u32*)pVal[2] = pDemux->context.GetHeader()->bps >> 3; 
			break;
		}
#endif

#if 0
		case APE_DEMUX_GET_SEEK_TABLE_BYTES:
			*(u32*)val = pDemux->context._context.descriptor.seektablelength;
			break;
		case APE_DEMUX_GET_SEEK_TABLE_POS:
			*(u32*)val = pDemux->context._context.seektablePos;
			break;
#else
		case APE_DEMUX_GET_SEEK_TABLE:
			*(u32*)((void**)val)[0] = pDemux->context.seektablePos;
			*(u32*)((void**)val)[1] = pDemux->context._context.descriptor.seektablelength;
			break;
#endif
		case APE_DEMUX_GET_START_INFO_FROM_POS:
#if 0
			*(u32*)((void**)val)[0] = pDemux->context.seektablePos;
			*(u32*)((void**)val)[1] = pDemux->context._context.descriptor.seektablelength;
			ret = pDemux->seekTable.GetStartPosFromFrame(*(u32*)val, (u32*)val);
#endif
			{
				void** pVal = (void**)val;
				if (!pDemux->seekTable.GetStartInfoFromPos((i32*)pVal[0], (i32*)pVal[1], (i32*)pVal[2]))
					return APERET_FAIL;
				break;
			}
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