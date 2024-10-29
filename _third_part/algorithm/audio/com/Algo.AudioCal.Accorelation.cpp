#pragma once
#include"Algo.Math.h"
#include"Algo.AudioCal.Accorelation.h"

#if 1

using namespace Algo;
using namespace Audio;

template<class T>
struct CrossCorrAccumulate_t;

template<>
struct CrossCorrAccumulate_t<i16> {
	static INLINE i32 Run(void* refQ15, void* tempQ15, i16 channels, i32 accorelationSamples, u64* normQ30)
	{
		i64 corrQ30;
		i16 i;
		i16* pRef = (i16*)refQ15;
		i16* pTmp = (i16*)tempQ15;
		//last sample
		for (i = -channels; i < 0 * channels; i++) {
			*normQ30 -= (u64)pRef[i] * pRef[i];
		}
		// last sample
		for (i = (accorelationSamples - 1) * channels; i < accorelationSamples * channels; i++) {
			*normQ30 += (u64)pRef[i] * pRef[i];
		}

		corrQ30 = 0;
		for (i = 0; i < accorelationSamples * channels; i++) {
			corrQ30 += (i64)pRef[i] * pTmp[i];
		}
		//outQ10
		u64 t1 = ((u64)Math_c::Sqrt((*normQ30 < 1) ? 1 : *normQ30));
		t1 = Math_c::Division(corrQ30, t1);
		return (i32)(t1 >> 5);
	}
};

template<>
struct CrossCorrAccumulate_t<i32> {
	static INLINE i32 Run(void* refQ31, void* tempQ31, i16 channels, i32 accorelationSamples, u64* normQ30)
	{
		i64 corrQ30;
		i16 i;
		i32* pRef = (i32*)refQ31;
		i32* pTmp = (i32*)tempQ31;
		//last sample
		for (i = -channels; i < 0 * channels; i++) {
			*normQ30 -= ((u64)pRef[i] * pRef[i]) >> 32;
		}
		// last sample
		for (i = (accorelationSamples - 1) * channels; i < accorelationSamples * channels; i++) {
			*normQ30 += ((u64)pRef[i] * pRef[i]) >> 32;
		}

		corrQ30 = 0;
		for (i = 0; i < accorelationSamples * channels; i++) {
			corrQ30 += ((i64)pRef[i] * pTmp[i])>>32;
		}
		//outQ10
		u64 t1 = ((u64)Math_c::Sqrt((*normQ30 < 1) ? 1 : *normQ30));
		t1 = Math_c::Division(corrQ30, t1);
		return (i32)(t1 >> 5);
	}
};


template<class T>
struct CrossCorr;

template<>
struct CrossCorr<i16> {
	static INLINE i32 Run(void* refQ15, void* tempQ15, i16 channels, i32 accorrelationSample, u64* normQ30)
	{
		i64 corrQ30;
		u64 lnormQ30;
		i32 i;
		corrQ30 = lnormQ30 = 0;
		i16* pRef = (i16*)refQ15;
		i16* pTemp = (i16*)tempQ15;
		for (i32 as = 0; as < accorrelationSample; as++) {
			for (i16 ch = 0; ch < channels; ch++) {
				corrQ30 += (i64)*pRef * *pTemp;
				lnormQ30 += (i64)*pRef * *pRef;
				pRef++;
				pTemp++;
			}
		}
		*normQ30 = lnormQ30;
		//out Q10
		u64 t1 = ((u64)Math_c::Sqrt((*normQ30 < 1) ? 1 : *normQ30));
		t1 = Math_c::Division(corrQ30, t1);
		return (i32)(t1 >> 5);
	}
};

template<>
struct CrossCorr<i32> {
	static INLINE i32 Run(void* refQ31, void* tempQ31, i16 channels, i32 accorrelationSample, u64* normQ30)
	{
		i64 corrQ30;
		u64 lnormQ30;
		i32 i;
		corrQ30 = lnormQ30 = 0;
		i32* pRef = (i32*)refQ31;
		i32* pTemp = (i32*)tempQ31;
		for (i32 as = 0; as < accorrelationSample; as++) {
			for (i16 ch = 0; ch < channels; ch++) {
				corrQ30 += ((i64)*pRef * *pTemp)>>32;
				lnormQ30 += ((i64)*pRef * *pRef)>>32;
				pRef++;
				pTemp++;
			}
		}
		*normQ30 = lnormQ30;
		//out Q10
		u64 t1 = ((u64)Math_c::Sqrt((*normQ30 < 1) ? 1 : *normQ30));
		t1 = Math_c::Division(corrQ30, t1);
		return (i32)(t1 >> 5);
	}
};


template<class T, int Ch>
struct Accorelation_t;

template<class T>
struct Accorelation_t<T, -1> {
	static i32 run(void* ref, void* cmp, i16 channels, i32 seekSample, i32 matchSample) {
		i32 lagOpt = 0;
		u64 normQ30 = 0;
		i32 bestCorrQ10 = 0;
		i32 accorrelationSample = matchSample;
		T* pRef = (T*)ref;
		bestCorrQ10 = CrossCorr<T>::Run(ref, cmp, channels, accorrelationSample, &normQ30);
		for (i32 s = 1; s < seekSample; s++) {
			pRef += channels;
			i32 corrQ10;
			corrQ10 = CrossCorrAccumulate_t<T>::Run(pRef, cmp, channels, accorrelationSample, &normQ30);
			if (bestCorrQ10 < corrQ10) {
				bestCorrQ10 = corrQ10;
				lagOpt = s;
			}
		}
		return lagOpt;
	}
};

void Accorelation_c::Init(i16 width, i16 channels){
	if(width==2){
		_Accorelation = Accorelation_t<i16,-1>::run;
	}
#if 0
	else if(width==3){
		_Accorelation = Accorelation_t<i24,-1>::run;
	}
#endif
	else if(width==4){
		_Accorelation = Accorelation_t<i32,-1>::run;
	}
	_channels = channels;
}



#endif