#if 1
#pragma once
#include"Algo.AudioCal.Com.h"
#include"Algo.AudioCal.BufferGenerator.h"
#include"Algo.BasePorting.Inner.h"
#include"Algo.AudioData.h"
namespace Algo {
namespace Audio {

template<class Ti, class To, class Tf>
struct OverlapAdd_t {
	STATIC b1 RunCh(void* dst, void* srcDecline, void* srcRise, const i16 channelSelect, const i16 channels, void* factorDecLine, const i32 startOverlapSample, const i32 endOverlapSample, i32 overlapSample) {
		const i32 fixNum = 13;
		Ti* pSrcRise = (Ti*)srcRise + channelSelect;
		Ti* pSrcDecline = (Ti*)srcDecline + channelSelect;
		To* pDst = (To*)dst + channelSelect;
		Tf* pFac = (Tf*)factorDecLine;
		for (i32 s = startOverlapSample; s < endOverlapSample; s++) {
			*pDst = (To)(((i64)(*pSrcRise) * pFac[overlapSample - s - 1] + (i64)(*pSrcDecline) * pFac[s]) >> fixNum);
			pDst += channels;
			pSrcRise += channels;
			pSrcDecline += channels;
		}
		return true;
	}
	STATIC b1 RunAllCh(void* dst, void* srcDecline, void* srcRise, const i16 channels, void* factorDecLine, const i32 startOverlapSample, const i32 endOverlapSample, i32 overlapSample) {
		const i32 fixNum = 13;
		Ti* pSrcRise = (Ti*)srcRise;
		Ti* pSrcDecline = (Ti*)srcDecline;
		To* pDst = (To*)dst;
		Tf* pFac = (Tf*)factorDecLine;
		for (i32 s = startOverlapSample; s < endOverlapSample; s++) {
			for (i16 ch = 0; ch < channels; ch++) {
				*pDst = (To)(((i64)(*pSrcRise) * pFac[overlapSample - s - 1] + (i64)(*pSrcDecline) * pFac[s]) >> fixNum);
				pDst++;
				pSrcRise++;
				pSrcDecline++;
			}
		}
		return true;
	}
};

template<>
struct OverlapAdd_t<f32, f32, f32> {
	STATIC b1 RunCh(void* dst, void* srcDecline, void* srcRise, const i16 channelSelect, const i16 channels, void* factorDecLine, const i32 startOverlapSample, const i32 endOverlapSample, i32 overlapSample) {
		f32* pSrcRise = (f32*)srcRise + channelSelect;
		f32* pSrcDecline = (f32*)srcDecline + channelSelect;
		f32* pDst = (f32*)dst + channelSelect;
		f32* pFac = (f32*)factorDecLine;
		for (i32 s = startOverlapSample; s < endOverlapSample; s++) {
			*pDst = (*pSrcRise) * pFac[overlapSample - s - 1] + (*pSrcDecline) * pFac[s];
			pDst += channels;
			pSrcRise += channels;
			pSrcDecline += channels;
		}
		return true;
	}
	STATIC b1 RunAllCh(void* dst, void* srcDecline, void* srcRise, const i16 channels, void* factorDecLine, const i32 startOverlapSample, const i32 endOverlapSample, i32 overlapSample) {
		f32* pSrcRise = (f32*)srcRise;
		f32* pSrcDecline = (f32*)srcDecline;
		f32* pDst = (f32*)dst;
		f32* pFac = (f32*)factorDecLine;
		for (i32 s = startOverlapSample; s < endOverlapSample; s++) {
			for (i16 ch = 0; ch < channels; ch++) {
				*pDst = (*pSrcRise) * pFac[overlapSample - s - 1] + (*pSrcDecline) * pFac[s];
				pDst++;
				pSrcRise++;
				pSrcDecline++;
			}
		}
		return true;
	}
};


enum class OverlapAddWindowChoose_e {
	Line = 0,
	Cosine,
};

template<class T, b1 type = TypeIdentify_c::IsFloat<T>()>
struct OverlapAddInner_t : OverlapAdd_t<T, T, T> {};
template<class T>
struct OverlapAddInner_t<T, false> : OverlapAdd_t<T, T, i32> {};

template<class T>
class OverlapAdd_c
{
public:
	OverlapAdd_c() {};
	~OverlapAdd_c() {};
public:
	INLINE b1 IsFinish(i16 ch) { return  b1(_overlapSamplesNow[ch] >= _overlapSamples); }
	INLINE b1 IsFinish() { return  b1(_overlapSamplesNow[0] >= _overlapSamples); }
	INLINE void Start(i16 ch) {
		_overlapSamplesNow[ch] = 0;
	}
	INLINE void Start() {
		_overlapSamplesNow[0] = 0;
	}

public:
	INLINE b1 Init(MemoryManger_c* MM, AudioInfo* info, OverlapAddWindowChoose_e windowChoose, i32 overlapSamples) {
		_info = info;
		_overlapSamples = overlapSamples;
		for (i16 ch = 0; ch < _info->_channels; ch++) {
			_overlapSamplesNow[ch] = _overlapSamples;
		}
		BufferGenerator_c::BufferChoose_e bufferChoose;
		if (windowChoose == OverlapAddWindowChoose_e::Line)
			bufferChoose = BufferGenerator_c::BufferChoose_e::WINDOW_LINE_FADE;
		else if (windowChoose == OverlapAddWindowChoose_e::Cosine)
			bufferChoose = BufferGenerator_c::BufferChoose_e::WINDOW_COSINE_FADE;
		else {
			bufferChoose = BufferGenerator_c::BufferChoose_e::WINDOW_COSINE_FADE;
		}
		if (TypeIdentify_c::IsF32<T>()) {
			_factor = MM->Malloc(_overlapSamples * sizeof(f32));//1->0
			_fixNum = 0;
			BufferGenerator_c::Generate(bufferChoose, (f32*)_factor, _overlapSamples, _fixNum);
		}
		else {
			_factor = MM->Malloc(_overlapSamples * sizeof(i32));//1->0
			_fixNum = 13;
			BufferGenerator_c::Generate(bufferChoose, (i32*)_factor, _overlapSamples, _fixNum);
		}
		return true;
	}

	INLINE void DoOverlapAdd(void* dst, void* srcDecline, void* srcRise, i32 doOverlapSamples, i16 ch) {
		i32 doOverlapSamples0 = _overlapSamples - _overlapSamplesNow[ch];
		doOverlapSamples0 = MIN(doOverlapSamples0, doOverlapSamples);
#if 1
		OverlapAddInner_t<T>::RunCh(
			dst,
			srcDecline,
			srcRise,
			ch,
			_info->_channels,
			_factor,
			_overlapSamplesNow[ch],
			_overlapSamplesNow[ch] + doOverlapSamples0,
			_overlapSamples);
#endif
#if 1
		if (doOverlapSamples > doOverlapSamples0) {
#if 0
			MUSIC_PLC_MEM_CPY(
				((u8*)dst + doOverlapSamples0 * _info->_bytesPerSample),
				((u8*)srcRise + doOverlapSamples0 * _info->_bytesPerSample),
				(doOverlapSamples - doOverlapSamples0) * _info->_bytesPerSample);
#else
			T* pSrc = ((T*)srcRise + doOverlapSamples0 * _info->_channels);
			T* pDst = ((T*)dst + doOverlapSamples0 * _info->_channels);
			for (i32 s = 0; s < doOverlapSamples - doOverlapSamples0; s++) {
				*pDst = *pSrc;
				//*pDst = 0;
				pDst += _info->_channels;
				pSrc += _info->_channels;
			}
#endif
		}
#endif
		_overlapSamplesNow[ch] += doOverlapSamples0;
	}


	INLINE void DoOverlapAdd(void* dst, void* srcDecline, void* srcRise, i32 doOverlapSamples) {
		i32 doOverlapSamples0 = _overlapSamples - _overlapSamplesNow[0];
		doOverlapSamples0 = MIN(doOverlapSamples0, doOverlapSamples);
#if 1
		OverlapAddInner_t<T>::RunAllCh(
			dst,
			srcDecline,
			srcRise,
			_info->_channels,
			_factor,
			_overlapSamplesNow[0],
			_overlapSamplesNow[0] + doOverlapSamples0,
			_overlapSamples);
#endif
		if (doOverlapSamples > doOverlapSamples0) {
			ALGO_MEM_CPY(
				((u8*)dst + doOverlapSamples0 * _info->_bytesPerSample),
				((u8*)srcRise + doOverlapSamples0 * _info->_bytesPerSample),
				(doOverlapSamples - doOverlapSamples0) * _info->_bytesPerSample);
		}
		_overlapSamplesNow[0] += doOverlapSamples0;
	}
private:
	AudioInfo* _info;
	i32 _overlapSamples;
	i32 _overlapSamplesNow[16];
	void* _factor;
	i32 _fixNum = 15;
};
}
}
#endif