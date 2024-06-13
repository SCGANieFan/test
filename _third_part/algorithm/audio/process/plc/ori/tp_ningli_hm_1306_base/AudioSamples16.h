#pragma once

#include<stdint.h>
#include"AudioSamples.h"
class AudioSamples16:public AudioSamples
{
public:
	AudioSamples16();
	~AudioSamples16();

public:
	virtual bool Init(uint8_t* buff, int16_t channels, int32_t samples, int16_t fixPointNum = 0) override;
	virtual bool ProductWithEverySamlpe(AudioSamples& src, int32_t productSample) override;
	virtual bool LeftShift(int16_t bits, bool isSigned) override;
	virtual bool RightShift(int16_t bits, bool isSigned) override;
	virtual bool AppendInFixPoint(int32_t num, int32_t den) override;
	virtual bool OverlapAdd(uint8_t* src, int32_t overlapSample) override;
	virtual int16_t WaveFormMatch(uint8_t* comp, int32_t seekSample, int32_t matchSample) override;
public:

private:

};




