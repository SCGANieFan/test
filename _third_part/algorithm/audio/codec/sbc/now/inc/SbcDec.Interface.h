#pragma once

#include <stdint.h>

#ifndef EXTERNC
#ifndef __cplusplus
#define EXTERNC
#else
#define EXTERNC extern "C"
#endif
#endif


#define SBC_RET_SUCCESS 0
#define SBC_RET_FAIL -1

typedef enum {
	SBC_DEC_GET_VLC_SIZE = 0,
	SBC_DEC_GET_PCM_SIZE = 1,
	SBC_DEC_GET_E_MAX,
}SbcDecGet_e;


EXTERNC int32_t SbcDec_GetSize();
EXTERNC bool SbcDec_Init(void* handle);
EXTERNC int32_t SbcDec_Run(void* handle, void* input, int32_t size, void* out, int32_t outSize);
EXTERNC int32_t SbcDec_Get(void* hd, SbcDecGet_e choose, void* val);







