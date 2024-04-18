#pragma once
#include<stdint.h>

class MAF_Memory
{
protected:
    typedef void* (*Malloc_t)(int32_t size);
    typedef void* (*Realloc_t)(void* block, int32_t size);
    typedef void* (*Calloc_t)(int32_t count, int32_t size);
    typedef void (*Free_t)(void* block);
public:
	MAF_Memory();
	~MAF_Memory();

public:
    void Init(void* malloc, void* realloc, void* calloc, void* free);

    void* Malloc(int32_t size);
    void* Realloc(void* block, int32_t size);
    void* Calloc(int32_t count, int32_t size);
    void Free(void* block);

private:
    Malloc_t _malloc;
    Realloc_t _realloc;
    Calloc_t _calloc;
    Free_t _free;


};

