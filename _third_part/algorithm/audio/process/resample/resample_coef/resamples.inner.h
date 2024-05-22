#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifndef EXTERNC
#ifndef __cplusplus
#define EXTERNC
#else
#define EXTERNC extern "C"
#endif
#endif

#define SAMPLE_NUM                      128
#define MAX(a,b)						(a)>(b)?(a):(b)
#define MIN(a,b)						(a)>(b)?(b):(a)


typedef struct {
	int32_t rate;
	int8_t ch;
	int8_t bits;
}BasicInfo;


#if 1
typedef struct {
    uint16_t coefNumPerGroup;
    uint16_t upsampleFactor;
    uint16_t downsampleFactor;
    uint16_t res;
    int16_t coefGroups[0];
}ResampleCoef;



typedef struct {
	BasicInfo iInfo;
	BasicInfo oInfo;
	uint16_t inStartSamples;
	uint16_t coefChoose;
	bool isChCopy;
	const ResampleCoef* coef;
	uint8_t inSamplesCache[8][4 * (32 + SAMPLE_NUM)]; //channels* width* (coefNumPerGroup + samples)
}ResampleState;


#define DefineResampleRegister(src,dst) void smf_resample_coef_register_##src##_##dst() {resample_coef_register(TABLE_NAME(src,dst));}

void resample_coef_register(const void*);
const ResampleCoef* resample_get_coef(int rate0, int target0);
enum RESAMPLE_STATUS_T audio_resample_ex_run(void* hd, const void* in, int32_t* inSize, void* out, int32_t* outSize);

#endif




