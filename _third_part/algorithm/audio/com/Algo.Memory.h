#pragma once

#if 1

#include<stdlib.h>
#include<string>
#include"Algo.Type.h"

#define ALGO_MALLOC(size)           malloc((i32)size)
#define ALGO_REALLOC(block,size)    realloc((void*)block,(i32)size)
#define ALGO_CALLOC(count,size)     calloc((void*)count,(i32)size)
#define ALGO_FREE(block)            free((void*)block)
#define ALGO_MEM_CPY(dst,src,size)	memcpy((void*)dst,(void*)src,(i32)size)
#define ALGO_MEM_SET(dst,val,size)	memset((void*)dst,(i8)val,(i32)size)
#define ALGO_MEM_MOVE(dst,src,size) memmove((void*)dst,(void*)src,(i32)size)

#else

#include"MAF.Memory.h"
#define ALGO_MALLOC(size)           malloc((i32)size)
#define ALGO_REALLOC(block,size)    realloc((void*)block,(i32)size)
#define ALGO_CALLOC(count,size)     calloc((void*)count,(i32)size)
#define ALGO_FREE(block)            free((void*)block)
#define ALGO_MEM_CPY(dst,src,size)	memcpy((void*)dst,(void*)src,(i32)size)
#define ALGO_MEM_SET(dst,val,size)	memset((void*)dst,(i8)val,(i32)size)
#define ALGO_MEM_MOVE(dst,src,size) memmove((void*)dst,(void*)src,(i32)size)
#endif

