#pragma once

#include"musicPlcCom.h"


EXTERNC int32_t MusicPlc24bLGetStateSize(int32_t overlapMs, MusicPlcSampleParam* sampleParam);
EXTERNC int32_t MusicPlc24bLStateInit(void* pMusicPlcStateIn, int32_t overlapMs, int32_t decayTimeMs, MusicPlcSampleParam* sampleParam);
EXTERNC int32_t MusicPlc24bL(void* pMusicPlcStateIn, uint8_t* in, int32_t inLen, int32_t* inUsed, uint8_t* out, int32_t* outLen, bool isLost);

