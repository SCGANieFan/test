#pragma once
#if 1
#include"ApeCom.h"

typedef enum {
	APE_DEMUX_INIT_MODE_DMUX_USE_BUFFER = 0,
	APE_DEMUX_INIT_MODE_DMUX,
	APE_DEMUX_INIT_MODE_MAX,
}ApeDemuxInitMode_e;

typedef struct
{
	AlgoBasePorting* basePorting;
	ApeDemuxInitMode_e mode;
	union{
		struct{
			void* buffer;
			int32_t* size;
		}bufferInit_t;
		struct {
			void* buffer;
			int32_t* size;
		}noParamInit_t;
	};
}ApeDemuxInitParam_t;


typedef enum {
	APE_DEMUX_SET_MAX,
}ApeDemuxSet_e;

typedef enum {
	APE_DEMUX_GET_HEADE=0,
	APE_DEMUX_GET_START_POS_FROM_FRAME,
	APE_DEMUX_GET_FRAME_INFO_FROM_POS,
	APE_DEMUX_GET_RATE,
	APE_DEMUX_GET_CH,
	APE_DEMUX_GET_WIDTH,
	APE_DEMUX_GET_MAX,
}ApeDemuxGet_e;



EXTERNC int32_t ApeDemux_GetSize();
EXTERNC int32_t ApeDemux_Init(void* pMusicPlcStateIn, ApeDemuxInitParam_t* sampleParam);
EXTERNC int32_t ApeDemux_Set(void* pMusicPlcStateIn, ApeDemuxSet_e choose, void* val);
EXTERNC int32_t ApeDemux_Get(void* pMusicPlcStateIn, ApeDemuxGet_e choose, void* val);
EXTERNC int32_t ApeDemux_Run(void* pMusicPlcStateIn, uint8_t* in, int32_t inLen);
EXTERNC int32_t ApeDemux_DeInit(void* pMusicPlcStateIn);

#endif