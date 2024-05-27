#include"Algo.AudioCal.Basic.h"
using namespace Algo;
using namespace Audio;


/*************************************************************************
******************************* shift ************************************
*************************************************************************/
template<int shiftType, class Ti, class To, class Tx>
class Algo_Shift {};

template<class Ti, class To, class Tx>
class Algo_Shift< 1, Ti, To, Tx > {
public:
	STATIC INLINE void DoShift(void*in, void* out, i32 samples, i8 shiftNum) {
		Ti* pIn = (Ti*)in;
		To* pOut = (To*)out;
		for (i32 s = 0; s < samples; s++) {
			*pOut = (To)(((Tx)*pIn) >> shiftNum);
			pIn++;
			pOut++;
		}
	}	
};

template<class Ti, class To, class Tx>
class Algo_Shift< -1, Ti, To, Tx > {
public:
	STATIC INLINE void DoShift(void* in, void* out, i32 samples, i8 shiftNum) {
		Ti* pIn = (Ti*)in;
		To* pOut = (To*)out;
		for (i32 s = 0; s < samples; s++) {
			*pOut = (To)(((Tx)*pIn) << shiftNum);
			pIn++;
			pOut++;
		}
	}
};

template<class Ti, class To, class Tx>
class Algo_Shift< 0, Ti, To, Tx > {
public:
	STATIC INLINE void DoShift(void* in, void* out, i32 samples, i8 shiftNum) {
		Ti* pIn = (Ti*)in;
		To* pOut = (To*)out;
		for (i32 s = 0; s < samples; s++) {
			*pOut = (To)(pIn);
			pIn++;
			pOut++;
		}
	}
};


STATIC ALGO_SHIFT_CB algoShift_L[4][4] = {
	{Algo_Shift<-1,i8,i8,i8>::DoShift,Algo_Shift<-1,i8,i16,i16>::DoShift,Algo_Shift<-1,i8,i24,i24>::DoShift,Algo_Shift<-1,i8,i32,i32>::DoShift},
	{Algo_Shift<-1,i16,i8,i16>::DoShift,Algo_Shift<-1,i16,i16,i16>::DoShift,Algo_Shift<-1,i16,i24,i24>::DoShift,Algo_Shift<-1,i16,i32,i32>::DoShift},
	{Algo_Shift<-1,i24,i8,i24>::DoShift,Algo_Shift<-1,i24,i16,i24>::DoShift,Algo_Shift<-1,i24,i24,i24>::DoShift,Algo_Shift<-1,i24,i32,i32>::DoShift},
	{Algo_Shift<-1,i32,i8,i32>::DoShift,Algo_Shift<-1,i32,i16,i32>::DoShift,Algo_Shift<-1,i32,i24,i32>::DoShift,Algo_Shift<-1,i32,i32,i32>::DoShift},
};

STATIC ALGO_SHIFT_CB algoShift_R[4][4] = {
	{Algo_Shift<1,i8,i8,i8>::DoShift,Algo_Shift<1,i8,i16,i16>::DoShift,Algo_Shift<1,i8,i24,i24>::DoShift,Algo_Shift<1,i8,i32,i32>::DoShift},
	{Algo_Shift<1,i16,i8,i16>::DoShift,Algo_Shift<1,i16,i16,i16>::DoShift,Algo_Shift<1,i16,i24,i24>::DoShift,Algo_Shift<1,i16,i32,i32>::DoShift},
	{Algo_Shift<1,i24,i8,i24>::DoShift,Algo_Shift<1,i24,i16,i24>::DoShift,Algo_Shift<1,i24,i24,i24>::DoShift,Algo_Shift<1,i24,i32,i32>::DoShift},
	{Algo_Shift<1,i32,i8,i32>::DoShift,Algo_Shift<1,i32,i16,i32>::DoShift,Algo_Shift<1,i32,i24,i32>::DoShift,Algo_Shift<1,i32,i32,i32>::DoShift},
};

EXTERNC ALGO_SHIFT_CB Algo_GetShift(i8 iWidth, i8 oWidth, AlgoShiftChoose_E choose)
{
	if (choose == AlgoShiftChoose_L)
		return algoShift_L[iWidth - 1][oWidth - 1];
	else
		return algoShift_R[iWidth - 1][oWidth - 1];
}


/*************************************************************************
******************************* append ************************************
*************************************************************************/
template<class Ti, class Tx>
class Algo_Append{
public:
	STATIC INLINE b1 AppendInFixPoint(void* src, void* num, void* den, const i32 samples, const i8 channels, const i8 fpNum) {
		const i8 bytesPerSample = sizeof(Ti) * channels;
		Tx* pNum = (Tx*)num;
		Tx* pDen = (Tx*)den;
		for (i32 s = 0; s < samples; s++) {
			if (*pDen != 0) {
				Ti fac = (Ti)(((i64)(*pNum) << fpNum) / (*pDen));
				for (i16 ch = 0; ch < channels; ch++)
					ALGO_MEM_CPY(src, &fac, sizeof(Ti));
			}
			else {
				ALGO_MEM_SET(src, 0, bytesPerSample);
				//ALGO_PRINT("warn, den = 0!");
			}
			pNum++;
			pDen++;
		}
		return true;
	}
};


STATIC ALGO_AppendInFixPoint_CB appendInFixPoint[4][4] = {
	{Algo_Append<i8,i8>::AppendInFixPoint,Algo_Append<i8,i16>::AppendInFixPoint,Algo_Append<i8,i24>::AppendInFixPoint,Algo_Append<i8,i32>::AppendInFixPoint},
	{Algo_Append<i16,i8>::AppendInFixPoint,Algo_Append<i16,i16>::AppendInFixPoint,Algo_Append<i16,i24>::AppendInFixPoint,Algo_Append<i16,i32>::AppendInFixPoint},
	{Algo_Append<i24,i8>::AppendInFixPoint,Algo_Append<i24,i16>::AppendInFixPoint,Algo_Append<i24,i24>::AppendInFixPoint,Algo_Append<i24,i32>::AppendInFixPoint},
	{Algo_Append<i32,i8>::AppendInFixPoint,Algo_Append<i32,i16>::AppendInFixPoint,Algo_Append<i32,i24>::AppendInFixPoint,Algo_Append<i32,i32>::AppendInFixPoint},
};

EXTERNC ALGO_AppendInFixPoint_CB Algo_GetAppendInFixedPoint(i8 iWidth, i8 fpWidth)
{
	return appendInFixPoint[iWidth - 1][fpWidth - 1];
}


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
	for (i32 s = channelSelect; s < samples * channels; s += channels) {
		*pDst++ = (To)(pSrc[s]);
	}
	return true;
}
#endif







