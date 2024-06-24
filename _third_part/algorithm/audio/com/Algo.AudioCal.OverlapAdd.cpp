#if 1

#pragma once
#include"Algo.AudioCal.Com.h"
#include"Algo.AudioCal.OverlapAdd.h"
#include"Algo.AudioCal.BufferGenerator.h"
using namespace Algo;
using namespace Audio;
OverlapAdd_c::OVERLAP_ADD_CORE_CB OverlapAdd_c::GetFunc(i8 Width) {
	if (Width == 1) {
		return OverlapAdd_Core<i8, i8>;
	}
	else if (Width == 2) {
		return OverlapAdd_Core<i16, i16>;
	}
	else if (Width == 3) {
		return OverlapAdd_Core<i24, i24>;
	}
	else {
		return OverlapAdd_Core<i32, i32>;
	}
}

void OverlapAdd_c::GenerateWindow(WindowChoose choose, i32* factor, i32 samples, i16 fpNum) {
	BufferGenerator_c::BufferChoose_e bufferChoose = BufferGenerator_c::BufferChoose_e::WINDOW_COSINE_FADE;
	if (choose == WindowChoose::Line)
		bufferChoose = BufferGenerator_c::BufferChoose_e::WINDOW_LINE_FADE;
	else if (choose == WindowChoose::Cosine)
		bufferChoose = BufferGenerator_c::BufferChoose_e::WINDOW_COSINE_FADE;
	BufferGenerator_c::Generate(bufferChoose, factor, samples, fpNum);
}

template<class Ti, class To>
b1 OverlapAdd_c::OverlapAdd_Core(void* dst, void* srcDecline, void* srcRise, const i16 channels, i32* factorDecLine, const i32 startOverlapSample, const i32 endOverlapSample, i32 overlapSample) {
	const i32 fixNum = 15;
	Ti* pSrcRise = (Ti*)srcRise;
	Ti* pSrcDecline = (Ti*)srcDecline;
	To* pDst = (To*)dst;
	for (i32 s = startOverlapSample; s < endOverlapSample; s++) {
		for (i16 ch = 0; ch < channels; ch++) {
			*pDst = (To)(((i64)(*pSrcRise) * factorDecLine[overlapSample - s - 1] + (i64)(*pSrcDecline) * factorDecLine[s]) >> fixNum);
			pDst++;
			pSrcRise++;
			pSrcDecline++;
		}
	}
	return true;
}


#endif