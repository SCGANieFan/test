#pragma once
#if 1
#include"Algo.AudioSamlpes.h"
class AS_Calculator
{
public:
	AS_Calculator() {};
	~AS_Calculator() {};
public:
	virtual b1 AppendInFixPoint(AudioSamples& as, i32 num, i32 den) = 0;
	virtual b1 LeftShift(AudioSamples& as, i32 startSample, i32 samples, i16 bits) = 0;
	virtual b1 LeftShiftSigned(AudioSamples& as, i32 startSample, i32 samples, i16 bits) = 0;
	virtual b1 RightShift(AudioSamples& as, i32 startSample, i32 samples, i16 bits) = 0;
	virtual b1 RightShiftSigned(AudioSamples& as, i32 startSample, i32 samples, i16 bits) = 0;
public:
	//add
	//reduce
	//product
	virtual b1 Product(AudioSamples& dst, i32 dstSample, AudioSamples& src, i32 srcSample, i32 productSample) = 0;
	virtual b1 OverlapAdd(AudioSamples& dst, i32 dstSample, AudioSamples& src, i32 srcSample, i32 overlapSample) = 0;
	virtual i16 WaveFormMatch(AudioSamples& dst, i32 dstSample, AudioSamples& cmp, i32 cmpSample, i32 seekSample, i32 matchSample) = 0;
};

#endif