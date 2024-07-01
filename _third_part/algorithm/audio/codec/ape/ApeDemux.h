#pragma once
#if 1
#include"ApeCom.h"


typedef enum {
	APE_DEMUX_SET_SEEK_TABLE=0,
	APE_DEMUX_SET_MAX,
}ApeDemuxSet_e;

typedef enum {
	APE_DEMUX_GET_IS_RUN_FINISH =0,
	APE_DEMUX_GET_HEADE,
	APE_DEMUX_GET_AUDIO_INFO,
	APE_DEMUX_GET_SEEK_TABLE,
	APE_DEMUX_GET_START_INFO_FROM_POS,
	APE_DEMUX_GET_MAX,
}ApeDemuxGet_e;


EXTERNC int32_t ApeDemux_GetSize();
EXTERNC int32_t ApeDemux_Init(void* pMusicPlcStateIn);
EXTERNC int32_t ApeDemux_Set(void* pMusicPlcStateIn, ApeDemuxSet_e choose, void* val);
EXTERNC int32_t ApeDemux_Get(void* pMusicPlcStateIn, ApeDemuxGet_e choose, void* val);
EXTERNC int32_t ApeDemux_Run(void* pMusicPlcStateIn, uint8_t* in, int32_t inLen);
EXTERNC int32_t ApeDemux_DeInit(void* pMusicPlcStateIn);

#endif