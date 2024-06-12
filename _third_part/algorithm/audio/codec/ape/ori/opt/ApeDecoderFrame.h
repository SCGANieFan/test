#pragma once


#include"ApeCom.h"

EXTERNC APE_RET_t ApeDecodeFrameGetSize(void* contextIn);
EXTERNC APE_RET_t ApeDecodeFrameInit(void* decoder, void* context);//context shoudl not be free
EXTERNC APE_RET_t ApeDecodeFrameStart(void* hd, int32_t currentframe);
EXTERNC APE_RET_t ApeDecodeFrameRun(void* hd, uint8_t* in, int32_t inSize, int32_t* inUsed, uint8_t* out, int32_t* outSize);
#if 0
EXTERNC APE_RET_t ApeDecodeFrameEnd(void* hd);
#endif
EXTERNC APE_RET_t ApeDecodeFrameDeInit(void* hd);

