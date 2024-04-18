#pragma once


#include<stdint.h>

typedef struct
{
	int32_t fsHz;
	int16_t channels;
	int16_t frameSamples;
	int16_t decayTimeMs;
	int16_t overlapMs;
}AA_MusicPlcParam;

#define DECAY_TIME_MS 50
#define OVERLAP_MS 2
#define FRAME_LEN_MS 2