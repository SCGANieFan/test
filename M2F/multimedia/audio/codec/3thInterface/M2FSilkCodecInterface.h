#ifndef __SilkCodecTestInterface__H_
#define __SilkCodecTestInterface__H_

#include"../src/M2FAudioCodec.h"



typedef struct {
    unsigned char complexity;
    unsigned int sample_rate;
    unsigned int max_internal_fs_Hz;
    unsigned int bit_rate;
    unsigned int packetSize_ms;
    unsigned int frameSizeReadFromFile_ms;
    unsigned char packetLoss_perc;
    unsigned char DTX_enabled;
    unsigned char INBandFEC_enabled;
}M2FSilkEncInfo_t;


typedef struct {
    unsigned int sample_rate;
    float loss_prob;
}M2FSilkDecInfo_t;


class M2FAudioCodecSilk: public M2FAudioCodec
{
public:
    M2FAudioCodecSilk();
    M2FAudioCodecSilk(void* p, m2f_uint32 size);
    ~M2FAudioCodecSilk();
protected:
    virtual int HdEncInit(void* param);
    virtual int Encode(M2FAudioCodecEncodeIn* pIn);
    virtual int HdDecInit(void* param);
    virtual int Decode(M2FAudioCodecDecodeIn* pIn);

private:
    void* hdEncControl;
    void* hdDecControl;
};








#endif