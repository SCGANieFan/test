#pragma once


#include<stdint.h>

typedef struct
{
	maf_int32 fsHz;
	maf_int16 channels;
	maf_int16 frameSamples;
	maf_int16 decayTimeMs;
	maf_int16 overlapMs;
}AA_MusicPlcParam;

#define DECAY_TIME_MS 50
#define OVERLAP_MS 2
#define FRAME_LEN_MS 2