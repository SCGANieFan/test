#pragma once
#include "MAF.Audio.h"
class MAFAA_OpusDec:public MAF_Audio
{
	typedef struct
	{
		maf_int32 fsHz;
		maf_int16 channels;
		maf_int32 width;
		maf_int32 frameSamples;
	}AudioInfo;

public:
	MAFAA_OpusDec();
	~MAFAA_OpusDec();
public:
	virtual maf_int32 Init() override;
	virtual maf_int32 Deinit() override;
	virtual maf_int32 Process(MAF_Data* dataIn, MAF_Data* dataOut) override;
private:
	maf_void *_hd;
	maf_int32 _hdSize;
	AudioInfo _audioInfo;
};
