#pragma once
#include "MAF.Audio.h"
class MAFA_OpusDec:public MAF_Audio
{
public:
	MAFA_OpusDec();
	~MAFA_OpusDec();
public:
	virtual maf_int32 Init() final;
	virtual maf_int32 Deinit() final;
	virtual maf_int32 Process(MAF_Data* dataIn, MAF_Data* dataOut) final;

	virtual maf_int32 Set(const maf_int8* key, maf_void* val) final;
	virtual maf_int32 Get(const maf_int8* key, maf_void* val) final;
private:
	maf_void *_hd;
	maf_int32 _hdSize;
};
