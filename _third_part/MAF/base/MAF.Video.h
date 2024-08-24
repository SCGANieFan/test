#pragma once
#include<stdint.h>
#include"MAF.Algorithm.h"
#include"MAFV.Info.h"

typedef enum {
	MAFV_FRAME_IS_EMPTY = 1 << 0,
	MAFV_FRAME_IS_EOS = 1 << 1,
	MAFV_FRAME_IS_EXTRA_INFO = 1 << 2,
}MAF_VideoFrameFlag;


class MAF_Video:public MAF_Algorithm, public MAFV_Info
{
public:
	MAF_Video();
	virtual ~MAF_Video();
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

