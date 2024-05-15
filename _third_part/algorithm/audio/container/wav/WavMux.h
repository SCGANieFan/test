#pragma once
#include"Algo.BasePorting.h"
#ifdef __cplusplus
#define EXTERNC	extern "C"
#else
#define EXTERNC
#endif

#define WAV_MUX_RET_SUCCESS 0
#define WAV_MUX_RET_FAIL -1

typedef struct
{
	AlgoBasePorting* basePorting;
	int32_t fsHz;
	int16_t channels;
	int16_t width;
	int16_t frameSamples;
	int32_t overlapMs;
	int32_t decayTimeMs;
}WavMuxInitParam;


typedef enum {
	WAV_MUX_SET_CHOOSE_MAX,
}WavMux_SetChhoose_e;

typedef enum {
	WAV_MUX_GET_CHOOSE_MAX,
}WavMux_GetChhoose_e;

EXTERNC int32_t WavMux_GetStateSize();
EXTERNC int32_t WavMux_StateInit(void* pStateIn, WavMuxInitParam* paramIns);
EXTERNC int32_t WavMux_Set(void* pStateIn, WavMux_SetChhoose_e choose, void** val);
EXTERNC int32_t WavMux_Get(void* pStateIn, WavMux_GetChhoose_e choose, void** val);
EXTERNC int32_t WavMux_Run(void* pStateIn, uint8_t* in, int32_t inLen, uint8_t* out, int32_t* outLen, bool isLost);
EXTERNC int32_t WavMux_StateDeInit(void* pStateIn);
