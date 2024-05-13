#include"Algo.AS_CalculatorInner.h"

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
