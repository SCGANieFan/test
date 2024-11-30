#if 1
#pragma once
#include"Algo.AudioCal.Product.h"
namespace Algo{
namespace Audio{

Product_c::PRODUCT_WITH_FIXED_POINT_CB Product_c::GetFunc_FixedPoint(FuncMode_e mode, i16 iWidth, i16 fpWidth) {
	i16 iIdx = (iWidth>>1) - 1;
	i16 fpIdx = (fpWidth>>1) - 1;
	if (mode == FuncMode_e::PRODUCT_WITH_FIXED_POINT)
	{
		const static PRODUCT_WITH_FIXED_POINT_CB cb[2][2] = {
			{ProductWithFixedPoint<i16,i16,i16>},
			{ProductWithFixedPoint<i32,i32,i16>}
		};
		return cb[iIdx][fpIdx];
	}
	else if (mode == FuncMode_e::PRODUCT_CH_WITH_FIXED_POINT) {
		const static PRODUCT_WITH_FIXED_POINT_CB cb[2][2] = {
				{ProductChWithFixedPoint<i16,i16,i16>},
				{ProductChWithFixedPoint<i32,i32,i16>}
		};
		return cb[iIdx][fpIdx];
	}
	return 0;
}

template<class Ti, class To, class Tx>
b1 Product_c::ProductWithFixedPoint(void* dst, void* src, void* fac, const i32 productSample, const i8 channels, const i8 facFpNum, i8 facDir) {
	To* pDst = (To*)dst;
	Ti* pSrc = (Ti*)src;
	Tx* pFac = (Tx*)fac;
	// i32 stride = channels;
	if (facDir == 1)
	{
		for (i32 s = 0; s < productSample; s++) {
			for (i8 ch = 0; ch < channels; ch++) {
				*pDst = (Ti)((((i64)(*pSrc)) * *pFac) >> facFpNum);
				pDst++;
				pSrc++;
			}
			pFac++;
		}

	}
	else
	{
		for (i32 s = 0; s < productSample; s++) {
			for (i8 ch = 0; ch < channels; ch++) {
				*pDst = (Ti)((((i64)(*pSrc)) * *pFac) >> facFpNum);
				pDst++;
				pSrc++;
			}
			pFac--;
		}

	}
	return true;
}

template<class Ti, class To, class Tx>
b1 Product_c::ProductChWithFixedPoint(void* dst, void* src, void* fac, const i32 productSample, const i8 channels, const i8 facFpNum, i8 facDir) {
	To* pDst = (To*)dst;
	Ti* pSrc = (Ti*)src;
	Tx* pFac = (Tx*)fac;
	i32 stride = channels;
	if (facDir == 1)
	{
		pDst = (To*)dst;
		pSrc = (Ti*)src;
		pFac = (Tx*)fac;
		for (i32 s = 0; s < productSample; s++) {
			*pDst = (Ti)((((i64)(*pSrc)) * *pFac) >> facFpNum);
			pDst += stride;
			pSrc += stride;
			pFac++;
		}
	}
	else
	{
		pDst = (To*)dst;
		pSrc = (Ti*)src;
		pFac = (Tx*)fac;
		for (i32 s = 0; s < productSample; s++) {
			*pDst = (Ti)((((i64)(*pSrc)) * *pFac) >> facFpNum);
			pDst += stride;
			pSrc += stride;
			pFac--;
		}
	}
	return true;
}

template<class Ti, class To, class Tx>
b1 Product_c::ProductAddWithFixedPoint(void* dst, void* src, void* fac, const i32 productSample, const i8 channels, const i8 facFpNum, i8 facDir) {
	To* pDst = (To*)dst;
	Ti* pSrc = (Ti*)src;
	Tx* pFac = (Tx*)fac;
	if (facDir == 1) {
		for (i32 s = 0; s < productSample; s++) {
			for (i8 ch = 0; ch < channels; ch++) {
				*pDst = *pDst + (To)((((i64)(*pSrc)) * *pFac) >> facFpNum);
				pDst++;
				pSrc++;
			}
			pFac++;
		}
	}
	else {
		for (i32 s = 0; s < productSample; s++) {
			for (i8 ch = 0; ch < channels; ch++) {
				*pDst = *pDst + (To)((((i64)(*pSrc)) * *pFac) >> facFpNum);
				pDst++;
				pSrc++;
			}
			pFac--;
		}
	}
	return true;
}

}
}

#endif