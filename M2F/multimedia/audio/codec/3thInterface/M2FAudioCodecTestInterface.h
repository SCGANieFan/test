#ifndef __AudioCodecTestInterface__H_
#define __AudioCodecTestInterface__H_

#include"../src/M2FAudioCodec.h"

typedef struct {
    int sampling_rate_hz;
    unsigned char channels;
    unsigned int bitrate_bps;
    unsigned char lsb_depth;
    unsigned int frame_duration_0p1ms;
}M2FACTEncInfo_t;


typedef struct {
    int sampling_rate_hz;
    unsigned char channels;
    unsigned int bitrate_bps;
    unsigned char lsb_depth;
    unsigned int frame_duration_0p1ms;
}M2FACTDecInfo_t;


class M2FAudioCodecACT : public M2FAudioCodec
{
public:
	M2FAudioCodecACT();
    M2FAudioCodecACT(void* p, m2f_uint32 size);
	~M2FAudioCodecACT();
protected:
	virtual int HdEncInit(void* param);
	virtual int Encode(M2FAudioCodecEncodeIn* pIn);
	virtual int HdDecInit(void* param);
	virtual int Decode(M2FAudioCodecDecodeIn* pIn);
private:
};



#endif