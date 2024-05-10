#pragma once

#include<stdint.h>

typedef void* (*ALGO_Malloc_t)(int32_t size);
typedef void* (*ALGO_Realloc_t)(void* block, int32_t size);
typedef void* (*ALGO_Calloc_t)(int32_t count, int32_t size);
typedef void  (*ALGO_Free_t)(void* block);

typedef void (*ALGO_MemCpy_t)(uint8_t* dst, uint8_t* src, int32_t size);
typedef void (*ALGO_MemSet_t)(uint8_t* dst, int32_t val, int32_t size);
typedef void (*ALGO_MemMove_t)(uint8_t* dst, uint8_t* src, int32_t size);

typedef void (*ALGO_Printf_t)(const char* _Format, ...);

typedef struct {
	ALGO_Malloc_t Malloc;
	ALGO_Realloc_t Realloc;
	ALGO_Calloc_t Calloc;
	ALGO_Free_t Free;

	ALGO_MemCpy_t MemCpy;
	ALGO_MemSet_t MemSet;
	ALGO_MemMove_t MemMove;

	ALGO_Printf_t Printf;
}BasePorting;




