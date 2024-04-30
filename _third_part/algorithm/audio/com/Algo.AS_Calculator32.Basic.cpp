#include"Algo.Memory.h"
#include"Algo.AS_Calculator32.h"

b1 AS_Calculator32::AppendInFixPoint(AudioSamples& as, i32 num, i32 den)
{
	i32* pBuff = (i32*)as.GetLeftData();
	as._size += as._info->_bytesPerSample;
	as._validSamples += 1;
	if (den == 0)
	{
		ALGO_MEM_SET(pBuff, 0, as._info->_bytesPerSample);
		return true;
	}
	i64 fac = ((i64)num << as._fpNum) / den;
	for (i16 ch = 0; ch < as._info->_channels; ch++) {
		pBuff[ch] = fac;
	}
	return true;
}

b1 AS_Calculator32::LeftShift(AudioSamples& as, i32 startSample, i32 samples, i16 bits)
{
	u32* pBuf = (u32*)as[startSample];
	for (i32 s = 0; s < samples * as._info->_channels; s++)
	{
		pBuf[s] << bits;
	}
	return true;
}

b1 AS_Calculator32::LeftShiftSigned(AudioSamples& as, i32 startSample, i32 samples, i16 bits)
{
	i32* pBuf = (i32*)as[startSample];
	for (i32 s = 0; s < samples * as._info->_channels; s++)
	{
		pBuf[s] << bits;
	}
	return true;
}

b1 AS_Calculator32::RightShift(AudioSamples& as, i32 startSample, i32 samples, i16 bits)
{
	u32* pBuf = (u32*)as[startSample];
	for (i32 s = 0; s < samples * as._info->_channels; s++)
	{
		pBuf[s] >> bits;
	}
	return true;
}

b1 AS_Calculator32::RightShiftSigned(AudioSamples& as, i32 startSample, i32 samples, i16 bits)
{
	i32* pBuf = (i32*)as[startSample];
	for (i32 s = 0; s < samples * as._info->_channels; s++)
	{
		pBuf[s] >> bits;
	}
	return true;
}
