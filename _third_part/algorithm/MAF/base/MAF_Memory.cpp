#include<string.h>
#include<stdlib.h>
#include "MAF_Memory.h"

MAF_Memory::MAF_Memory()
{
    _malloc = (MAF_Memory::Malloc_t)malloc;
    _realloc = (MAF_Memory::Realloc_t)realloc;
    _calloc = (MAF_Memory::Calloc_t)calloc;
    _free = (MAF_Memory::Free_t)free;
}
MAF_Memory::~MAF_Memory()
{
}


void MAF_Memory::Init(void* malloc, void* realloc, void* calloc, void* free)
{
    _malloc = (MAF_Memory::Malloc_t)malloc;
    _realloc = (MAF_Memory::Realloc_t)realloc;
    _calloc = (MAF_Memory::Calloc_t)calloc;
    _free = (MAF_Memory::Free_t)free;
}

void* MAF_Memory::Malloc(int32_t size)
{
    if (_malloc)
        return _malloc(size);
    return NULL;
}
void* MAF_Memory::Realloc(void* block, int32_t size)
{
    if (_realloc)
        return _realloc(block, size);
    return NULL;
}
void* MAF_Memory::Calloc(int32_t count, int32_t size)
{
    if (_calloc)
        return _calloc(count, size);
    return NULL;
}
void MAF_Memory::Free(void* block)
{
    if (_free)
        return _free(block);
}
