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
	INLINE void SetDir(DirChoose_e dir, i16 ch) { _dir[ch] = dir; }
	INLINE void SetDir(DirChoose_e dir) { SetDir(dir, 0); }
	INLINE void Sync(Muter_c dst, i16 ch) {
		//_muteSamplesNow = dst._muteSamplesNow * _muteSamplesMax / dst._muteSamplesMax;
		_muteSamplesNow[ch] = Math_c::Division(dst._muteSamplesNow[ch] * _muteSamplesMax, dst._muteSamplesMax);
	}
	INLINE void Sync(Muter_c dst) {
		Sync(dst, 0);
	}
	INLINE void Reset(DirChoose_e dir, i16 ch) {
		_dir[ch] = dir;
		if (_dir[ch] == DirChoose_e::ATTENUATION)
			_muteSamplesNow[ch] = 0;
		else if (_dir[ch] == DirChoose_e::AMPLIFICATION)
			_muteSamplesNow[ch] = _muteSamplesMax;
	}
	INLINE void Reset(DirChoose_e dir) {
		Reset(dir, 0);
	}
	INLINE b1 IsMuteFinish(i16 ch) {
		if (_dir[ch] == DirChoose_e::ATTENUATION)
			return (b1)(_muteSamplesNow[ch] >= _muteSamplesMax);
		else if (_dir[ch] == DirChoose_e::AMPLIFICATION)
			return (b1)(_muteSamplesNow[ch] <= 0);
		return false;
	}
	INLINE b1 IsMuteFinish() {
		return IsMuteFinish(0);
	}
	void Init(MemoryManger_c* MM, AudioInfo* info, WindowChoose_e windowChoose, DirChoose_e dirChoose, i32 decaySamples);

	INLINE void DoMute(void* src, const i32 productSample, i16 ch) {
#if 1
#if 0
		{static int num = 0; 
			printf("(%s)[%d]<%d> %d,%d,%d\n", __func__, __LINE__, num++,
				ch, _dir[ch],_muteSamplesNow[ch]);
		}
#endif
		if (_dir[ch] == DirChoose_e::ATTENUATION) {
			i32 doMuteSamples = _muteSamplesMax - _muteSamplesNow[ch];
			doMuteSamples = MIN(doMuteSamples, productSample);
			_productCh(
				src,
				src,
				&_muteFactorQ14[_muteSamplesNow[ch]],
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
			_muteSamplesNow[ch] += productSample;
			_muteSamplesNow[ch] = MIN(_muteSamplesNow[ch], _muteSamplesMax);
		}
		else {
			i32 doMuteSamples = _muteSamplesNow[ch];
			doMuteSamples = MIN(doMuteSamples, productSample);
			_productCh(
				src,
				src,
				&_muteFactorQ14[_muteSamplesNow[ch] - 1],
				doMuteSamples,
				_info->_channels,
				_facFpNum,
				-1);
			_muteSamplesNow[ch] -= productSample;
			_muteSamplesNow[ch] = MAX(_muteSamplesNow[ch], 0);
		}
#endif
	}

	INLINE void DoMute(void* src, const i32 productSample) {
#if 1
		if (_dir[0] == DirChoose_e::ATTENUATION) {
			i32 doMuteSamples = _muteSamplesMax - _muteSamplesNow[0];
			doMuteSamples = MIN(doMuteSamples, productSample);
			_productAllCh(
				src,
				src,
				&_muteFactorQ14[_muteSamplesNow[0]],
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
			_muteSamplesNow[0] += productSample;
			_muteSamplesNow[0] = MIN(_muteSamplesNow[0], _muteSamplesMax);
		}
		else {
			i32 doMuteSamples = _muteSamplesNow[0];
			doMuteSamples = MIN(doMuteSamples, productSample);
			_productAllCh(
				src,
				src,
				&_muteFactorQ14[_muteSamplesNow[0] - 1],
				doMuteSamples,
				_info->_channels,
				_facFpNum,
				-1);
			_muteSamplesNow[0] -= productSample;
			_muteSamplesNow[0] = MAX(_muteSamplesNow[0], 0);
		}
#endif
	}

private:
	PRODUCT _productCh;
	PRODUCT _productAllCh;
	i16* _muteFactorQ14;
	i32 _muteSamplesNow[16];
	i32 _muteSamplesMax;
	DirChoose_e _dir[16];
	i8 _facFpNum;
	AudioInfo* _info;
};
}
}

#endif