#pragma once
#include"Algo.BasePorting.h"
#ifndef EXTERNC
#ifndef __cplusplus
#define EXTERNC
#else
#define EXTERNC extern "C"
#endif
#endif

#define AUDIO_RESAMPLE_RET_SUCCESS 0
#define AUDIO_RESAMPLE_RET_FAIL -1
#define AUDIO_RESAMPLE_RET_INPUT_ERROR -2
#define AUDIO_RESAMPLE_RET_OUT_BUFF_NOT_ENOUGH -3

typedef int32_t AudioResampleRet;

typedef struct{
    BasePorting* basePorting;
    int32_t iFs;
    int32_t oFs;
    int16_t width;
    int16_t channels;
}AudioResampleInitParam;

typedef enum {
    AUDIO_RESAMPLE_SET_CHOOSE_MAX,
}AudioResample_SetChhoose_e;

typedef enum {
    AUDIO_RESAMPLE_GET_CHOOSE_MAX,
}AudioResample_GetChhoose_e;

EXTERNC int32_t AudioResample_GetStateSize();
EXTERNC AudioResampleRet AudioResample_Init(void* pStateIn, AudioResampleInitParam *param);
EXTERNC AudioResampleRet AudioResample_Set(void* pStateIn, AudioResample_SetChhoose_e choose, void** val);
EXTERNC AudioResampleRet AudioResample_Get(void* pStateIn, AudioResample_GetChhoose_e choose, void** val);
EXTERNC AudioResampleRet AudioResample_Run(void* hd, uint8_t* in, int32_t *inSize, uint8_t* out, int32_t* outSize);
EXTERNC AudioResampleRet AudioResample_DeInit(void* pStateIn);


