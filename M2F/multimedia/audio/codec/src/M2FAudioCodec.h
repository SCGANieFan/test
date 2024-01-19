#ifndef __M2FAudioCodec__H_
#define __M2FAudioCodec__H_

#include"../common/M2FCommonInterface.h"

struct M2FAudioCodecEncodeIn {
	short* pcmIn;
	unsigned int pcmInLen;
	unsigned char* bitOut;
	unsigned int* bitOutLen;
};

struct M2FAudioCodecDecodeIn {
	unsigned char* bitIn;
	unsigned int bitInLen;
	short* pcmOut;
	unsigned int* pcmOutLen;
};

class M2FAudioCodec:virtual public M2FCommon
{
	//function 
public:
	M2FAudioCodec();
	~M2FAudioCodec();

	virtual int HdEncInit(void* param) = 0;
	virtual int Encode(M2FAudioCodecEncodeIn* pIn) = 0;
	virtual int HdDecInit(void* param) = 0;
	virtual int Decode(M2FAudioCodecDecodeIn* pIn) = 0;

	const char* getCodecType();
protected:

private:
	//variable
public:
	int hdEncSize;
	int hdDecSize;

	int frameSize;
	int sampleRate_hz;
	int frameDuration_0p1ms;
protected:
	const char *codecType;
	void* hdEnc;
	void* hdDec;
private:
};
m2f_uint32 getM2FAudioCodecSize(const char* type);
m2f_int32 M2FAudioCodecInit(M2FAudioCodec* codec, const char* type, void* p, m2f_uint32 size);










#endif