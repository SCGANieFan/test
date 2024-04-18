#include"AudioSample.h"

#include<stdio.h>
#include<string.h>



using namespace PlcCom;

AudioSample::AudioSample()
{

}
AudioSample::~AudioSample()
{

}




uint8_t& AudioSample::operator[](int32_t sample)
{
	return _buff[sample * _width * _channels];
}


int32_t AudioSample::GetSizeInByte()
{
	return _sizeInByte;
}

int32_t AudioSample::GetSamples()
{
	return _samples;
}

int16_t AudioSample::GetWidth()
{
	return _width;
}

int16_t AudioSample::GetFixPointNum()
{
	return _fixPointNum;
}

int32_t AudioSample::GetValidSamples()
{
	return _validSamples;
}


int16_t AudioSample::GetChannels()
{
	return _channels;
}

bool AudioSample::Init(uint8_t* buff, int16_t width, int16_t channels, int32_t samples, int16_t fixPointNum)
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

bool AudioSample::Move(int32_t moveSample)
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
			memmove(dst, src, copyByte);
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

bool AudioSample::ProductWithEverySamlpe(AudioSample& src, int32_t productSample)
{
#if 0
	if (productSample < 0)
		return false;
	if (productSample == 0)
		return true;

	if ((dstSample + productSample) > _samples)
		return false;
	if ((srcSample + productSample) > src._samples)
		return false;
	if (src._channels != 1)
		return false;
#endif
	switch (_width)
	{
	case 2:
		if (src.GetWidth() == 2)
		{
			for (int32_t s = 0; s < productSample; s++)
			{
				int16_t* p = (int16_t*)(_buff + (_startSamples + s) * _width * _channels);
				for (int16_t ch = 0; ch < _channels; ch++)
				{
					p[ch] = (((int64_t)p[ch]) * *((int16_t*)&src[s])) >> src.GetFixPointNum();
				}
			}
		}
		else
		{
			for (int32_t s = 0; s < productSample; s++)
			{
				int16_t* p = (int16_t*)(_buff + (_startSamples + s) * _width * _channels);
				for (int16_t ch = 0; ch < _channels; ch++)
				{
					p[ch] = (((int64_t)p[ch]) * *((int32_t*)&src[s])) >> src.GetFixPointNum();
				}
			}
		}
		break;
	case 4:
		if (src.GetWidth() == 2)
		{
			int16_t* pSrc = (int16_t*)&src[0];
			for (int32_t s = 0; s < productSample; s++)
			{
				int32_t* p = (int32_t*)(_buff + (_startSamples + s) * _width * _channels);
				for (int16_t ch = 0; ch < _channels; ch++)
				{
					p[ch] = (((int64_t)p[ch]) * *((int16_t*)&src[s])) >> src.GetFixPointNum();
				}
			}
		}
		else
		{
			int32_t* pSrc = (int32_t*)&src[0];
			for (int32_t s = 0; s < productSample; s++)
			{
				int32_t* p = (int32_t*)(_buff + (_startSamples + s) * _width * _channels);
				for (int16_t ch = 0; ch < _channels; ch++)
				{
					p[ch] = (((int64_t)p[ch]) * *((int32_t*)&src[s])) >> src.GetFixPointNum();
				}
			}
		}
		break;
	default:
		break;
	}
	return true;
}

bool AudioSample::LeftShift(int16_t bits, bool isSigned)
{
	if (bits < 0
		|| bits > 32)
		return false;
	if (bits < 1)
		return false;

	if (isSigned)
	{
		switch (_width)
		{
		case 2:
		{
			int16_t* pSrc = (int16_t*)_buff;
			for (int32_t s = 0; s < _samples * _channels; s++)
			{
				pSrc[s] = pSrc[s] << bits;
			}
			break;
		}
		case 4:
		{
			int32_t* pSrc = (int32_t*)_buff;
			for (int32_t s = 0; s < _samples * _channels; s++)
			{
				pSrc[s] = pSrc[s] << bits;
			}
			break;
		}
		}
	}
	else
	{
		switch (_width)
		{
		case 2:
		{
			uint16_t* pSrc = (uint16_t*)_buff;
			for (int32_t s = 0; s < _samples * _channels; s++)
			{
				pSrc[s] = pSrc[s] << bits;
			}
			break;
		}
		case 4:
		{
			uint32_t* pSrc = (uint32_t*)_buff;
			for (int32_t s = 0; s < _samples * _channels; s++)
			{
				pSrc[s] = pSrc[s] << bits;
			}
			break;
		}
		}
	}
	return true;
}

bool AudioSample::RightShift(int16_t bits, bool isSigned)
{
	if (bits < 0
		|| bits > 32)
		return false;
	if (bits < 1)
		return false;

	if (isSigned)
	{
		switch (_width)
		{
		case 2:
		{
			int16_t* pSrc = (int16_t*)_buff;
			for (int32_t s = 0; s < _samples * _channels; s++)
			{
				pSrc[s] = pSrc[s] >> bits;
			}
			break;
		}
		case 4:
		{
			int32_t* pSrc = (int32_t*)_buff;
			for (int32_t s = 0; s < _samples * _channels; s++)
			{
				pSrc[s] = pSrc[s] >> bits;
			}
			break;
		}
		}
	}
	else
	{
		switch (_width)
		{
		case 2:
		{
			uint16_t* pSrc = (uint16_t*)_buff;
			for (int32_t s = 0; s < _samples * _channels; s++)
			{
				pSrc[s] = pSrc[s] >> bits;
			}
			break;
		}
		case 4:
		{
			uint32_t* pSrc = (uint32_t*)_buff;
			for (int32_t s = 0; s < _samples * _channels; s++)
			{
				pSrc[s] = pSrc[s] >> bits;
			}
			break;
		}
		}
	}
	return true;
}

void AudioSample::Clear()
{
	_startSamples = 0;
	_validSamples = 0;
}

bool AudioSample::Append(uint8_t* src, int32_t srcSample)
{
	//check
	uint8_t* pBuf = _buff + (_startSamples + _validSamples) * _width * _channels;
	int32_t copySize = srcSample * _width * _channels;
	memcpy(pBuf, src, copySize);
	_validSamples += srcSample;
	return true;
}

bool AudioSample::AppendInFixPoint(int32_t num, int32_t den)
{
	uint8_t* pBuff = _buff + _validSamples * _width * _channels;
	_validSamples++;
	if (den == 0)
	{
		memset(pBuff, 0, _width * _channels);
		return true;
	}
	switch (_width)
	{
	case 2:
	{
		int16_t* psrc = (int16_t*)pBuff;
		int64_t fac = ((int64_t)num << _fixPointNum) / den;
		for (int16_t ch = 0; ch < _channels; ch++) {
			psrc[ch] = fac;
		}
		break;
	}
	case 4:
	{
		int32_t* psrc = (int32_t*)pBuff;
		int64_t fac = ((int64_t)num << _fixPointNum) / den;
		for (int16_t ch = 0; ch < _channels; ch++) {
			psrc[ch] = fac;
		}
		break;
	}
	default:
		break;
	}
	return true;
}

bool AudioSample::OverlapAdd(uint8_t* src, int32_t overlapSample)
{
	//check
	int32_t fixNum = 15;
	int32_t factor;
	switch (_width)
	{
	case 2:
	{
		int16_t* pBuf = (int16_t*)(_buff + _startSamples * _width * _channels);
		int16_t* pSrc = (int16_t*)src;
		for (int32_t s = 0; s < overlapSample; s++)
		{
			factor = ((int32_t)s << fixNum) / overlapSample;
			for (int16_t ch = 0; ch < _channels; ch++)
			{
				pBuf[s * _channels + ch] = ((int64_t)pBuf[s * _channels + ch] * (((int32_t)1 << fixNum) - factor)
					+ (int64_t)pSrc[s * _channels + ch] * factor) >> fixNum;
			}
		}
	}
	break;
	case 4:
	{
		int32_t* pBuf = (int32_t*)(_buff + _startSamples * _width * _channels);
		int32_t* pSrc = (int32_t*)src;
		for (int32_t s = 0; s < overlapSample; s++)
		{
			factor = ((int32_t)s << fixNum) / overlapSample;
			for (int16_t ch = 0; ch < _channels; ch++)
			{
				pBuf[s * _channels + ch] = ((int64_t)pBuf[s * _channels + ch] * (((int32_t)1 << fixNum) - factor)
					+ (int64_t)pSrc[s * _channels + ch] * factor) >> fixNum;
			}
		}
	}
	break;
	}



	return true;
}

void AudioSample::used(int32_t usedSample)
{
	_validSamples -= usedSample;
	_validSamples = _validSamples > 0 ? _validSamples : 0;
	_startSamples += usedSample;
	_startSamples = _startSamples < _samples ? _startSamples : _samples;
}