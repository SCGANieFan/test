#if 1
#pragma once
#include"Algo.AudioCal.Product.h"
using namespace Algo;
using namespace Audio;


Product_c::PRODUCT_WITH_FIXED_POINT_CB Product_c::GetFunc_FixedPoint(FuncMode_e mode, i16 iWidth, i16 fpWidth) {
	i16 iIdx = iWidth - 1;
	i16 fpIdx = fpWidth - 1;
	if (mode == FuncMode_e::PRODUCT_WITH_FIXED_POINT)
	{
		const static PRODUCT_WITH_FIXED_POINT_CB cb[4][4] = {
			{ProductWithFixedPoint<i8,i8,i8>,ProductWithFixedPoint<i8,i8,i16>,ProductWithFixedPoint<i8,i8,i24>,ProductWithFixedPoint<i8,i8,i32>,},
			{ProductWithFixedPoint<i16,i16,i8>,ProductWithFixedPoint<i16,i16,i16>,ProductWithFixedPoint<i16,i16,i24>,ProductWithFixedPoint<i16,i16,i32>,},
			{ProductWithFixedPoint<i24,i24,i8>,ProductWithFixedPoint<i24,i24,i16>,ProductWithFixedPoint<i24,i24,i24>,ProductWithFixedPoint<i24,i24,i32>,},
			{ProductWithFixedPoint<i32,i32,i8>,ProductWithFixedPoint<i32,i32,i16>,ProductWithFixedPoint<i32,i32,i24>,ProductWithFixedPoint<i32,i32,i32>,}
		};
		return cb[iIdx][fpIdx];
	}
	else if (mode == FuncMode_e::PRODUCT_ADD_WITH_FIXED_POINT)
	{
		const static PRODUCT_WITH_FIXED_POINT_CB cb[4][4] = {
			{ProductAddWithFixedPoint<i8,i8,i8>,ProductAddWithFixedPoint<i8,i8,i16>,ProductAddWithFixedPoint<i8,i8,i24>,ProductAddWithFixedPoint<i8,i8,i32>,},
			{ProductAddWithFixedPoint<i16,i16,i8>,ProductAddWithFixedPoint<i16,i16,i16>,ProductAddWithFixedPoint<i16,i16,i24>,ProductAddWithFixedPoint<i16,i16,i32>,},
			{ProductAddWithFixedPoint<i24,i24,i8>,ProductAddWithFixedPoint<i24,i24,i16>,ProductAddWithFixedPoint<i24,i24,i24>,ProductAddWithFixedPoint<i24,i24,i32>,},
			{ProductAddWithFixedPoint<i32,i32,i8>,ProductAddWithFixedPoint<i32,i32,i16>,ProductAddWithFixedPoint<i32,i32,i24>,ProductAddWithFixedPoint<i32,i32,i32>,}
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


#endif