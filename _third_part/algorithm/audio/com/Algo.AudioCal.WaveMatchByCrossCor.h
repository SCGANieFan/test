#pragma once
#include"Algo.Math.h"
#include"Algo.AudioCal.Com.h"
#include"Algo.AudioCal.Accorelation.h"
#include"Algo.AudioData.h"

#if 1

namespace Algo {
namespace Audio {
	
template<class Tref, class Tcmp, class Tnorm, int _rShitf = 0>
class WaveMatchByCrossCor_c
{
public:
	WaveMatchByCrossCor_c() {}
	~WaveMatchByCrossCor_c() {}

public:
	static i32 RunAllCh(Tref* ref, Tcmp* cmp, i16 channels, i32 seekSample, i32 matchSample) {
		i32 lagOpt = 0;
		Tnorm norm = 0;
		Tnorm bestCorr = 0;
		i32 accorrelationSample = matchSample;
		Tref* pRef = (Tref*)ref;
		bestCorr = CrossCorr_t<Tref,Tcmp,Tnorm,_rShitf>::runAllCh(ref, cmp, channels, accorrelationSample, &norm);
		for (i32 s = 1; s < seekSample; s++) {
			pRef += channels;
			Tnorm corr;
			corr = CrossCorrAccumulate_t<Tref,Tcmp,Tnorm,_rShitf>::RunAllCh(pRef, cmp, channels, accorrelationSample, &norm);
			if (bestCorr < corr) {
				bestCorr = corr;
				lagOpt = s;
			}
		}
		return lagOpt;
	}

	static i32 RunCh(Tref* ref, Tcmp* cmp, i16 channels, u16 channelSelect, i32 seekSample, i32 matchSample) {
		i32 lagOpt = 0;
		Tnorm norm = 0;
		Tnorm bestCorr = 0;
		i32 accorrelationSample = matchSample;
		Tref* pRef = (Tref*)ref;
		bestCorr = CrossCorr_t<Tref, Tcmp, Tnorm, _rShitf>::runCh(ref, cmp, channels, channelSelect, accorrelationSample, &norm);
		for (i32 s = 1; s < seekSample; s++) {
			pRef += channels;
			Tnorm corr;
			corr = CrossCorrAccumulate_t<Tref, Tcmp, Tnorm, _rShitf>::RunCh(pRef, cmp, channels, channelSelect, accorrelationSample, &norm);
			if (bestCorr < corr) {
				bestCorr = corr;
				lagOpt = s;
			}
		}
		return lagOpt;
	}
private:
};

}
}


#endif