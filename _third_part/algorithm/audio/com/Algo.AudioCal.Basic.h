#if 1
#pragma once
#include"Algo.Type.h"
#include"Algo.Macro.h"
namespace Algo {
	namespace Audio {
#if 1
		/*************************************************************************
		******************************* shift ************************************
		*************************************************************************/
#if 0
template<class Ti, class To, class Tx=int>
class CaculatorShift {
protected:
	typedef To(*ShiftCB)(Ti x, int8_t shift);
public:
	//CaculatorShift() {};
	inline CaculatorShift(int8_t shift) {
		_cb = NoShift;
		if (shift > 0){//left shift
			_cb = LeftShift;
		}
		else if (shift < 0){//right shift
			_cb = RightShift;
		}
	};
	~CaculatorShift() {};
public:
	inline To DoShift(Ti x, int8_t shift) {
		return _cb(x, shift);
	};
protected:
	static inline To RightShift(Ti x, int8_t shift) {
		return (To)((Tx)x >> shf);
	}
	static inline To LeftShift(Ti x, int8_t shift) {
		return (To)((Tx)x << shf);
	}
	static inline To NoShift(Ti x, int8_t shift) {
		return (To)x;
	}
protected:
	ShiftCB _cb = 0;
};
#endif
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

#if 1
		/*************************************************************************
		******************************* append ************************************
		*************************************************************************/
		typedef b1(*ALGO_APPEND_IN_FIXPOINT)(u8* src, u8* num, u8* den, const i32 samples, const i16 channels, const i16 fpNum);
		template<class T, class Tfp>
		STATIC INLINE b1 Algo_AppendInFixPoint(u8* src, u8* num, u8* den, const i32 samples, const i16 channels, const i16 fpNum)
		{
			const i16 bytesPerSample = sizeof(T) * channels;
			Tfp *pNum=(Tfp *)num;
			Tfp *pDen=(Tfp *)den;
			for(i32 s=0;s<samples;s++){
				if (*pDen != 0) {
					T fac = (T)(((i64)(*pNum) << fpNum) / (*pDen));
					for (i16 ch = 0; ch < channels; ch++)
						ALGO_MEM_CPY(src, &fac, sizeof(T));
				}
				else {
					ALGO_MEM_SET(src, 0, bytesPerSample);
					//ALGO_PRINT("warn, den = 0!");
				}
			}
			return true;
		}
#endif


#if 0
	/*************************************************************************
	******************************* channel  ************************************
	*************************************************************************/
	typedef b1(*ALGO_APPEND_IN_FIXPOINT)(u8* dst, u8* src, const i32 samples, const i16 channelSelect, const i16 channels);
	template<class Ti, class To>
	STATIC INLINE b1 Algo_ChannelSelect(u8* dst, u8* src, const i32 samples, const i16 channelSelect, const i16 channels)
	{
		const i16 bytesPerSample = sizeof(T) * channels;
		Ti* pSrc = (Ti*)src;
		To* pDst = (To*)dst;
		for (i32 s = channelSelect; s < samples* channels; s+= channels) {
			*pDst++ = (To)(pSrc[s]);
		}
		return true;
	}
#endif


}

}


#endif