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
	typedef b1(*OVERLAP_ADD_CORE_CB)(void* dst, void* srcDecline, void* srcRise, const i16 channelSelect, const i16 channels, i32* factorDecLine, const i32 startOverlapSample, const i32 endOverlapSample, i32 overlapSample);
	typedef b1(*OVERLAP_ADD_ALL_CH_CORE_CB)(void* dst, void* srcDecline, void* srcRise, const i16 channels, i32* factorDecLine, const i32 startOverlapSample, const i32 endOverlapSample, i32 overlapSample);
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
	INLINE b1 Init(MemoryManger_c* MM, AudioInfo* info, WindowChoose windowChoose, i32 overlapSamples) {
		_info = info;
		_overlapSamples = overlapSamples;
		for (i16 ch = 0; ch < _info->_channels; ch++) {
			_overlapSamplesNow[ch] = _overlapSamples;
		}
		_overlapaddCore = OverlapAdd_c::GetFunc(_info->_width);
		_overlapaddAllChCore = OverlapAdd_c::GetFuncAllCh(_info->_width);
		_factor = (i32*)MM->Malloc(_overlapSamples * sizeof(i32));//1->0
		_fixNum = 15;
		GenerateWindow(windowChoose, _factor, _overlapSamples, _fixNum);
		return true;
	}

	INLINE void DoOverlapAdd(void* dst, void* srcDecline, void* srcRise, i32 doOverlapSamples, i16 ch) {
		i32 doOverlapSamples0 = _overlapSamples - _overlapSamplesNow[ch];
		doOverlapSamples0 = MIN(doOverlapSamples0, doOverlapSamples);
#if 1
		_overlapaddCore(
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
			if (_info->_width == 2) {
				i16* pSrc = ((i16*)srcRise + doOverlapSamples0 * _info->_channels);
				i16* pDst = ((i16*)dst + doOverlapSamples0 * _info->_channels);
				for (i32 s = 0; s < doOverlapSamples - doOverlapSamples0; s++) {
					*pDst = *pSrc;
					//*pDst = 0;
					pDst += _info->_channels;
					pSrc += _info->_channels;
				}
			}
			else if (_info->_width == 4) {
				i32* pSrc = ((i32*)srcRise + doOverlapSamples0 * _info->_channels);
				i32* pDst = ((i32*)dst + doOverlapSamples0 * _info->_channels);
				for (i32 s = 0; s < doOverlapSamples - doOverlapSamples0; s++) {
					*pDst = *pSrc;
					//*pDst = 0;
					pDst += _info->_channels;
					pSrc += _info->_channels;
				}
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
		_overlapaddAllChCore(
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
	STATIC OVERLAP_ADD_CORE_CB GetFunc(i8 Width);
	STATIC OVERLAP_ADD_ALL_CH_CORE_CB GetFuncAllCh(i8 Width);
	STATIC void GenerateWindow(WindowChoose choose, i32* factor, i32 samples, i16 fpNum);
	template<class Ti, class To>
	STATIC b1 OverlapAdd_Core(void* dst, void* srcDecline, void* srcRise, const i16 channelsSelect, const i16 channels, i32* factorDecLine, const i32 startOverlapSample, const i32 endOverlapSample, i32 overlapSample);
	template<class Ti, class To>
	STATIC b1 OverlapAddAllCh_Core(void* dst, void* srcDecline, void* srcRise, const i16 channels, i32* factorDecLine, const i32 startOverlapSample, const i32 endOverlapSample, i32 overlapSample);
private:
	AudioInfo* _info;
	OVERLAP_ADD_CORE_CB _overlapaddCore;
	OVERLAP_ADD_ALL_CH_CORE_CB _overlapaddAllChCore;
	i32 _overlapSamples;
	i32 _overlapSamplesNow[16];
	i32* _factor;
	i32 _fixNum = 15;


};
}
}
#endif