//std
#include <string>
//lib
#include"AudioCodecTest.h"
//other
#include"M2FAudioCodecTestInterface.h"





M2FAudioCodecACT::M2FAudioCodecACT()
{
	this->HeapInit(NULL, 0);
	codecType = "ACT";
}
M2FAudioCodecACT::M2FAudioCodecACT(void* p, m2f_uint32 size)
{
	this->HeapInit(p, size);
	codecType = "ACT";
}
M2FAudioCodecACT::~M2FAudioCodecACT()
{


}



int M2FAudioCodecACT::HdEncInit(void* param)
{
	M2FACTEncInfo_t* pInfo = (M2FACTEncInfo_t*)param;
	
	this->hdEncSize = 0;
	this->hdEnc = NULL;

	this->sampleRate_hz = pInfo->sampling_rate_hz;
	this->frameDuration_0p1ms = pInfo->frame_duration_0p1ms;
	this->frameSize = pInfo->sampling_rate_hz * pInfo->frame_duration_0p1ms / 10000;


	return 0;
}
int M2FAudioCodecACT::Encode(M2FAudioCodecEncodeIn* pIn)
{
	ACTEncode((char *)pIn->pcmIn, pIn->pcmInLen, (char*)pIn->bitOut, pIn->bitOutLen);
	return 0;
}
int M2FAudioCodecACT::HdDecInit(void* param)
{
	M2FACTDecInfo_t* pInfo = (M2FACTDecInfo_t*)param;

	this->hdDecSize = 0;
	this->hdDec = NULL;

	return 0;
}
int M2FAudioCodecACT::Decode(M2FAudioCodecDecodeIn* pIn)
{
	ACTDecode((char*)pIn->bitIn, pIn->bitInLen, (char*)pIn->pcmOut, pIn->pcmOutLen);
	return 0;
}



