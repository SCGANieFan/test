#if 0
#include"AudioSamplesCom.h"
#include"AudioSamples.h"



AudioSamples::AudioSamples()
{

}
AudioSamples::~AudioSamples()
{

}




uint8_t& AudioSamples::operator[](int32_t sample)
{
	return _buff[sample * _width * _channels];
}


int32_t AudioSamples::GetSizeInByte()
{
	return _sizeInByte;
}

int32_t AudioSamples::GetSamples()
{
	return _samples;
}

int16_t AudioSamples::GetWidth()
{
	return _width;
}

int16_t AudioSamples::GetFixPointNum()
{
	return _fixPointNum;
}

int32_t AudioSamples::GetValidSamples()
{
	return _validSamples;
}


int16_t AudioSamples::GetChannels()
{
	return _channels;
}

bool AudioSamples::Init(uint8_t* buff, int16_t width, int16_t channels, int32_t samples, int16_t fixPointNum)
{
	if (!buff)
		return false;
	if (width != 2
		&& width != 4)
	{
		return false;
	}
	if (channels < 1
		|| channels > 4)
		return false;
	if (samples < 1)
		return false;
	_buff = buff;
	_width = width;
	_channels = channels;
	_samples = samples;
	_startSamples = 0;
	_validSamples = 0;
	_fixPointNum = fixPointNum;
	_sizeInByte = _width * _channels * _samples;
	return true;
}

bool AudioSamples::Move(int32_t moveSample)
{
	//check
	if (moveSample < 0)
	{
		//left move
		moveSample = -moveSample;
		int32_t copyByte = (_startSamples + _validSamples - moveSample) * _width * _channels;
		uint8_t* dst = _buff;
		uint8_t* src = _buff + moveSample * _width * _channels;

		if (copyByte > 0) {
			AS_MCM_MOVE(dst, src, copyByte);
		}

		if (moveSample > _startSamples)
		{
			_startSamples = 0;
			_validSamples -= (moveSample - _startSamples);
			_validSamples = _validSamples > 0 ? _validSamples : 0;
		}
		else
		{
			_startSamples -= moveSample;
		}
	}
	else
	{
		//right move
		//not support
		return false;
	}
	return true;
}


void AudioSamples::Clear()
{
	_startSamples = 0;
	_validSamples = 0;
}

bool AudioSamples::Append(uint8_t* src, int32_t srcSample)
{
	//check
	uint8_t* pBuf = _buff + (_startSamples + _validSamples) * _width * _channels;
	int32_t copySize = srcSample * _width * _channels;
	AS_MCM_CPY(pBuf, src, copySize);
	_validSamples += srcSample;
	return true;
}

void AudioSamples::used(int32_t usedSample)
{
	_validSamples -= usedSample;
	_validSamples = _validSamples > 0 ? _validSamples : 0;
	_startSamples += usedSample;
	_startSamples = _startSamples < _samples ? _startSamples : _samples;
}
#endif