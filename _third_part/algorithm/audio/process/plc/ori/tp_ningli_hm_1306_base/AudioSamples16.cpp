#include"AudioSamples16.h"



bool AudioSamples16::Init(uint8_t* buff, int16_t width, int16_t channels, int32_t samples, int16_t fixPointNum)
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

bool AudioSamples16::Init(uint8_t* buff, int16_t channels, int32_t samples, int16_t fixPointNum)
{
	_width = sizeof(int16_t);
	return Init(buff, sizeof(int16_t), channels, samples, fixPointNum);
}