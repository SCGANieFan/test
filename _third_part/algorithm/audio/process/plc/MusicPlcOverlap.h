#pragma once
#include"Algo.BasePorting.h"
#include"Algo.Macro.h"
#include"Algo.Type.h"
#include"Algo.AudioCal.OverlapAdd.h"
#include"MusicPlcMem.h"
#include"Algo.AudioSamlpes.h"

using namespace Algo;
using namespace Audio;


class MusicPlcOverlap_c
{
public:
	MusicPlcOverlap_c() {};
	~MusicPlcOverlap_c() {};
public:
	INLINE b1 IsFinish() { return  b1(_overlapSamplesNow >= _overlapSamples); }
	INLINE void Start() {_overlapSamplesNow = 0;}

	INLINE void Init(MusicPlcMem_c *MM,i32 overlapSamples, AudioInfo* info) {
		_info = info;
		_overlapSamples = overlapSamples;
		_overlapSamplesNow = _overlapSamples;
		_overlapadd = Algo_GetOverlapAdd(_info->_width, _info->_width);
		_factor = (i32*)MM->Malloc(_overlapSamples * sizeof(i32));//1->0
		_fixNum = 15;
#if 1
		for (i32 i = 0; i < _overlapSamples; i++)
		{
			_factor[i] = ((_overlapSamples - i - 1) << _fixNum) / _overlapSamples;
		}
#else

		i32 PI_Q15((i32)102943);
		i32 ONE_Q15((i32)1 << 15);
		for (i32 i = 0; i < (_overlapSamples + 1)/2; i++)
		{
			i32 xQ15 = PI_Q15 * i / _overlapSamples;
			i32 xHat2Q15 = xQ15 >> 7;
			xHat2Q15 = (xHat2Q15 * xHat2Q15) >> 1;
			i32 xHat4Q15 = xHat2Q15 >> 7;
			xHat4Q15 = (xHat4Q15 * xHat4Q15) >> 1;			
			_factor[i] = ONE_Q15 - (xHat2Q15 >> 2) + xHat4Q15/48;
		}
		i32 xx = ((_overlapSamples + 1) / 2 * 2) - 1;
		for (i32 i = (_overlapSamples + 1) / 2; i < _overlapSamples; i++)
		{
			_factor[i] = ONE_Q15 - _factor[xx-i];
		}
#endif
	}

	INLINE void DoOverlapAdd(void* dst, void* srcDecline, void* srcRise, i32 doOverlapSamples) {
#if 1
		i32 doOverlapSamples0 = _overlapSamples - _overlapSamplesNow;
		doOverlapSamples0 = MIN(doOverlapSamples0, doOverlapSamples);

		_overlapadd(
			dst,
			srcDecline,
			srcRise,
			_info->_channels,
			_factor,
			_overlapSamplesNow,
			_overlapSamplesNow + doOverlapSamples,
			_overlapSamples);

		ALGO_MEM_CPY(
			((u8*)dst + doOverlapSamples0 * _info->_bytesPerSample),
			((u8*)srcRise + doOverlapSamples0 * _info->_bytesPerSample),
			(doOverlapSamples - doOverlapSamples0) * _info->_bytesPerSample);
		_overlapSamplesNow += doOverlapSamples0;

#endif
	}

private:
	ALGO_OVERLAP_ADD_CB _overlapadd;
	i32 _overlapSamples;
	i32 _overlapSamplesNow;
	i32 *_factor;
	i32 _fixNum = 15;
	AudioInfo *_info;
};

