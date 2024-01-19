#ifndef __M2F_COMMON__H_
#define __M2F_COMMON__H_

#include"./M2FObject.h"


class M2FCommon : public M2FObject
{
public:
	M2FCommon();
	~M2FCommon();

	void HeapInit(void* p, m2f_uint32 size);
	void StackInit(void* p, m2f_uint32 size);

	void* getHeapAddress();
	void* getStackAddress();
	
	m2f_uint32 getMemUsed();
	m2f_uint32 getMemUsedMax();
protected:
	void* MAlloc(m2f_uint32 size);
	void* SAlloc(m2f_uint32 size);
	void Free(void* p);
	void Print(const char* _Format, ...);



public:
private:
	void* memory;//M2FMemory*
};


#endif