//std
#include <string>
//lib
#include"../codec/src/M2FAudioCodec.h"
//other
#include"M2FAudio.h"


//print
#define M2FAudioDebug 1
#if M2FAudioDebug 
#define M2F_AudioPrint(s,...)				this->Print("[M2F_Audio](%d)<%s> " s "\n", __LINE__, __FUNCTION__,##__VA_ARGS__)
#else
#define M2F_AudioPrint(s,...)				
#endif


M2FAudio::M2FAudio()
{
	this->HeapInit(NULL, 0);
	this->codecCur = NULL;
	for (int i = 0; i < 10; i++)
	{
		this->codecs[i] = NULL;
	}
	this->codecsNum = 0;
}

M2FAudio::M2FAudio(void* p, m2f_uint32 size)
{
	this->HeapInit(p, size);
	this->codecCur = NULL;
	for (int i = 0; i < 10; i++)
	{
		this->codecs[i] = NULL;
	}
	this->codecsNum = 0;
}

M2FAudio::~M2FAudio()
{
	for (int i = 0; i < 10; i++)
	{
		if (this->codecs[i])
		{
			this->Free(this->codecs[i]->getHeapAddress());
			this->codecs[i]->~M2FAudioCodec();
			this->Free(this->codecs[i]);
		}
		else
			break;
	}
}


m2f_int32 M2FAudio::CodecCreate(const char* codecType)
{
	if (this->codecsNum >= 10)
	{
		M2F_AudioPrint("codecsNum is greater than %d", this->codecsNum);
		return -1;
	}
	M2FAudioCodec* codec = (M2FAudioCodec*)this->MAlloc(getM2FAudioCodecSize(codecType));
	if (!codec)
	{
		M2F_AudioPrint("MAlloc fail");
		return -1;
	}
	m2f_uint32 size = 0;
	void *p = this->MAlloc(size);
	if (M2FAudioCodecInit(codec, codecType, p, size))
	{
		M2F_AudioPrint("M2FAudioCodecInit fail");
		this->Free(p);
		this->Free(codec);
		return -1;
	}
	this->codecCur = codec;
	this->codecs[this->codecsNum] = this->codecCur;
	this->codecsNum++;
	return 0;
}


M2FAudioCodec* M2FAudio::getCodec(const char* pCodecType)
{
	for (int i = 0; i < this->codecsNum; i++)
	{
		if (!strcmp(pCodecType, this->codecs[i]->getCodecType()))
		{
			this->codecCur = this->codecs[i];
			return this->codecCur;
		}
	}
	return NULL;
}


M2FAudioCodec* M2FAudio::getCodecCur()
{
	return this->codecCur;
}






