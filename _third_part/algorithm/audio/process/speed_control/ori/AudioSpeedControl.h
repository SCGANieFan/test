#pragma once
#include<stdint.h>

#ifndef EXTERNC
#ifndef __cplusplus
#define EXTERNC
#else
#define EXTERNC extern "C"
#endif
#endif
#define DEFAULT_OVERLAP_MS 8

EXTERNC bool TDStretchSetChannels(void* hd, int32_t channels);
EXTERNC bool TDStretchSetSampleRate(void* hd, int32_t samplerate);
EXTERNC bool TDStretchSetSampleWidth(void* hd, int32_t sampleWidth);
EXTERNC bool TDStretchSetSeekMs(void* hd, int32_t seekMs);
EXTERNC bool TDStretchSetOverlapMs(void* hd, int32_t overlapMs);
EXTERNC bool TDStretchSetConstMs(void* hd, int32_t constMs);
EXTERNC bool TDStretchSetSpeed(void* hd, double speed);
EXTERNC bool TDStretchSetFinished(void* hd);
EXTERNC bool TDStretchSetEnd(void* hd, bool isEnd);

EXTERNC int32_t TDStretchGetDealInMinByte(void* hd);

EXTERNC int32_t TDStretchGetSize(int32_t sampleRate, int32_t sampleWidth, int32_t channels, int32_t overlapMs);
EXTERNC void TDStretchInit(void* hd);
EXTERNC int32_t TDStretchRun(void* hd, int8_t* in, int32_t inSize, int32_t* inUsed, int8_t* out, int32_t* outSize);
