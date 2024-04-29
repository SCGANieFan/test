#pragma once

#include"MAF.Type.h"



class MAF_Data
{
public:
	MAF_Data();
	~MAF_Data();
public:
	//set
	maf_int32 SetFlag(maf_uint32 flag);

	//get
	maf_uint8* GetBuf();
	maf_int32 GetMax();
	maf_uint8* GetData();
	maf_int32 GetSize();
	maf_uint8* GetLeftData();
	maf_int32 GetLeftSize();
	maf_int32 GetFlags();

	//
	maf_int32 Init(maf_uint8* buf, maf_int32 size);
	maf_int32 Init(maf_uint8* buf, maf_int32 size, maf_int32 max);
	//
	maf_int32 Append(maf_uint8* buf, maf_int32 size);
	maf_int32 Append(maf_int32 size);
	maf_int32 Used(maf_int32 size);
	//
	maf_bool CheckFlag(maf_uint32 flag);
	//
	maf_int32 ClearFlag(maf_uint32 flag);
	maf_int32 ClearUsed();

private:
	maf_uint8* _buff;
	maf_int32 _off;
	maf_int32 _size;
	maf_int32 _max;
	maf_uint32 _flags;
};

