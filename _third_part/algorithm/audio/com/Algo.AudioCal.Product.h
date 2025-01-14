#pragma once
#include"Algo.Type.h"
#include"Algo.Macro.h"
#include"Algo.Printer.h"
#include"Algo.AudioCal.Basic.h"

namespace Algo {
namespace Audio {
template<class Ti, class To, class Tf, class Tx, i8 _rShift, i8 dir, i8 type = ((i8)(_rShift == 0 ? 0 : 1) * 10 + (i8)(dir == -1 ? 0 : 1))>
struct Product_t {
	STATIC void RunAllCh(void* dst, void* src, void* fac, const i32 productSample, i8 channels) {
		return;
	}
	STATIC void RunCh(void* dst, void* src, void* fac, const i32 productSample, i8 channels) {
		return;
	}
};
#if 1
//_rShift=0,dir=-1
template<class Ti, class To, class Tf, class Tx, i8 _rShift, i8 dir>
struct Product_t<Ti, To, Tf, Tx, _rShift, dir, 0> {
	STATIC void RunAllCh(void* dst, void* src, void* fac, const i32 productSample, i8 channels) {
		To* pDst = (To*)dst;
		Ti* pSrc = (Ti*)src;
		Tf* pFac = (Tf*)fac;
		for (i32 s = 0; s < productSample; s++) {
			for (i8 ch = 0; ch < channels; ch++) {
				Tx tmp = static_cast<Tx>(*pSrc) * *pFac;
				*pDst = static_cast<To>(tmp);
				pDst++;
				pSrc++;
			}
			pFac--;
		}
	}
	STATIC void RunCh(void* dst, void* src, void* fac, const i32 productSample, i8 channels) {
		To* pDst = (To*)dst;
		Ti* pSrc = (Ti*)src;
		Tf* pFac = (Tf*)fac;
		for (i32 s = 0; s < productSample; s++) {
			Tx tmp = static_cast<Tx>(*pSrc) * *pFac;
			*pDst = static_cast<To>(tmp);
			pDst += channels;
			pSrc += channels;
			pFac--;
		}
	}
};

//_rShift=0,dir=1
template<class Ti, class To, class Tf, class Tx, i8 _rShift, i8 dir>
struct Product_t<Ti, To, Tf, Tx, _rShift, dir, 1> {
	STATIC void RunAllCh(void* dst, void* src, void* fac, const i32 productSample, i8 channels) {
		To* pDst = (To*)dst;
		Ti* pSrc = (Ti*)src;
		Tf* pFac = (Tf*)fac;
		for (i32 s = 0; s < productSample; s++) {
			for (i8 ch = 0; ch < channels; ch++) {
				Tx tmp = static_cast<Tx>(*pSrc) * *pFac;
				*pDst = static_cast<To>(tmp);
				pDst++;
				pSrc++;
			}
			pFac++;
		}
	}
	STATIC void RunCh(void* dst, void* src, void* fac, const i32 productSample, i8 channels) {
		To* pDst = (To*)dst;
		Ti* pSrc = (Ti*)src;
		Tf* pFac = (Tf*)fac;
		for (i32 s = 0; s < productSample; s++) {
			Tx tmp = static_cast<Tx>(*pSrc) * *pFac;
			*pDst = static_cast<To>(tmp);
			pDst += channels;
			pSrc += channels;
			pFac++;
		}
	}
};

//_rShift!=0,dir=-1
template<class Ti, class To, class Tf, class Tx, i8 _rShift, i8 dir>
struct Product_t<Ti, To, Tf, Tx, _rShift, dir, 10> {
	STATIC void RunAllCh(void* dst, void* src, void* fac, const i32 productSample, i8 channels) {
		To* pDst = (To*)dst;
		Ti* pSrc = (Ti*)src;
		Tf* pFac = (Tf*)fac;
		for (i32 s = 0; s < productSample; s++) {
			for (i8 ch = 0; ch < channels; ch++) {
				Tx tmp = static_cast<Tx>(*pSrc)* *pFac;
				*pDst = RShift_c::Run<_rShift>(tmp);
				pDst++;
				pSrc++;
			}
			pFac--;
		}
	}
	STATIC void RunCh(void* dst, void* src, void* fac, const i32 productSample, i8 channels) {
		To* pDst = (To*)dst;
		Ti* pSrc = (Ti*)src;
		Tf* pFac = (Tf*)fac;
		for (i32 s = 0; s < productSample; s++) {
			Tx tmp = static_cast<Tx>(*pSrc) * *pFac;
			*pDst = RShift_c::Run<_rShift>(tmp);
			pDst += channels;
			pSrc += channels;
			pFac--;
		}
	}
};

//_rShift!=0,dir=1
template<class Ti, class To, class Tf, class Tx, i8 _rShift, i8 dir>
struct Product_t<Ti, To, Tf, Tx, _rShift, dir, 11> {
	STATIC void RunAllCh(void* dst, void* src, void* fac, const i32 productSample, i8 channels) {
		To* pDst = (To*)dst;
		Ti* pSrc = (Ti*)src;
		Tf* pFac = (Tf*)fac;
		for (i32 s = 0; s < productSample; s++) {
			for (i8 ch = 0; ch < channels; ch++) {
				Tx tmp = static_cast<Tx>(*pSrc) * *pFac;
				*pDst = RShift_c::Run<_rShift>(tmp);
				pDst++;
				pSrc++;
			}
			pFac++;
		}
	}
	STATIC void RunCh(void* dst, void* src, void* fac, const i32 productSample, i8 channels) {
		To* pDst = (To*)dst;
		Ti* pSrc = (Ti*)src;
		Tf* pFac = (Tf*)fac;
		for (i32 s = 0; s < productSample; s++) {
			Tx tmp = static_cast<Tx>(*pSrc) * *pFac;
			*pDst = RShift_c::Run<_rShift>(tmp);
			pDst += channels;
			pSrc += channels;
			pFac++;
		}
	}
};
#endif
#if 1
template<class Ti, class To, class Tf, class Tx, i8 _rShift = 0>
#endif
class Product_c
{
public:
	Product_c() {}
	~Product_c() {}
public:
	STATIC INLINE void RunAllChForWard(void* dst, void* src, void* fac, const i32 productSample, i32 channels) {
		Product_t<Ti, Ti, Tf, Tx, _rShift, 1>::RunAllCh(dst, src, fac, productSample, channels);
	}
	STATIC INLINE void RunAllChBackWard(void* dst, void* src, void* fac, const i32 productSample, i32 channels) {
		Product_t<Ti, Ti, Tf, Tx, _rShift, -1>::RunAllCh(dst, src, fac, productSample, channels);
	}
	STATIC INLINE void RunChForWard(void* dst, void* src, void* fac, const i32 productSample, i32 channels) {
		Product_t<Ti, Ti, Tf, Tx, _rShift, 1>::RunCh(dst, src, fac, productSample, channels);
	}
	STATIC INLINE void RunChBackWard(void* dst, void* src, void* fac, const i32 productSample, i32 channels) {
		Product_t<Ti, Ti, Tf, Tx, _rShift, -1>::RunCh(dst, src, fac, productSample, channels);
	}
};

}
}