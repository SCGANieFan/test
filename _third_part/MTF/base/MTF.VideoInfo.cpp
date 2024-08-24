#pragma once
#include"MTF.VideoInfo.h"
#include"MTF.String.h"


mtf_int32 MTF_VideoInfo::_rate = 0;
mtf_int32 MTF_VideoInfo::_frameMs = 0;
mtf_int32 MTF_VideoInfo::_frameSamples = 0;
mtf_int32 MTF_VideoInfo::_frameBytes = 0;
mtf_int8 MTF_VideoInfo::_ch = 0;
mtf_int8 MTF_VideoInfo::_width = 0;
mtf_bool MTF_VideoInfo::isInterlace = 0;

MTF_VideoInfo::MTF_VideoInfo()
{
}
MTF_VideoInfo::~MTF_VideoInfo()
{
}


mtf_int32 MTF_VideoInfo::Set(const mtf_int8* key, mtf_void* val)
{
#if 0
	if (MTF_String::StrCompare(key, "rate")) {
		_rate = (mtf_int32)val;
		if (_frameMs)
			_frameSamples = _rate * _frameMs / 1000;
		else
			_frameMs = _frameSamples * 1000 / _rate;
		_frameBytes = _frameSamples * _width * _ch;
		return 0;
	}
	else if (MTF_String::StrCompare(key, "fMs")) {
		_frameMs = (mtf_int32)val;
		_frameSamples = _rate * _frameMs / 1000;
		_frameBytes = _frameSamples * _width * _ch;
		return 0;
	}
	else if (MTF_String::StrCompare(key, "fSamples")) {
		_frameSamples = (mtf_int32)val;
		if (_rate){
			_frameMs = _frameSamples * 1000 / _rate;
		}
		_frameBytes = _frameSamples * _width * _ch;
		return 0;
	}
	else if (MTF_String::StrCompare(key, "ch")) {
		_ch = (mtf_int8)val; 
		_frameBytes = _frameSamples * _width * _ch;
		return 0;
	}
	else if (MTF_String::StrCompare(key, "width")) {
		_width = (mtf_int8)val;
		_frameBytes = _frameSamples * _width * _ch;
		return 0;
	}
	else if (MTF_String::StrCompare(key, "isInterlace")) {
		isInterlace = (mtf_bool)val; return 0;
	}
	return -1;
#else
	return -1;
#endif
}

mtf_int32 MTF_VideoInfo::Get(const mtf_int8* key, mtf_void* val)
{
#if 0
	return MTF_Process::Get(key, val);
#else
	return -1;
#endif
}
