#if 1
#include"Algo.AudioCal.OverlapAdd.h"
using namespace Algo;
using namespace Audio;


template<class Ti, class To>
class Algo_OverlapAdd {
public:
	STATIC INLINE b1 OverlapAdd(void* dst, void* srcDecline, void* srcRise, const i16 channels, i32* factor, const i32 startOverlapSample, const i32 endOverlapSample,i32 overlapSample){
		const i32 fixNum = 15;
		//i32 factor;
		Ti* pSrcRise = (Ti*)srcRise;
		Ti* pSrcDecline = (Ti*)srcDecline;
		To* pDst = (To*)dst;
		for (i32 s = startOverlapSample; s < endOverlapSample; s++) {
			//factor = ((i32)s << fixNum) / overlapSample;
			for (i16 ch = 0; ch < channels; ch++) {
				//*pDst = (To)((((i64)(*pSrcRise) * factor + (i64)(*pSrcDecline) * ((i32)1 << fixNum) - factor)) >> fixNum);
#if 0
				*pDst = (To)(((i64)(*pSrcRise) * (((i32)1 << fixNum) - factor) + (i64)(*pSrcDecline) * factor) >> fixNum);
#else
				//* pDst = (To)(((i64)(*pSrcRise) * factor + (i64)(*pSrcDecline) * (((i32)1 << fixNum) - factor)) >> fixNum);
				* pDst = (To)(((i64)(*pSrcRise) * factor[overlapSample-s-1] + (i64)(*pSrcDecline) * factor[s]) >> fixNum);
#endif
				pDst++;
				pSrcRise++;
				pSrcDecline++;
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