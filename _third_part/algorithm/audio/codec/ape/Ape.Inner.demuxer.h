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
        context.InitWithBuffer(&MM, in, inByte);
        return APERET_SUCCESS;
    }
    i32 InitWithNoBuffer(AlgoBasePorting* basePorting)
    {
        MM.Init(basePorting);
        return APERET_SUCCESS;
    }
    i32 Run(uint8_t* in, int32_t inLen, uint8_t* out, int32_t* outLen)
    {
        return APERET_SUCCESS;
    }
//private:
public:
    ApeMemManger MM;
    ApeContext context;
};
