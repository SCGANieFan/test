#if 0
#include"Algo.AS_CalculatorInner.h"
#include"Algo.AS_Calculator.Resample.h"


typedef b1(*Resample_CB)(AudioSamples& in, AudioSamples& out);
b1 AS_Calculator::Resample(AudioSamples& in, AudioSamples& out)
{
	return ((Resample_CB)((FuncList*)_funcList)->resample_cb)(in,out);
}
#endif