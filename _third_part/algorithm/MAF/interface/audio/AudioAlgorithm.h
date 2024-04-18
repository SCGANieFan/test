#pragma once

#include<stdint.h>
#include"MAFA_AudioFrame.h"

typedef void* (*Malloc_t)(int32_t size);
typedef void* (*Realloc_t)(void* block, int32_t size);
typedef void* (*Calloc_t)(int32_t count, int32_t size);
typedef void (*Free_t)(void* block);


typedef int32_t AudioAlgorithmRet;


#define AUDIO_ALGORITHM_RET_SUCCESS 0
#define AUDIO_ALGORITHM_RET_FAIL -1
#define AUDIO_ALGORITHM_RET_INPUT_ERROR -2


AudioAlgorithmRet AlgorithmSetMemory(void* hd, Malloc_t* malloc, Realloc_t* realloc, Calloc_t* calloc, Free_t* free);


AudioAlgorithmRet AudioAlgorithmGetHandleSize(const char* type, int32_t* size);
AudioAlgorithmRet AudioAlgorithmCreate(const char* type, void* hd);

AudioAlgorithmRet AudioAlgorithmDestory(void* hd);


AudioAlgorithmRet AlgorithmInit(void* hd, void* param);
AudioAlgorithmRet AlgorithmDeinit(void* hd);

AudioAlgorithmRet AudioAlgorithmRun(void* hd, MAFA_Frame* frameIn, MAFA_Frame* frameOut);


