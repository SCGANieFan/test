#include"Algo.AudioSamlpes.h"



u8& AudioSamples::operator[](i32 sample)
{
	return _buff[sample * _info->_bytesPerSample];
}

i32 AudioSamples::GetValidSamples()
{
	return _validSamples;
}

i32 AudioSamples::GetSamplesMax()
{
	return _samples;
}

i32 AudioSamples::GetSizeMax()
{
	return _samplesTotal * _info->_width;
}

b1 AudioSamples::Init(const AudioInfo* pInfo, u8* buf, i32 samples, i16 fpNum)
{
	_samples = samples;
	_validSamples = 0;
	_samplesTotal = _samples * pInfo->_channels;
	_fpNum = fpNum;
	return AudioData::Init(pInfo, buf, _samplesTotal * pInfo->_width);
}

b1 AudioSamples::Append(AudioSamples& src, i32 srcSample, i32 appendSample)
{
	i32 copyByte = appendSample * _info->_bytesPerSample;
	ALGO_MEM_CPY(GetLeftData(), &src[srcSample], copyByte);
	_validSamples += appendSample;
	_size += copyByte;
	return true;
}

b1 AudioSamples::Used(i32 usedSample)
{
	_off += usedSample * _info->_bytesPerSample;
	_size -= usedSample * _info->_bytesPerSample;
	_validSamples -= usedSample;
	return true;
}

b1 AudioSamples::ClearUsed()
{
	Data::ClearUsed();
	return true;
}

b1 AudioSamples::Clear(i32 usedSample)
{
	Used(usedSample);
	ClearUsed();
	return true;
}
