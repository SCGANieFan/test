#if 1
#include"Algo.AudioCal.OverlapAdd.h"
using namespace Algo;
using namespace Audio;


template<class Ti, class To>
class Algo_OverlapAdd {
public:
	STATIC INLINE b1 OverlapAdd(void* dst, void* src0, void* src1, const i32 overlapSample, const i16 channels) {
		const i32 fixNum = 15;
		i32 factor;
		Ti* pSrc0 = (Ti*)src0;
		Ti* pSrc1 = (Ti*)src1;
		To* pDst = (To*)dst;
		for (i32 s = 0; s < overlapSample; s++) {
			factor = ((i32)s << fixNum) / overlapSample;
			for (i16 ch = 0; ch < channels; ch++) {
				*pDst = (To)(((i64)(*pSrc0) * (((i32)1 << fixNum) - factor) + (i64)(*pSrc1) * factor) >> fixNum);
				pDst++;
				pSrc0++;
				pSrc1++;
			}
		}
		return true;
	}
};



STATIC const ALGO_OVERLAP_ADD_CB overlapAdd[4][4] = {
	{Algo_OverlapAdd<i8,i8>::OverlapAdd,Algo_OverlapAdd<i8,i16>::OverlapAdd,Algo_OverlapAdd<i8,i24>::OverlapAdd,Algo_OverlapAdd<i8,i32>::OverlapAdd},
	{Algo_OverlapAdd<i16,i8>::OverlapAdd,Algo_OverlapAdd<i16,i16>::OverlapAdd,Algo_OverlapAdd<i16,i24>::OverlapAdd,Algo_OverlapAdd<i16,i32>::OverlapAdd},
	{Algo_OverlapAdd<i24,i8>::OverlapAdd,Algo_OverlapAdd<i24,i16>::OverlapAdd,Algo_OverlapAdd<i24,i24>::OverlapAdd,Algo_OverlapAdd<i24,i32>::OverlapAdd},
	{Algo_OverlapAdd<i32,i8>::OverlapAdd,Algo_OverlapAdd<i32,i16>::OverlapAdd,Algo_OverlapAdd<i32,i24>::OverlapAdd,Algo_OverlapAdd<i32,i32>::OverlapAdd},
};

EXTERNC ALGO_OVERLAP_ADD_CB Algo_GetOverlapAdd(i8 iWidth, i8 oWidth) 
{
	return overlapAdd[iWidth - 1][oWidth - 1];
}

#endif