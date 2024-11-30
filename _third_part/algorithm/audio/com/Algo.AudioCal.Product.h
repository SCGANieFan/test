#if 1
#pragma once
#include"Algo.AudioCal.Com.h"
namespace Algo {
namespace Audio {

class Product_c
{
public:
	enum class FuncMode_e {
		PRODUCT_WITH_FIXED_POINT = 0,
		PRODUCT_CH_WITH_FIXED_POINT = 0,
		PRODUCT_ADD_WITH_FIXED_POINT,
	};
	typedef b1(*PRODUCT_WITH_FIXED_POINT_CB)(void* dst, void* src, void* fac, const i32 productSample, const i8 channels, const i8 facFpNum, i8 facDir);
public:
	Product_c() {};
	~Product_c() {};
public:
	STATIC PRODUCT_WITH_FIXED_POINT_CB GetFunc_FixedPoint(FuncMode_e mode, i16 iWidth, i16 fpWidth);
private:
	template<class Ti, class To, class Tx>
	STATIC b1 ProductWithFixedPoint(void* dst, void* src, void* fac, const i32 productSample, const i8 channels, const i8 facFpNum, i8 facDir);
	template<class Ti, class To, class Tx>
	STATIC b1 ProductChWithFixedPoint(void* dst, void* src, void* fac, const i32 productSample, const i8 channels, const i8 facFpNum, i8 facDir);

	template<class Ti, class To, class Tx>
	STATIC b1 ProductAddWithFixedPoint(void* dst, void* src, void* fac, const i32 productSample, const i8 channels, const i8 facFpNum, i8 facDir);
};
}
}

#endif