#if 1
#pragma once
#include"Algo.Type.h"
#include"Algo.Macro.h"
namespace Algo {
	namespace Audio {
		typedef b1(*ALGO_OVERLAP_ADD_CB)(u8* dst, u8* src0, u8* src1, const i32 overlapSample, const i16 channels);
		template<class T>
		STATIC INLINE b1 Algo_OverlapAdd(u8* dst, u8* src0, u8* src1, const i32 overlapSample, const i16 channels)
		{
			const i32 fixNum = 15;
			i32 factor;

			T* pDst = (T*)dst;
			T* pSrc0 = (T*)src0;
			T* pSrc1 = (T*)src1;

			for (i32 s = 0; s < overlapSample; s++) {
				factor = ((i32)s << fixNum) / overlapSample;
				for (i16 ch = 0; ch < channels; ch++) {
					*pDst = ((i64)(*pSrc0) * (((i32)1 << fixNum) - factor) + (i64)(*pSrc1) * factor) >> fixNum;
					pDst++;
					pSrc0++;
					pSrc1++;
				}
			}
			return true;
		}

#if 0
#pragma once
#include"Algo.Type.h"
#include"Algo.Macro.h"

		template<class T>
		STATIC INLINE b1 OverlapAdd_Local(AudioSamples& dst, i32 dstSample, AudioSamples& src, i32 srcSample, i32 overlapSample)
		{
			//check
			i32 fixNum = 15;
			i32 factor;

			auto pDst = (T*)dst.GetBufInSample(dstSample);
			auto pSrc = (T*)src.GetBufInSample(srcSample);

			for (i32 s = 0; s < overlapSample; s++)
			{
				factor = ((i32)s << fixNum) / overlapSample;
				for (i16 ch = 0; ch < dst._info->_channels; ch++)
				{
					*pDst = ((i64)(*pDst) * (((i32)1 << fixNum) - factor) + (i64)(*pSrc) * factor) >> fixNum;
					pDst++;
					pSrc++;
				}
			}
			return true;
		}
#endif
	}
}
#endif