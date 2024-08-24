#pragma once

#include"MAF.Type.h"
#include"MAF.Memory.h"
#include"MAF.Printf.h"
#include"MAF.Data.h"

class MAF_Algorithm
{
public:
	MAF_Algorithm();
	virtual ~MAF_Algorithm();
public:
	MAF_Memory _memory;
public:
	virtual maf_int32 Init() = 0;
	virtual maf_int32 Deinit() = 0;
	virtual maf_int32 Process(MAF_Data* dataIn, MAF_Data* dataOut) = 0;
	virtual maf_int32 Rceive(MAF_Data* dataIn) = 0;
	virtual maf_int32 Generate(MAF_Data* dataOut) = 0;
	virtual maf_int32 Set(const maf_int8* key, maf_void* val);
	virtual maf_int32 Get(const maf_int8* key, maf_void* val);
protected:

private:

};

