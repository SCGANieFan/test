#if 1
#include"Algo.AS_Calculator.h"

#include"Algo.AS_Calculator.basic.h"
#include"Algo.AS_Calculator.OverlapAdd.h"
#include"Algo.AS_Calculator.Product.h"
#include"Algo.AS_Calculator.WaveFormMatch.h"


static AS_Calculator::FuncList funcList16 = {
	AppendInFixPoint_Local<i16>,
	LeftShift_Local<i16>,
	LeftShiftSigned_Local<i16>,
	RightShift_Local<i16>,
	RightShiftSigned_Local<i16>,
	Product_Local<i16>,
	OverlapAdd_Local<i16>,
	WaveFormMatch_Local<i16>
};

static AS_Calculator::FuncList funcList32 = {
	AppendInFixPoint_Local<i32>,
	LeftShift_Local<i32>,
	LeftShiftSigned_Local<i32>,
	RightShift_Local<i32>,
	RightShiftSigned_Local<i32>,
	Product_Local<i32>,
	OverlapAdd_Local<i32>,
	WaveFormMatch_Local<i32>
};


void AS_Calculator::Init(i16 width)
{

	if (width == 2)
	{
		_funcList = &funcList16;
	}
	else if (width == 3)
	{

	}
	else
	{
		_funcList = &funcList32;
	}
}
#endif