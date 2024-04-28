#pragma once

#include<stdint.h>

typedef void* (*Malloc_t)(int32_t size);
typedef void* (*Realloc_t)(void* block, int32_t size);
typedef void* (*Calloc_t)(int32_t count, int32_t size);
typedef void (*Free_t)(void* block);


typedef int32_t AudioAlgorithmRet;


#define AUDIO_ALGORITHM_RET_SUCCESS 0
#define AUDIO_ALGORITHM_RET_FAIL -1
#define AUDIO_ALGORITHM_RET_INPUT_ERROR -2

#if 0
typedef enum {
	MAFA_FRAME_IS_EMPTY = 1 << 0,
}AA_DataFlag;
#endif


typedef struct {
	uint8_t* buff;
	int32_t off;
	int32_t size;
	int32_t max;
	uint32_t flags;
}AA_Data;


AudioAlgorithmRet AudioAlgorithm_GetHandleSize(const char* type, int32_t* size);
AudioAlgorithmRet AudioAlgorithm_Create(const char* type, void* hd);
AudioAlgorithmRet AudioAlgorithm_Destory(void* hd);
AudioAlgorithmRet AudioAlgorithm_Init(void* hd, void* param);
AudioAlgorithmRet AudioAlgorithm_Deinit(void* hd);
AudioAlgorithmRet AudioAlgorithm_SetMemory(void* hd, Malloc_t* malloc, Realloc_t* realloc, Calloc_t* calloc, Free_t* free);
AudioAlgorithmRet AudioAlgorithm_Run(void* hd, AA_Data* dataIn, AA_Data* dataOut);


