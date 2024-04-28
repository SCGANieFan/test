#pragma once
#include "MAFA_Audio.h"
class MAFAA_PcmMux:public MAFA_Audio
{
	typedef struct
	{
		int32_t fsHz;
		int16_t channels;
		int32_t width;
		int32_t frameSamples;
	}AudioInfo;

public:
	MAFAA_PcmMux();
	~MAFAA_PcmMux();
public:
	virtual int32_t Init(void* param) override;
	virtual int32_t Deinit() override;
	virtual int32_t Process(MAFA_Frame* frameIn, MAFA_Frame* frameOut) override;
private:
	AudioInfo _audioInfo;
};
