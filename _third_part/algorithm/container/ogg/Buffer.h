#pragma once
#include<stdint.h>
#if 0
typedef struct {
    uint8_t* date;
    int32_t max;
    int32_t size;
    int32_t used;
}Buffer;
#endif
namespace OggDemux {
    class Buffer
    {
    public:
        Buffer();
        ~Buffer();
    public:
        int32_t GetMax();
        uint8_t* GetBuff();
        int32_t GetUsed();
        uint8_t* GetBuffValid();
        int32_t GetBuffValidSize();
    public:
        bool SetBuf(uint8_t* data, int32_t size);
    public:
        bool Init(uint8_t* data, int32_t size);
        bool Append(uint8_t* data, int32_t size);
        bool Used(int32_t size);
        void MoveToHead();

    private:
        uint8_t* _data;
        int32_t _max;
        int32_t _size;
        int32_t _used;
    };




}

