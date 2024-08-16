#pragma once
#include"MAF.Audio.h"

class MAFA_SbcDec :public MAF_Audio
{
public:
	MAFA_SbcDec();
	~MAFA_SbcDec();
public:
	virtual maf_int32 Init() final;
	virtual maf_int32 Deinit() final;
	virtual maf_int32 Process(MAF_Data* dataIn, MAF_Data* dataOut) final;

	virtual maf_int32 Set(const maf_int8* key, maf_void* val) final;
	virtual maf_int32 Get(const maf_int8* key, maf_void* val) final;
private:
	static maf_void* MallocLocal(int32_t size);
	static maf_void FreeLocal(maf_void* block);
private:
	maf_void* _hd = 0;
	maf_int32 _hdSize = 0;

	maf_int16 _decayMs = 0;
	maf_int16 _gainMs = 0;
	maf_int16 _overlapMs = 0;
	maf_void* _basePorting;

	maf_bool _isFirst = true;
	maf_int32 _vlcSize;
	maf_int32 _pcmSize;
private:
	static maf_void* _malloc;
	static maf_void* _free;
};
