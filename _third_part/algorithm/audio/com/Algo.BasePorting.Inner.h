#pragma once
#include"Algo.Macro.h"
#include"Algo.Type.h"
#include"Algo.BasePorting.h"


namespace Algo{

class MemoryManger_c
{
public:
	MemoryManger_c() {};
	~MemoryManger_c() {};
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

	INLINE void Free(void *ptr) {
		for (void*& p : _allocList) {
			if (p== ptr) {
				_basePorting->Free(p);
				p = 0;
				return;
			}
		}
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
	void* _allocList[10];
};



}







