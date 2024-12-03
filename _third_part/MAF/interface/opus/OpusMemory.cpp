#include <stdlib.h>
void* opus_malloc(int size)
{
    return malloc(size);
}
void* opus_realloc(void* rmem, int newsize)
{
    return realloc(rmem, newsize);
}
void* opus_calloc(int count, int size)
{
    return calloc(count, size);
}
void opus_free(void* rmem)
{
    free(rmem);
}