#include"SbcDec.h"
#include"SbcDec.Interface.h"

typedef struct _sbc_decoder_t {
    SbcDec_c dec;
    b1 isInited;
} sbc_decoder_t;


//
EXTERNC int32_t SbcDec_GetSize()
{
    return sizeof(sbc_decoder_t);
}
EXTERNC bool SbcDec_Init(void* handle)
{
    memset((uint8_t *)handle, 0, sizeof(sbc_decoder_t));
    ((sbc_decoder_t*)handle)->isInited = true;
    return true;
}
EXTERNC int32_t SbcDec_Run(void *handle, void *input, int32_t size, void* out, int32_t outSize)
{
    return ((sbc_decoder_t*)handle)->dec.Run(input, size, out, outSize);
}

EXTERNC int32_t SbcDec_Get(void* hd, SbcDecGet_e choose, void* val) {
    //check
    if (!hd
        || choose >= SBC_DEC_GET_E_MAX
        || !val)
        return SBC_RET_FAIL;
    sbc_decoder_t *pHd = (sbc_decoder_t*)hd;
    if (pHd->isInited == false)
        return SBC_RET_FAIL;
    switch (choose)
    {
    case SBC_DEC_GET_VLC_SIZE:
        *(u32*)val = pHd->dec._streamInfo.vlc_size; break;
    case SBC_DEC_GET_PCM_SIZE:
        *(u32*)val = pHd->dec._streamInfo.pcm_size; break;
    default:
        break;
    }
    return SBC_RET_SUCCESS;
}
