#pragma once
#include"Algo.Type.h"
#include"Algo.Macro.h"

template<class T>
STATIC INLINE b1 Product_Local(AudioSamples& dst, i32 dstSample, AudioSamples& src, i32 srcSample, i32 productSample)
{
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
}
