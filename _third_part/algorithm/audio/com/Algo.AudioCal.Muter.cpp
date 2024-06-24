#if 1
#pragma once
#include"Algo.AudioCal.Product.h"
#include"Algo.AudioCal.BufferGenerator.h"
#include"Algo.AudioCal.Muter.h"

using namespace Algo;
using namespace Audio;


void Muter_c::Init(MemoryManger_c* MM, AudioInfo* info, WindowChoose_e windowChoose, DirChoose_e dirChoose, i32 decaySamples) {
	_product = Product_c::GetFunc_FixedPoint(Product_c::FuncMode_e::PRODUCT_WITH_FIXED_POINT, info->_width, sizeof(i16));
	_info = info;
	_facFpNum = 14;
	_muteFactorQ14 = (i16*)MM->Malloc(decaySamples * sizeof(i16));
	ALGO_MEM_SET(_muteFactorQ14, 0, decaySamples * sizeof(i16));
	_muteSamplesMax = decaySamples;

	BufferGenerator_c::BufferChoose_e bufferChoose = BufferGenerator_c::BufferChoose_e::WINDOW_LINE_FADE;
	if (windowChoose == WindowChoose_e::LINE)
		bufferChoose = BufferGenerator_c::BufferChoose_e::WINDOW_LINE_FADE;
	else if (windowChoose == WindowChoose_e::COSINE)
		bufferChoose = BufferGenerator_c::BufferChoose_e::WINDOW_COSINE_FADE;
	BufferGenerator_c::Generate(bufferChoose, _muteFactorQ14, _muteSamplesMax, _facFpNum);
	Reset(dirChoose);
}
#endif