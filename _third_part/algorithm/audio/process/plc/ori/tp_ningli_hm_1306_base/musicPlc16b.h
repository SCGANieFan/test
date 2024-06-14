#pragma once

#include<stdint.h>

#ifdef __cplusplus
#define EXTERNC	extern "C"
#else
#define EXTERNC
#endif

#define MUSIC_PLC_RET_SUCCESS 0
#define MUSIC_PLC_RET_FAIL -1


typedef struct
{
	int32_t fsHzMax;
	int16_t channels;
	int16_t frameSamples;
}MusicPlcSampleParam;


EXTERNC int32_t MusicPlc16bGetStateSize(int32_t overlapSamples, int32_t decayTimeSamples, MusicPlcSampleParam* sampleParam);
EXTERNC int32_t MusicPlc16bStateInit(void* pMusicPlcStateIn, int32_t overlapSamples, int32_t decayTimeSamples, MusicPlcSampleParam* sampleParam);
EXTERNC int32_t MusicPlc16b(void* pMusicPlcStateIn, uint8_t* in, int32_t inLen, int32_t* inUsed, uint8_t* out, int32_t* outLen, bool isLost);

