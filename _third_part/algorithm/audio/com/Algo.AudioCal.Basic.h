#if 1
#pragma once
#include"Algo.AudioCal.Com.h"
namespace Algo {
	namespace Audio {
#if 1
		typedef enum {
			AlgoShiftChoose_L = 0,
			AlgoShiftChoose_R,
		}AlgoShiftChoose_E;
		typedef void (*ALGO_SHIFT_CB)(void* in, void* out, i32 samples, i8 shiftNum);
		EXTERNC ALGO_SHIFT_CB Algo_GetShift(i8 iWidth, i8 oWidth, AlgoShiftChoose_E choose);

#endif

#if 1
		typedef b1(*ALGO_AppendInFixPoint_CB)(void* src, void* num, void* den, const i32 samples, const i8 channels, const i8 fpNum);
		EXTERNC ALGO_AppendInFixPoint_CB Algo_GetAppendInFixedPoint(i8 iWidth, i8 fpWidth);
#endif


		
}

}


#endif