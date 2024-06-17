#if 1
#pragma once
#include"Algo.AudioCal.Com.h"
namespace Algo {
	namespace Audio {

#if 1
		typedef b1(*ALGO_PRODUCT_WITH_FIXED_POINT_CB)(void* src, void* fac, const i32 productSample, const i8 channels, const i8 facFpNum, i8 dir);
		EXTERNC ALGO_PRODUCT_WITH_FIXED_POINT_CB Algo_GetProductWithFixedPoint(i8 iWidth, i8 fpWidth);
#endif

#if 1
		typedef b1(*ALGO_DOT_PRODUCT_WITH_FIXED_POINT_CB)(void* src, void* fac, const i32 productSample, const i8 channels, const i8 facFpNum);
		EXTERNC ALGO_DOT_PRODUCT_WITH_FIXED_POINT_CB Algo_GetDotProductWithFixedPoint(i8 iWidth, i8 fpWidth);
#endif

#if 1
		typedef b1(*ALGO_PRODUCT_ADD_WITH_FIXED_POINT_CB)(void* dst, void* src, void* fac, const i32 productSample, const i8 channels, const i8 facFpNum);
		EXTERNC ALGO_PRODUCT_ADD_WITH_FIXED_POINT_CB Algo_GetProductAddWithFixedPoint(i8 iWidth, i8 fpWidth, i8 oWidth);
#endif

#if 1
		typedef b1(*ALGO_DOT_PRODUCT_ADD_WITH_FIXED_POINT_CB)(void* dst, void* src, void* fac, const i32 productSample, const i8 channels, const i8 facFpNum);
		EXTERNC ALGO_DOT_PRODUCT_ADD_WITH_FIXED_POINT_CB Algo_GetDotProductAddWithFixedPoint(i8 iWidth, i8 fpWidth, i8 oWidth);
#endif
	}
}

#endif