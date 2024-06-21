#pragma once
#include"MTF.AudioInfo.h"
#include"MTF.String.h"


mtf_int32 MTF_AudioInfo::_rate = 0;
mtf_int32 MTF_AudioInfo::_frameMs = 0;
mtf_int32 MTF_AudioInfo::_frameSamples = 0;
mtf_int32 MTF_AudioInfo::_frameBytes = 0;
mtf_int8 MTF_AudioInfo::_ch = 0;
mtf_int8 MTF_AudioInfo::_width = 0;
mtf_bool MTF_AudioInfo::isInterlace = 0;

MTF_AudioInfo::MTF_AudioInfo()
{
}
MTF_AudioInfo::~MTF_AudioInfo()
{
}


mtf_int32 MTF_AudioInfo::Set(const mtf_int8* key, mtf_void* val)
{
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
}

mtf_int32 MTF_AudioInfo::Get(const mtf_int8* key, mtf_void* val)
{
#if 0
	return MTF_Process::Get(key, val);
#else
	return -1;
#endif
}
