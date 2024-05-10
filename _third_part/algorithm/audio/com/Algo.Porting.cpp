#if 0
#pragma once

#include"Algo.PortingInner.h"

//inner
b1 PortingSet(PortingSetChoose choose, void* val)
{
	if (choose >= PortingSetChoose::PORTING_SET_CHOOSE_MAX)
		return false;
	switch (choose)
	{
	case PortingSetChoose::PORTING_SET_CHOOSE_DYNAMIC_MEMORY:
		break;
	case PortingSetChoose::PORTING_SET_CHOOSE_MEMORY_OPERATION:
		break;
	case PortingSetChoose::PORTING_SET_CHOOSE_MEMORY_PRINTER:
		break;
	default:
		break;
	}
	return true;
}



//mem set


typedef Printer_t 



//mem alloc
//printf



#endif