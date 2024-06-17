#pragma once
#include"MusicPlcMem.h"
#include"Algo.AudioCal.Basic.h"
#include"Algo.AudioCal.Product.h"
#include"Algo.AudioSamlpes.h"
using namespace Algo;
using namespace Audio;

class MusicPlcMuter_c
{
public:
	MusicPlcMuter_c() {};
	~MusicPlcMuter_c() {};
public:

	INLINE void SetMuteAttenuation() { _dir = 1; }
	INLINE void SetMuteAmplification() { _dir = -1; }
	INLINE void SetMuteStart() { _muteSamplesNow = 0; }
	INLINE b1 IsMuteFinish() { return (b1)((_muteSamplesNow - 1) < 0); }
	

	INLINE void Init(MusicPlcMem_c* MM, i32 decaySamples,AudioInfo *info) {
		_product= Algo_GetProductWithFixedPoint(info->_width, sizeof(i16));
		_appendInFixpoint = Algo_GetAppendInFixedPoint(info->_width, sizeof(i32));
		_info.Init(info->_rate, info->_width, 1);
		_facFpNum = 14;
		_muteFactorQ14= (i16*)MM->Malloc(decaySamples * sizeof(i16));
		ALGO_MEM_SET(_muteFactorQ14, 0, decaySamples * sizeof(i16));
		_muteSamplesMax = decaySamples;
		_muteSamplesNow = 0;
		_iCh = info->_channels;
#if 0
		for (i32 i = 0; i < _muteSamplesMax; i++)
		{
			i32 num = _muteSamplesMax - i;
			_muteFactorQ14[i] = (num << 14) / _muteSamplesMax;
		}
#else
		i32 PI_Q14((i32)51471);
		i32 ONE_Q14((i32)1 << 14);
		for (i32 i = 0; i < (_muteSamplesMax + 1)/2; i++)
		{
			i32 xQ14 = PI_Q14 * i / _muteSamplesMax;
			i32 xHat2Q14 = xQ14 >> 7;
			xHat2Q14 = xHat2Q14* xHat2Q14;
			i32 xHat4Q14 = xHat2Q14 >> 7;
			xHat4Q14 = xHat4Q14* xHat4Q14;
			_muteFactorQ14[i] = (i16)(ONE_Q14 - (xHat2Q14 >> 2) + xHat4Q14 / 48);			
		}
		i32 xx = ((_muteSamplesMax + 1) / 2 * 2) - 1;
		for (i32 i = (_muteSamplesMax + 1) / 2; i < _muteSamplesMax; i++)
		{
			_muteFactorQ14[i] = ONE_Q14 - _muteFactorQ14[xx - i];
		}
#endif
		_dir = 1;
	}
	INLINE void DoMute(void* src, const i32 productSample) {
#if 1
		if (_dir == 1)
		{
			i32 doMuteSamples = _muteSamplesMax - _muteSamplesNow;
			doMuteSamples = MIN(doMuteSamples, productSample);
			_product(
				src,
				&_muteFactorQ14[_muteSamplesNow],
				doMuteSamples,
				_iCh,
				_facFpNum,
				_dir);
			if (doMuteSamples < productSample)
			{
				ALGO_MEM_SET(
					((u8*)src + doMuteSamples * _info._bytesPerSample*_iCh),
					0,
					(productSample - doMuteSamples) * _info._bytesPerSample * _iCh);
			}
			_muteSamplesNow += productSample;
			_muteSamplesNow = MIN(_muteSamplesNow, _muteSamplesMax);
		}
		else
		{
			i32 doMuteSamples = _muteSamplesNow;
			doMuteSamples = MIN(doMuteSamples, productSample);
			_product(
				src,
				&_muteFactorQ14[_muteSamplesNow-1],
				doMuteSamples,
				_iCh,
				_facFpNum,
				_dir);
			_muteSamplesNow -= productSample;
			_muteSamplesNow = MAX(_muteSamplesNow, 0);
		}
#endif
	}

private:
	ALGO_AppendInFixPoint_CB _appendInFixpoint;
	ALGO_PRODUCT_WITH_FIXED_POINT_CB _product;
	i16* _muteFactorQ14;
	i32 _muteSamplesNow;
	i32 _muteSamplesMax;
	i8 _dir;
	i8 _facFpNum;
	AudioInfo _info;
	i8 _iCh;
};

