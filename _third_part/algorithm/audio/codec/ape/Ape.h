#pragma once
#if 0
#include"Algo.BasePorting.h"
#ifdef __cplusplus
#define EXTERNC	extern "C"
#else
#define EXTERNC
#endif

#define APERET_SUCCESS 0
#define APERET_FAIL -1

typedef struct
{
	AlgoBasePorting* basePorting;
	int32_t fsHz;
	int16_t channels;
	int16_t width;
	int16_t frameSamples;
	int32_t overlapMs;
	int32_t decayTimeMs;
}ApeInitParam;


typedef enum {
	APESET_CHOOSE_MAX,
}ApeSetChhoose_e;

typedef enum {
	APEGET_CHOOSE_MAX,
}ApeGetChhoose_e;

EXTERNC int32_t Ape_GetStateSize();
EXTERNC int32_t Ape_StateInit(void* pMusicPlcStateIn, ApeInitParam* sampleParam);
EXTERNC int32_t Ape_Set(void* pMusicPlcStateIn, ApeSetChhoose_e choose, void** val);
EXTERNC int32_t Ape_Get(void* pMusicPlcStateIn, ApeGetChhoose_e choose, void** val);
EXTERNC int32_t Ape_Run(void* pMusicPlcStateIn, uint8_t* in, int32_t inLen, uint8_t* out, int32_t* outLen, bool isLost);
EXTERNC int32_t Ape_StateDeInit(void* pMusicPlcStateIn);
#endif