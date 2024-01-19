#ifndef __SILK_ENC__H_
#define __SILK_ENC__H_




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
}SilkEncInfo_t;

typedef struct {
    //void* enc;//OpusEncoder
    short* pcmIn;
    unsigned int pcmInLen;
    unsigned char* bitOut;
    unsigned int* bitOutLen;
}SilkEncIn_t;

int SilkEncoderSize();
int SilkEncoderControlSize();
int SilkEncoderInit(void* hd, void* encControl, SilkEncInfo_t* pInfo);
int SilkEnc(void* hd, void* encControl, SilkEncIn_t* pIn);



#endif