//std
#include<string.h>
//lib
#include"SilkEnc.h"
#include"SilkDec.h"
//other
#include"M2FSilkCodecInterface.h"


M2FAudioCodecSilk::M2FAudioCodecSilk()
{
	this->HeapInit(NULL, 0);
	this->codecType = "Silk";
	this->hdEncControl = NULL;
	this->hdDecControl = NULL;
}
M2FAudioCodecSilk::M2FAudioCodecSilk(void* p, m2f_uint32 size)
{	
	this->HeapInit(p, size);
	this->codecType = "Silk";
	this->hdEncControl = NULL;
	this->hdDecControl = NULL;
}
M2FAudioCodecSilk::~M2FAudioCodecSilk()
{
	if (this->hdEnc)
		this->Free(this->hdEnc);
	if (this->hdEncControl)
		this->Free(this->hdEncControl);
	if (this->hdDec)
		this->Free(this->hdDec);
	if (this->hdDecControl)
		this->Free(this->hdDecControl);
}

int M2FAudioCodecSilk::HdEncInit(void* param)
{
	SilkEncInfo_t* pInfo = (SilkEncInfo_t*)param;
	this->hdEncSize = SilkEncoderSize();
	this->hdEnc = this->MAlloc(this->hdEncSize);
	this->hdEncControl = this->MAlloc(SilkEncoderControlSize());
	/* defaults: */
	/* default settings */
	pInfo->frameSizeReadFromFile_ms = pInfo->packetSize_ms;
	pInfo->packetLoss_perc = 0;	
	pInfo->DTX_enabled = 0;
	pInfo->INBandFEC_enabled = 0;
	this->sampleRate_hz = pInfo->sample_rate;
	this->frameDuration_0p1ms = pInfo->packetSize_ms * 10;
	this->frameSize = pInfo->sample_rate * pInfo->packetSize_ms / 1000;
	return SilkEncoderInit(this->hdEnc, this->hdEncControl, pInfo);
}
int M2FAudioCodecSilk::Encode(M2FAudioCodecEncodeIn* pIn)
{
	return SilkEnc(this->hdEnc, this->hdEncControl, (SilkEncIn_t*)pIn);
}


int M2FAudioCodecSilk::HdDecInit(void* param)
{
	SilkDecInfo_t* pInfo = (SilkDecInfo_t*)param;
	/* default settings */
	pInfo->loss_prob = 0.0f;
	
	this->hdDecSize = SilkDecoderSize();
	this->hdDec = this->MAlloc(this->hdDecSize);
	this->hdDecControl = this->MAlloc(SilkEncoderControlSize());

	return SilkDecoderCreate(this->hdDec, this->hdDecControl, pInfo);
}
int M2FAudioCodecSilk::Decode(M2FAudioCodecDecodeIn* pIn)
{
	return SilkDec(this->hdDec, this->hdDecControl, (SilkDecIn_t*)pIn);
}



