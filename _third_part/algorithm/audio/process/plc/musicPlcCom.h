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
	int32_t fsHz;
	int16_t channels;
	int16_t frameSamples;
}MusicPlcSampleParam;

