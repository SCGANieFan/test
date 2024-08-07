#pragma once
#include "sbc_priv.h"
#include "bs.h"

/*******************************************************/
typedef struct _sbc_decoder_t {
    sbc_info_t streamInfo;
    int32_t VV[2][160+16*15] __ALIGN8__;
	uint8_t scale_factors[SBC_MAX_NUM_CHNL][SBC_MAX_NUM_SB];
	uint8_t bits[SBC_MAX_NUM_CHNL][SBC_MAX_NUM_SB];
	int32_t sbSample[SBC_MAX_NUM_BLK][SBC_MAX_NUM_CHNL][SBC_MAX_NUM_SB];
	uint32_t joins;
	uint32_t index;
    unsigned maxPcmLen;
    uint8_t block;
    uint8_t blockOld;
} sbc_decoder_t;

/*******************************************************/
void sbc_4band_synth_filter(sbc_decoder_t* Decoder, pcm_s16_t* pcm);
void sbc_8band_synth_filter(sbc_decoder_t* Decoder, pcm_s16_t* pcm);
char sbc_frames_parser(sbc_decoder_t *Decoder, void *Buff, uint16_t Len, uint16_t *BytesDecoded);
int sbc_dec_frame(sbc_decoder_t* Decoder,void* input,int size,pcm_s16_t* output);
void sbc_sbsamples_make(sbc_decoder_t *Decoder, bs_t *bs);
