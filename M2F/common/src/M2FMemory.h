#ifndef __M2F_MEMORY__H_
#define __M2F_MEMORY__H_

#include"./M2FObject.h"

class M2FMemory :virtual public M2FObject
{

private:
	typedef struct Memory
	{
		void* address;
		m2f_uint32 sizeMax;
		m2f_uint32 sizeUsed;
		m2f_uint32 sizeUsedMax;
		void* usedLists;
		void* resLists;
	};
public:
	M2FMemory();
	M2FMemory(void* pHeap, m2f_uint32 heapSize, void* pStack, m2f_uint32 stackSize);
	~M2FMemory();
	
	void* MAlloc(m2f_uint32 size);
	// void* CAlloc(m2f_uint32 count, m2f_uint32 size);
	void* SAlloc(m2f_uint32 size);
	m2f_uint32 Free(void* p);

	m2f_int32 HeapInit(void* p, m2f_uint32 size);
	m2f_int32 StackInit(void* p, m2f_uint32 size);

	m2f_uint32 getMemUsedMax();
	m2f_uint32 getMemUsed();

	void* getHeapAddress();
	void* getStackAddress();

private:
	m2f_int32 MemoryInit(Memory* mem, void *p, m2f_uint32 size);
	m2f_int32 MemoryDeInit(Memory* mem);

	void* AllocInner(Memory* pMemory, m2f_uint32 size);
	m2f_uint32 FreeInner(Memory* mem, void* p);
	//variable
public:

protected:
private:
	Memory heap;//heap
	Memory stack;//stack
};


#endif