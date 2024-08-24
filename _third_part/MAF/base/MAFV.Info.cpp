#pragma once
#include"MAFV.Info.h"
#include"MAF.String.h"



MAFV_Info::MAFV_Info()
{
}
MAFV_Info::~MAFV_Info()
{
}


maf_int32 MAFV_Info::Set(const maf_int8* key, maf_void* val)
{
#if 0
	if (MAF_String::StrCompare(key, "rate")) {
		_rate = (maf_int32)val;
		_frameSamples = _rate * _frameMs / 1000;
		_frameBytes = _frameSamples * _width * _ch;
		return 0;
	}
	else if (MAF_String::StrCompare(key, "fMs")) {
		_frameMs = (maf_int32)val;
		_frameSamples = _rate * _frameMs / 1000;
		_frameBytes = _frameSamples * _width * _ch;
		return 0;
	}
	else if (MAF_String::StrCompare(key, "fSamples")) {
		_frameSamples = (maf_int32)val;
		_frameMs = _frameSamples * 1000 / _rate;
		_frameBytes = _frameSamples * _width * _ch;
		return 0;
	}
	else if (MAF_String::StrCompare(key, "ch")) {
		_ch = (maf_int8)val; 
		_frameBytes = _frameSamples * _width * _ch;
		return 0;
	}
	else if (MAF_String::StrCompare(key, "width")) {
		_width = (maf_int8)val;
		_frameBytes = _frameSamples * _width * _ch;
		return 0;
	}
	else if (MAF_String::StrCompare(key, "isInterlace")) {
		isInterlace = (maf_bool)val; return 0;
	}
	return -1;
#else
	return 0;
#endif
}

maf_int32 MAFV_Info::Get(const maf_int8* key, maf_void* val)
{
#if 0
	if (MAF_String::StrCompare(key, "rate")) {
		*(maf_uint32*)val = _rate; return 0;
	}
	else if (MAF_String::StrCompare(key, "fMs")) {
		*(maf_uint32*)val = _frameMs; return 0;
	}
	else if (MAF_String::StrCompare(key, "fSamples")) {
		*(maf_uint32*)val = _frameSamples; return 0;
	}
	else if (MAF_String::StrCompare(key, "ch")) {
		*(maf_uint32*)val = _ch; return 0;
	}
	else if (MAF_String::StrCompare(key, "width")) {
		*(maf_uint32*)val= _width; return 0;
	}
	else if (MAF_String::StrCompare(key, "isInterlace")) {
		*(maf_uint32*)val = isInterlace; return 0;
	}
	return -1;
#else
	return 0;
#endif
}
