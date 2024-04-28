#pragma once


#include<stdint.h>

typedef struct
{
	int32_t fsHz;
	int16_t channels;
	int16_t frameSamples;
}MAF_Interface_PcmDemux;

typedef struct
{
	int32_t fsHz;
	int16_t channels;
	int16_t frameSamples;
}MAF_Interface_PcmMux;

