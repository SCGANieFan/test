#pragma once
#include<new>
#include"Algo.BasePorting.h"
#include"Algo.Macro.h"
#include"Algo.Type.h"
#include"Algo.BasePorting.h"


namespace Algo{

class MemoryManger_c
{
public:
	MemoryManger_c() {};
	~MemoryManger_c() {};
//public:
	//INLINE AlgoBasePorting* GetBasePorting() { return _basePorting; }
public:
	INLINE void Init(AlgoBasePorting *basePorting) {
		_basePorting = basePorting;
		for (void*& p : _allocList) {
			p = 0;
		}
	}

	INLINE void* Malloc(i32 size) {
		void* ptr = _basePorting->Malloc(size);
		for (void*& p : _allocList){
			if (!p){
				p = ptr;
				return p;
			}
		}
		return 0;
	}

	template<class T>
	INLINE void* New(i32 size){
		void *ptr = Malloc(size);
		if(!ptr)
			return 0;
		return new(ptr) T();
	}

	INLINE void Free(void *ptr) {
		for (void*& p : _allocList) {
			if (p== ptr) {
				_basePorting->Free(p);
				p = 0;
				return;
			}
		}
	}

	template<class T>
	INLINE void* Delete(void *ptr){
		(T*)ptr->~T();
		Free(ptr);
	}

	INLINE void FreeAll () {
		for (void*& p : _allocList) {
			if (p) {
				_basePorting->Free(p);
				p = 0;
			}
		}
	}
private:
	AlgoBasePorting* _basePorting;
	void* _allocList[100];
};



}







