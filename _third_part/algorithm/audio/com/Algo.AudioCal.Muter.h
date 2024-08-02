#if 1
#pragma once
#include"Algo.AudioCal.Com.h"
#include"Algo.BasePorting.Inner.h"
#include"Algo.AudioData.h"
#include"Algo.Math.h"

namespace Algo {
	namespace Audio {
		class Muter_c
		{
		public:
			enum class WindowChoose_e {
				LINE = 0,
				COSINE,
			};
			enum class DirChoose_e {
				ATTENUATION = 0,
				AMPLIFICATION,
			};
		private:
			typedef b1(*PRODUCT)(void* dst, void* src, void* fac, const i32 productSample, const i8 channels, const i8 facFpNum, i8 facDir);
		public:
			Muter_c() {};
			~Muter_c() {};
		public:
			INLINE void SetDir(DirChoose_e dir) { _dir = dir; }
			INLINE void Sync(Muter_c dst) {
				//_muteSamplesNow = dst._muteSamplesNow * _muteSamplesMax / dst._muteSamplesMax;
				_muteSamplesNow = Math_c::DivisionLocal(dst._muteSamplesNow * _muteSamplesMax, dst._muteSamplesMax);
			}
			INLINE void Reset(DirChoose_e dir) {
				_dir = dir;
				if (_dir == DirChoose_e::ATTENUATION)
					_muteSamplesNow = 0;
				else if (_dir == DirChoose_e::AMPLIFICATION)
					_muteSamplesNow = _muteSamplesMax;
			}
			INLINE b1 IsMuteFinish() {
				if (_dir == DirChoose_e::ATTENUATION)
					return (b1)(_muteSamplesNow >= _muteSamplesMax);
				else if (_dir == DirChoose_e::AMPLIFICATION)
					return (b1)(_muteSamplesNow <= 0);
				return false;
			}

			void Init(MemoryManger_c* MM, AudioInfo* info, WindowChoose_e windowChoose, DirChoose_e dirChoose, i32 decaySamples);
			INLINE void DoMute(void* src, const i32 productSample) {
#if 1
				if (_dir == DirChoose_e::ATTENUATION) {
					i32 doMuteSamples = _muteSamplesMax - _muteSamplesNow;
					doMuteSamples = MIN(doMuteSamples, productSample);
					_product(
						src,
						src,
						&_muteFactorQ14[_muteSamplesNow],
						doMuteSamples,
						_info->_channels,
						_facFpNum,
						1);
					if (doMuteSamples < productSample) {
						ALGO_MEM_SET(
							((u8*)src + doMuteSamples * _info->_bytesPerSample),
							0,
							(productSample - doMuteSamples) * _info->_bytesPerSample);
					}
					_muteSamplesNow += productSample;
					_muteSamplesNow = MIN(_muteSamplesNow, _muteSamplesMax);
				}
				else {
					i32 doMuteSamples = _muteSamplesNow;
					doMuteSamples = MIN(doMuteSamples, productSample);
					_product(
						src,
						src,
						&_muteFactorQ14[_muteSamplesNow - 1],
						doMuteSamples,
						_info->_channels,
						_facFpNum,
						-1);
					_muteSamplesNow -= productSample;
					_muteSamplesNow = MAX(_muteSamplesNow, 0);
				}
#endif
			}
		private:
			PRODUCT _product;
			i16* _muteFactorQ14;
			i32 _muteSamplesNow;
			i32 _muteSamplesMax;
			DirChoose_e _dir;
			i8 _facFpNum;
			AudioInfo* _info;
		};
	}
}

#endif