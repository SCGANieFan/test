#if 1
#pragma once
#include"Algo.AudioCal.Com.h"
#include"Algo.BasePorting.Inner.h"
#include"Algo.AudioData.h"

namespace Algo {
	namespace Audio {
		class OverlapAdd_c
		{
		public:
			enum class WindowChoose {
				Line = 0,
				Cosine,
			};
		private:
			typedef b1(*OVERLAP_ADD_CORE_CB)(void* dst, void* srcDecline, void* srcRise, const i16 channels, i32* factorDecLine, const i32 startOverlapSample, const i32 endOverlapSample, i32 overlapSample);
		public:
			OverlapAdd_c() {};
			~OverlapAdd_c() {};
		public:
			INLINE b1 IsFinish() { return  b1(_overlapSamplesNow >= _overlapSamples); }
			INLINE void Start() { _overlapSamplesNow = 0; }
		public:
			INLINE b1 Init(MemoryManger_c* MM, AudioInfo* info, WindowChoose windowChoose, i32 overlapSamples) {
				_info = info;
				_overlapSamples = overlapSamples;
				_overlapSamplesNow = _overlapSamples;
				_overlapaddCore = OverlapAdd_c::GetFunc(_info->_width);
				_factor = (i32*)MM->Malloc(_overlapSamples * sizeof(i32));//1->0
				_fixNum = 15;
				GenerateWindow(windowChoose, _factor, _overlapSamples, _fixNum);
				return true;
			}

			INLINE void DoOverlapAdd(void* dst, void* srcDecline, void* srcRise, i32 doOverlapSamples) {
				i32 doOverlapSamples0 = _overlapSamples - _overlapSamplesNow;
				doOverlapSamples0 = MIN(doOverlapSamples0, doOverlapSamples);
				_overlapaddCore(
					dst,
					srcDecline,
					srcRise,
					_info->_channels,
					_factor,
					_overlapSamplesNow,
					_overlapSamplesNow + doOverlapSamples,
					_overlapSamples);
				if (doOverlapSamples > doOverlapSamples0) {
					ALGO_MEM_CPY(
						((u8*)dst + doOverlapSamples0 * _info->_bytesPerSample),
						((u8*)srcRise + doOverlapSamples0 * _info->_bytesPerSample),
						(doOverlapSamples - doOverlapSamples0) * _info->_bytesPerSample);
				}
				_overlapSamplesNow += doOverlapSamples0;
			}
		private:
			STATIC OVERLAP_ADD_CORE_CB GetFunc(i8 Width);
			STATIC void GenerateWindow(WindowChoose choose, i32* factor, i32 samples, i16 fpNum);
			template<class Ti, class To>
			STATIC b1 OverlapAdd_Core(void* dst, void* srcDecline, void* srcRise, const i16 channels, i32* factorDecLine, const i32 startOverlapSample, const i32 endOverlapSample, i32 overlapSample);
		private:
			AudioInfo* _info;
			OVERLAP_ADD_CORE_CB _overlapaddCore;
			i32 _overlapSamples;
			i32 _overlapSamplesNow;
			i32* _factor;
			i32 _fixNum = 15;
		};
	}
}
#endif