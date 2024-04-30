#pragma once
#include"musicPlcCom.h"
EXTERNC int32_t MusicPlc16bGetStateSize(int32_t overlapMs, MusicPlcSampleParam* sampleParam);
EXTERNC int32_t MusicPlc16bStateInit(void* pMusicPlcStateIn, int32_t overlapMs, int32_t decayTimeMs, MusicPlcSampleParam* sampleParam);
EXTERNC int32_t MusicPlc16b(void* pMusicPlcStateIn, uint8_t* in, int32_t inLen, int32_t* inUsed, uint8_t* out, int32_t* outLen, bool isLost);