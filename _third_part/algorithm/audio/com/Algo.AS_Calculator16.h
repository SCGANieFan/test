#pragma once
#if 1
#include"Algo.AS_Calculator.h"
class AS_Calculator16:public AS_Calculator
{
public:
	AS_Calculator16() {};
	~AS_Calculator16() {};
public:
	virtual b1 AppendInFixPoint(AudioSamples& as, i32 num, i32 den) final;
	virtual b1 LeftShift(AudioSamples &as, i32 startSample, i32 samples, i16 bits) final;
	virtual b1 LeftShiftSigned(AudioSamples& as, i32 startSample, i32 samples, i16 bits) final;
	virtual b1 RightShift(AudioSamples& as, i32 startSample, i32 samples, i16 bits) final;
	virtual b1 RightShiftSigned(AudioSamples& as, i32 startSample, i32 samples, i16 bits) final;
public:
	virtual b1 Product(AudioSamples& dst, i32 dstSample, AudioSamples& src, i32 srcSample, i32 productSample) final;
	virtual b1 OverlapAdd(AudioSamples& dst, i32 dstSample, AudioSamples& src, i32 srcSample, i32 overlapSample) final;
	virtual i16 WaveFormMatch(AudioSamples& dst, i32 dstSample, AudioSamples& cmp, i32 cmpSample, i32 seekSample, i32 matchSample) final;
};
#endif