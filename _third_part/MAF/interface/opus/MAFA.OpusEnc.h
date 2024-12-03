#pragma once
#include "MAF.Audio.h"
class MAFA_OpusEnc:public MAF_Audio
{
public:
	MAFA_OpusEnc();
	~MAFA_OpusEnc();
public:
	virtual maf_int32 Init() final;
	virtual maf_int32 Deinit() final;
	virtual maf_int32 Process(MAF_Data* dataIn, MAF_Data* dataOut) final;
	virtual maf_int32 Set(const maf_int8* key, maf_void* val) final;
	virtual maf_int32 Get(const maf_int8* key, maf_void* val) final;
private:
	maf_void *_hd;
	maf_int32 _hdSize;
	maf_int32 _bitrate;
	maf_int32 _complexity;
	maf_int32 _frame0p1Ms;
	maf_bool _vbr;
	
};
