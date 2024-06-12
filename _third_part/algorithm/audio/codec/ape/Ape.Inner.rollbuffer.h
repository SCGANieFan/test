#pragma once
#include"Ape.Inner.basic.h"

class ApeRollBuffer
{
public:
    ApeRollBuffer() {};
    ~ApeRollBuffer() {};
public:
    bool Init(uint8_t* buffer, uint32_t bufferLen, uint32_t bufferLenRemMin);
    void Increment(uint32_t bytes);
    INLINE uint8_t* GetBuffer(){
        return buffer + offset;
    }
private:
    u8* buffer;
    u32 bufferLen;
    u32 bufferLenRemMin;
    u32 offset;
    u32 bufferLenRem;
};
