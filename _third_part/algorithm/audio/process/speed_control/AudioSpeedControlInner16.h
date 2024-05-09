#pragma once
#include"Algo.AudioSamlpes.h"
typedef struct {
    AudioInfo info;
    i32 seekSamples;
    i32 overlapSamples;
    i32 constSamples;
    AudioSamples tmpBuf;
    AudioSamples iCache;
    f32 speed;
    b1 isBeginning;
    b1 isInited;
    char XX[10];
}TDStretch;


void* AudioSpeedControl_Malloc(i32 size);
void AudioSpeedControl_Free(void* ptr);
void AudioSpeedControl_MemCpy(u8* dst,u8* src,i32 size);
void AudioSpeedControl_MemSet(u8* dst,i8 val,i32 size);
void AudioSpeedControl_Memmove(u8* dst,u8* src,i32 size);
