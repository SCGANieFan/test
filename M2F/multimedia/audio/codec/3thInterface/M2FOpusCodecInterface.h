#ifndef __OpusCodecTestInterface__H_
#define __OpusCodecTestInterface__H_

#include"../src/M2FAudioCodec.h"


#define M2F_OPUS_AUTO                           -1000 /**<Auto/default setting @hideinitializer*/

#define M2F_OPUS_FRAMESIZE_2_5_MS                5001 /**< Use 2.5 ms frames */
#define M2F_OPUS_FRAMESIZE_5_MS                  5002 /**< Use 5 ms frames */
#define M2F_OPUS_FRAMESIZE_10_MS                 5003 /**< Use 10 ms frames */
#define M2F_OPUS_FRAMESIZE_20_MS                 5004 /**< Use 20 ms frames */
#define M2F_OPUS_FRAMESIZE_40_MS                 5005 /**< Use 40 ms frames */
#define M2F_OPUS_FRAMESIZE_60_MS                 5006 /**< Use 60 ms frames */
#define M2F_OPUS_FRAMESIZE_80_MS                 5007 /**< Use 80 ms frames */
#define M2F_OPUS_FRAMESIZE_100_MS                5008 /**< Use 100 ms frames */
#define M2F_OPUS_FRAMESIZE_120_MS                5009 /**< Use 120 ms frames */


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
}M2FOpusEncInfo_t;

typedef struct {
    int sampling_rate_hz;
    unsigned char channels;
}M2FOpusDecInfo_t;


class M2FAudioCodecOpus: public M2FAudioCodec
{
public:
    M2FAudioCodecOpus();
    M2FAudioCodecOpus(void* p, m2f_uint32 size);
    ~M2FAudioCodecOpus();
protected:
    virtual int HdEncInit(void* param);
    virtual int Encode(M2FAudioCodecEncodeIn* pIn);
    virtual int HdDecInit(void* param);
    virtual int Decode(M2FAudioCodecDecodeIn* pIn);
private:
};








#endif