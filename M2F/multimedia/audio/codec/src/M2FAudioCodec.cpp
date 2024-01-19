//std
#include <stdlib.h>
#include <string>
//lib
#include"M2FAudioCodec.h"
//other
#include"../3thInterface/M2FAudioCodecTestInterface.h"
#include"../3thInterface/M2FOpusCodecInterface.h"
#include"../3thInterface/M2FSilkCodecInterface.h"




M2FAudioCodec::M2FAudioCodec()
{
	this->hdEncSize = 0;
	this->hdDecSize = 0;

	this->frameSize = 0;
	this->sampleRate_hz = 0;
	this->frameDuration_0p1ms = 0;

	this->hdEnc = NULL;
	this->hdDec = NULL;
	this->codecType = NULL;
}

M2FAudioCodec::~M2FAudioCodec()
{

}

const char* M2FAudioCodec::getCodecType()
{
	return this->codecType;
}

m2f_uint32 getM2FAudioCodecSize(const char* type)
{
	m2f_uint32 size = 0;
	if (!strcmp(type, "AudioCodecTest"))
	{
		size = sizeof(M2FAudioCodecACT);
	}
	else if (!strcmp(type, "Opusx"))
	{
		size = sizeof(M2FAudioCodecOpus);
	}
	else if (!strcmp(type, "Silk"))
	{
		size = sizeof(M2FAudioCodecSilk);
	}
	return size;
}
m2f_int32 M2FAudioCodecInit(M2FAudioCodec* codec, const char* type, void* p, m2f_uint32 size)
{
	if (!codec)
		return -1;
	if (!strcmp(type, "AudioCodecTest"))
	{
		new(codec)M2FAudioCodecACT(p, size);
	}
	else if (!strcmp(type, "Opusx"))
	{
		new(codec)(M2FAudioCodecOpus)(p, size);
	}
	else if (!strcmp(type, "Silk"))
	{
		new(codec)(M2FAudioCodecSilk)(p, size);
	}
	else 
	{
		return -1;
	}
	return 0;
}


