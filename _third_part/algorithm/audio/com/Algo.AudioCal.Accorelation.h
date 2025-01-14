#pragma once
#include"Algo.Math.h"
#include"Algo.AudioCal.Com.h"
#include"Algo.AudioData.h"
#include"Algo.AudioCal.Basic.h"
#if 1

namespace Algo {
namespace Audio {

template<class Tref, class Ttmp, class Tnorm, int _rShift = 0>
struct CrossCorr_t{
	static inline Tnorm runAllCh(Tref* ref, Ttmp* temp, i16 channels, i32 accorrelationSample, Tnorm* normRef){
		Tnorm corr = 0;
		Tnorm normRefTmp = 0;
		Tnorm normTempTmp = 0;
		Tref* pRef = ref;
		Ttmp* pTemp = temp;
		for (i32 as = 0; as < accorrelationSample; ++as) {
			for (i16 ch = 0; ch < channels; ++ch) {
				Tnorm tmp = static_cast<Tnorm>(*pRef);
				corr += RShift_c::Run<_rShift>(tmp * *pTemp);
				normRefTmp += RShift_c::Run<_rShift>(tmp * tmp);
				++pRef;
				++pTemp;
			}
		}
		*normRef = normRefTmp;
		Tnorm t1 = Math_c<Tnorm>::Sqrt(normRefTmp);
		t1 = Math_c<Tnorm>::Division(corr, t1);
		return t1;
	}
	
	static inline Tnorm runCh(Tref* ref, Ttmp* temp, i16 channels, u16 chSelect, i32 accorrelationSample, Tnorm* normRef){
		Tnorm corr = 0;
		Tnorm normRefTmp = 0;
		for (i16 ch = 0; ch < channels; ++ch) {
			if (chSelect & (1 << ch)) {
				Tref* pRef = ref + ch;
				Ttmp* pTemp = temp + ch;
				for (i32 as = 0; as < accorrelationSample; ++as) {
					Tnorm tmp = static_cast<Tnorm>(*pRef);
					corr += RShift_c::Run<_rShift>( tmp * *pTemp);
					normRefTmp += RShift_c::Run<_rShift>(tmp * tmp);
					pRef += channels;
					pTemp += channels;
				}
			}
		}
		*normRef = normRefTmp;
		Tnorm t1 = Math_c<Tnorm>::Sqrt(*normRef);
		t1 = Math_c<Tnorm>::Division(corr, t1);
		return t1;
	}
};


template<class Tref, class Ttmp, class Tnorm, int _rShift = 0>
struct CrossCorrAccumulate_t {
	static INLINE Tnorm RunAllCh(Tref* ref, Ttmp* temp, i16 channels, i32 accorelationSamples, Tnorm* norm) {
		Tnorm corr;
		i16 i;
		Tref* pRef = ref;
		Ttmp* pTmp = temp;
		for (i = -channels; i < 0 * channels; ++i) {
			Tnorm tmp = static_cast<Tnorm>(pRef[i]);
			*norm -= RShift_c::Run<_rShift>(tmp*tmp);
		}
		for (i = (accorelationSamples - 1) * channels; i < accorelationSamples * channels; ++i) {
			Tnorm tmp = static_cast<Tnorm>(pRef[i]);
			*norm += RShift_c::Run<_rShift>(tmp * tmp);
		}
		corr = 0;
		for (i = 0; i < accorelationSamples * channels; ++i) {
			Tnorm tmp = static_cast<Tnorm>(pRef[i]);
			corr += RShift_c::Run<_rShift>(tmp * pTmp[i]);
		}
		Tnorm t1 = Math_c<Tnorm>::Sqrt(*norm);
		t1 = Math_c<Tnorm>::Division(corr, t1);
		return t1;
	}

	static INLINE Tnorm RunCh(Tref* ref, Ttmp* temp, i16 channels, u16 chSelect, i32 accorelationSamples, Tnorm* norm) {
		Tnorm corr;
		i16 i;
		//last sample
		corr = 0;
		for (i16 ch = 0; ch < channels; ++ch) {
			if (chSelect & (1 << ch)) {
				Tref* pRef = ref + ch;
				Ttmp* pTmp = temp + ch;
				i = -channels;
				Tnorm tmp = static_cast<Tnorm>(pRef[i]);
				*norm -= RShift_c::Run<_rShift>(tmp * tmp);
				tmp = static_cast<Tnorm>(pRef[i + accorelationSamples * channels]);
				*norm += RShift_c::Run<_rShift>(tmp * tmp);
				for (i = 0; i < accorelationSamples; ++i) {
					tmp = static_cast<Tnorm>(pRef[i]);
					corr += RShift_c::Run<_rShift>(tmp * pTmp[i]);
					pRef += channels;
					pTmp += channels;
				}
			}
		}
		Tnorm t1 = Math_c<Tnorm>::Sqrt(*norm);
		t1 = Math_c<Tnorm>::Division(corr, t1);
		return t1;
	}
};


}
}


#endif