#pragma once
#include "MAF.Audio.h"
class MAF_MusicPlc24L:public MAF.Audio
{
public:
	MAF_MusicPlc24L();
	~MAF_MusicPlc24L();
public:
	virtual maf_int32 Init() override;
	virtual maf_int32 Deinit() override;
	virtual maf_int32 Process(MAF_AudioFrame* frameIn, MAF_AudioFrame* frameOut) override;
private:
	maf_void *_hd;
	maf_int32 _hdSize;

	maf_int32 _fsHz;
	maf_int16 _channels;
	maf_int16 _frameSamples;


};
