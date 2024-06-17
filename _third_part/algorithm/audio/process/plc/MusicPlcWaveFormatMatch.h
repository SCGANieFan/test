#pragma once
#include"Algo.BasePorting.h"
#include"Algo.Macro.h"
#include"Algo.Type.h"

#include"Algo.AudioCal.WaveFormMatch.h"

using namespace Algo;
using namespace Audio;

class MusicPlcWaveFormMatch_c
{
public:
	MusicPlcWaveFormMatch_c() {};
	~MusicPlcWaveFormMatch_c() {};
public:
	INLINE void Init(i32 seekSample, i32 matchSample, AudioInfo* info) {
		_info = info;
		_seekSample = seekSample;
		_matchSample= matchSample;
		_waveFormMatch = Get_Algo_WaveFormMatch(_info->_width);
	}

	INLINE i32 DoWaveFormMatch(u8* ref, u8* cmp) {
#if 1
		return _waveFormMatch(
			Algo_WaveformMatchChoose_e::ALGO_WAVEFORM_MATCH_ACCORELATION,
			ref,
			cmp,
			_info->_channels,
			_seekSample,
			_matchSample);
#endif
	}


private:
	ALGO_WAVE_FORM_MATCH_CB _waveFormMatch;
	i32 _seekSample;
	i32 _matchSample;
	AudioInfo *_info;
};

