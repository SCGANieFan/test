#pragma once
#include<stdint.h>
#include"MAF_Algorithm.h"
#include"MAFA_AudioFrame.h"

class MAFA_Audio:public MAF_Algorithm
{
public:
	MAFA_Audio();
	virtual ~MAFA_Audio();
public:
	virtual int32_t Init(void* param) = 0;
	virtual int32_t Deinit() = 0;
	virtual int32_t Process(MAFA_Frame *frameIn, MAFA_Frame* frameOut) = 0;
private:

};

