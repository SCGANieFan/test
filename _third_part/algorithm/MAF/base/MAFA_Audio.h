#pragma once
#include<stdint.h>
#include"MAF_Algorithm.h"

typedef enum {
	MAFA_FRAME_IS_EMPTY = 1 << 0,
}MAFA_FrameFlag;

typedef struct {
	uint8_t* buff;
	int32_t off;
	int32_t size;
	int32_t max;
	uint32_t flags;
}MAFA_Frame;

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

