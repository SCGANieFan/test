#if 1
#pragma once
#include"Algo.Type.h"
#include"Algo.Macro.h"
namespace Algo {
	namespace Audio {
		enum class Algo_WaveformMatchChoose_e {
			ALGO_WAVEFORM_MATCH_SUM = 0,
			ALGO_WAVEFORM_MATCH_ACCORELATION,
			ALGO_WAVEFORM_MATCH_MAX,
		};

		typedef i16(*ALGO_WAVE_FORM_MATCH_CB)(Algo_WaveformMatchChoose_e mode, u8* ref, u8* cmp, i16 channels, i32 seekSample, i32 matchSample);

		ALGO_WAVE_FORM_MATCH_CB Get_Algo_WaveFormMatch(i16 width);
	}
}


#endif