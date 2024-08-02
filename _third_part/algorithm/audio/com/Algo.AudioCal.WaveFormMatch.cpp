#pragma once
#include"Algo.Math.h"
#include"Algo.AudioCal.WaveFormMatch.h"
#if 1

using namespace Algo;
using namespace Audio;


template<class T>
INLINE i64 Sum(T* ref, T* temp, i16 channels, i32 seekSample, i32 matchSample)
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
i32 WaveFormMatch_Sum_Local(void* ref, void* cmp, i16 channels, i32 seekSample, i32 matchSample) {
	i64 matchFactor = 0;
	i64 matchFactorOpt = 0;
	i32 lagOpt = 0;
	T* pRef = (T*)ref;
	T* pCmp = (T*)cmp;
	for (i16 s = 0; s < seekSample; s++) {
		pRef += channels;
		matchFactor = Sum(pRef, pCmp, channels, seekSample, matchSample);
		if (matchFactorOpt < matchFactor) {
			matchFactorOpt = matchFactor;
			lagOpt = s;
		}
	}
	return lagOpt;
}


void WaveFormMatch_c::Init(FuncMode_e mode, AudioInfo* info) {
	_info = info;
	
	if (mode == FuncMode_e::SUM) {
		if (_info->_width == 2) {
			_waveFormMatch_Sum = WaveFormMatch_Sum_Local<i16>;
		}
		else if (_info->_width == 3) {
			_waveFormMatch_Sum = WaveFormMatch_Sum_Local<i24>;
		}
		else if (_info->_width == 4) {
			_waveFormMatch_Sum = WaveFormMatch_Sum_Local<i32>;
		}
		_waveFormMatch = WaveFormMatch_Sum;
	}
	else if (mode == FuncMode_e::ACCORELATION) {
		_accorelation.Init(_info->_width, _info->_channels);
		_waveFormMatch = WaveFormMatch_Accorelation;
	}
}


i32 WaveFormMatch_c::WaveFormMatch_Accorelation(WaveFormMatch_c* hd, void* ref, void* cmp, i32 seekSample, i32 matchSample){
	return hd->_accorelation.DoCrossCorr(ref,cmp,seekSample,matchSample);
	
}

i32 WaveFormMatch_c::WaveFormMatch_Sum(WaveFormMatch_c* hd, void* ref, void* cmp, i32 seekSample, i32 matchSample) {
	return hd->_waveFormMatch_Sum(ref, cmp, hd->_info->_channels,seekSample, matchSample);
}
#endif