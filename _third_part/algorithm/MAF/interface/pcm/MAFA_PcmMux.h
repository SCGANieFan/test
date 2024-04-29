#pragma once
#include "MAF.Audio.h"
class MAFA_PcmMux:public MAF_Audio
{
	typedef struct
	{
		maf_int32 fsHz;
		maf_int16 channels;
		maf_int32 width;
		maf_int32 frameSamples;
	}AudioInfo;

public:
	MAFA_PcmMux();
	~MAFA_PcmMux();
public:
	virtual maf_int32 Init() override;
	virtual maf_int32 Deinit() override;
	virtual maf_int32 Process(MAF_Data* dataIn, MAF_Data* dataOut) override;
private:
};
