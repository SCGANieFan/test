#ifndef __M2F_MULTIMEDIA_AudioCodec__H_
#define __M2F_MULTIMEDIA_AudioCodec__H_

#include"../audio/src/M2FAudio.h"
class M2FMultimedia :virtual public M2FCommon
{
	//functions
public:
	M2FMultimedia();
	M2FMultimedia(void* p, m2f_uint32 size);
	~M2FMultimedia();
	
	m2f_int32 AudioRegister();
	M2FAudio* getAudio();
protected:
private:
	//variable
public:
	
protected:

private:
	M2FAudio* audio;
};


#endif