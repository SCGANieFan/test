#if 1
#pragma once
#include"Algo.AudioCal.Com.h"
#include"Algo.AudioCal.Product.h"
#include"Algo.AudioCal.BufferGenerator.h"
#include"Algo.BasePorting.Inner.h"
#include"Algo.AudioData.h"
#include"Algo.Math.h"

namespace Algo {
namespace Audio {

enum class MuterWindowChoose_e {
	MUTER_WINDOW_CHOOSE_LINE=0,
	MUTER_WINDOW_CHOOSE_COSINE,
};
enum class MuterDirChoose_e {
	MUTER_DIR_CHOOSE_ATTENUATION = 0,
	MUTER_DIR_CHOOSE_AMPLIFICATION,
};

#if 1
template<class T, i8 type=(TypeIdentify_c::IsI16<T>() ? 0 : (TypeIdentify_c::IsI32<T>() ? 1 : 2))>
class MuterProduct_c : public Product_c<T, T, T, T> {};
template<class T>
class MuterProduct_c<T, 0> : public Product_c<T, T, i16, i32, 14> {};
template<class T>
class MuterProduct_c<T, 1> : public Product_c<T, T, i16, i64, 14> {};
//template<class T>
//class MuterProduct_c<T, 1> : public Product_c<T, T, T, T> {};
#endif

template<class T>
class Muter_c
{
public:
	Muter_c() {}
	~Muter_c() {}
public:
	void Init(MemoryManger_c* MM, AudioInfo* info, MuterWindowChoose_e windowChoose, MuterDirChoose_e dirChoose, i32 decaySamples) {
		_info = info;
		_muteSamplesMax = decaySamples;
		for (i16 ch = 0; ch < info->_channels; ch++)
			Reset(dirChoose, ch);
		BufferGenerator_c::BufferChoose_e bufferChoose = BufferGenerator_c::BufferChoose_e::WINDOW_LINE_FADE;
		if (windowChoose == MuterWindowChoose_e::MUTER_WINDOW_CHOOSE_LINE)
			bufferChoose = BufferGenerator_c::BufferChoose_e::WINDOW_LINE_FADE;
		else if (windowChoose == MuterWindowChoose_e::MUTER_WINDOW_CHOOSE_COSINE)
			bufferChoose = BufferGenerator_c::BufferChoose_e::WINDOW_COSINE_FADE;
		if(TypeIdentify_c::IsFloat<T>()){
			_muteFactorWidth = sizeof(T);
			_muteFactor = (u8*)MM->Malloc(decaySamples * _muteFactorWidth);
			ALGO_MEM_SET(_muteFactor, 0, decaySamples * _muteFactorWidth);
			BufferGenerator_c::Generate<T>(bufferChoose, (T*)_muteFactor, _muteSamplesMax);
		}
		else {
			_facFpNum = 14;
			_muteFactorWidth = sizeof(i16);
			_muteFactor = (u8*)MM->Malloc(decaySamples * _muteFactorWidth);
			ALGO_MEM_SET(_muteFactor, 0, decaySamples * _muteFactorWidth);
			BufferGenerator_c::Generate<i16>(bufferChoose, (i16*)_muteFactor, _muteSamplesMax, _facFpNum);
		}
	}
	INLINE void SetDir(MuterDirChoose_e dir, i16 ch) { _dir[ch] = dir; }
	INLINE void SetDir(MuterDirChoose_e dir) { SetDir(dir, 0); }
	INLINE void Sync(Muter_c dst, i16 ch) {
		//_muteSamplesNow = dst._muteSamplesNow * _muteSamplesMax / dst._muteSamplesMax;
		_muteSamplesNow[ch] = Math_c<i32>::Division(dst._muteSamplesNow[ch] * _muteSamplesMax, dst._muteSamplesMax);
	}
	INLINE void Sync(Muter_c dst) {
		Sync(dst, 0);
	}
	INLINE void Reset(MuterDirChoose_e dir, i16 ch) {
		_dir[ch] = dir;
		if (_dir[ch] == MuterDirChoose_e::MUTER_DIR_CHOOSE_ATTENUATION)
			_muteSamplesNow[ch] = 0;
		else if (_dir[ch] == MuterDirChoose_e::MUTER_DIR_CHOOSE_AMPLIFICATION)
			_muteSamplesNow[ch] = _muteSamplesMax;
	}
	INLINE void Reset(MuterDirChoose_e dir) {
		Reset(dir, 0);
	}
	INLINE b1 IsMuteFinish(i16 ch) {
		if (_dir[ch] == MuterDirChoose_e::MUTER_DIR_CHOOSE_ATTENUATION)
			return (b1)(_muteSamplesNow[ch] >= _muteSamplesMax);
		else if (_dir[ch] == MuterDirChoose_e::MUTER_DIR_CHOOSE_AMPLIFICATION)
			return (b1)(_muteSamplesNow[ch] <= 0);
		return false;
	}
	INLINE b1 IsMuteFinish() {
		return IsMuteFinish(0);
	}

	INLINE void DoMute(void* src, const i32 productSample, i16 ch) {
#if 1
#if 0
		{static int num = 0; 
			printf("(%s)[%d]<%d> %d,%d,%d\n", __func__, __LINE__, num++,
				ch, _dir[ch],_muteSamplesNow[ch]);
		}
#endif
		if (_dir[ch] == MuterDirChoose_e::MUTER_DIR_CHOOSE_ATTENUATION) {
			i32 doMuteSamples = _muteSamplesMax - _muteSamplesNow[ch];
			doMuteSamples = MIN(doMuteSamples, productSample);			
			MuterProduct_c<T>::RunChForWard(
				src,
				src,
				&_muteFactor[_muteFactorWidth * _muteSamplesNow[ch]],
				doMuteSamples,
				_info->_channels);
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
			MuterProduct_c<T>::RunChBackWard(
				src,
				src,
				&_muteFactor[_muteFactorWidth * (_muteSamplesNow[ch] - 1)],
				doMuteSamples,
				_info->_channels);
			_muteSamplesNow[ch] -= productSample;
			_muteSamplesNow[ch] = MAX(_muteSamplesNow[ch], 0);
		}
#endif
	}

	INLINE void DoMute(void* src, const i32 productSample) {
#if 1
		if (_dir[0] == MuterDirChoose_e::MUTER_DIR_CHOOSE_ATTENUATION) {
			i32 doMuteSamples = _muteSamplesMax - _muteSamplesNow[0];
			doMuteSamples = MIN(doMuteSamples, productSample);
			MuterProduct_c<T>::RunAllChForWard(
				src,
				src,
				&_muteFactor[_muteFactorWidth * _muteSamplesNow[0]],
				doMuteSamples,
				_info->_channels);
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
			MuterProduct_c<T>::RunAllChBackWard(src,
				src,
				&_muteFactor[_muteFactorWidth * (_muteSamplesNow[0] - 1)],
				doMuteSamples,
				_info->_channels);
			_muteSamplesNow[0] -= productSample;
			_muteSamplesNow[0] = MAX(_muteSamplesNow[0], 0);
		}
#endif
	}

private:
	u8* _muteFactor;
	i32 _muteFactorWidth;
	i32 _muteSamplesNow[16];
	i32 _muteSamplesMax;
	MuterDirChoose_e _dir[16];
	i8 _facFpNum;
	AudioInfo* _info;
};
}
}

#endif