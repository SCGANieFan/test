//#include"OpusCodecInterface.h"
//std
#include<string.h>
//other
#include"OpusDec.h"
#include"OpusEnc.h"
//lib
#include"M2FOpusCodecInterface.h"
M2FAudioCodecOpus::M2FAudioCodecOpus()
{
	this->HeapInit(NULL, 0);
	this->codecType = "Opusx";
}
M2FAudioCodecOpus::M2FAudioCodecOpus(void* p, m2f_uint32 size)
{
	this->HeapInit(p, size);
	this->codecType = "Opusx";
}

M2FAudioCodecOpus::~M2FAudioCodecOpus()
{
	if(this->hdEnc)
		this->Free(this->hdEnc);
	if (this->hdDec)
		this->Free(this->hdDec);
}

int M2FAudioCodecOpus::HdEncInit(void* param)
{
	OpusEncInfo_t* pInfo = (OpusEncInfo_t*)param;
	this->hdEncSize = OpusEncoderSize(pInfo->channels);
	this->hdEnc = this->MAlloc(this->hdEncSize);
	/* defaults: */
	pInfo->bandwidth = M2F_OPUS_AUTO;
	pInfo->use_vbr = 0;
	pInfo->cvbr = 0;
	pInfo->use_inbandfec = 0;
	pInfo->forcechannels = M2F_OPUS_AUTO;
	pInfo->use_dtx = 0;
	pInfo->lsb_depth = 16;
	pInfo->variable_duration = M2F_OPUS_FRAMESIZE_20_MS;
	switch (pInfo->frame_duration_0p1ms)
	{
	case 25:
		pInfo->variable_duration = M2F_OPUS_FRAMESIZE_2_5_MS;
		break;
	case 50:
		pInfo->variable_duration = M2F_OPUS_FRAMESIZE_5_MS;
		break;
	case 100:
		pInfo->variable_duration = M2F_OPUS_FRAMESIZE_10_MS;
		break;
	case 200:
		pInfo->variable_duration = M2F_OPUS_FRAMESIZE_20_MS;
		break;
	case 400:
		pInfo->variable_duration = M2F_OPUS_FRAMESIZE_40_MS;
		break;
	case 600:
		pInfo->variable_duration = M2F_OPUS_FRAMESIZE_60_MS;
		break;
	case 800:
		pInfo->variable_duration = M2F_OPUS_FRAMESIZE_80_MS;
		break;
	case 1000:
		pInfo->variable_duration = M2F_OPUS_FRAMESIZE_100_MS;
		break;
	case 1200:
		pInfo->variable_duration = M2F_OPUS_FRAMESIZE_120_MS;
		break;
	default:
		break;
	}
	this->sampleRate_hz = pInfo->sampling_rate_hz;
	this->frameDuration_0p1ms = pInfo->frame_duration_0p1ms;
	this->frameSize = pInfo->sampling_rate_hz * pInfo->channels * pInfo->frame_duration_0p1ms / 10000;
	return OpusEncoderInit(this->hdEnc, pInfo);
}
int M2FAudioCodecOpus::Encode(M2FAudioCodecEncodeIn* pIn)
{
	return OpusEnc(this->hdEnc, (OpusEncIn_t*)pIn);
}
int M2FAudioCodecOpus::HdDecInit(void* param)
{
	OpusDecInfo_t* pInfo = (OpusDecInfo_t*)param;
	this->hdDecSize = OpusDecoderSize(pInfo->channels);
	this->hdDec = this->MAlloc(this->hdDecSize);
	return OpusDecoderInit(this->hdDec, pInfo);
}
int M2FAudioCodecOpus::Decode(M2FAudioCodecDecodeIn* pIn)
{
	return OpusDec(this->hdDec, (OpusDecIn_t*)pIn);
}



