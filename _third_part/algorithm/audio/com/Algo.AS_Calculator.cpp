#if 1
#include"Algo.AS_Calculator.h"

extern AS_Calculator* asCalculator16;
extern AS_Calculator* asCalculator32;

AS_Calculator* AS_Calculator::GetCaculator(i16 width)
{
	switch (width)
	{
	case 2:
		return asCalculator16;
	case 4:
		return asCalculator32;
	default:
		return 0;
		break;
	}
}
#endif