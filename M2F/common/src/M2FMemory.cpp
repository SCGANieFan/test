
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#include"./M2FMemory.h"
#include"./M2FPrint.h"
#include"./M2FList.h"



//heap
#define m2f_malloc(size)					malloc(size)
#define m2f_calloc(count,size)				calloc(count,size)
//stack
#define m2f_alloca(size)					alloca(size)
//free
#define m2f_free(p)							free(p)
//memset
#define m2f_memClear(p,size)				memset(p,0,size)

//print
#define M2FMemeryDebug 1
#if M2FMemeryDebug 
#define M2F_MemeryPrint(s,...)				M2FPrint::PrintInner("[M2F_MEM](%d)<%s> " s "\n", __LINE__, __FUNCTION__,##__VA_ARGS__)
#else
#define M2F_MemeryPrint(s,...)				
#endif


M2FMemory::M2FMemory()
{
	this->stack = { 0 };
	this->heap = { 0 };	
}
M2FMemory::M2FMemory(void* pHeap, m2f_uint32 heapSize, void* pStack, m2f_uint32 stackSize)
{
	this->heap = { 0 };
	this->stack = { 0 };
	this->MemoryInit(&this->heap, pHeap, heapSize);
	this->MemoryInit(&this->stack, pStack, stackSize);
}
M2FMemory::~M2FMemory()
{
	this->MemoryDeInit(&this->heap);
	this->MemoryDeInit(&this->stack);
}

void* M2FMemory::MAlloc(m2f_uint32 size)
{
	return AllocInner(&this->heap, size);
}
//void* M2FMemory::CAlloc(m2f_uint32 count, m2f_uint32 size)
//{
//
//}
void* M2FMemory::SAlloc(m2f_uint32 size)
{
	return AllocInner(&this->stack, size);
}
m2f_uint32 M2FMemory::Free(void* p)
{
	m2f_uint32 ret = 0;
	ret = this->FreeInner(&this->heap, p);
	if (!ret)
		ret = this->FreeInner(&this->stack, p);
	return ret;
}	

m2f_int32 M2FMemory::HeapInit(void* p, m2f_uint32 size)
{
	m2f_int32 ret = -1;
	if (this->heap.usedLists == NULL)
	{
		if (!this->MemoryInit(&this->heap, p, size))
			ret = 0;
	}
	else
		M2F_MemeryPrint("just can initialize once");
	return ret;
}


m2f_int32 M2FMemory::StackInit(void* p, m2f_uint32 size)
{
	m2f_int32 ret = -1;
	if (this->stack.usedLists == NULL)
	{
		if (!this->MemoryInit(&this->stack, p, size))
			ret = 0;
	}
	else
		M2F_MemeryPrint("just can initialize once");
	return ret;
}

m2f_uint32 M2FMemory::getMemUsedMax()
{
	m2f_uint32 size = 0;
	if (this->heap.usedLists)
		size += this->heap.sizeUsedMax;
	if (this->stack.usedLists)
		size += this->stack.sizeUsedMax;
	return size;
}
m2f_uint32 M2FMemory::getMemUsed()
{
	m2f_uint32 size = 0;
	if (this->heap.usedLists)
		size += this->heap.sizeUsed;
	if (this->stack.usedLists)
		size += this->stack.sizeUsed;
	return size;
}

void* M2FMemory::getHeapAddress()
{
	if (this->heap.usedLists)
		return this->heap.address;
	else
		NULL;
}
void* M2FMemory::getStackAddress()
{
	if (this->stack.usedLists)
		return this->stack.address;
	else
		NULL;
}





m2f_int32 M2FMemory::MemoryInit(Memory* mem, void* p, m2f_uint32 size)
{
	if (!mem)
	{
		M2F_MemeryPrint("input error,%x", mem);
		return -1;
	}
	m2f_uint32 listsSize = sizeof(Lists);
	m2f_uint32 listSize = sizeof(List);
	if (	p
		&&	size > 0)
	{
		mem->address = p;
		mem->sizeMax = size;
		mem->sizeUsed = 0;
		mem->sizeUsedMax = 0;
		mem->usedLists = m2f_malloc(listsSize);
		mem->resLists = m2f_malloc(listsSize);
		m2f_memClear(mem->usedLists, listsSize);
		m2f_memClear(mem->resLists, listsSize);
		List* list = (List*)m2f_malloc(listSize);
		m2f_memClear(list, listSize);
		list->p = p;
		list->size = size;
		list->next = NULL;
		ListAdd((Lists*)mem->resLists, list);
	}
	else
	{
		mem->address = NULL;
		mem->sizeMax = 0;
		mem->sizeUsed = 0;
		mem->sizeUsedMax = 0;
		mem->usedLists = m2f_malloc(listsSize);
		mem->resLists = NULL;
		m2f_memClear(mem->usedLists, listsSize);
	}
	return 0;
}
m2f_int32 M2FMemory::MemoryDeInit(Memory* mem)
{
	if (!mem)
	{
		M2F_MemeryPrint("input error,%x", mem);
		return -1;
	}
	Lists* lists = NULL;
	List* list = NULL;
	if (mem->usedLists)
	{
		if (mem->resLists)
		{
			lists = (Lists*)mem->usedLists;
			list = lists->first;
			for (m2f_uint32 i = 0; i < lists->num; i++)
			{
				lists->first = list->next;
				m2f_free(list);
				list = lists->first;
			}
			m2f_free(mem->usedLists);
			m2f_free(mem->resLists);
		}
		else
		{
			lists = (Lists*)mem->usedLists;
			list = lists->first;
			for (m2f_uint32 i = 0; i < lists->num; i++)
			{
				lists->first = list->next;
				m2f_free(list->p);
				m2f_free(list);
				list = lists->first;
			}
			m2f_free(mem->usedLists);
		}
	}
	M2F_MemeryPrint("input error,%x", mem);
	return 0;
}

void* M2FMemory::AllocInner(Memory* mem, m2f_uint32 size)
{
	if (!mem)
	{
		M2F_MemeryPrint("mem is null");
	}
	if (!size)
	{
		return NULL;
	}
	Lists* resLists = (Lists*)mem->resLists;
	Lists* usedLists = (Lists*)mem->usedLists;
	List* listUsed = NULL;
	if (resLists)
	{
		List* listRes = ListSearch(resLists, size);
		if (!listRes)
		{
			M2F_MemeryPrint("have not enough space to alloc");
			return NULL;
		}
		listUsed = (List*)m2f_malloc(size);
		listUsed->p = listRes->p;
		listUsed->size = listRes->size;
		ListAdd(usedLists, listUsed);

		listRes->size = listRes->size - size;
		listRes->p = (char*)listRes->p + size;
	}
	else
	{
		listUsed = (List*)m2f_malloc(size);
		if (mem == &this->heap)
			listUsed->p = m2f_malloc(size);
		else
			listUsed->p = m2f_alloca(size);
		listUsed->size = size;
		ListAdd(usedLists, listUsed);
	}
	mem->sizeUsed += listUsed->size;
	mem->sizeUsedMax = mem->sizeUsedMax > mem->sizeUsed ? mem->sizeUsedMax : mem->sizeUsed;
	return listUsed->p;
}

m2f_uint32 M2FMemory::FreeInner(Memory* mem, void* p)
{
	if (!mem)
	{
		M2F_MemeryPrint("mem is null", mem, p);
		return 0;
	}
	if (!p)
	{
		return 0;
	}
	Lists* resLists = (Lists*)mem->resLists;
	Lists* usedLists = (Lists*)mem->usedLists;
	List* listUsed = ListSearch(usedLists, p);
	if (!listUsed)
	{
		M2F_MemeryPrint("no such pointer,%x", p);
		return 0;
	}
	if (resLists)
	{
		List* listRes = (List*)m2f_malloc(sizeof(List));
		listRes->p = listUsed->p;
		listRes->size = listUsed->size;
		ListAdd(resLists, listRes);
	}
	m2f_uint32 freeSize = listUsed->size;;
	//usedLists
	if (ListDelete(usedLists, listUsed))
	{
		M2F_MemeryPrint("ListDelete error");
		return 0;
	}
	m2f_free(listUsed->p);
	m2f_free(listUsed);

	mem->sizeUsed -= freeSize;
	return freeSize;
}




