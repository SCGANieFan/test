#pragma once

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdint.h>
#include<math.h>
#include <cassert>


#include"Algo.AudioSamlpes.h"

#ifndef EXTERNC
#ifndef __cplusplus
#define EXTERNC
#else
#define EXTERNC extern "C"
#endif
#endif

#define AUTOSEQ_TEMPO_LOW   0.5     // auto setting low tempo range (-50%)
#define AUTOSEQ_TEMPO_TOP   2.0     // auto setting top tempo range (+100%)

// sequence-ms setting values at above low & top tempo
#define AUTOSEQ_AT_MIN      90.0
#define AUTOSEQ_AT_MAX      40.0
#define AUTOSEQ_K           ((AUTOSEQ_AT_MAX - AUTOSEQ_AT_MIN) / (AUTOSEQ_TEMPO_TOP - AUTOSEQ_TEMPO_LOW))
#define AUTOSEQ_C           (AUTOSEQ_AT_MIN - (AUTOSEQ_K) * (AUTOSEQ_TEMPO_LOW))

// seek-window-ms setting values at above low & top tempoq
#define AUTOSEEK_AT_MIN     20.0
#define AUTOSEEK_AT_MAX     15.0
#define AUTOSEEK_K          ((AUTOSEEK_AT_MAX - AUTOSEEK_AT_MIN) / (AUTOSEQ_TEMPO_TOP - AUTOSEQ_TEMPO_LOW))
#define AUTOSEEK_C          (AUTOSEEK_AT_MIN - (AUTOSEEK_K) * (AUTOSEQ_TEMPO_LOW))

#define FLT_DECIMAL_DIG  9                       // # of decimal digits of rounding precision
#define FLT_DIG          6                       // # of decimal digits of precision
#define FLT_EPSILON      1.192092896e-07F        // smallest such that 1.0+FLT_EPSILON != 1.0
#define FLT_HAS_SUBNORM  1                       // type does support subnormal numbers
#define FLT_GUARD        0
#define FLT_MANT_DIG     24                      // # of bits in mantissa
#define FLT_MAX          3.402823466e+38F        // max value
#define FLT_MAX_10_EXP   38                      // max decimal exponent
#define FLT_MAX_EXP      128                     // max binary exponent
#define FLT_MIN          1.175494351e-38F        // min normalized positive value
#define FLT_MIN_10_EXP   (-37)                   // min decimal exponent
#define FLT_MIN_EXP      (-125)                  // min binary exponent
#define FLT_NORMALIZE    0
#define FLT_RADIX        2                       // exponent radix
#define FLT_TRUE_MIN     1.401298

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)>(b)?(b):(a))
#define CHECK_LIMITS(x, mi, ma) (((x) < (mi)) ? (mi) : (((x) > (ma)) ? (ma) : (x)))


#define ALIGEN8(x) (((x+7)>>3)<<3)
#define MULQ15(x1,x2) ((int16_t)((((int32_t)x1)*x2)>>15))

typedef struct {
    int32_t channels;
    int32_t sampleRate;
    int32_t sampleWidth;

    AudioInfo info;

    int32_t seekMs;
    int32_t overlapMs;
    int32_t constMs;

    int32_t seekSamples;
    int32_t overlapSamples;
    int32_t constSamples;
#if 0
    int8_t* bufferTemplate;
#else
    AudioSamples bufferTemplate;
#endif
    AudioSamples iCache;

    double speed;
    bool isBeginning;
    bool isEnd;
}TDStretch;



int32_t SeekBestLag(int8_t* in, int8_t* templateBuff, int32_t sampleWidth, int32_t seekSamples, int32_t accorelationSamples, int32_t channels);
int32_t overlap(int8_t* out, const int8_t* in, int8_t* bufferTemplate, int32_t overlapLen, int32_t channels, int32_t sampleWidth);


#include"Algo.Type.h"
#include"Algo.AudioSamlpes.h"
#include"Algo.AS_Calculator.h"
void* AudioSpeedControl_Malloc(i32 size);
void AudioSpeedControl_Free(void* ptr);

void AudioSpeedControl_MemCpy(u8* dst,u8* src,i32 size);
void AudioSpeedControl_MemSet(u8* dst,i8 val,i32 size);
void AudioSpeedControl_Memmove(u8* dst,u8* src,i32 size);



