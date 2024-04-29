#pragma once


#include<stdint.h>

typedef struct
{
	maf_int32 fsHz;
	maf_int16 channels;
	maf_int16 frameSamples;
}MAF_Interface_PcmDemux;

typedef struct
{
	maf_int32 fsHz;
	maf_int16 channels;
	maf_int16 frameSamples;
}MAF_Interface_PcmMux;

