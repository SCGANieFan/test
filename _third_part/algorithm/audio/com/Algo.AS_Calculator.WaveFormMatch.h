#pragma once
#include"Algo.Type.h"
#include"Algo.Macro.h"

template<class T>
STATIC INLINE i64 Sum(T* ref, T* temp, i16 channels, i32 seekSample, i32 matchSample)
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
STATIC INLINE i16 SumMatch_Local(T* ref, T* temp, i16 channels, i32 seekSample, i32 matchSample)
{
	i64 matchFactor = 0;
	i64 matchFactorOpt = 0;
	i16 lagOpt = 0;
	auto pRef = ref;
	auto pTemp = temp;
	for (i16 s = 0; s < seekSample; s++) {
		//get matchFactor
		pRef = ref + s * channels;
		pTemp = temp;
		matchFactor = Sum(pRef, pTemp, channels, seekSample, matchSample);

		//choose best matchFactor
		if (matchFactorOpt < matchFactor) {
			matchFactorOpt = matchFactor;
			lagOpt = s;
		}
	}
	return lagOpt;
}

template<class T>
STATIC INLINE i32 CrossCorrAccumulate(T* refQ15, T* tempQ15, i16 channels, int32_t accorelationSamples, u64* normQ30)
{
	int64_t corrQ30;
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
		corrQ30 += (int64_t)refQ15[i] * tempQ15[i];
	}

	//outQ10
	return (i32)((corrQ30 / ((u64)sqrt((*normQ30 < 1) ? 1 : *normQ30))) >> 5);
}

template<class T>
STATIC INLINE i32 CrossCorr(T* refQ15, T* tempQ15, i16 channels, i32 accorrelationSample, u64* normQ30)
{
	int64_t corrQ30;
	uint64_t lnormQ30;
	int32_t i;
	corrQ30 = lnormQ30 = 0;
	for (i32 as = 0; as < accorrelationSample; as++)
	{
		for (i16 ch = 0; ch < channels; ch++)
		{
			corrQ30 += (int64_t)*refQ15 * *tempQ15;
			lnormQ30 += (int64_t)*refQ15 * *refQ15;
			refQ15++;
			tempQ15++;
		}
	}
	* normQ30 = lnormQ30;
	//out Q10
	return (i32)((corrQ30 / ((u64)sqrt((*normQ30 < 1) ? 1 : *normQ30))) >> 5);
}

template<class T>
STATIC INLINE i16 AccorelationMatch_Local(T* ref, T* temp, i16 channels, i32 seekSample, i32 accorrelationSample)
{
	i16 lagOpt = 0;
	u64 normQ30 = 0;
	i32 bestCorrQ10 = 0;
	auto pRef = ref;
	auto pTemp = temp;
	bestCorrQ10 = CrossCorr<T>(ref, pTemp, channels, accorrelationSample, &normQ30);
	//start 1
	for (i16 s = 1; s < seekSample; s++) {
		//get matchFactor
		pRef = ref + s * channels;
		pTemp = temp;

		i32 corrQ10;
		corrQ10 = CrossCorrAccumulate(pRef, pTemp, channels, accorrelationSample, &normQ30);
		if (bestCorrQ10 < corrQ10) {
			bestCorrQ10 = corrQ10;
			lagOpt = s;
		}
	}
	return lagOpt;
}

template<class T>
i16 WaveFormMatch_Local(AS_Calculator::WaveformMatchChoose_e mode, AudioSamples& dst, i32 dstSample, AudioSamples& cmp, i32 cmpSample, i32 seekSample, i32 matchSample)
{
	auto pRef = (T*)&dst[dstSample];
	auto pTemp = (T*)&cmp[cmpSample];
	switch (mode)
	{
	case AS_Calculator::WaveformMatchChoose_e::WAVEFORM_MATCH_SUM:
		return SumMatch_Local<T>(pRef, pTemp, dst._info->_channels, seekSample, matchSample);
		break;
	case AS_Calculator::WaveformMatchChoose_e::WAVEFORM_MATCH_ACCORELATION:
		return AccorelationMatch_Local<T>(pRef, pTemp, dst._info->_channels, seekSample, matchSample);
		break;
	case AS_Calculator::WaveformMatchChoose_e::WAVEFORM_MATCH_MAX:
		break;
	default:
		break;
	}
}
