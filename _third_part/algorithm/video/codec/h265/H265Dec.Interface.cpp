#include"H265Dec.Interface.h"
#include"H265Dec.h"


#if 0
class H265Dec_c
{
public:
    H265Dec_c(){}
    ~H265Dec_c(){}

public:
    b1 Init();
    b1 Receive(u8 *in,i32 inByte);
    b1 Generate(u8 *out,i32 *outByte);
    b1 DeInit();
private:
    void *_hd;
    b1 _check_hash=false;
    i32 _framecnt=0;
};

#endif

typedef struct H265Dec_t{
    H265Dec_c dec;
    b1 isInited;
} ;


EXTERNC{

int32_t H265Dec_GetSize(){
    return sizeof(H265Dec_t);
}
bool H265Dec_Init(void* hd, H265DecInitParam_t* initParam){
    if(!hd
    ||!initParam)
        return false;
    H265Dec_t *pHd = (H265Dec_t *)hd;
    pHd->dec.H265Dec_c::H265Dec_c();
    pHd->dec.Init();
    pHd->isInited=true;
}
int32_t H265Dec_Receive(void* hd, uint8_t* in, int32_t inLen){
    if(!hd
    ||!in
    ||inLen<0){
        return H265_DEC_RET_FAIL;
    }
    H265Dec_t *pHd = (H265Dec_t *)hd;
    b1 ret = pHd->dec.Receive(in,inLen);
    if(!ret)
        return H265_DEC_RET_FAIL;
    return H265_DEC_RET_SUCCESS;
}
int32_t H265Dec_Generate(void* hd, uint8_t* out, int32_t *outLen){
    if(!hd
    ||!out
    ||!outLen
    ||*outLen<0){
        return H265_DEC_RET_FAIL;
    }
    H265Dec_t *pHd = (H265Dec_t *)hd;
    b1 ret = pHd->dec.Generate(out,outLen);
    if(!ret)
        return H265_DEC_RET_FAIL;
    return H265_DEC_RET_SUCCESS;
}
int32_t H265Dec_Set(void* hd, H265DecSet_e choose, void* val){
    //check
    if (!hd
        || choose >= H265_DEC_SET_E_MAX
        || !val)
        return H265_DEC_RET_FAIL;
    H265Dec_t *pHd = (H265Dec_t *)hd;
    if (pHd->isInited == false)
        return H265_DEC_RET_FAIL;
    switch (choose)
    {
#if 0
    case SBC_DEC_SET_VLC_SIZE:
        *(u32*)val = pHd->dec._streamInfo.vlc_size; break;
    case SBC_DEC_SET_PCM_SIZE:
        *(u32*)val = pHd->dec._streamInfo.pcm_size; break;
    default:
        break;
#endif
    }
    return H265_DEC_RET_SUCCESS;

}
int32_t H265Dec_Get(void* hd, H265DecGet_e choose, void* val){
        //check
    if (!hd
        || choose >= H265_DEC_GET_E_MAX
        || !val)
        return H265_DEC_RET_FAIL;
    H265Dec_t *pHd = (H265Dec_t *)hd;
    if (pHd->isInited == false)
        return H265_DEC_RET_FAIL;
    switch (choose)
    {
#if 0
    case SBC_DEC_GET_VLC_SIZE:
        *(u32*)val = pHd->dec._streamInfo.vlc_size; break;
    case SBC_DEC_GET_PCM_SIZE:
        *(u32*)val = pHd->dec._streamInfo.pcm_size; break;
    default:
        break;
#endif
    }
    return H265_DEC_RET_SUCCESS;

}
bool H265Dec_DeInit(void* hd){
    if(!hd){
        return H265_DEC_RET_FAIL;
    }
    H265Dec_t *pHd = (H265Dec_t *)hd;
    b1 ret = pHd->dec.DeInit();
    if(!ret)
        return H265_DEC_RET_FAIL;
    return H265_DEC_RET_SUCCESS;
}

}