#include"Algo.AudioSamlpes.h"



u8& AudioSamples::operator[](i32 sample)
{
	return _buff[sample * _info->_bytesPerSample];
}

i32 AudioSamples::GetValidSamples()
{
	return _validSamples;
}

i32 AudioSamples::GetUsedSamples()
{
	return _usedSamples;
}

i32 AudioSamples::GetSamplesMax()
{
	return _samples;
}

i32 AudioSamples::GetLeftSamples()
{
	return _samples - _usedSamples - _validSamples;
}

i32 AudioSamples::GetSizeMax()
{
	return _samplesTotal * _info->_width;
}

b1 AudioSamples::IsFull()
{
	return GetValidSamples() == GetSamplesMax();
}


b1 AudioSamples::Init(const AudioInfo* pInfo, Buffer* buffer, i16 fpNum)
{
	_samples = buffer->_max / pInfo->_bytesPerSample;
	_validSamples = 0;
	_samplesTotal = _samples * pInfo->_channels;
	_fpNum = fpNum;
	return AudioData::Init(pInfo, buffer);
}

b1 AudioSamples::Init(const AudioInfo* pInfo, BufferSamples* bufferSamples, i16 fpNum)
{
	_samples = bufferSamples->_samples;
	_validSamples = 0;
	_samplesTotal = _samples * pInfo->_channels;
	_fpNum = fpNum;

	Buffer buffer;
	buffer._buf = bufferSamples->_buf;
	buffer._max = _samples * pInfo->_bytesPerSample;
	return AudioData::Init(pInfo, &buffer);
}

b1 AudioSamples::Append(AudioSamples& src, i32 srcSample, i32 appendSample)
{
	i32 copyByte = appendSample * _info->_bytesPerSample;
	ALGO_MEM_CPY(GetLeftData(), &src[srcSample], copyByte);
	_validSamples += appendSample;
	_size += copyByte;
	return true;
}
b1 AudioSamples::Append(i32 appendSample)
{
	_validSamples += appendSample;
	_size += appendSample * _info->_bytesPerSample;
	return true;
}

b1 AudioSamples::AppendFully(AudioSamples& src, i32* usedSamples)
{
	i32 AppendSamples;
	AppendSamples = GetSamplesMax() - GetValidSamples();
	AppendSamples = AppendSamples < src.GetValidSamples() ? AppendSamples : src.GetValidSamples();
	Append(src, src.GetUsedSamples(), AppendSamples);
	*usedSamples = AppendSamples;
	return true;
}

b1 AudioSamples::Used(i32 usedSample)
{
	_off += usedSample * _info->_bytesPerSample;
	_size -= usedSample * _info->_bytesPerSample;
	_validSamples -= usedSample;
	_usedSamples += usedSample;
	return true;
}

b1 AudioSamples::ClearUsed()
{
	_usedSamples = 0;
	Data::ClearUsed();
	return true;
}

b1 AudioSamples::Clear(i32 usedSample)
{
	Used(usedSample);
	ClearUsed();
	return true;
}
