#ifndef __M2F__H_
#define __M2F__H_

#include"../multimedia/src/M2FMultimedia.h"
#include"../management/src/M2FManagement.h"

class M2F :virtual public M2FCommon
{	
	//functions	
public:
	M2F();
	M2F(void* p, m2f_uint32 size);
	~M2F();

	//audio
	m2f_int32 AudioRegister();

	//codec
	m2f_int32 CodecCreate(const char* pCodecType);

	m2f_int32 EncoderInit(const char* pCodecType, void* param);
	m2f_int32 AudioEncode(const char* pCodecType, short* pcmIn, unsigned int pcmInLen, unsigned char* bitOut, unsigned int* bitOutLen);

	m2f_int32 DecoderInit(const char* pCodecType, void* param);
	m2f_int32 AudioDecode(const char* pCodecType, unsigned char* bitIn, unsigned int bitInLen, short* pcmOut, unsigned int* pcmOutLen);

	//monitor
	void UseMipsMonitor();
	void UseMemMonitor();
	void PrintCodecMips(const char* pCodecTypes);
protected:

private:
	void Init();
	void Deinit();
	//variable
public:

protected:

private:
	M2FMultimedia* multimedia;
	M2FManagement* management;


	bool useMipsMonitor;
	bool useMemMonitor;
};









#endif