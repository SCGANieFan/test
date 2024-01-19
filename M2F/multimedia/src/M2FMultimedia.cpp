#include"M2FMultimedia.h"
#include<string>

//print
#define M2FMultimediaDebug 1
#if M2FMultimediaDebug 
#define M2F_MultimediaPrint(s,...)				this->Print("[M2F_MTM](%d)<%s> " s "\n", __LINE__, __FUNCTION__,##__VA_ARGS__)
#else
#define M2F_MultimediaPrint(s,...)				
#endif

M2FMultimedia::M2FMultimedia()
{
	this->HeapInit(NULL, 0);
	this->audio = NULL;
}
M2FMultimedia::M2FMultimedia(void* p, m2f_uint32 size)
{
	this->HeapInit(p, size);
	this->audio = NULL;
}
M2FMultimedia::~M2FMultimedia()
{
	if (this->audio)
	{
		this->Free(this->audio->getHeapAddress());
		this->audio->~M2FAudio();
		this->Free(this->audio);
	}
		
}

m2f_int32 M2FMultimedia::AudioRegister()
{
	if (this->audio)
	{
		M2F_MultimediaPrint("just can register once");
		return -1;
	}
	this->audio = (M2FAudio*)this->MAlloc(sizeof(M2FAudio));
	if (this->audio)
	{
		//new(this->audio)M2FAudio(NULL, 0);
		m2f_uint32 size = 0;
		//m2f_uint32 size = 10 * 1024;
		void *p = this->MAlloc(size);
		new(this->audio)M2FAudio(p, size);
		return 0;
	}
	else
	{
		M2F_MultimediaPrint("MAlloc fail");
		return -1;
	}
}

M2FAudio* M2FMultimedia::getAudio()
{
	if (!this->audio)
		M2F_MultimediaPrint("should use AudioRegister() before getAudio");
	return this->audio;
}

