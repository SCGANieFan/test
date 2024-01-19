#ifndef __SILK_DEC__H_
#define __SILK_DEC__H_




typedef struct {
    unsigned int sample_rate;
    float loss_prob;
}SilkDecInfo_t;

typedef struct {
    unsigned char* bitIn;
    unsigned int bitInLen;
    short* pcmOut;
    unsigned int* pcmOutLen;

}SilkDecIn_t;

int SilkDecoderSize();
int SilkDecoderControlSize();
int SilkDecoderCreate(void* hd, void* encControl, SilkDecInfo_t* pInfo);
int SilkDec(void* hd, void* encControl, SilkDecIn_t* pIn);



#endif