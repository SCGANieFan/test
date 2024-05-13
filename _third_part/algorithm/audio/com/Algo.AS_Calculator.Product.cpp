#include"Algo.AS_CalculatorInner.h"
#include"Algo.AS_Calculator.Product.h"


typedef b1(*Product_CB)(AudioSamples& dst, i32 dstSample, AudioSamples& src, i32 srcSample, i32 productSample);
b1 AS_Calculator::Product(AudioSamples& dst, i32 dstSample, AudioSamples& src, i32 srcSample, i32 productSample)
{
	return ((Product_CB)((FuncList*)_funcList)->product_cb)(dst, dstSample, src, srcSample, productSample);
}
