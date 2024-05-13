#pragma once
#include"Algo.Type.h"
#include"Algo.Macro.h"
#include"Algo.Memory.h"
#include"Algo.Printer.h"

template<class T>
STATIC INLINE b1 AppendInFixPoint_Local(AudioSamples& as, i32 num, i32 den)
{
	u8* buf = &as[as.GetUsedSamples() + as.GetValidSamples()];
	if (den != 0) {
		auto fac = (T)(((i64)num << as.GetFPNum()) / den);
		for (i16 ch = 0; ch < as._info->_channels; ch++)
			ALGO_MEM_CPY(buf, &fac, sizeof(T));
	}
	else {
		ALGO_MEM_SET(buf, 0, as._info->_bytesPerSample);
		ALGO_PRINT("warn, den = 0!");
	}
	as.Append(1);
	return true;
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

