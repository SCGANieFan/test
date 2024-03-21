#ifndef MUSIC_PLC_H
#define MUSIC_PLC_H

#include<stdint.h>

#ifdef __cplusplus
#define EXTERNC	extern "C"
#else
#define EXTERNC
#endif

#define MUSIC_PLC_RET_SUCCESS 0
#define MUSIC_PLC_RET_FAIL -1


enum MusicPlcSampleAlignMode_e {
	MusicPlcSampleHighByteAlign = 0,
	MusicPlcSampleLowByteAlign,
};

typedef struct {
	int16_t validWidthPerSample;
	int16_t storedWidthPerSample;
	MusicPlcSampleAlignMode_e alignMode;
}MusicPlcSampleStoreFormat;

typedef struct
{
	int32_t fsHz;
	MusicPlcSampleStoreFormat sampleStoreFormat;
	int16_t channels;
}MusicPlcSampleParam;

EXTERNC int32_t MusicPlcGetStateSize(int32_t overlapMs, int32_t frameSamples, MusicPlcSampleParam* sampleParam);
EXTERNC int32_t MusicPlcStateInit(void* pMusicPlcStateIn, int32_t overlapMs, int32_t frameSamples, int32_t decayTimeMs, MusicPlcSampleParam* sampleParam);
EXTERNC int32_t MusicPlc(void* pMusicPlcStateIn, int8_t* in, int32_t inLen, int32_t* inUsed, int8_t* out, int32_t* outLen, bool isLost);

#endif