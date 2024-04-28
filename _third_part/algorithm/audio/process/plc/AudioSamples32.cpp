#include"AudioSamplesCom.h"
#include"AudioSamples32.h"

AudioSamples32::AudioSamples32()
{

}
AudioSamples32::~AudioSamples32()
{

}


bool AudioSamples32::Init(uint8_t* buff, int16_t channels, int32_t samples, int16_t fixPointNum)
{
	_width = sizeof(int32_t);
	return AudioSamples::Init(buff, sizeof(int32_t), channels, samples, fixPointNum);
}

bool AudioSamples32::ProductWithEverySamlpe(AudioSamples& src, int32_t productSample)
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
	for (int32_t s = 0; s < productSample; s++)
	{
		int32_t* p = (int32_t*)(_buff + (_startSamples + s) * _width * _channels);
		for (int16_t ch = 0; ch < _channels; ch++)
		{
			p[ch] = (((int64_t)p[ch]) * *((int32_t*)&src[s])) >> src.GetFixPointNum();
		}
	}
	return true;
}
bool AudioSamples32::LeftShift(int16_t bits, bool isSigned)
{
	if (bits < 0
		|| bits > 32)
		return false;
	if (bits < 1)
		return false;

	if (isSigned)
	{
		int32_t* pSrc = (int32_t*)_buff;
		for (int32_t s = 0; s < _samples * _channels; s++)
		{
			pSrc[s] = pSrc[s] << bits;
		}
	}
	else
	{
		uint32_t* pSrc = (uint32_t*)_buff;
		for (int32_t s = 0; s < _samples * _channels; s++)
		{
			pSrc[s] = pSrc[s] << bits;
		}
	}
	return true;
}
bool AudioSamples32::RightShift(int16_t bits, bool isSigned)
{
	if (bits < 0
		|| bits > 32)
		return false;
	if (bits < 1)
		return false;

	if (isSigned)
	{
		int32_t* pSrc = (int32_t*)_buff;
		for (int32_t s = 0; s < _samples * _channels; s++)
		{
			pSrc[s] = pSrc[s] >> bits;
		}
	}
	else
	{
		uint32_t* pSrc = (uint32_t*)_buff;
		for (int32_t s = 0; s < _samples * _channels; s++)
		{
			pSrc[s] = pSrc[s] >> bits;
		}
	}
	return true;
}
bool AudioSamples32::AppendInFixPoint(int32_t num, int32_t den)
{
	uint8_t* pBuff = _buff + _validSamples * _width * _channels;
	_validSamples++;
	if (den == 0)
	{
		AS_MCM_SET(pBuff, 0, _width * _channels);
		return true;
	}
	int32_t* psrc = (int32_t*)pBuff;
	int64_t fac = ((int64_t)num << _fixPointNum) / den;
	for (int16_t ch = 0; ch < _channels; ch++) {
		psrc[ch] = fac;
	}
	return true;
}
bool AudioSamples32::OverlapAdd(uint8_t* src, int32_t overlapSample)
{
	//check
	int32_t fixNum = 15;
	int32_t factor;

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
	return true;
}


int16_t AudioSamples32::WaveFormMatch(uint8_t* comp, int32_t seekSample, int32_t matchSample)
{
	int64_t matchFactor = 0;
	int32_t matchFactorOpt = 0;
	int16_t lagOpt = 0;

	int32_t* pTempl = (int32_t*)_buff;
	int32_t* pComp = (int32_t*)comp;

	for (int16_t s = 0; s < seekSample; s++) {
		matchFactor = 0;
		for (int16_t m = 0; m < matchSample; m++) {
			for (int16_t ch = 0; ch < _channels; ch++) {
				matchFactor += ((int64_t)pTempl[m * _channels + ch] * pComp[(s + m) * _channels + ch]) >> 8;
			}
		}
		if (matchFactorOpt < matchFactor)
		{
			matchFactorOpt = matchFactor;
			lagOpt = s;
		}
	}

	return lagOpt;
}
