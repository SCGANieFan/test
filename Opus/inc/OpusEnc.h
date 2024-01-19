#ifndef __OPUS_ENC__H_
#define __OPUS_ENC__H_


typedef struct {
    int sampling_rate_hz;
    unsigned char channels;
    unsigned int application;
    unsigned int bitrate_bps;
    int bandwidth;
    unsigned char use_vbr;
    unsigned char cvbr;
    unsigned char complexity;
    unsigned char use_inbandfec;
    int forcechannels;
    unsigned char use_dtx;
    //unsigned char packet_loss_perc;
    unsigned char lsb_depth;
    unsigned int variable_duration;
    unsigned int frame_duration_0p1ms;
}OpusEncInfo_t;

typedef struct {
    //void* enc;//OpusEncoder
    short* pcmIn;
    unsigned int pcmInLen;
    unsigned char* bitOut;
    unsigned int* bitOutLen;
}OpusEncIn_t;

int OpusEncoderSize(int channels);
int OpusEncoderInit(void* hd, OpusEncInfo_t* pEncInfo);
int OpusEnc(void* hd, OpusEncIn_t* pOpusEncIn);



#endif