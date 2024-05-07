#pragma once
#include"musicPlcCom.h"
EXTERNC int32_t MusicPlc32bGetStateSize(int32_t overlapMs, MusicPlcSampleParam* sampleParam);
EXTERNC int32_t MusicPlc32bStateInit(void* pMusicPlcStateIn, int32_t overlapMs, int32_t decayTimeMs, MusicPlcSampleParam* sampleParam);
EXTERNC int32_t MusicPlc32b(void* pMusicPlcStateIn, uint8_t* in, int32_t inLen, int32_t* inUsed, uint8_t* out, int32_t* outLen, bool isLost);
EXTERNC int32_t MusicPlc32bStateDeInit(void* pMusicPlcStateIn);
