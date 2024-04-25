#pragma once
#include "MAFA_Audio.h"
class MAFAA_OpusDec:public MAFA_Audio
{
	typedef struct
	{
		int32_t fsHz;
		int16_t channels;
		int32_t width;
		int32_t frameSamples;
	}AudioInfo;

public:
	MAFAA_OpusDec();
	~MAFAA_OpusDec();
public:
	virtual int32_t Init(void* param) override;
	virtual int32_t Deinit() override;
	virtual int32_t Process(MAFA_Frame* frameIn, MAFA_Frame* frameOut) override;
private:
	void *_hd;
	int32_t _hdSize;
	AudioInfo _audioInfo;
};
