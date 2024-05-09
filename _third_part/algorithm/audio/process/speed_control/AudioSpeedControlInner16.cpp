#include"Algo.Printer.h"
#include"Algo.Memory.h"
#include"AudioSpeedControlInner16.h"

void* AudioSpeedControl_Malloc(i32 size) {
#if 1
    static i32 sizeTotal = 0;
    sizeTotal += size;
    void* ptr = ALGO_MALLOC(size);
    ALGO_PRINT("malloc, ptr:%x, size:%d, sizeTotal:%d,", (u32)ptr, size, sizeTotal);
    return ptr;
#else
    return ALGO_MALLOC(size);
#endif
}

void AudioSpeedControl_Free(void* ptr) {
#if 1
    ALGO_PRINT("free, ptr:%x", (u32)ptr);
    return ALGO_FREE(ptr);
#else
    return ALGO_FREE(ptr);
#endif
}

void AudioSpeedControl_MemCpy(u8* dst, u8* src, i32 size)
{
    ALGO_MEM_CPY(dst, src, size);
}
void AudioSpeedControl_MemSet(u8* dst, i8 val, i32 size)
{
    ALGO_MEM_SET(dst, val, size);
}

void AudioSpeedControl_Memmove(u8* dst, u8* src, i32 size)
{
    ALGO_MEM_MOVE(dst, src, size);
}

