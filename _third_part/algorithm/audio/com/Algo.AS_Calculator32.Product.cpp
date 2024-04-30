#include"Algo.AS_Calculator32.h"
b1 AS_Calculator32::Product(AudioSamples& dst, i32 dstSample, AudioSamples& src, i32 srcSample, i32 productSample)
{
	i32* pBuf = (i32*)&dst[dstSample];
	i32* pSrcBuf = (i32*)&src[srcSample];
	
	for (i32 s = 0; s < productSample * dst._info->_channels; s++)
	{
		pBuf[s] = (((i64)pBuf[s]) * pSrcBuf[s]) >> src._fpNum;
	}
	return true;
}