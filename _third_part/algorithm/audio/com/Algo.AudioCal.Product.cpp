#if 1
#include"Algo.AudioCal.Product.h"
using namespace Algo;
using namespace Audio;

#if 1
template<class Ti, class Tx, class To = i32>
class Algo_Product {
public:
	STATIC INLINE b1 ProductWidFixedPoint(void* src, void* fac, const i32 productSample, const i8 channels, const i8 facFpNum) {
		Ti* pSrc = (Ti*)src;
		Tx* pFac = (Tx*)fac;
		for (i32 s = 0; s < productSample; s++) {
			for (i8 ch = 0; ch < channels; ch++) {
				*pSrc = (Ti)((((i64)(*pSrc)) * *pFac) >> facFpNum);
				pSrc++;
			}
			pFac++;
		}
		return true;
	}

	STATIC INLINE b1 DotProductWidFixedPoint(void* src, void* fac, const i32 productSample, const i8 channels, const i8 facFpNum) {
		Ti* pSrc = (Ti*)src;
		Tx* pFac = (Tx*)fac;
		for (i32 s = 0; s < productSample; s++) {
			for (i8 ch = 0; ch < channels; ch++) {
				*pSrc = (((i64)(*pSrc)) * *pFac) >> facFpNum;
				pSrc++;
				pFac++;
			}
		}
		return true;
	}

	STATIC INLINE b1 ProductAddWidFixedPoint(void* dst, void* src, void* fac, const i32 productSample, const i8 channels, const i8 facFpNum) {
		To* pDst = (To*)dst;
		Ti* pSrc = (Ti*)src;
		Tx* pFac = (Tx*)fac;
		for (i32 s = 0; s < productSample; s++) {
			for (i8 ch = 0; ch < channels; ch++) {
				*pDst = *pDst + (To)((((i64)(*pSrc)) * *pFac) >> facFpNum);
				pDst++;
				pSrc++;
			}
			pFac++;
		}
		return true;
	}

	STATIC INLINE b1 DotProductAddWidFixedPoint(void* dst, void* src, void* fac, const i32 productSample, const i8 channels, const i8 facFpNum) {
		To* pDst = (To*)dst;
		Ti* pSrc = (Ti*)src;
		Tx* pFac = (Tx*)fac;
		for (i32 s = 0; s < productSample; s++) {
			for (i8 ch = 0; ch < channels; ch++) {
				*pDst = *pDst + (To)((((i64)(*pSrc)) * *pFac) >> facFpNum);
				pDst++;
				pSrc++;
				pFac++;
			}
		}
		return true;
	}

};


STATIC const ALGO_PRODUCT_WITH_FIXED_POINT_CB productWithFixPoint[4][4] = {
	{Algo_Product<i8,i8>::ProductWidFixedPoint,Algo_Product<i8,i16>::ProductWidFixedPoint,Algo_Product<i8,i24>::ProductWidFixedPoint,Algo_Product<i8,i32>::ProductWidFixedPoint},
	{Algo_Product<i16,i8>::ProductWidFixedPoint,Algo_Product<i16,i16>::ProductWidFixedPoint,Algo_Product<i16,i24>::ProductWidFixedPoint,Algo_Product<i16,i32>::ProductWidFixedPoint},
	{Algo_Product<i24,i8>::ProductWidFixedPoint,Algo_Product<i24,i16>::ProductWidFixedPoint,Algo_Product<i24,i24>::ProductWidFixedPoint,Algo_Product<i24,i32>::ProductWidFixedPoint},
	{Algo_Product<i32,i8>::ProductWidFixedPoint,Algo_Product<i32,i16>::ProductWidFixedPoint,Algo_Product<i32,i24>::ProductWidFixedPoint,Algo_Product<i32,i32>::ProductWidFixedPoint},
};

EXTERNC ALGO_PRODUCT_WITH_FIXED_POINT_CB Algo_GetProductWithFixedPoint(i8 iWidth, i8 fpWidth)
{
	return productWithFixPoint[iWidth - 1][fpWidth - 1];
}


STATIC const ALGO_DOT_PRODUCT_WITH_FIXED_POINT_CB dotProductWithFixPoint[4][4] = {
	{Algo_Product<i8,i8>::DotProductWidFixedPoint,Algo_Product<i8,i16>::DotProductWidFixedPoint,Algo_Product<i8,i24>::DotProductWidFixedPoint,Algo_Product<i8,i32>::DotProductWidFixedPoint},
	{Algo_Product<i16,i8>::DotProductWidFixedPoint,Algo_Product<i16,i16>::DotProductWidFixedPoint,Algo_Product<i16,i24>::DotProductWidFixedPoint,Algo_Product<i16,i32>::DotProductWidFixedPoint},
	{Algo_Product<i24,i8>::DotProductWidFixedPoint,Algo_Product<i24,i16>::DotProductWidFixedPoint,Algo_Product<i24,i24>::DotProductWidFixedPoint,Algo_Product<i24,i32>::DotProductWidFixedPoint},
	{Algo_Product<i32,i8>::DotProductWidFixedPoint,Algo_Product<i32,i16>::DotProductWidFixedPoint,Algo_Product<i32,i24>::DotProductWidFixedPoint,Algo_Product<i32,i32>::DotProductWidFixedPoint},
};

EXTERNC ALGO_DOT_PRODUCT_WITH_FIXED_POINT_CB Algo_GetDotProductWithFixedPoint(i8 iWidth, i8 fpWidth)
{
	return dotProductWithFixPoint[iWidth - 1][fpWidth - 1];
}


STATIC const ALGO_PRODUCT_ADD_WITH_FIXED_POINT_CB productAddWithFixPoint[4][4][4] = {
	{
		{Algo_Product<i8,i8,i8>::ProductAddWidFixedPoint,Algo_Product<i8,i8,i16>::ProductAddWidFixedPoint,Algo_Product<i8,i8,i24>::ProductAddWidFixedPoint,Algo_Product<i8,i8,i32>::ProductAddWidFixedPoint},
		{Algo_Product<i8,i16,i8>::ProductAddWidFixedPoint,Algo_Product<i8,i16,i16>::ProductAddWidFixedPoint,Algo_Product<i8,i16,i24>::ProductAddWidFixedPoint,Algo_Product<i8,i16,i32>::ProductAddWidFixedPoint},
		{Algo_Product<i8,i24,i8>::ProductAddWidFixedPoint,Algo_Product<i8,i24,i16>::ProductAddWidFixedPoint,Algo_Product<i8,i24,i24>::ProductAddWidFixedPoint,Algo_Product<i8,i24,i32>::ProductAddWidFixedPoint},
		{Algo_Product<i8,i32,i8>::ProductAddWidFixedPoint,Algo_Product<i8,i32,i16>::ProductAddWidFixedPoint,Algo_Product<i8,i32,i24>::ProductAddWidFixedPoint,Algo_Product<i8,i32,i32>::ProductAddWidFixedPoint},
	},
	{
		{Algo_Product<i16,i8,i8>::ProductAddWidFixedPoint,Algo_Product<i16,i8,i16>::ProductAddWidFixedPoint,Algo_Product<i16,i8,i24>::ProductAddWidFixedPoint,Algo_Product<i16,i8,i32>::ProductAddWidFixedPoint},
		{Algo_Product<i16,i16,i8>::ProductAddWidFixedPoint,Algo_Product<i16,i16,i16>::ProductAddWidFixedPoint,Algo_Product<i16,i16,i24>::ProductAddWidFixedPoint,Algo_Product<i16,i16,i32>::ProductAddWidFixedPoint},
		{Algo_Product<i16,i24,i8>::ProductAddWidFixedPoint,Algo_Product<i16,i24,i16>::ProductAddWidFixedPoint,Algo_Product<i16,i24,i24>::ProductAddWidFixedPoint,Algo_Product<i16,i24,i32>::ProductAddWidFixedPoint},
		{Algo_Product<i16,i32,i8>::ProductAddWidFixedPoint,Algo_Product<i16,i32,i16>::ProductAddWidFixedPoint,Algo_Product<i16,i32,i24>::ProductAddWidFixedPoint,Algo_Product<i16,i32,i32>::ProductAddWidFixedPoint},
	},
	{
		{Algo_Product<i24,i8,i8>::ProductAddWidFixedPoint,Algo_Product<i24,i8,i16>::ProductAddWidFixedPoint,Algo_Product<i24,i8,i24>::ProductAddWidFixedPoint,Algo_Product<i24,i8,i32>::ProductAddWidFixedPoint},
		{Algo_Product<i24,i16,i8>::ProductAddWidFixedPoint,Algo_Product<i24,i16,i16>::ProductAddWidFixedPoint,Algo_Product<i24,i16,i24>::ProductAddWidFixedPoint,Algo_Product<i24,i16,i32>::ProductAddWidFixedPoint},
		{Algo_Product<i24,i24,i8>::ProductAddWidFixedPoint,Algo_Product<i24,i24,i16>::ProductAddWidFixedPoint,Algo_Product<i24,i24,i24>::ProductAddWidFixedPoint,Algo_Product<i24,i24,i32>::ProductAddWidFixedPoint},
		{Algo_Product<i24,i32,i8>::ProductAddWidFixedPoint,Algo_Product<i24,i32,i16>::ProductAddWidFixedPoint,Algo_Product<i24,i32,i24>::ProductAddWidFixedPoint,Algo_Product<i24,i32,i32>::ProductAddWidFixedPoint},
	},
	{
		{Algo_Product<i32,i8,i8>::ProductAddWidFixedPoint,Algo_Product<i32,i8,i16>::ProductAddWidFixedPoint,Algo_Product<i32,i8,i24>::ProductAddWidFixedPoint,Algo_Product<i32,i8,i32>::ProductAddWidFixedPoint},
		{Algo_Product<i32,i16,i8>::ProductAddWidFixedPoint,Algo_Product<i32,i16,i16>::ProductAddWidFixedPoint,Algo_Product<i32,i16,i24>::ProductAddWidFixedPoint,Algo_Product<i32,i16,i32>::ProductAddWidFixedPoint},
		{Algo_Product<i32,i24,i8>::ProductAddWidFixedPoint,Algo_Product<i32,i24,i16>::ProductAddWidFixedPoint,Algo_Product<i32,i24,i24>::ProductAddWidFixedPoint,Algo_Product<i32,i24,i32>::ProductAddWidFixedPoint},
		{Algo_Product<i32,i32,i8>::ProductAddWidFixedPoint,Algo_Product<i32,i32,i16>::ProductAddWidFixedPoint,Algo_Product<i32,i32,i24>::ProductAddWidFixedPoint,Algo_Product<i32,i32,i32>::ProductAddWidFixedPoint},
	},
};

EXTERNC ALGO_PRODUCT_ADD_WITH_FIXED_POINT_CB Algo_GetProductAddWithFixedPoint(i8 iWidth, i8 fpWidth, i8 oWidth)
{
	return productAddWithFixPoint[iWidth - 1][fpWidth - 1][oWidth - 1];
}


STATIC const ALGO_DOT_PRODUCT_ADD_WITH_FIXED_POINT_CB dotProductAddWithFixPoint[4][4][4] = {
	{
		{Algo_Product<i8,i8,i8>::DotProductAddWidFixedPoint,Algo_Product<i8,i8,i16>::DotProductAddWidFixedPoint,Algo_Product<i8,i8,i24>::DotProductAddWidFixedPoint,Algo_Product<i8,i8,i32>::DotProductAddWidFixedPoint},
		{Algo_Product<i8,i16,i8>::DotProductAddWidFixedPoint,Algo_Product<i8,i16,i16>::DotProductAddWidFixedPoint,Algo_Product<i8,i16,i24>::DotProductAddWidFixedPoint,Algo_Product<i8,i16,i32>::DotProductAddWidFixedPoint},
		{Algo_Product<i8,i24,i8>::DotProductAddWidFixedPoint,Algo_Product<i8,i24,i16>::DotProductAddWidFixedPoint,Algo_Product<i8,i24,i24>::DotProductAddWidFixedPoint,Algo_Product<i8,i24,i32>::DotProductAddWidFixedPoint},
		{Algo_Product<i8,i32,i8>::DotProductAddWidFixedPoint,Algo_Product<i8,i32,i16>::DotProductAddWidFixedPoint,Algo_Product<i8,i32,i24>::DotProductAddWidFixedPoint,Algo_Product<i8,i32,i32>::DotProductAddWidFixedPoint},
	},
	{
		{Algo_Product<i16,i8,i8>::DotProductAddWidFixedPoint,Algo_Product<i16,i8,i16>::DotProductAddWidFixedPoint,Algo_Product<i16,i8,i24>::DotProductAddWidFixedPoint,Algo_Product<i16,i8,i32>::DotProductAddWidFixedPoint},
		{Algo_Product<i16,i16,i8>::DotProductAddWidFixedPoint,Algo_Product<i16,i16,i16>::DotProductAddWidFixedPoint,Algo_Product<i16,i16,i24>::DotProductAddWidFixedPoint,Algo_Product<i16,i16,i32>::DotProductAddWidFixedPoint},
		{Algo_Product<i16,i24,i8>::DotProductAddWidFixedPoint,Algo_Product<i16,i24,i16>::DotProductAddWidFixedPoint,Algo_Product<i16,i24,i24>::DotProductAddWidFixedPoint,Algo_Product<i16,i24,i32>::DotProductAddWidFixedPoint},
		{Algo_Product<i16,i32,i8>::DotProductAddWidFixedPoint,Algo_Product<i16,i32,i16>::DotProductAddWidFixedPoint,Algo_Product<i16,i32,i24>::DotProductAddWidFixedPoint,Algo_Product<i16,i32,i32>::DotProductAddWidFixedPoint},
	},
	{
		{Algo_Product<i24,i8,i8>::DotProductAddWidFixedPoint,Algo_Product<i24,i8,i16>::DotProductAddWidFixedPoint,Algo_Product<i24,i8,i24>::DotProductAddWidFixedPoint,Algo_Product<i24,i8,i32>::DotProductAddWidFixedPoint},
		{Algo_Product<i24,i16,i8>::DotProductAddWidFixedPoint,Algo_Product<i24,i16,i16>::DotProductAddWidFixedPoint,Algo_Product<i24,i16,i24>::DotProductAddWidFixedPoint,Algo_Product<i24,i16,i32>::DotProductAddWidFixedPoint},
		{Algo_Product<i24,i24,i8>::DotProductAddWidFixedPoint,Algo_Product<i24,i24,i16>::DotProductAddWidFixedPoint,Algo_Product<i24,i24,i24>::DotProductAddWidFixedPoint,Algo_Product<i24,i24,i32>::DotProductAddWidFixedPoint},
		{Algo_Product<i24,i32,i8>::DotProductAddWidFixedPoint,Algo_Product<i24,i32,i16>::DotProductAddWidFixedPoint,Algo_Product<i24,i32,i24>::DotProductAddWidFixedPoint,Algo_Product<i24,i32,i32>::DotProductAddWidFixedPoint},
	},
	{
		{Algo_Product<i32,i8,i8>::DotProductAddWidFixedPoint,Algo_Product<i32,i8,i16>::DotProductAddWidFixedPoint,Algo_Product<i32,i8,i24>::DotProductAddWidFixedPoint,Algo_Product<i32,i8,i32>::DotProductAddWidFixedPoint},
		{Algo_Product<i32,i16,i8>::DotProductAddWidFixedPoint,Algo_Product<i32,i16,i16>::DotProductAddWidFixedPoint,Algo_Product<i32,i16,i24>::DotProductAddWidFixedPoint,Algo_Product<i32,i16,i32>::DotProductAddWidFixedPoint},
		{Algo_Product<i32,i24,i8>::DotProductAddWidFixedPoint,Algo_Product<i32,i24,i16>::DotProductAddWidFixedPoint,Algo_Product<i32,i24,i24>::DotProductAddWidFixedPoint,Algo_Product<i32,i24,i32>::DotProductAddWidFixedPoint},
		{Algo_Product<i32,i32,i8>::DotProductAddWidFixedPoint,Algo_Product<i32,i32,i16>::DotProductAddWidFixedPoint,Algo_Product<i32,i32,i24>::DotProductAddWidFixedPoint,Algo_Product<i32,i32,i32>::DotProductAddWidFixedPoint},
	},
};

EXTERNC ALGO_DOT_PRODUCT_ADD_WITH_FIXED_POINT_CB Algo_GetDotProductAddWithFixedPoint(i8 iWidth, i8 fpWidth, i8 oWidth)
{
	return dotProductAddWithFixPoint[iWidth - 1][fpWidth - 1][oWidth - 1];
}

#endif


#endif