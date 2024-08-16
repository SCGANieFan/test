#include "SbcEnc.Interface.h"
#include "SbcEnc.h"

/***********************interface***********************/

typedef struct {
    SbcEncoder_c encoder;
    bool isInited;
}SbcEncoder_t;

EXTERNC{

int32_t SbcEnc_GetSize()
{
    return sizeof(SbcEncoder_t);
}

bool SbcEnc_Init(void* hd, SbcEncInitParam_t* initParam)
{
    if (!initParam) {
        return false;
    }
    SbcEncoder_t* pHd = (SbcEncoder_t*)hd;
    bool ret = pHd->encoder.Init(
        initParam->sampleRate,
        initParam->channels,
        initParam->bitPool,
        (SbcChannelMode_e)initParam->channelMode,
        (SbcAllocMethod_e)initParam->allocMethod,
        (SbcBlocks_e)initParam->blocks,
        (SbcSubBandS_e)initParam->subBands);

    pHd->isInited = false;
    if (ret)
        pHd->isInited = true;
    return true;
}

int32_t SbcEnc_Run(void* hd, uint8_t* in, int32_t inLen, uint8_t* out, int32_t outLen) {
    SbcEncoder_c* encoder = &((SbcEncoder_t*)hd)->encoder;
    return encoder->EncodeFrame(in, inLen, out, outLen);
}


int32_t SbcEnc_Set(void* hd, SbcEncSet_e choose, void* val){
    return SBC_RET_SUCCESS;
}
int32_t SbcEnc_Get(void* hd, SbcEncGet_e choose, void* val) {
    //check
    if (!hd
        || choose >= SBC_ENC_GET_E_MAX
        || !val)
        return SBC_RET_FAIL;

    SbcEncoder_t* pHd = (SbcEncoder_t*)hd;

    if (pHd->isInited == false)
        return SBC_RET_FAIL;
    switch (choose)
    {
    case SBC_ENC_GET_PCM_SIZE:
        *(u32*)val = pHd->encoder.GetPcmSize(); break;
    case SBC_ENC_GET_VLC_SIZE:
        *(u32*)val = pHd->encoder.GetVlcSize(); break;
    default:
        break;
    }
    return SBC_RET_SUCCESS;
}

bool SbcEnc_DeInit(void* hd) {
    ALGO_MEM_SET(hd,0,SbcEnc_GetSize());
    return true;
}


#if 1
int32_t sbc_bps2bitPool(int32_t sampleRate, uint8_t channels, SbcInterfaceChannelMode_e channelMode, uint8_t blocks, uint8_t subBands, int32_t bps)
{
    int bytes = (bps * blocks * subBands / sampleRate) >> 3;
    int bitsMax = bytes << 3;
    //int bitsMin = bitsMax - 7;
    int bitUseage = 4 * 8;///header
    if (SbcInterfaceChannelMode_e::SBC_INTERFACE_CHNL_MODE_JOINT_STEREO_E == channelMode) {
        bitUseage += subBands;
    }
    //scale factor
    bitUseage += channels * subBands * 4;
    int div = blocks;
    if (SbcInterfaceChannelMode_e::SBC_INTERFACE_CHNL_MODE_DUAL_CHNL_E == channelMode) {
        div += blocks;
    }
    int bitPoolMax = (bitsMax - bitUseage) / div;
    return bitPoolMax;
}

int32_t sbc_bitPool2bps(int32_t sampleRate, uint8_t channels, SbcInterfaceChannelMode_e channelMode, uint8_t blocks, uint8_t subBands, uint8_t bitPool)
{
    // one frame bytes useage
    int bits = 4 * 8;///header
    if (SbcInterfaceChannelMode_e::SBC_INTERFACE_CHNL_MODE_JOINT_STEREO_E == channelMode) {
        bits += subBands;
    }
    //scale factor
    bits += channels * subBands * 4;
    //data
    bits += blocks * bitPool;
    if (SbcInterfaceChannelMode_e::SBC_INTERFACE_CHNL_MODE_DUAL_CHNL_E == channelMode) {
        bits += blocks * bitPool;
    }
    //    int bytes = (bits + 7) >> 3;
       // samplerate    
    int bps = sampleRate * (((bits + 7) >> 3) << 3) / (blocks * subBands);
    return bps;
}
#endif

}

