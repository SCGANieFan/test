#pragma once
#include"Ape.Inner.basic.h"
#include"Ape.Inner.bufferread.h"

typedef struct {
    u32 k;
    u32 ksum;
} ApeRice;

class ApeRangecoder
{
public:
    ApeRangecoder(){};
    ~ApeRangecoder(){};
public:
    void Init(u32 buffer){
        _buffer=buffer;
        _low=_buffer>>1;
        _range=((u32)1)<<7;
    }
    uint32_t range_dec_normalize(BufferRead* bufferRead);
    int32_t range_decode_bits(int n, BufferRead* bufferRead);
    bool range_get_symbol(const uint16_t counts[], const uint16_t counts_diff[], BufferRead* bufferRead, int32_t* symbol);
    bool EntropyDecode(ApeRice* rice, BufferRead* bufferRead, int32_t* out, uint16_t version);
    void normalize(BufferRead* bufferRead)
    {
        while (_range <= ((1u << (32 - 1)) >> 8))
        {
            bufferRead->Read(1);
            _range <<= 8;
            if (_range == 0)
                break;
        }
    }
private:
    u32 _low=0;           ///< low end of interval
    u32 _range=0;         ///< length of interval
    u32 _help = 0;          ///< bytes_to_follow resp. intermediate value
    u32 _buffer=0;    ///< buffer for input/output
};
