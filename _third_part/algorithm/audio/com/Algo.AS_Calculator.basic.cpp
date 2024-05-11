#if 1
#include"Algo.AS_Calculator.h"
#include"Algo.AS_Calculator.basic.h"

typedef b1(*AppendInFixPoint_CB)(AudioSamples& as, i32 num, i32 den);
b1 AS_Calculator::AppendInFixPoint(AudioSamples& as, i32 num, i32 den)
{
	return ((AppendInFixPoint_CB)_funcList->appendInFixPoint_cb)(as, num, den);
}

typedef b1(*LeftShift_CB)(AudioSamples& as, i32 startSample, i32 samples, i16 bits);
b1 AS_Calculator::LeftShift(AudioSamples& as, i32 startSample, i32 samples, i16 bits)
{
	return ((LeftShift_CB)_funcList->leftShift_cb)(as, startSample, samples, bits);
}

typedef b1(*LeftShiftSigned_CB)(AudioSamples& as, i32 startSample, i32 samples, i16 bits);
b1 AS_Calculator::LeftShiftSigned(AudioSamples& as, i32 startSample, i32 samples, i16 bits)
{
	return ((LeftShiftSigned_CB)_funcList->leftShiftSigned_cb)(as, startSample, samples, bits);
}

typedef b1(*RightShift_CB)(AudioSamples& as, i32 startSample, i32 samples, i16 bits);
b1 AS_Calculator::RightShift(AudioSamples& as, i32 startSample, i32 samples, i16 bits)
{
	return ((RightShift_CB)_funcList->RightShift_cb)(as, startSample, samples, bits);
}

typedef b1(*RightShiftSigned_CB)(AudioSamples& as, i32 startSample, i32 samples, i16 bits);
b1 AS_Calculator::RightShiftSigned(AudioSamples& as, i32 startSample, i32 samples, i16 bits)
{
	return ((RightShiftSigned_CB)_funcList->RightShiftSigned_cb)(as, startSample, samples, bits);
}






#endif