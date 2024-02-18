#pragma once
#include"plcCommon.h"
#include<stdint.h>

#define MUSIC_PLC_RET_SUCCESS 0
#define MUSIC_PLC_RET_FAIL -1

EXTERNC int32_t MusicPlcGetStateSize(int32_t overlapMs, int32_t frameInSamples, int32_t fsHz, int16_t channels, int16_t sampleWidth);
EXTERNC int32_t MusicPlcStateInit(void* pMusicPlcStateIn, int32_t overlapMs, int32_t frameInSamples, int32_t decayTimeMs, int32_t fsHz, int16_t channels, int16_t sampleWidth);
EXTERNC int32_t MusicPlc(void* pMusicPlcStateIn, int8_t* in, int32_t inLen, int32_t* inUsed, int8_t* out, int32_t* outLen, bool isLost);
