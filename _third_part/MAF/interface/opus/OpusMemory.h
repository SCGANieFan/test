#pragma once
#include <stdint.h>
void* opus_malloc(int size);
void* opus_realloc(void* rmem, int newsize);
void* opus_calloc(int count, int size);
void opus_free(void* rmem);
