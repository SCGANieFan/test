#pragma once
#include"Algo.BasePorting.h"
#ifdef __cplusplus
#define EXTERNC	extern "C"
#else
#define EXTERNC
#endif

#define MUSIC_PLC_RET_SUCCESS 0
#define MUSIC_PLC_RET_FAIL -1

typedef struct
{
	AlgoBasePorting* basePorting;
	int32_t fsHz;
	int16_t channels;
	int16_t width;
	int16_t frameSamples;
	int32_t overlapMs;
	int32_t decayTimeMs;
}MusicPlcInitParam;


typedef enum {
	MUSIC_PLC_SET_CHOOSE_MAX,
}MusicPlc_SetChhoose_e;

typedef enum {
	MUSIC_PLC_GET_CHOOSE_MAX,
}MusicPlc_GetChhoose_e;

EXTERNC int32_t MusicPlc_GetStateSize();
EXTERNC int32_t MusicPlc_StateInit(void* pMusicPlcStateIn, MusicPlcInitParam* sampleParam);
EXTERNC int32_t MusicPlc_Set(void* pMusicPlcStateIn, MusicPlc_SetChhoose_e choose, void** val);
EXTERNC int32_t MusicPlc_Get(void* pMusicPlcStateIn, MusicPlc_GetChhoose_e choose, void** val);
EXTERNC int32_t MusicPlc_Run(void* pMusicPlcStateIn, uint8_t* in, int32_t inLen, uint8_t* out, int32_t* outLen, bool isLost);
EXTERNC int32_t MusicPlc_StateDeInit(void* pMusicPlcStateIn);
