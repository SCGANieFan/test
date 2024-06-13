#pragma once

#include<stdint.h>

class AudioSamples
{
public:
	AudioSamples();
	~AudioSamples();

public:

private:

public:
	uint8_t& operator[](int32_t sample);
public:
	//get
	int32_t GetSizeInByte();
	int32_t GetSamples();
	int16_t GetWidth();
	int16_t GetFixPointNum();
	int32_t GetValidSamples();
	int16_t GetChannels();


	bool Move(int32_t moveSample);
	void Clear();
	bool Append(uint8_t* src, int32_t srcSample);
	void used(int32_t usedSample);



	virtual bool Init(uint8_t* buff, int16_t channels, int32_t samples, int16_t fixPointNum = 0) = 0;
	virtual bool ProductWithEverySamlpe(AudioSamples& src, int32_t productSample) = 0;
	virtual bool LeftShift(int16_t bits, bool isSigned) = 0;
	virtual bool RightShift(int16_t bits, bool isSigned) = 0;
	virtual bool AppendInFixPoint(int32_t num, int32_t den) = 0;
	virtual bool OverlapAdd(uint8_t* src, int32_t overlapSample) = 0;
	virtual int16_t WaveFormMatch(uint8_t* comp, int32_t seekSample, int32_t matchSample) = 0;
protected:
	bool Init(uint8_t* buff, int16_t width, int16_t channels, int32_t samples, int16_t fixPointNum);

protected:
	uint8_t* _buff;
	int16_t _width;
	int16_t _channels;
	int16_t _fixPointNum;
	int32_t _samples;
	int32_t _startSamples;
	int32_t _validSamples;
	int32_t _sizeInByte;
};




