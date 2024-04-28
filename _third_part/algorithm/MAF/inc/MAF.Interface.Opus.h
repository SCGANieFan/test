#pragma once


#include<stdint.h>


typedef struct
{
	int32_t fsHz;
	int16_t channels;
	int16_t frameSamples;
}MAF_InterfaceOpusDec;

typedef struct
{
	int32_t fsHz;
	int16_t channels;
	int16_t frameSamples;

	int32_t bitrate;



}MAF_InterfaceOpusEnc;