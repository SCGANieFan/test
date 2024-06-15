#include"Ape.Inner.basic.h"
#include"Ape.Inner.mem.manger.h"
#include"Ape.Inner.context.h"


class ApeDemuxer
{
public:
    ApeDemuxer() {};
    ~ApeDemuxer() {};
    ApeMemManger* GetMM() { return &MM; }
    i32 InitWithBuffer(AlgoBasePorting* basePorting, u8* in, i32* inByte)
    {
        MM.Init(basePorting);
        return context.InitWithBuffer(&MM, in, inByte);
    }
    i32 InitWithNoBuffer(AlgoBasePorting* basePorting)
    {
        MM.Init(basePorting);
        return APERET_SUCCESS;
    }
    i32 Run(uint8_t* in, int32_t inLen)
    {
        return APERET_SUCCESS;
    }

    i32 DeInit()
    {
        ALGO_PRINT();
        MM.FreeAll();
        return APERET_SUCCESS;
    }

//private:
public:
    ApeMemManger MM;
    ApeContext context;
};
