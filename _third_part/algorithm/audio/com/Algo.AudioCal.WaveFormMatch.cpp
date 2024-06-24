#pragma once
#include"Algo.AudioCal.WaveFormMatch.h"
#include<math.h>
#if 1

using namespace Algo;
using namespace Audio;

WaveFormMatch_c::WAVE_FORM_MATCH_CB WaveFormMatch_c::GetFunc(FuncMode_e mode, i16 width) {
	if (mode == FuncMode_e::SUM) {
		if (width == 2) {
			return WaveFormMatch_Sum<i16>;
		}
		else if (width == 3) {
			return WaveFormMatch_Sum<i24>;
		}
		else if (width == 4) {
			return WaveFormMatch_Sum<i32>;
		}
	}

	else if (mode == FuncMode_e::ACCORELATION) {
		if (width == 2) {
			return WaveFormMatch_Accorelation<i16>;
		}
		else if (width == 3) {
			return WaveFormMatch_Accorelation<i24>;
		}
		else if (width == 4) {
			return WaveFormMatch_Accorelation<i32>;
		}
	}
	return 0;
}
		
template<class T>
INLINE i64 WaveFormMatch_c::Sum(T* ref, T* temp, i16 channels, i32 seekSample, i32 matchSample)
{
	i64 matchFactor = 0;
	for (i16 m = 0; m < matchSample; m++) {
		for (i16 ch = 0; ch < channels; ch++) {
			matchFactor += ((i64)(*ref) * (*temp)) >> 8;
			ref++;
			temp++;
		}
	}
	return matchFactor;
}

template<class T>
i16 WaveFormMatch_c::WaveFormMatch_Sum(void* ref, void* cmp, i16 channels, i32 seekSample, i32 matchSample) {
	i64 matchFactor = 0;
	i64 matchFactorOpt = 0;
	i16 lagOpt = 0;
	T* pRef = (T*)ref;
	T* pCmp = (T*)cmp;
	for (i16 s = 0; s < seekSample; s++) {
		//get matchFactor
		//pRef = ref + s * channels;
		pRef += channels;
		//pCmp = cmp;
		matchFactor = Sum(pRef, pCmp, channels, seekSample, matchSample);
		//choose best matchFactor
		if (matchFactorOpt < matchFactor) {
			matchFactorOpt = matchFactor;
			lagOpt = s;
		}
	}
	return lagOpt;
}


template<class T>
INLINE i32 WaveFormMatch_c::CrossCorrAccumulate(T* refQ15, T* tempQ15, i16 channels, i32 accorelationSamples, u64* normQ30)
{
	i64 corrQ30;
	i16 i;

	//last sample
	for (i = -channels; i < 0 * channels; i++)
	{
		*normQ30 -= (u64)refQ15[i] * refQ15[i];
	}
	// last sample
	for (i = (accorelationSamples - 1) * channels; i < accorelationSamples * channels; i++)
	{
		*normQ30 += (u64)refQ15[i] * refQ15[i];
	}

	corrQ30 = 0;
	for (; i < accorelationSamples * channels; i++)
	{
		corrQ30 += (i64)refQ15[i] * tempQ15[i];
	}

	//outQ10
#if 0
	return (i32)((corrQ30 / ((u64)sqrt((*normQ30 < 1) ? 1 : *normQ30))) >> 5);
#else
	i64 t0 = (i64)sqrt((*normQ30 < 1) ? 1 : *normQ30);
	i64 t1 = corrQ30 / t0;
	i64 t2 = t1 >> 5;
	return (i32)t2;
#endif
}

template<class T>
INLINE i32 WaveFormMatch_c::CrossCorr(T* refQ15, T* tempQ15, i16 channels, i32 accorrelationSample, u64* normQ30)
{
	i64 corrQ30;
	u64 lnormQ30;
	i32 i;
	corrQ30 = lnormQ30 = 0;
	for (i32 as = 0; as < accorrelationSample; as++)
	{
		for (i16 ch = 0; ch < channels; ch++)
		{
			corrQ30 += (i64)*refQ15 * *tempQ15;
			lnormQ30 += (i64)*refQ15 * *refQ15;
			refQ15++;
			tempQ15++;
		}
	}
	*normQ30 = lnormQ30;
	//out Q10
#if 0
	return (i32)((corrQ30 / ((u64)sqrt((*normQ30 < 1) ? 1 : *normQ30))) >> 5);
#else
	i64 t0 = (i64)sqrt((*normQ30 < 1) ? 1 : *normQ30);
	i64 t1 = corrQ30 / t0;
	i64 t2 = t1 >> 5;
	return (i32)t2;
#endif
}


template<class T>
i16 WaveFormMatch_c::WaveFormMatch_Accorelation(void* ref, void* cmp, i16 channels, i32 seekSample, i32 matchSample) {
	i16 lagOpt = 0;
	u64 normQ30 = 0;
	i32 bestCorrQ10 = 0;
	i32 accorrelationSample = matchSample;
	T* pRef = (T*)ref;
	T* pCmp = (T*)cmp;
	bestCorrQ10 = CrossCorr<T>(pRef, pCmp, channels, accorrelationSample, &normQ30);
	//start 1
	for (i16 s = 1; s < seekSample; s++) {
		//get matchFactor
		//pRef = ref + s * channels;
		pRef += channels;
		//pCmp = cmp;

		i32 corrQ10;
		corrQ10 = CrossCorrAccumulate(pRef, pCmp, channels, accorrelationSample, &normQ30);
		if (bestCorrQ10 < corrQ10) {
			bestCorrQ10 = corrQ10;
			lagOpt = s;
		}
	}
	return lagOpt;
}




#endif