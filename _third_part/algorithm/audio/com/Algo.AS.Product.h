#if 1
#pragma once
#include"Algo.Type.h"
#include"Algo.Macro.h"
#include"Algo.AudioData.h"

typedef b1(*ALGO_PRODUCT_CB)(const AudioInfo* info, u8* src, u8* fac, i16 facFpNum, i32 productSample);
template<class T>
STATIC INLINE b1 Algo_Product(const AudioInfo* info, u8* src, u8* fac, i16 facFpNum,i32 productSample)
{
	T* pSrc = (T*)src;
	T* pFac = (T*)fac;
	for (i32 s = 0; s < productSample; s++) {
		for (i16 ch = 0; ch < info->_channels; ch++) {
			*pSrc = (((i64)(*pSrc)) * *pFac) >> facFpNum;
			pSrc++;
		}
		pFac++;
	}
	return true;
}

template<class T>
STATIC INLINE b1 Algo_DotProduct(AudioInfo* info, u8* src, u8* fac, i16 facFpNum, i32 productSample)
{
	T* pSrc = (T*)src;
	T* pFac = (T*)fac;
	for (i32 s = 0; s < productSample; s++) {
		for (i16 ch = 0; ch < info->_channels; ch++) {
			*pSrc = (((i64)(*pSrc)) * *pFac) >> facFpNum;
			pSrc++;
			pFac++;
		}
	}
	return true;
}



#endif