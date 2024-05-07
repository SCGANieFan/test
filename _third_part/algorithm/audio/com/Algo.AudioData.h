#pragma once

#include"Algo.Type.h"
#include"Algo.Data.h"


class AudioInfo
{
public:
	AudioInfo() {};
	~AudioInfo() {};
public:
	b1 Init(i32 rate, i16 width, i16 channels) {
		_rate = rate;
		_width = width;
		_channels = channels;
		_bytesPerSample = _width * _channels;
		return true;
	};
public:
	i32 _rate = 0;
	i16 _width = 0;
	i16 _channels = 0;
	i32 _bytesPerSample = 0;
};



class AudioData :public Data
{
public:
	AudioData() {};
	~AudioData() {};
public:
	virtual b1 Init(const AudioInfo* pInfo,u8* buf,i32 max);
protected:
	const AudioInfo *_info = 0;
};







