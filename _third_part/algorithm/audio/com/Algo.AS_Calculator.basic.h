#pragma once
#include"Algo.Type.h"
#include"Algo.Macro.h"
#include"Algo.Memory.h"

template<class T>
STATIC INLINE b1 AppendInFixPoint_Local(AudioSamples& as, i32 num, i32 den)
{
#if 1
	T* buf = (T*)ALGO_MALLOC(as._info->_bytesPerSample);
	if (den != 0){
		auto fac = (T)(((i64)num << as.GetFPNum()) / den);
		for (i16 ch = 0; ch < as._info->_channels; ch++)
		{
			buf[ch] = fac;
		}
	}
	else{

		ALGO_MEM_SET(buf, 0, as._info->_bytesPerSample);
	}
	AudioSamples x;
	BufferSamples bufSamples;
	bufSamples._buf = (u8*)buf;
	bufSamples._samples= 1;
	x.Init(as._info, &bufSamples);
	as.Append(x, 0, 1);
	return true;
#else
	auto pBuff = (T*)as.GetLeftData();
	as.Append(,)
	as.Append(1);
	if (den == 0)
	{
		ALGO_MEM_SET(pBuff, 0, as._info->_bytesPerSample);
		return true;
	}
	i64 fac = ((i64)num << as.GetFPNum()) / den;
	for (i16 ch = 0; ch < as._info->_channels; ch++) {
		pBuff[ch] = fac;
	}
	return true;
#endif
}

template<class T>
STATIC INLINE b1 LeftShift_Local(AudioSamples& as, i32 startSample, i32 samples, i16 bits)
{
	auto pBuf = (T*)as[startSample];
	for (i32 s = 0; s < samples * as._info->_channels; s++)
	{
		pBuf[s] << bits;
	}
	return true;
}

template<class T>
STATIC INLINE b1 LeftShiftSigned_Local(AudioSamples& as, i32 startSample, i32 samples, i16 bits)
{
	auto pBuf = (T*)as[startSample];
	for (i32 s = 0; s < samples * as._info->_channels; s++)
	{
		pBuf[s] << bits;
	}
	return true;
}

template<class T>
STATIC INLINE b1 RightShift_Local(AudioSamples& as, i32 startSample, i32 samples, i16 bits)
{
	auto pBuf = (T*)as[startSample];
	for (i32 s = 0; s < samples * as._info->_channels; s++)
	{
		pBuf[s] >> bits;
	}
	return true;
}

template<class T>
STATIC INLINE b1 RightShiftSigned_Local(AudioSamples& as, i32 startSample, i32 samples, i16 bits)
{
	auto pBuf = (T*)as[startSample];
	for (i32 s = 0; s < samples * as._info->_channels; s++)
	{
		pBuf[s] >> bits;
	}
	return true;
}

