#include"Algo.Memory.h"
#include"Algo.AS_Calculator16.h"

AS_Calculator* asCalculator16 = new(ALGO_MALLOC(sizeof(AS_Calculator16))) AS_Calculator16();

b1 AS_Calculator16::AppendInFixPoint(AudioSamples& as, i32 num, i32 den)
{
	i16* pBuff = (i16*)as.GetLeftData();
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

b1 AS_Calculator16::LeftShift(AudioSamples& as, i32 startSample, i32 samples, i16 bits)
{
	u16* pBuf = (u16*)as[startSample];
	for (i32 s = 0; s < samples * as._info->_channels; s++)
	{
		pBuf[s] << bits;
	}
	return true;
}

b1 AS_Calculator16::LeftShiftSigned(AudioSamples& as, i32 startSample, i32 samples, i16 bits)
{
	i16* pBuf = (i16*)as[startSample];
	for (i32 s = 0; s < samples * as._info->_channels; s++)
	{
		pBuf[s] << bits;
	}
	return true;
}

b1 AS_Calculator16::RightShift(AudioSamples& as, i32 startSample, i32 samples, i16 bits)
{
	u16* pBuf = (u16*)as[startSample];
	for (i32 s = 0; s < samples * as._info->_channels; s++)
	{
		pBuf[s] >> bits;
	}
	return true;
}

b1 AS_Calculator16::RightShiftSigned(AudioSamples& as, i32 startSample, i32 samples, i16 bits)
{
	i16* pBuf = (i16*)as[startSample];
	for (i32 s = 0; s < samples * as._info->_channels; s++)
	{
		pBuf[s] >> bits;
	}
	return true;
}
