#include"Ape.Inner.basic.h"
#include"Ape.Inner.context.h"


class ApeDemuxer
{
public:
    ApeDemuxer() {};
    ~ApeDemuxer() {};
    MemoryManger_c* GetMM() { return &MM; }
#if 0
    i32 InitWithBuffer(AlgoBasePorting* basePorting, u8* in, i32* inByte)
    {
        MM.Init(basePorting);
        Buffer buff;
        buff._buf = inTempBuff;
        buff._max = inTempBuffSize;
        inTemp.Init(&buff);
        return context.InitWithBuffer(&MM, in, inByte);
    }
#endif
    i32 InitWithNoBuffer(AlgoBasePorting* basePorting)
    {
#if 0
        MemoryManger_c MM;
        ApeContext context;
        ApeSeekTable_c seekTable;
        Data inTemp;
        b1 _isDemuxHeadSuccess;
        u8 inTempBuff[inTempBuffSize];
#endif
#if 0
        MM.Init(basePorting);
        Buffer buff;
        buff._buf = inTempBuff;
        buff._max = inTempBuffSize;
        inTemp.Init(&buff);
        return context.InitWithBuffer(&MM, in, inByte);
#endif
        MM.Init(basePorting);
        context.Init();
        seekTable.Init();
        Buffer buff = { inTempBuff,inTempBuffSize };
        inTemp.Init(&buff);
        _isDemuxHeadSuccess = false;
        ALGO_MEM_SET(inTempBuff, 0, inTempBuffSize);
        return APERET_SUCCESS;
    }
    i32 Run(uint8_t* in, int32_t inLen)
    {
        if (_isDemuxHeadSuccess) {
            return APERET_SUCCESS;
        }

        inTemp.AppendFully(in, inLen, 0);
        if (inTemp.GetLeftSize() > 0) {
            return APERET_SUCCESS;
        }
        if (context.Parser(&MM, inTemp.GetData(), inTemp.GetSize()) == APERET_SUCCESS) {
            _isDemuxHeadSuccess = true;
            return APERET_SUCCESS;
        }
        return APERET_FAIL;
    }

    i32 DeInit()
    {
        ALGO_PRINT();
        MM.FreeAll();
        return APERET_SUCCESS;
    }

//private:
private:
    const STATIC i32 inTempBuffSize = sizeof(ApeContext_t);
public:
    MemoryManger_c MM;
    ApeContext context;  
    ApeSeekTable_c seekTable;
    Data inTemp;
    b1 _isDemuxHeadSuccess;
    u8 inTempBuff[inTempBuffSize];

};
