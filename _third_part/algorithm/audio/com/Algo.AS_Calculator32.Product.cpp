#include"Algo.AS_Calculator32.h"
b1 AS_Calculator32::Product(AudioSamples& dst, i32 dstSample, AudioSamples& src, i32 srcSample, i32 productSample)
{
	i32* pBuf = (i32*)&dst[dstSample];
	i32* pSrcBuf = (i32*)&src[srcSample];

	for (i32 s = 0; s < productSample; s++)
	{
		for (i16 ch = 0; ch < dst._info->_channels; ch++)
		{
			*pBuf = (((i64)(*pBuf)) * *pSrcBuf) >> src._fpNum;
			pBuf++;
		}
		pSrcBuf++;
	}
	return true;
}