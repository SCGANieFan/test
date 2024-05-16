#pragma once
#include"Algo.BasePorting.h"
#ifdef __cplusplus
#define EXTERNC	extern "C"
#else
#define EXTERNC
#endif

#define WAV_DEMUX_RET_SUCCESS 0
#define WAV_DEMUX_RET_FAIL -1

typedef struct
{
	AlgoBasePorting* basePorting;
	int16_t frameSamples;
}WavDemuxInitParam;


typedef enum {
	WAV_DEMUX_SET_CHOOSE_MAX,
}WavDemux_SetChhoose_e;

typedef enum {
	WAV_DEMUX_GET_CHOOSE_HAS_HEAD=0,
#if 0
	WAV_DEMUX_GET_CHOOSE_RATE,
	WAV_DEMUX_GET_CHOOSE_CH,
	WAV_DEMUX_GET_CHOOSE_WIDTH,
#endif
	WAV_DEMUX_GET_CHOOSE_BASIC_INFO,
	WAV_DEMUX_GET_CHOOSE_MAX,
}WavDemux_GetChhoose_e;

EXTERNC int32_t WavDemux_GetStateSize();
EXTERNC int32_t WavDemux_StateInit(void* pStateIn, WavDemuxInitParam* paramIns);
EXTERNC int32_t WavDemux_Set(void* pStateIn, WavDemux_SetChhoose_e choose, void** val);
EXTERNC int32_t WavDemux_Get(void* pStateIn, WavDemux_GetChhoose_e choose, void** val);
EXTERNC int32_t WavDemux_Run(void* pStateIn, uint8_t* in, int32_t inLen, uint8_t* out, int32_t* outLen);
EXTERNC int32_t WavDemux_StateDeInit(void* pStateIn);
