#pragma once
#include"Ape.Inner.basic.h"

class ApeMemManger
{
public:
    ApeMemManger() {}
    ~ApeMemManger() {}
public:
    void Init(AlgoBasePorting* basePorting) {
        this->basePorting = basePorting;
        ALGO_MEM_SET(mallocPtr, 0, sizeof(mallocPtr));
    }

    AlgoBasePorting* GetBasePorting() {
        return basePorting;
    }

    INLINE void* Malloc(i32 size){
        for (i16 i = 0; i < sizeof(mallocPtr) / sizeof(mallocPtr[0]); i++){
            if (mallocPtr[i] == 0){
                mallocPtr[i] = basePorting->Malloc(size);
                return mallocPtr[i];
            }
        }
        return 0;
    }

    INLINE void Free(void* block){
        for (i16 i = 0; i < sizeof(mallocPtr) / sizeof(mallocPtr[0]); i++){
            if (mallocPtr[i] == block){
                basePorting->Free(mallocPtr[i]);
                mallocPtr[i] = 0;
                return;
            }
        }
        return;
    }
    INLINE void FreeAll() {
        for (i16 i = 0; i < sizeof(mallocPtr) / sizeof(mallocPtr[0]); i++) {
            if (mallocPtr[i] != 0) {
                basePorting->Free(mallocPtr[i]);
                mallocPtr[i] = 0;
            }
        }
        return;
    }
private:
    AlgoBasePorting* basePorting;
    void* mallocPtr[10];
};

