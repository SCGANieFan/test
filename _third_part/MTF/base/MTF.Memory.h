#pragma once
#include"MTF.Type.h"
class MTF_Memory
{
public:
	MTF_Memory();
	~MTF_Memory();
protected:
    typedef void* (*Malloc_t)(int32_t size);
    typedef void* (*Realloc_t)(void* block, int32_t size);
    typedef void* (*Calloc_t)(int32_t count, int32_t size);
    typedef void (*Free_t)(void* block);
public:
    static void Init(void* malloc, void* realloc, void* calloc, void* free);
    static void* Malloc(int32_t size);
    static void* Realloc(void* block, int32_t size);
    static void* Calloc(int32_t count, int32_t size);
    static void Free(void* block);
private:
    Malloc_t _malloc;
    Realloc_t _realloc;
    Calloc_t _calloc;
    Free_t _free;
private:
};



#if 1

#define MTF_MALLOC(size)            MTF_Memory::Malloc(size)
#define MTF_REALLOC(block,size)     MTF_Memory::Realloc(block,size)
#define MTF_CALLOC(count,size)      MTF_Memory::Calloc(count,size)
#define MTF_FREE(block)             MTF_Memory::Free(block)
#else
#include<stdlib.h>
#define MTF_MALLOC(size)            malloc(size)
#define MTF_REALLOC(block,size)     realloc(block,size)
#define MTF_CALLOC(count,size)      calloc(count,size)
#define MTF_FREE(block)             free(block)

#endif
