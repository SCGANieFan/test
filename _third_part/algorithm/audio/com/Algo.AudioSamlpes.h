#pragma once
#include"Algo.AudioData.h"

class BufferSamples
{
public:
	BufferSamples() {};
	~BufferSamples() {};
public:
	u8* _buf = 0;
	i32 _samples = 0;
};

class AudioSamples:public AudioData
{
public:
	AudioSamples() {};
	~AudioSamples() {};
public:
#if 0
	INLINE AudioSamples& operator[](i32 sample) { return _buff[sample * _info->_bytesPerSample]; };
#endif
public:
	//set
	
	//get
	INLINE u8* GetBufInSample(i32 sample) { return &_buff[sample * _info->_bytesPerSample]; };
	INLINE i32 GetValidSamples() { return _validSamples; };
	INLINE i32 GetUsedSamples() { return _usedSamples; };
	INLINE i32 GetSamplesMax() { return _samples; };
	INLINE i32 GetLeftSamples() { return _samples - _usedSamples - _validSamples; };
	INLINE i32 GetSizeMax() { return _samplesTotal * _info->_width; };
	INLINE i16 GetFPNum() { return _fpNum; };
	INLINE b1 IsFull() { return GetValidSamples() == GetSamplesMax(); };

	//
	b1 Init(const AudioInfo* pInfo, Buffer *buffer, i16 fpNum = 0);
	b1 Init(const AudioInfo* pInfo, BufferSamples *buffer, i16 fpNum = 0);
	INLINE b1 Append(AudioSamples& src, i32 srcSample, i32 appendSample) {
		i32 copyByte = appendSample * _info->_bytesPerSample;
		ALGO_MEM_CPY(GetLeftData(), src.GetBufInSample(srcSample), copyByte);
		_validSamples += appendSample;
		_size += copyByte;
		return true;
	};
	INLINE b1 Append(i32 appendSample) {
		_validSamples += appendSample;
		_size += appendSample * _info->_bytesPerSample;
		return true;
	};
	INLINE b1 AppendFully(AudioSamples& src, i32 *usedSamples) {
		i32 AppendSamples;
		AppendSamples = GetSamplesMax() - GetValidSamples();
		AppendSamples = AppendSamples < src.GetValidSamples() ? AppendSamples : src.GetValidSamples();
		Append(src, src.GetUsedSamples(), AppendSamples);
		*usedSamples = AppendSamples;
		return true;
	};
	INLINE b1 Used(i32 usedSample) {
		_off += usedSample * _info->_bytesPerSample;
		_size -= usedSample * _info->_bytesPerSample;
		_validSamples -= usedSample;
		_usedSamples += usedSample;
		return true;
	};
	
	INLINE b1 ClearUsed() { _usedSamples = 0; Data::ClearUsed(); return true; };
	INLINE b1 Clear(i32 usedSample) { Used(usedSample); ClearUsed(); return true; };
protected:
	i16 _fpNum = 0;	
	i32 _samples = 0;
	i32 _validSamples = 0;
	i32 _usedSamples = 0;
	i32 _samplesTotal = 0;//_samples * channel
};








