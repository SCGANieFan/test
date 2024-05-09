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

class AudioSamples:protected AudioData
{
	friend class AS_Calculator16;
	friend class AS_Calculator32;
public:
	AudioSamples() {};
	~AudioSamples() {};
public:
	u8& operator[](i32 sample);
public:
	//set
	
	//get
	i32 GetValidSamples();
	i32 GetUsedSamples();
	i32 GetSamplesMax();
	i32 GetLeftSamples();
	i32 GetSizeMax();
	b1 IsFull();

	//
	virtual b1 Init(const AudioInfo* pInfo, Buffer *buffer, i16 fpNum = 0) final;
	b1 Init(const AudioInfo* pInfo, BufferSamples *buffer, i16 fpNum = 0);
	b1 Append(AudioSamples& src, i32 srcSample, i32 appendSample);
	b1 Append(i32 appendSample);
	b1 AppendFully(AudioSamples& src, i32 *usedSamples);
	b1 Used(i32 usedSample);
	b1 ClearUsed();
	b1 Clear(i32 usedSample);
protected:
	i16 _fpNum = 0;	
	i32 _samples = 0;
	i32 _validSamples = 0;
	i32 _usedSamples = 0;
	i32 _samplesTotal = 0;//_samples * channel
};








