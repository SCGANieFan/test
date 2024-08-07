#pragma once

#include "sbc_priv.h"
#include "bs.h"

/*******************************************************/
typedef struct _sbc_encoder_t {
    sbc_info_t streamInfo;

    int16_t XX[2][8 * 16 + 72] __ALIGN8__;
	uint8_t scale_factors[SBC_MAX_NUM_CHNL][SBC_MAX_NUM_SB];
	uint8_t bits[SBC_MAX_NUM_CHNL][SBC_MAX_NUM_SB];
	int32_t sbSample[SBC_MAX_NUM_BLK][SBC_MAX_NUM_CHNL][SBC_MAX_NUM_SB];
	int32_t as[SBC_MAX_NUM_CHNL][SBC_MAX_NUM_SB];
	int32_t bs[SBC_MAX_NUM_CHNL][SBC_MAX_NUM_SB];
	int32_t Y[16];
	uint32_t  joins;
	uint8_t block;
	uint8_t blockOld;
} sbc_encoder_t;


/*******************************************************/
void sbc_filter_4band_analysis(sbc_encoder_t* Encoder, pcm_s16_t* PcmData);
void sbc_filter_8band_analysis(sbc_encoder_t* Encoder, pcm_s16_t* PcmData);
void sbc_header_make_x(sbc_encoder_t* Encoder, bs_t*bs);
void sbc_header_scale_factors_make_x(sbc_encoder_t* Encoder, bs_t* bs);
void sbc_samples_quant_x(sbc_encoder_t* Encoder, bs_t* bs);
void sbc_factors_calc_x(sbc_encoder_t*Encoder);

