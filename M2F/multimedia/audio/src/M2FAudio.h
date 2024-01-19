#ifndef __M2FAudio__H_
#define __M2FAudio__H_

#include"../codec/src/M2FAudioCodec.h"

class M2FAudio :virtual public M2FCommon
{
	//functions
public:
	M2FAudio();
	M2FAudio(void* p, m2f_uint32 size);
	~M2FAudio();
	m2f_int32 CodecCreate(const char* codecType);
	M2FAudioCodec* getCodec(const char* codecType);
	M2FAudioCodec* getCodecCur();
protected:
private:

	//variable
public:
	
protected:
	
private:
	M2FAudioCodec* codecCur;
	M2FAudioCodec* codecs[10];
	unsigned char codecsNum;

};




#endif