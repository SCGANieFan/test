#pragma once
#include<stdint.h>

#ifndef EXTERNC
#ifndef __cplusplus
#define EXTERNC
#else
#define EXTERNC extern "C"
#endif
#endif

#if 1

#define AUDIO_SPEED_CONTROL_Ret_SUCCESS 0
#define AUDIO_SPEED_CONTROL_Ret_FAIL -1
#define AUDIO_SPEED_CONTROL_Ret_INPUT_ERROR -2

typedef  int32_t AudioSpeedControlRet;

typedef struct{
    int32_t fsHz;
    int16_t width;
    int16_t channels;
    int16_t seekMs;
    int16_t overlapMs;
    int16_t constMs;
    float speed;
}AudioSpeedInitParam;

typedef enum{
    AUDIO_SPEED_CONTROL_SET_CHOOSE_SPEEDQ8=0,
    AUDIO_SPEED_CONTROL_SET_CHOOSE_MAX,
}AudioSpeedControl_SetChhoose_e;

EXTERNC int32_t AudioSpeedControl_GetStateSize16b();
EXTERNC AudioSpeedControlRet AudioSpeedControl_Init16b(void* pMusicPlcStateIn, AudioSpeedInitParam *param);
EXTERNC AudioSpeedControlRet AudioSpeedControl_Set(void* pMusicPlcStateIn, AudioSpeedControl_SetChhoose_e choose, void* val);
EXTERNC AudioSpeedControlRet AudioSpeedControl_Run16b(void* hd, int8_t* in, int32_t inSize, int8_t* out, int32_t* outSize);
EXTERNC AudioSpeedControlRet AudioSpeedControl_DeInit16b(void* pMusicPlcStateIn);

#endif
