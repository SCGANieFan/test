#pragma once


#include<stdint.h>


typedef struct
{
	maf_int32 fsHz;
	maf_int16 channels;
	maf_int16 frameSamples;
}MAF_InterfaceOpusDec;

typedef struct
{
	maf_int32 fsHz;
	maf_int16 channels;
	maf_int16 frameSamples;

	maf_int32 bitrate;



}MAF_InterfaceOpusEnc;