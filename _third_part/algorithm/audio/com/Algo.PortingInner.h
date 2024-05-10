#if 0
#pragma once
#include"Algo.Porting.h"
#include"Algo.Type.h"


typedef enum {
	PORTING_SET_CHOOSE_DYNAMIC_MEMORY= 0 ,
	PORTING_SET_CHOOSE_MEMORY_OPERATION,
	PORTING_SET_CHOOSE_MEMORY_PRINTER,
	PORTING_SET_CHOOSE_MAX,
}PortingSetChoose;




b1 PortingSet(PortingSetChoose choose, void* val);




#endif