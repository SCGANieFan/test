#if 0
#include"Algo.AS_CalculatorInner.h"

//#include"Algo.AS_Calculator.basic.h"
//#include"Algo.AS_Calculator.OverlapAdd.h"
//#include"Algo.AS_Calculator.Product.h"
//#include"Algo.AS_Calculator.WaveFormMatch.h"

#if 0
static FuncList funcList16 = {
	AppendInFixPoint_Local<i16>,
	LeftShift_Local<i16>,
	LeftShiftSigned_Local<i16>,
	RightShift_Local<i16>,
	RightShiftSigned_Local<i16>,
	//Product_Local<i16>,
	//OverlapAdd_Local<i16>,
	//WaveFormMatch_Local<i16>
};

static FuncList funcList24 = {
	AppendInFixPoint_Local<i24>,
	LeftShift_Local<i24>,
	LeftShiftSigned_Local<i24>,
	RightShift_Local<i24>,
	RightShiftSigned_Local<i24>,
	//Product_Local<i24>,
	//OverlapAdd_Local<i24>,
	//WaveFormMatch_Local<i24>
};

static FuncList funcList32 = {
	//AppendInFixPoint_Local<i32>,
	//LeftShift_Local<i32>,
	//LeftShiftSigned_Local<i32>,
	//RightShift_Local<i32>,
	//RightShiftSigned_Local<i32>,
	//Product_Local<i32>,
	//OverlapAdd_Local<i32>,
	//WaveFormMatch_Local<i32>
};
#endif

void AS_Calculator::Init(i16 width)
{
#if 0
	if (width == 2)
	{
		_funcList = &funcList16;
	}
	else if (width == 3)
	{
		_funcList = &funcList24;
	}
	else
	{
		_funcList = &funcList32;
	}
#endif
}
#endif