#pragma once
#include"MAF.Audio.h"

class MAFA_SbcEnc :public MAF_Audio
{
public:
	MAFA_SbcEnc();
	~MAFA_SbcEnc();
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

	maf_uint8 _bitPool = 0;
	maf_uint8 _blocks = 0;
	maf_uint8 _subBands = 0;
	maf_uint8 _channelMode = 0;
	maf_uint8 _allocMethod = 0;
	maf_uint8 _sbcMode = 0;
	maf_uint16 _pcmSize;
	maf_uint16 _vlcSize;
private:
	static maf_void* _malloc;
	static maf_void* _free;
};
