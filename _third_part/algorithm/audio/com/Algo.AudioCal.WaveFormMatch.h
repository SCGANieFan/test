#pragma once
#include"Algo.AudioCal.Com.h"
#include"Algo.AudioData.h"
#include"Algo.AudioCal.WaveMatchByCrossCor.h"
#if 1

namespace Algo {
namespace Audio {

enum class WaveFormMatchFuncMode_e {
	WAVE_FORM_MATCH_FUNC_MODE_SUM = 0,
	WAVE_FORM_MATCH_FUNC_MODE_ACCORELATION,
};
template<class Tref, class Tcmp, class Tnorm, int _rShitf = 0>
class WaveFormMatch_c
{
public:
	WaveFormMatch_c() {}
	~WaveFormMatch_c() {}
public:
	INLINE void Init(WaveFormMatchFuncMode_e mode, AudioInfo* info) {
			_info = info;
			_waveFormMatchCh = WaveMatchByCrossCor_c<Tref, Tcmp, Tnorm, _rShitf>::RunCh;
			_waveFormMatchAllCh = WaveMatchByCrossCor_c<Tref, Tcmp, Tnorm, _rShitf>::RunAllCh;
	}
	INLINE i32 DoWaveFormMatchCh(Tref* ref, Tcmp* cmp, u16 chSelect, i32 seekSample, i32 matchSample) {
		return _waveFormMatchCh(
			ref,
			cmp,
			_info->_channels,
			chSelect,
			seekSample,
			matchSample);
	}

	INLINE i32 DoWaveFormMatchAllCh(Tref* ref, Tcmp* cmp, i32 seekSample, i32 matchSample) {
		return _waveFormMatchAllCh(
			ref,
			cmp,
			_info->_channels,
			seekSample,
			matchSample);
	}
#if 0
#endif
private:
#if 0
	STATIC i32 WaveFormMatch_Sum(WaveFormMatch_c* hd, void* ref, void* cmp, u16 chSelect, i32 seekSample, i32 matchSample);
	STATIC i32 WaveFormMatch_Accorelation(WaveFormMatch_c* hd, void* ref, void* cmp, u16 chSelect, i32 seekSample, i32 matchSample);
	STATIC i32 WaveFormMatch_Accorelation_AllCh(WaveFormMatch_c* hd, void* ref, void* cmp, u16 chSelect, i32 seekSample, i32 matchSample);
#endif
private:
#if 1
	i32 (*_waveFormMatchCh)(Tref* ref, Tcmp* cmp, i16 channels, u16 channelSelect, i32 seekSample, i32 matchSample);
	i32 (*_waveFormMatchAllCh)(Tref* ref, Tcmp* cmp, i16 channels, i32 seekSample, i32 matchSample);
#if 0
	WaveFormMatch_Sum_CB _waveFormMatch_Sum;
#endif
	AudioInfo* _info;
#endif

};
}
}


#endif