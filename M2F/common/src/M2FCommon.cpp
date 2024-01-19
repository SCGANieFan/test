#include <stdio.h>
#include <stdarg.h>

#include"./M2FPrint.h"
#include"./M2FMemory.h"
#include"M2FCommon.h"


//print
#define M2FCommonDebug 1
#if M2FCommonDebug 
#define M2F_CommonPrint(s,...)				M2FPrint::PrintInner("[M2F_COM](%d)<%s> " s "\n", __LINE__, __FUNCTION__,##__VA_ARGS__)
#else
#define M2F_CommonPrint(s,...)				
#endif



M2FCommon::M2FCommon()
{
	this->memory = NULL;
}
M2FCommon::~M2FCommon()
{
	if (this->memory)
		delete(this->memory);
}

void M2FCommon::HeapInit(void* p, m2f_uint32 size)
{
	if (!this->memory)
	{
		this->memory = new(M2FMemory)(p, size, NULL, 0);
	}
		
	else
	{
		((M2FMemory*)this->memory)->HeapInit(p, size);
	}
}
void M2FCommon::StackInit(void* p, m2f_uint32 size)
{
	if (!this->memory)
	{
		this->memory = new(M2FMemory)(NULL, 0, p, size);
	}
	else
	{
		((M2FMemory*)this->memory)->StackInit(p, size);
	}
}

void* M2FCommon::getHeapAddress()
{
	return ((M2FMemory*)this->memory)->getHeapAddress();
}
void* M2FCommon::getStackAddress()
{
	return ((M2FMemory*)this->memory)->getStackAddress();
}

void* M2FCommon::MAlloc(m2f_uint32 size)
{
	void* ret = NULL;
	if (this->memory)
	{
		ret = ((M2FMemory*)this->memory)->MAlloc(size);
		//if (!ret)
		//	M2F_CommonPrint("MAlloc fail");
	}
	else
		M2F_CommonPrint("should first use init HeapInit() or StackInit()");
	return ret;
}
void* M2FCommon::SAlloc(m2f_uint32 size)
{
	void* ret = NULL;
	if (this->memory)
	{
		ret = ((M2FMemory*)this->memory)->SAlloc(size);
		//if (!ret)
		//	M2F_CommonPrint("SAlloc fail");
	}
	else
		M2F_CommonPrint("should first use init HeapInit() or StackInit()");		
	return ret;
}
void M2FCommon::Free(void* p)
{
	if (this->memory)
	{
		m2f_uint32 ret = ((M2FMemory*)this->memory)->Free(p);
		//if (!ret)
		//	M2F_CommonPrint("Free fail");
	}
	else
	{
		M2F_CommonPrint("should first use init HeapInit() or StackInit()");
	}
}

void M2FCommon::Print(const char* _Format, ...)
{
	if (!_Format)
	{
		M2F_CommonPrint("input is null");
		return;
	}
	va_list args;
	va_start(args, _Format);
	M2FPrint::Print(_Format, (void*)args);
	va_end(args);
}


m2f_uint32 M2FCommon::getMemUsed()
{
	if (this->memory)
	{
		return ((M2FMemory*)this->memory)->getMemUsed();
	}
	else
	{
		M2F_CommonPrint("should first use init HeapInit() or StackInit()");
		return -1;
	}
}

m2f_uint32 M2FCommon::getMemUsedMax()
{
	if (this->memory)
	{
		return ((M2FMemory*)this->memory)->getMemUsedMax();
	}
	else
	{
		M2F_CommonPrint("should first use init HeapInit() or StackInit()");
		return -1;
	}
}

