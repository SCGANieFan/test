#pragma once
#include"MAF.Audio.h"

class MAFA_SpeedCtr:public MAF_Audio
{
public:
	MAFA_SpeedCtr();
	~MAFA_SpeedCtr();
public:
	virtual maf_int32 Init() final;
	virtual maf_int32 Deinit() final;
	virtual maf_int32 Process(MAF_Data* dataIn, MAF_Data* dataOut) final;

	virtual maf_int32 Set(const maf_int8* key, maf_void* val) final;
	virtual maf_int32 Get(const maf_int8* key, maf_void* val) final;
private:
	maf_void* _hd = 0;
	maf_int32 _hdSize = 0;

	maf_float _speed = 0;
	maf_bool _isSpeech = true;

	//MAF_Data _iDataCache;
	MAF_Data _oDataCache;
};
