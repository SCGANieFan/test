#pragma once
#include"Algo.AudioCal.Com.h"
#include"Algo.AudioData.h"
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
			typedef i16(*WAVE_FORM_MATCH_CB)(void* ref, void* cmp, i16 channels, i32 seekSample, i32 matchSample);

		public:
			WaveFormMatch_c() {};
			~WaveFormMatch_c() {};
		public:
			INLINE void Init(FuncMode_e mode, AudioInfo* info,i32 seekSample, i32 matchSample) {
					_info = info;
					_seekSample = seekSample;
					_matchSample = matchSample;
					_waveFormMatch = GetFunc(mode, _info->_width);
			}

			INLINE i32 DoWaveFormMatch(void* ref, void* cmp) {
				return _waveFormMatch(
					ref,
					cmp,
					_info->_channels,
					_seekSample,
					_matchSample);
			}
		private:
			STATIC WAVE_FORM_MATCH_CB GetFunc(FuncMode_e mode, i16 width);
			template<class T>
			STATIC i64 Sum(T* ref, T* temp, i16 channels, i32 seekSample, i32 matchSample);
			template<class T>
			STATIC i16 WaveFormMatch_Sum(void* ref, void* cmp, i16 channels, i32 seekSample, i32 matchSample);
			template<class T>
			STATIC i32 CrossCorrAccumulate(T* refQ15, T* tempQ15, i16 channels, i32 accorelationSamples, u64* normQ30);
			template<class T>
			STATIC i32 CrossCorr(T* refQ15, T* tempQ15, i16 channels, i32 accorrelationSample, u64* normQ30);
			template<class T>
			STATIC i16 WaveFormMatch_Accorelation(void* ref, void* cmp, i16 channels, i32 seekSample, i32 matchSample);
		private:
			WAVE_FORM_MATCH_CB _waveFormMatch;
			i32 _seekSample;
			i32 _matchSample;
			AudioInfo* _info;
		};

	}
}


#endif