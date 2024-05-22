#pragma once
#include <stdbool.h>

#ifndef EXTERNC
#ifndef __cplusplus
#define EXTERNC
#else
#define EXTERNC extern "C"
#endif
#endif


#define DefineResampleCoefRegister(src,dst) extern void smf_resample_coef_register_##src##_##dst();smf_resample_coef_register_##src##_##dst();

enum RESAMPLE_STATUS_T {
	RESAMPLE_STATUS_OK = 0,
	RESAMPLE_STATUS_ERROR,
};

typedef struct {
	int iRate;
	char iCh;
	char iBits;
	int oRate;
	char oCh;
	char oBits;
	bool isChCopy;
}ResamplesInfo;

int resample_get_size(ResamplesInfo* info);
bool audio_resample_open(void* handle, ResamplesInfo*info);
enum RESAMPLE_STATUS_T audio_resample_run(void* hd, const void* in, int* inSize, void* out, int* outSize);
void audio_resample_close(void* hd);


