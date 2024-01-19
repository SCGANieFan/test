#ifndef __M2F_API__H_
#define __M2F_API__H_

#include"../src/M2F.h"

#include"../multimedia/audio/codec/3thInterface/M2FAudioCodecTestInterface.h"
#include"../multimedia/audio/codec/3thInterface/M2FOpusCodecInterface.h"
#include"../multimedia/audio/codec/3thInterface/M2FSilkCodecInterface.h"



//M2FAudioCodec* M2FCreateAudioCodec(const char *type);
//void M2FDeleteAudioCodec(M2FAudioCodec* pM2FAudioCodec);
//int M2FEncode(M2FAudioCodec* pM2FAudioCodec, M2FAudioCodecEncodeIn* pIn);
//int M2FHdEncInit(M2FAudioCodec* pM2FAudioCodec, void* param);
//int M2FDecode(M2FAudioCodec* pM2FAudioCodec, M2FAudioCodecDecodeIn* pIn);
//int M2FHdDecInit(M2FAudioCodec* pM2FAudioCodec, void* param);



class M2FApi :public M2FObject
{
	//functions
public:
	M2FApi();
	~M2FApi();

	int Encode(M2F* pM2F, short* pcmIn, unsigned int pcmInLen, unsigned char* bitOut, unsigned int* bitOutLen);

protected:
private:
	//variable
public:
protected:
private:

};







#endif