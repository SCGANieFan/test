#if 0
#pragma once
#include"Algo.Type.h"
#include"Algo.Macro.h"

#define ONE_Q15	((i32)1 << 15)
#define MASK_Q15 (ONE_Q15 - 1)
template<class T>
STATIC INLINE b1 Resample_Local(AudioSamples& in, AudioSamples& out)
{
#if 0
	auto pBuf = (T*)&dst[dstSample];
	auto pSrcBuf = (T*)&src[srcSample];

	for (i32 s = 0; s < productSample; s++)
	{
		for (i16 ch = 0; ch < dst._info->_channels; ch++)
		{
			*pBuf = (((i64)(*pBuf)) * *pSrcBuf) >> src.GetFPNum();
			pBuf++;
		}
		pSrcBuf++;
	}
	return true;
#else
	i32 KQ15 = (i32)(((i64)in._info->_rate << 15) / out._info->_rate);
	i32 oSamplesMax = out.GetLeftSamples();
	i32 oSamples;
	for (oSamples = 0; oSamples<oSamplesMax; oSamples++)
	{
		i32 iSamplesQ15 = oSamples * kQ15;
		i32 iSamplesFloor = iSamplesQ15 >> 15;
		i32 iSamplesCeil = (iSamplesQ15 + MASK_Q15) >> 15;
		if (iSamplesCeil >= in.GetValidSamples())
			break;

		i32 iSamplesFracFloorQ15 = iSamplesQ15 & MASK_Q15;
		i32 iSamplesFracCeilQ15 = ONE_Q15 - iSamplesQ15 & MASK_Q15;
		

		auto pIn0 = (T*)&in[iSamplesFloor];
		auto pIn1 = (T*)&in[iSamplesCeil];
		auto pOut = (T*)&out[out.GetUsedSamples()+ out._validSamples()];

		for (i16 ch = 0; ch < in._info->_channels; ch++)
		{
			*pOut = (T)(((i64)(*pIn0) * iSamplesFracCeilQ15 + (i64)(*pIn1) * iSamplesFracFloorQ15) >> 15);
		}
		out.Used(1);
	}
	oSamples+1
	in.Used()
	return true;
#endif
}
#endif