#pragma once
#include"Algo.AudioCal.Com.h"
#include"Algo.AudioData.h"
#include"Algo.AudioCal.Accorelation.h"
#if 1

namespace Algo {
namespace Audio {
class WaveFormMatch_c
{
public:
	enum class FuncMode_e {
		SUM = 0,
		ACCORELATION,
	};
private:
	typedef i32(*WaveFormMatch_Sum_CB)(void* ref, void* cmp, i16 channels, i32 seekSample, i32 matchSample);
	typedef i32(*WAVE_FORM_MATCH_CB)(WaveFormMatch_c* hd, void* ref, void* cmp, u16 chSelect, i32 seekSample, i32 matchSample);

public:
	WaveFormMatch_c() {};
	~WaveFormMatch_c() {};
public:
#if 0
	INLINE void Init(FuncMode_e mode, AudioInfo* info,i32 seekSample, i32 matchSample) {
			_info = info;
			_seekSample = seekSample;
			_matchSample = matchSample;
			_waveFormMatch = GetFunc(mode, _info->_width);
	}
#endif
	void Init(FuncMode_e mode, AudioInfo* info);
	INLINE i32 DoWaveFormMatch(void* ref, void* cmp, u16 chSelect, i32 seekSample, i32 matchSample) {
		return _waveFormMatch(
			this,
			ref,
			cmp,
			chSelect,
			seekSample,
			matchSample);
	}

	INLINE i32 DoWaveFormMatch(void* ref, void* cmp, i32 seekSample, i32 matchSample) {
		return _waveFormMatchAllCh(
			this,
			ref,
			cmp,
			0,
			seekSample,
			matchSample);
	}
private:
	STATIC i32 WaveFormMatch_Sum(WaveFormMatch_c* hd, void* ref, void* cmp, u16 chSelect, i32 seekSample, i32 matchSample);
	STATIC i32 WaveFormMatch_Accorelation(WaveFormMatch_c* hd, void* ref, void* cmp, u16 chSelect, i32 seekSample, i32 matchSample);
	STATIC i32 WaveFormMatch_Accorelation_AllCh(WaveFormMatch_c* hd, void* ref, void* cmp, u16 chSelect, i32 seekSample, i32 matchSample);
private:
	WAVE_FORM_MATCH_CB _waveFormMatch;
	WAVE_FORM_MATCH_CB _waveFormMatchAllCh;
	Accorelation_c _accorelation;
	WaveFormMatch_Sum_CB _waveFormMatch_Sum;
	AudioInfo* _info;


};
}
}


#endif