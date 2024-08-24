#pragma once
#include<stdint.h>
#include"MAF.Algorithm.h"
#include"MAFA.Info.h"

typedef enum {
	MAFA_FRAME_IS_EMPTY = 1 << 0,
	MAFA_FRAME_IS_EOS = 1 << 1,
	MAFA_FRAME_IS_EXTRA_INFO = 1 << 2,
}MAF_AudioFrameFlag;


class MAF_Audio:public MAF_Algorithm, public MAFA_Info
{
public:
	MAF_Audio();
	virtual ~MAF_Audio();
public:
	virtual maf_int32 Init() = 0;
	virtual maf_int32 Deinit() = 0;
	virtual maf_int32 Process(MAF_Data* dataIn, MAF_Data* dataOut)override{return 0;}
	virtual maf_int32 Rceive(MAF_Data* dataIn)override{return 0;}
	virtual maf_int32 Generate(MAF_Data* dataOut)override{return 0;}
protected:
	virtual maf_int32 Set(const maf_int8* key, maf_void* val) override;
	virtual maf_int32 Get(const maf_int8* key, maf_void* val) override;
private:

};

