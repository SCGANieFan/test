#pragma once
#include"MTF.Type.h"
enum MTF_DataFlag {
	MTF_DataFlag_ESO=1<<0,
	MTF_DataFlag_EMPTY=1<<1,
};

class MTF_Data
{
public:
	MTF_Data();
	~MTF_Data();

public:
	mtf_int32 Init(mtf_uint8* buff, mtf_int32 len);
	mtf_int32 DeInit();
	mtf_int32 Append(mtf_uint8* buff, mtf_int32 len);
	mtf_void Clear();
	mtf_uint8* Data();
	mtf_uint8* LeftData();
	mtf_int32 LeftSize();
	mtf_int32 Used(mtf_int32 size);
public:
	mtf_int32 _size;
	mtf_int32 _flags;
private:
	mtf_uint8* _buff;
	mtf_int32 _off;
	mtf_int32 _max;
};

