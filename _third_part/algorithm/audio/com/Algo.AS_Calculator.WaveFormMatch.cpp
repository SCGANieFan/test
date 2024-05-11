#include"Algo.AS_Calculator.h"
#include"Algo.AS_Calculator.WaveFormMatch.h"


typedef b1(*WaveFormMatch_CB)(AS_Calculator::WaveformMatchChoose_e mode, AudioSamples& dst, i32 dstSample, AudioSamples& cmp, i32 cmpSample, i32 seekSample, i32 matchSample);
i16 AS_Calculator::WaveFormMatch(WaveformMatchChoose_e mode, AudioSamples& dst, i32 dstSample, AudioSamples& cmp, i32 cmpSample, i32 seekSample, i32 matchSample)
{
	return ((WaveFormMatch_CB)_funcList->waveFormMatch_cb)(mode,dst, dstSample, cmp, cmpSample, seekSample,matchSample);
}

