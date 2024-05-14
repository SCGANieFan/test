#include<math.h>
#include"Algo.AudioCal.WaveFormMatch.h"

using namespace Algo;
using namespace Audio;
#if 1
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
STATIC INLINE i32 CrossCorrAccumulate(T* refQ15, T* tempQ15, i16 channels, i32 accorelationSamples, u64* normQ30)
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
	return (i32)((corrQ30 / ((u64)sqrt((*normQ30 < 1) ? 1 : *normQ30))) >> 5);
}

template<class T>
STATIC INLINE i32 CrossCorr(T* refQ15, T* tempQ15, i16 channels, i32 accorrelationSample, u64* normQ30)
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
i16 Algo_WaveFormMatch(Algo_WaveformMatchChoose_e mode, u8* ref, u8* cmp, i16 channels, i32 seekSample, i32 matchSample)
{
	T* pRef = (T*)ref;
	T* pCmp = (T*)cmp;
	switch (mode)
	{
	case Algo_WaveformMatchChoose_e::ALGO_WAVEFORM_MATCH_SUM:
		return SumMatch_Local<T>(pRef, pCmp, channels, seekSample, matchSample);
		break;
	case Algo_WaveformMatchChoose_e::ALGO_WAVEFORM_MATCH_ACCORELATION:
		return AccorelationMatch_Local<T>(pRef, pCmp, channels, seekSample, matchSample);
		break;
	case Algo_WaveformMatchChoose_e::ALGO_WAVEFORM_MATCH_MAX:
		break;
	default:
		break;
	}
}

#endif



ALGO_WAVE_FORM_MATCH_CB Algo::Audio::Get_Algo_WaveFormMatch(i16 width)
{
	if (width == 2)
	{
		return Algo_WaveFormMatch<i16>;
	}
	else if (width == 3)
	{
		return Algo_WaveFormMatch<i24>;
	}
	else if (width == 4)
	{
		return Algo_WaveFormMatch<i32>;
	}
	return 0;
}


