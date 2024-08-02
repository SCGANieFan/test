#include "MTF.Memory.h"

MTF_Memory _memory;

MTF_Memory::MTF_Memory()
{
}
MTF_Memory::~MTF_Memory()
{
}

void MTF_Memory::Init(void* malloc, void* realloc, void* calloc, void* free)
{
    
    _memory._malloc = (MTF_Memory::Malloc_t)malloc;
    _memory._realloc = (MTF_Memory::Realloc_t)realloc;
    _memory._calloc = (MTF_Memory::Calloc_t)calloc;
    _memory._free = (MTF_Memory::Free_t)free;
}

void* MTF_Memory::Malloc(int32_t size)
{
    if (_memory._malloc)
        return _memory._malloc(size);
    return NULL;
}
void* MTF_Memory::Realloc(void* block, int32_t size)
{
    if (_memory._realloc)
        return _memory._realloc(block, size);
    return NULL;
}
void* MTF_Memory::Calloc(int32_t count, int32_t size)
{
    if (_memory._calloc)
        return _memory._calloc(count, size);
    return NULL;
}
void MTF_Memory::Free(void* block)
{
    if (_memory._free)
        return _memory._free(block);
}
