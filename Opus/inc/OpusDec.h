#ifndef __OPUS_DEC__H_
#define __OPUS_DEC__H_



typedef struct {
    int sampling_rate_hz;
    unsigned char channels;
}OpusDecInfo_t;

typedef struct {
    //void* dec;//OpusDecoder
    unsigned char* bitIn;
    unsigned int bitInLen;
    short* pcmOut;
    unsigned int* pcmOutLen;
}OpusDecIn_t;

int OpusDecoderInit(void* hd, OpusDecInfo_t* pOpusDecInfo);
int OpusDec(void* hd, OpusDecIn_t* pOpusDecIn);
int OpusDecoderSize(int channels);


#endif