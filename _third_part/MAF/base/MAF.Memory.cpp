#include "MAF.Memory.h"

MAF_Memory::MAF_Memory()
{
#if 0
    _malloc = (MAF_Memory::Malloc_t)malloc;
    _realloc = (MAF_Memory::Realloc_t)realloc;
    _calloc = (MAF_Memory::Calloc_t)calloc;
    _free = (MAF_Memory::Free_t)free;
#endif
}
MAF_Memory::~MAF_Memory()
{
}


maf_void* MAF_Memory::GetMalloc()
{
    return (maf_void*)_malloc;
}
maf_void* MAF_Memory::GetRealloc()
{
    return (maf_void*)_realloc;
}
maf_void* MAF_Memory::GetCalloc()
{
    return (maf_void*)_calloc;
}
maf_void* MAF_Memory::GetFree()
{
    return (maf_void*)_free;
}


maf_void MAF_Memory::Init(maf_void* malloc, maf_void* realloc, maf_void* calloc, maf_void* free)
{
    _malloc = (MAF_Memory::Malloc_t)malloc;
    _realloc = (MAF_Memory::Realloc_t)realloc;
    _calloc = (MAF_Memory::Calloc_t)calloc;
    _free = (MAF_Memory::Free_t)free;
}

maf_void* MAF_Memory::Malloc(maf_int32 size)
{
    if (_malloc)
        return _malloc(size);
    return NULL;
}
maf_void* MAF_Memory::Realloc(maf_void* block, maf_int32 size)
{
    if (_realloc)
        return _realloc(block, size);
    return NULL;
}
maf_void* MAF_Memory::Calloc(maf_int32 count, maf_int32 size)
{
    if (_calloc)
        return _calloc(count, size);
    return NULL;
}
maf_void MAF_Memory::Free(maf_void* block)
{
    if (_free)
        return _free(block);
}
