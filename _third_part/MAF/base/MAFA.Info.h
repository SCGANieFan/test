#pragma once
#include"MAF.Type.h"
//MAF audio info
class MAFA_Info
{
public:
	MAFA_Info();
	~MAFA_Info();
public:
	maf_int32 Set(const maf_int8* key, maf_void* val);
	maf_int32 Get(const maf_int8* key, maf_void* val);
public:
	maf_int32 _rate = 0;
	maf_int32 _frameMs = 0;
	maf_int32 _frameSamples = 0;
	maf_int32 _frameBytes = 0;
	maf_int8 _ch = 0;
	maf_int8 _width = 0;
	maf_bool isInterlace = true;
};

