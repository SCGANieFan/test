//std
#include <string>
#include <stdlib.h>
//lib
//other
#include"./M2F.h"


//print
#define M2FDebug 1
#if M2FDebug 
#define M2FPrint(s,...)				this->Print("[M2F](%d)<%s> " s "\n", __LINE__, __FUNCTION__,##__VA_ARGS__)
#else
#define M2FPrint(s,...)				
#endif


void M2F::Init()
{	
	this->multimedia = (M2FMultimedia*)this->MAlloc(sizeof(M2FMultimedia));
	if (this->multimedia)
	{
		m2f_uint32 size = 0;
		void *p = this->MAlloc(size);
		new(this->multimedia)(M2FMultimedia)(p, size);
	}
	this->management = (M2FManagement*)this->MAlloc(sizeof(M2FManagement));
	if (this->management)
	{
		m2f_uint32 size = 0;
		void* p = this->MAlloc(size);
		new(this->management)(M2FManagement)(p, size);
	}

	this->useMipsMonitor = false;
	this->useMemMonitor = false;
}

void M2F::Deinit()
{
	if (this->multimedia)
	{
		this->Free(this->multimedia->getHeapAddress());
		this->multimedia->~M2FMultimedia();
		this->Free(this->multimedia);
	}
	if (this->management)
	{
		this->Free(this->management->getHeapAddress());
		this->management->~M2FManagement();
		this->Free(this->management);
	}
}
M2F::M2F()
{
	this->HeapInit(NULL, 0);
	Init();
}
M2F::M2F(void *p,m2f_uint32 size)
{
	this->HeapInit(p, size);
	Init();
}
M2F::~M2F()
{
	Deinit();
}


m2f_int32 M2F::AudioRegister()
{
	return this->multimedia->AudioRegister();
}


m2f_int32 M2F::CodecCreate(const char* pCodecType)
{
	M2FAudio* audio = this->multimedia->getAudio();
	if (audio)
		return audio->CodecCreate(pCodecType);
	else
	{
		M2FPrint("should use AudioRegister()");
		return -1;
	}
}

m2f_int32 M2F::EncoderInit(const char* pCodecType, void* param)
{
	m2f_int32 ret = 0;

	M2FAudio* audio = this->multimedia->getAudio();
	if (!audio)
	{
		M2FPrint("should use AudioRegister()");
		return -1;
	}
	M2FAudioCodec* codec = audio->getCodec(pCodecType);
	if (!codec)
	{
		M2FPrint("no such codec:%s", pCodecType);
		return -1;
	}
	if (this->useMipsMonitor)
	{				
		char monitorName[10] = { 0 };
		sprintf_s(monitorName, 10, "%sEnc", pCodecType);
		this->management->MipsMonitorCreat((const char*)monitorName);
	}

	ret = codec->HdEncInit(param);

	if (this->useMemMonitor)
	{
		M2FPrint("%dEnc mem sued:%d", pCodecType, codec->getMemUsed());
	}

	return ret;
}

m2f_int32 M2F::DecoderInit(const char* pCodecType, void* param)
{
	m2f_int32 ret = 0;
	M2FAudio* audio = this->multimedia->getAudio();
	if (!audio)
	{
		M2FPrint("should use AudioRegister()");
		return -1;
	}
	M2FAudioCodec* codec = audio->getCodec(pCodecType);
	if (!codec)
	{
		M2FPrint("no such codec:%s", pCodecType);
		return -1;
	}
	if (this->useMipsMonitor)
	{
		char monitorName[10] = { 0 };
		sprintf_s(monitorName, 10, "%sDec", pCodecType);
		this->management->MipsMonitorCreat((const char*)monitorName);
	}
	m2f_int32 temp = codec->getMemUsed();
	ret = codec->HdDecInit(param);
	if (this->useMemMonitor)
	{
		M2FPrint("%dDec mem sued:%d", pCodecType, codec->getMemUsed() - temp);
	}
	
	return ret;
}

m2f_int32 M2F::AudioEncode(const char* pCodecType, short* pcmIn, unsigned int pcmInLen, unsigned char* bitOut, unsigned int* bitOutLen)
{
	int ret = 0;

	M2FAudioCodecEncodeIn encodeIn = { 0 };
	encodeIn.pcmIn = pcmIn;
	encodeIn.pcmInLen = pcmInLen;
	encodeIn.bitOut = bitOut;
	encodeIn.bitOutLen = bitOutLen;
	M2FAudioCodec* codec = this->multimedia->getAudio()->getCodec(pCodecType);
	char monitorName[10] = { 0 };
	sprintf_s(monitorName, 10, "%sEnc", pCodecType);
	M2FMipsMonitor* mipsMonitor = NULL;
	if (this->useMipsMonitor)
	{
		mipsMonitor = this->management->getMipsMonitor(monitorName);
		mipsMonitor->start();
	}
	ret = codec->Encode(&encodeIn);
	if (this->useMipsMonitor)
	{
		mipsMonitor = this->management->getMipsMonitor(monitorName);
		mipsMonitor->end(codec->frameDuration_0p1ms);
	}
	return ret;
}
m2f_int32 M2F::AudioDecode(const char* pCodecType, unsigned char* bitIn, unsigned int bitInLen, short* pcmOut, unsigned int* pcmOutLen)
{
	int ret = 0;
	M2FAudioCodecDecodeIn decodeIn = { 0 };
	decodeIn.bitIn = bitIn;
	decodeIn.bitInLen = bitInLen;
	decodeIn.pcmOut = pcmOut;
	decodeIn.pcmOutLen = pcmOutLen;
	M2FAudioCodec* codec = this->multimedia->getAudio()->getCodec(pCodecType);
	char monitorName[10] = { 0 };
	sprintf_s(monitorName, 10, "%sDec", pCodecType);
	M2FMipsMonitor* mipsMonitor = NULL;
	if (this->useMipsMonitor)
	{
		mipsMonitor = this->management->getMipsMonitor(monitorName);
		mipsMonitor->start();
	}
	ret = codec->Decode(&decodeIn);
	if (this->useMipsMonitor)
	{
		mipsMonitor = this->management->getMipsMonitor(monitorName);
		mipsMonitor->end(codec->frameDuration_0p1ms);
	}
	return ret;
}



void M2F::UseMipsMonitor()
{
	this->useMipsMonitor = true;
}

void M2F::UseMemMonitor()
{
	this->useMemMonitor = true;
}

void M2F::PrintCodecMips(const char* pCodecType)
{
	M2FAudio* audio = this->multimedia->getAudio();
	if (!audio)
		return;
	if (this->useMipsMonitor)
	{
		M2FMipsMonitor* mipsMonitor = NULL;
		char monitorName[10] = { 0 };

		m2f_uint32 encMips = 0;
		m2f_uint32 decMips = 0;

		sprintf_s(monitorName, 10, "%sEnc", pCodecType);
		mipsMonitor = this->management->getMipsMonitor(monitorName);
		encMips = mipsMonitor->getAveMips();

		sprintf_s(monitorName, 10, "%sDec", pCodecType);
		mipsMonitor = this->management->getMipsMonitor(monitorName);
		decMips = mipsMonitor->getAveMips();

		M2FPrint("[mips] enc:%d,dec:%d", encMips, decMips);
	}	
	
}




