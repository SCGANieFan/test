#pragma once
#include "MAF.Audio.h"
class MAF_MusicPlc32:public MAF.Audio
{
public:
	MAF_MusicPlc32();
	~MAF_MusicPlc32();
public:
	virtual maf_int32 Init() override;
	virtual maf_int32 Deinit() override;
	virtual maf_int32 Process(MAF_AudioFrame* frameIn, MAF_AudioFrame* frameOut) override;
private:
	maf_void *_hd;
	maf_int32 _hdSize;
};
