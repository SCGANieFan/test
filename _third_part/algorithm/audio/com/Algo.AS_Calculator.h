#pragma once
#if 1
#include"Algo.AudioSamlpes.h"
class AS_Calculator
{
public:

public:
	AS_Calculator() {};
	~AS_Calculator() {};
public:
	void Init(i16 width);
public:
	b1 AppendInFixPoint(AudioSamples& as, i32 num, i32 den);
	b1 LeftShift(AudioSamples& as, i32 startSample, i32 samples, i16 bits);
	b1 LeftShiftSigned(AudioSamples& as, i32 startSample, i32 samples, i16 bits);
	b1 RightShift(AudioSamples& as, i32 startSample, i32 samples, i16 bits);
	b1 RightShiftSigned(AudioSamples& as, i32 startSample, i32 samples, i16 bits);
public:
	b1 Product(AudioSamples& dst, i32 dstSample, AudioSamples& src, i32 srcSample, i32 productSample);
	b1 OverlapAdd(AudioSamples& dst, i32 dstSample, AudioSamples& src, i32 srcSample, i32 overlapSample);
	enum class WaveformMatchChoose_e {
		WAVEFORM_MATCH_SUM=0,
		WAVEFORM_MATCH_ACCORELATION,
		WAVEFORM_MATCH_MAX,
	};
	i16 WaveFormMatch(WaveformMatchChoose_e mode, AudioSamples& dst, i32 dstSample, AudioSamples& cmp, i32 cmpSample, i32 seekSample, i32 matchSample);
private:
	void*_funcList;
};

#endif