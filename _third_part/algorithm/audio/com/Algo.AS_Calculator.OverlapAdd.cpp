#if 1
#include"Algo.AS_Calculator.h"
#include"Algo.AS_Calculator.OverlapAdd.h"

typedef b1(*OverlapAdd_CB)(AudioSamples& dst, i32 dstSample, AudioSamples& src, i32 srcSample, i32 overlapSample);
b1 AS_Calculator::OverlapAdd(AudioSamples& dst, i32 dstSample, AudioSamples& src, i32 srcSample, i32 overlapSample)
{
	return ((OverlapAdd_CB)_funcList->overlapAdd_cb)(dst, dstSample, src, srcSample, overlapSample);
}


#endif