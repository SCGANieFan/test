#include"Algo.AS_Calculator16.h"
b1 AS_Calculator16::Product(AudioSamples& dst, i32 dstSample, AudioSamples& src, i32 srcSample, i32 productSample)
{
	i16* pBuf = (i16*)&dst[dstSample];
	i16* pSrcBuf = (i16*)&src[srcSample];
	
	for (i32 s = 0; s < productSample * dst._info->_channels; s++)
	{
		pBuf[s] = (((i64)pBuf[s]) * pSrcBuf[s]) >> src._fpNum;
	}
	return true;
}