#pragma once
#include"Algo.AudioData.h"
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
	i32 GetSamplesMax();
	i32 GetSizeMax();

	//
	b1 Init(const AudioInfo* pInfo, u8* buf, i32 samples, i16 fpNum = 0);
	b1 Append(AudioSamples& src, i32 srcSample, i32 appendSample);
	b1 Used(i32 usedSample);
	b1 ClearUsed();
	b1 Clear(i32 usedSample);
protected:
	i16 _fpNum = 0;	
	i32 _samples = 0;
	i32 _validSamples = 0;
	i32 _samplesTotal = 0;//_samples * channel
};








