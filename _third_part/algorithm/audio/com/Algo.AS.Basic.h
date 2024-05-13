#if 1
#pragma once
#include"Algo.Type.h"
#include"Algo.Macro.h"
#include"Algo.AudioData.h"
#if 1
typedef b1(*ALGO_LEFT_SHIFT)(const AudioInfo* info, u8* src, i32 samples, i16 bits);
template<class T>
STATIC INLINE b1 Algo_LeftShift(const AudioInfo* info, u8* src, i32 samples, i16 bits)
{
	T* pBuf = (T*)src;
	for (i32 s = 0; s < samples * info->_channels; s++)
	{
		pBuf[s] << bits;
	}
	return true;
}

typedef b1(*ALGO_RIGHT_SHIFT)(const AudioInfo* info, u8* src, i32 samples, i16 bits);
template<class T>
STATIC INLINE b1 Algo_RightShift(const AudioInfo* info, u8* src, i32 samples, i16 bits)
{
	T* pBuf = (T*)src;
	for (i32 s = 0; s < samples * info->_channels; s++)
	{
		pBuf[s] >> bits;
	}
	return true;
}
#endif


typedef b1(*ALGO_APPEND_IN_FIXPOINT)(const AudioInfo* info, u8* src, i32 num, i32 den, i16 fpNum);
template<class T>
STATIC INLINE b1 Algo_AppendInFixPoint(const AudioInfo* info, u8* src, i32 num, i32 den, i16 fpNum)
{
	if (den != 0) {
		T fac = (T)(((i64)num << fpNum) / den);
		for (i16 ch = 0; ch < info->_channels; ch++)
			ALGO_MEM_CPY(src, &fac, sizeof(T));
	}
	else {
		ALGO_MEM_SET(src, 0, info->_bytesPerSample);
		//ALGO_PRINT("warn, den = 0!");
	}
	return true;
}




#endif