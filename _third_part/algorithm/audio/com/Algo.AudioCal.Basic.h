#if 1
#pragma once
#include"Algo.Type.h"
#include"Algo.Macro.h"
#if 1
namespace Algo {
	namespace Audio {
		typedef b1(*ALGO_LEFT_SHIFT)(u8* src, const i32 samples, const i16 bits, const i16 channels);
		template<class T>
		STATIC INLINE b1 Algo_LeftShift(u8* src, const i32 samples, const i16 bits, const i16 channels)
		{
			T* pBuf = (T*)src;
			for (i32 s = 0; s < samples * channels; s++)
			{
				pBuf[s] << bits;
			}
			return true;
		}

		typedef b1(*ALGO_RIGHT_SHIFT)(u8* src, const i32 samples, const i16 bits, const i16 channels);
		template<class T>
		STATIC INLINE b1 Algo_RightShift(u8* src, const i32 samples, const i16 bits, const i16 channels)
		{
			T* pBuf = (T*)src;
			for (i32 s = 0; s < samples * channels; s++)
			{
				pBuf[s] >> bits;
			}
			return true;
		}
#endif


		typedef b1(*ALGO_APPEND_IN_FIXPOINT)(u8* src, i32 num, i32 den, const i16 channels, const i16 fpNum);
		template<class T>
		STATIC INLINE b1 Algo_AppendInFixPoint(u8* src, i32 num, i32 den, const i16 channels, const i16 fpNum)
		{
			const i16 bytesPerSample = sizeof(T) * channels;
			if (den != 0) {
				T fac = (T)(((i64)num << fpNum) / den);
				for (i16 ch = 0; ch < channels; ch++)
					ALGO_MEM_CPY(src, &fac, sizeof(T));
			}
			else {
				ALGO_MEM_SET(src, 0, bytesPerSample);
				//ALGO_PRINT("warn, den = 0!");
			}
			return true;
		}

	}
}


#endif