#include <stdio.h>
#include "opus.h"
#include "opus_memory.h"
#include "OpusApi.h"

namespace OpusApi_ns{

#define LOG(fmt,...) printf("[%s](%d)" fmt "\n",__func__, __LINE__, ##__VA_ARGS__)


//base
OpusApiRet_t OpusApi::memory_register(OpusApiMemory_t *memory){
    if(!memory)
        return OPUS_API_RET_FAIL;
    opus_memory_register_inner((OpusMemory_t*)memory);
    return OPUS_API_RET_SUCCESS;
}

//enc
typedef struct {
    OpusEncoder* hd;
    OpusApiRet_t(*run_cb)(OpusEncoder* hd, short* in, int inSample, unsigned char* out, int* outByte);
}OpusEnc_t;

OpusApiRet_t opus_encoder_run_no_head(OpusEncoder* hd, short* in, int inSample, unsigned char* out, int* outByte);
OpusApiRet_t opus_encoder_run_with_head(OpusEncoder* hd, short* in, int inSample, unsigned char* out, int* outByte);

OpusApiRet_t OpusApi::create_encoder(void** pHd, int fs, int channels, bool isWithHead) {
    if (!pHd)
        return OPUS_API_RET_FAIL;
    int err = 0;
    int application = OPUS_APPLICATION_VOIP;

    int encSize = sizeof(OpusEnc_t);
    OpusEnc_t* enc = (OpusEnc_t*)opus_malloc_inner(encSize);
    LOG("enc :%p", enc);
    if (!enc) {
        LOG("enc Create fail, encSize:%d", encSize);
        opus_free_inner(enc);
        return OPUS_API_RET_FAIL;
    }
    enc->hd = opus_encoder_create(fs, channels, application, &err);
    int hdSize = opus_encoder_get_size(channels);
    LOG("hd Create Success, hdSize:%d, Encoding with %d Hz output (%d _channels)", hdSize, fs, channels);
    opus_encoder_ctl(enc->hd, OPUS_SET_EXPERT_FRAME_DURATION(OPUS_FRAMESIZE_ARG));
    opus_encoder_ctl(enc->hd, OPUS_SET_BANDWIDTH(OPUS_BANDWIDTH_WIDEBAND));

    if (isWithHead) {
        enc->run_cb = opus_encoder_run_with_head;
    }
    else {
        enc->run_cb = opus_encoder_run_no_head;
    }
    *pHd = (void*)enc;
    return OPUS_API_RET_SUCCESS;
}
#include<stdlib.h>
OpusApiRet_t OpusApi::destory_encoder(void* hd) {
    OpusEnc_t* enc = (OpusEnc_t*)hd;
    if (enc) {
        if (enc->hd) {
            opus_encoder_destroy(enc->hd);
        }
        opus_free_inner(enc);
    }
    return OPUS_API_RET_SUCCESS;
}

 
static OpusApiRet_t toOpusFrameDuration(int in, int *out) {
   int out0;
	switch (in) {
    case 0u: out0 = OPUS_FRAMESIZE_ARG; break;
	case 25u:out0 = OPUS_FRAMESIZE_2_5_MS; break;
	case 50u:out0 = OPUS_FRAMESIZE_5_MS; break;
	case 100u:out0 = OPUS_FRAMESIZE_10_MS; break;
	case 200u:out0 = OPUS_FRAMESIZE_20_MS; break;
	case 400u:out0 = OPUS_FRAMESIZE_40_MS; break;
	case 600u:out0 = OPUS_FRAMESIZE_60_MS; break;
	case 800u:out0 = OPUS_FRAMESIZE_80_MS; break;
	case 1000u:out0 = OPUS_FRAMESIZE_100_MS; break;
	case 1200u:out0 = OPUS_FRAMESIZE_120_MS; break;
	default:
		return OPUS_API_RET_FAIL;
	}
    *out = out0;
   return OPUS_API_RET_SUCCESS;
}

OpusApiRet_t OpusApi::encoder_set(void *hd, OpusApi_EncSetChhoose_e choose, void* val){
#if 0
      OpusEncoder *enc = *pHd;
      opus_int32 skip = 0;
      opus_encoder_ctl(enc, OPUS_SET_BITRATE(_bitrate));
      //opus_encoder_ctl(enc, OPUS_SET_BANDWIDTH(OPUS_AUTO));
      opus_encoder_ctl(enc, OPUS_SET_BANDWIDTH(OPUS_BANDWIDTH_WIDEBAND));
      opus_encoder_ctl(enc, OPUS_SET_VBR(__USE_VBR));
      opus_encoder_ctl(enc, OPUS_SET_VBR_CONSTRAINT(__CONSTRAINT_USE_VBR));
      opus_encoder_ctl(enc, OPUS_SET_COMPLEXITY(_complexity));
      opus_encoder_ctl(enc, OPUS_SET_INBAND_FEC(__USE_INBANDFEC));
      opus_encoder_ctl(enc, OPUS_SET_FORCE_CHANNELS(OPUS_AUTO));
      opus_encoder_ctl(enc, OPUS_SET_DTX(__USE_DTX));
      opus_encoder_ctl(enc, OPUS_SET_PACKET_LOSS_PERC(__PACKET_LOSS_PERC));
      opus_encoder_ctl(enc, OPUS_GET_LOOKAHEAD(&skip));
      opus_encoder_ctl(enc, OPUS_SET_LSB_DEPTH(_lsb_depth));
      opus_encoder_ctl(enc, OPUS_SET_FORCE_CHANNELS(_channels));
      opus_encoder_ctl(enc, OPUS_SET_EXPERT_FRAME_DURATION(toOpusFrameDuration(_frameDMs)));
#endif

   if(!hd
   ||choose>=OpusApi_EncSetChhoose_e::OPUS_API_ENC_SET_MAX)
      return OPUS_API_RET_FAIL;
   OpusEncoder *enc = ((OpusEnc_t*)hd)->hd;

   switch (choose) {
   case OpusApi_EncSetChhoose_e::OPUS_API_ENC_SET_BIT_RATE:
   {
       opus_encoder_ctl(enc, OPUS_SET_BITRATE((int)val));
       break;
   }
   case OpusApi_EncSetChhoose_e::OPUS_API_ENC_SET_FRAME_DURATION_0P1MS:
   {
       int fd;
       if (toOpusFrameDuration((int)val, &fd) != OPUS_API_RET_SUCCESS)
           return OPUS_API_RET_FAIL;
       opus_encoder_ctl(enc, OPUS_SET_EXPERT_FRAME_DURATION(fd));
       break;
   }
   case OpusApi_EncSetChhoose_e::OPUS_API_ENC_SET_USE_VBR:
   {
       opus_encoder_ctl(enc, OPUS_SET_VBR(!!(int)val));
       break;
   }
   case OpusApi_EncSetChhoose_e::OPUS_API_ENC_SET_COMPLEXITY:
   {
       opus_encoder_ctl(enc, OPUS_SET_COMPLEXITY((int)val));
       break;
   }
   default:
       break;
   }
   return OPUS_API_RET_SUCCESS;
}

OpusApiRet_t OpusApi::encoder_get(void* hd, OpusApi_EncGetChhoose_e choose, void* val) {
#if 0
    if (!hd
        || choose >= OpusApi_EncGetChhoose_e::OPUS_API_ENC_GET_MAX
        || !val)
        return OPUS_API_RET_FAIL;
    OpusEncoder* enc = (OpusEncoder*)hd;
    switch (choose) {
    case OpusApi_EncGetChhoose_e::OPUS_API_ENC_GET_FINAL_RANGE:
    {
        opus_encoder_ctl(enc, OPUS_GET_FINAL_RANGE((opus_uint32*)val));
        break;
    }
    }
#endif
    return OPUS_API_RET_SUCCESS;
}

OpusApiRet_t opus_encoder_run_no_head(OpusEncoder* hd, short* in, int inSample, unsigned char* out, int *outByte) {
    int ret = opus_encode(hd, in, inSample, out, *outByte);
    if (ret < 0) {
        LOG("enc fail, %d, (%p,%p,%d,%p,%d)", ret, hd, in, inSample, out, *outByte);
        return OPUS_API_RET_FAIL;
    }
    *outByte = ret;
    return OPUS_API_RET_SUCCESS;
}

static inline void int_to_char(opus_uint32 i, unsigned char ch[4])
{
    ch[0] = i >> 24;
    ch[1] = (i >> 16) & 0xFF;
    ch[2] = (i >> 8) & 0xFF;
    ch[3] = i & 0xFF;
}

OpusApiRet_t opus_encoder_run_with_head(OpusEncoder* hd, short* in, int inSample, unsigned char* out, int *outByte) {
    const int headByte = 8;
    if (*outByte < headByte)
        return OPUS_API_RET_FAIL;
    
    int ret = opus_encode(hd, in, inSample, out + headByte, *outByte - headByte);
    if (ret < 0) {
        LOG("enc fail, %d, (%p,%p,%d,%p,%d)", ret, hd, in, inSample, out, *outByte);
        return OPUS_API_RET_FAIL;
    }
    int_to_char(ret, out);
    opus_uint32 enc_final_range;
    opus_encoder_ctl(hd, OPUS_GET_FINAL_RANGE(&enc_final_range));
    int_to_char(enc_final_range, out + 4);
    *outByte = ret + 4;
    return OPUS_API_RET_SUCCESS;
}

OpusApiRet_t OpusApi::encoder_run(void *hd, short* in, int inSample, unsigned char* out, int *outByte){
#if 0
   if(!hd
   ||!in
   ||inSample<0
   ||!out
   ||!outByte
   ||*outByte<=0)
      return OPUS_API_RET_FAIL;
#endif
#if 0
   int ret = opus_encode((OpusEncoder *)hd, in, inSample, out, outByte);
   if(ret<0){
      LOG("enc fail, %d, (%p,%p,%d,%p,%d)",ret,hd,in,inSample,out,outByte);
      return OPUS_API_RET_FAIL;
   }
   return OPUS_API_RET_SUCCESS;
#else
   OpusEnc_t* enc = (OpusEnc_t*)hd;
   return enc->run_cb(enc->hd, in, inSample, out, outByte);
#endif
}

//dec
typedef struct {
    OpusDecoder* hd;
    void* other;
}OpusDec_t;

OpusApiRet_t OpusApi::create_decoder(void **pHd, int fs, int channels){
   if(!pHd)
      return OPUS_API_RET_FAIL;

   int decSize = sizeof(OpusDec_t);
   OpusDec_t* dec = (OpusDec_t*)opus_malloc_inner(decSize);
   if (!dec) {
       LOG("dec malloc fail, %d\n", decSize);
       opus_free_inner(dec);
       return OPUS_API_RET_FAIL;
   }
   int err;
   int hdSize = opus_decoder_get_size(channels);
   
   dec->hd = opus_decoder_create(fs, channels, &err);
	if (err != OPUS_OK){
		LOG("Cannot create decoder: %s\n", opus_strerror(err));
		return OPUS_API_RET_FAIL;
	}
    *pHd = (void*)dec;
	LOG("_dec Create Success, hdSize:%d, Decoding with %d Hz output (%d _channels)", hdSize, fs, channels);
	return OPUS_API_RET_SUCCESS;
}

OpusApiRet_t OpusApi::destory_decoder(void *hd){
    OpusDec_t* dec = (OpusDec_t*)hd;
    if (dec) {
        if (dec->hd) {
            opus_decoder_destroy(dec->hd);
        }
        opus_free_inner(dec);
    }
   return OPUS_API_RET_SUCCESS;
}

OpusApiRet_t OpusApi::decoder_set(void *hd, OpusApi_DecSetChhoose_e choose, void* val){
   return OPUS_API_RET_SUCCESS;

}

OpusApiRet_t OpusApi::decoder_run(void* hd, unsigned char* in, int inByte, short* out, int* outSample, bool isPlc) {
#if 0
    if (!hd
        || !in
        || inByte < 0
        || out
        || !outSample
        || *outSample <= 0) {
}
#endif
   OpusDec_t* dec = (OpusDec_t*)hd;
   int ret = opus_decode(dec->hd, in, inByte, out, *outSample, isPlc);
   if(ret<0){
      LOG("dec fail, %d, (%p,%p,%d,%p,%d)",ret, dec->hd,in,inByte,out, *outSample);
      return OPUS_API_RET_FAIL;
   }
   *outSample = ret;
   return OPUS_API_RET_SUCCESS;
}

}