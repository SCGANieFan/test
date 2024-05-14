#if 1
#pragma once
#include"Algo.Type.h"
#include"Algo.Macro.h"
namespace Algo {
	namespace Audio {
		typedef b1(*ALGO_PRODUCT_CB)(u8* src, u8* fac, const i32 productSample, const i16 channels, const i16 facFpNum);
		template<class T, class TF>
		STATIC INLINE b1 Algo_Product(u8* src, u8* fac, const i32 productSample, i16 const channels, const i16 facFpNum)
		{
			T* pSrc = (T*)src;
			TF* pFac = (TF*)fac;
			for (i32 s = 0; s < productSample; s++) {
				for (i16 ch = 0; ch < channels; ch++) {
					*pSrc = (((i64)(*pSrc)) * *pFac) >> facFpNum;
					pSrc++;
				}
				pFac++;
			}
			return true;
		}

		typedef b1(*ALGO_DOT_PRODUCT_CB)(u8* src, u8* fac, const i32 productSample, const i16 channels, const i16 facFpNum);
		template<class T, class TF>
		STATIC INLINE b1 Algo_DotProduct(u8* src, u8* fac, const i32 productSample, const i16 channels, const i16 facFpNum)
		{
			T* pSrc = (T*)src;
			TF* pFac = (TF*)fac;
			for (i32 s = 0; s < productSample; s++) {
				for (i16 ch = 0; ch < channels; ch++) {
					*pSrc = (((i64)(*pSrc)) * *pFac) >> facFpNum;
					pSrc++;
					pFac++;
				}
			}
			return true;
		}

		typedef b1(*ALGO_DOT_PRODUCT_ADD_CB)(u8* dst, u8* src, u8* fac, const i32 productSample, const i16 channels, const i16 facFpNum);
		template<class T, class TF>
		STATIC INLINE b1 Algo_ProductAdd(u8* dst, u8* src, u8* fac, const i32 productSample, const i16 channels, const i16 facFpNum)
		{
			T* pDst = (T*)dst;
			T* pSrc = (T*)src;
			TF* pFac = (TF*)fac;
			for (i32 s = 0; s < productSample; s++) {
				for (i16 ch = 0; ch < channels; ch++) {
					*pDst = (((i64)(*pSrc)) * *pFac) >> facFpNum;
					pDst++;
					pSrc++;
				}
				pFac++;
			}
			return true;
		}

		typedef b1(*ALGO_DOT_PRODUCT_ADD_ACC_CB)(u8* dst, u8* src, u8* fac, const i32 productSample, const i16 channels, const i16 facFpNum);
		template<class T, class TF>
		STATIC INLINE b1 Algo_ProductAddAcc(u8* dst, u8* src, u8* fac, const i32 productSample, const i16 channels, const i16 facFpNum)
		{
			T* pDst = (T*)dst;
			T* pSrc = (T*)src;
			TF* pFac = (TF*)fac;
			for (i32 s = 0; s < productSample; s++) {
				for (i16 ch = 0; ch < channels; ch++) {
					*pDst = *pDst + (i32)((((i64)(*pSrc)) * *pFac) >> facFpNum);
					pDst++;
					pSrc++;
				}
				pFac++;
			}
			return true;
		}


	}
}

#endif