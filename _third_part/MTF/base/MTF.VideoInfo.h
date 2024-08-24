#pragma once
#include"MTF.Type.h"
class MTF_VideoInfo
{
public:
	MTF_VideoInfo();
	~MTF_VideoInfo();
public:
	mtf_int32 Set(const mtf_int8* key, mtf_void* val);
	mtf_int32 Get(const mtf_int8* key, mtf_void* val);
public:
	static mtf_int32 _rate;
	static mtf_int32 _frameMs;
	static mtf_int32 _frameSamples;
	static mtf_int32 _frameBytes;
	static mtf_int8 _ch;
	static mtf_int8 _width;
	static mtf_bool isInterlace;
};

