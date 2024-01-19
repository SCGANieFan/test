#ifndef __M2F_PRINT__H_
#define __M2F_PRINT__H_

#include"./M2FObject.h"


class M2FPrint :virtual public M2FObject
{
	friend class M2FCommon;
	friend class M2FMemory;
public:
	M2FPrint();
	~M2FPrint();
	static void Print(const char* _Format, void* args);
private:
	static void PrintInner(const char* _Format, ...);
	static void PrintInner(char* buf);
};





#endif