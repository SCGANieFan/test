#include "sbc_enc_priv.h"
#include <string.h>
#include <stdio.h>

static const unsigned int sbc_enc_analyze_4band_tbl_x[4][4] __ALIGN8__ = {
	{0x2D413CCB, 0x3B20D79D, 0x3FFFFFFF, 0x187DE2A5},
	{0xD2BEC335, 0x187DE2A5, 0x3FFFFFFF, 0xC4DF2863},
	{0xD2BEC334, 0xE7821D5B, 0x3FFFFFFF, 0x3B20D79D},
	{0x2D413CCB, 0xC4DF2863, 0x3FFFFFFF, 0xE7821D5B}
};

static const unsigned int sbc_coef_4band_x[40] __ALIGN8__ = {
	0x0008CA72, 0x00187168, 0x002CCA00, 0x003EDE63, 0x003FC471, 0x001E91CC, 0xFFCDDCEC, 0xFF4D305F,
	0x014EDD50, 0x01D9199F, 0x020F771F, 0x01A7F715, 0x0064795D, 0xFE27C8EB, 0xFB07D79D, 0xF7527098,
	0x0C7AAE47, 0x0FC8E522, 0x12097927, 0x12D60FF8, 0x12097927, 0x0FC8E522, 0x0C7AAE47, 0x08AD8F68,
	0xFB07D79D, 0xFE27C8EB, 0x0064795D, 0x01A7F715, 0x020F771F, 0x01D9199F, 0x014EDD50, 0x00B2CFA1,
	0xFFCDDCEC, 0x001E91CC, 0x003FC471, 0x003EDE63, 0x002CCA00, 0x00187168, 0x0008CA72, 0x00000000,
};

static void sbc_filter_4band_analysis_0(short* pcm, int* Y) {
	int* out = Y + 7;
	const int* tb = (const int*)sbc_coef_4band_x;
	for (int i = 0; i < 8; i++) {
		long long int tmp = 0;
		for (int j = 0; j < 5; j++) {
			tmp += (((long long int)tb[j * 8] * (long long int)pcm[j * 8]) >> 15);
		}
		*out-- = tmp;
		tb++;
		pcm++;
	}
}

static void sbc_filter_4band_analysis_1(int* Y,int* samples) {
	Y[0] = Y[0] + Y[4];
	Y[1] = Y[1] + Y[3];
	//Y[4] = Y[4];
	Y[3] = Y[5] - Y[7];
	for (int i = 0; i < 4; i++) {
		const int* t = (const int*)sbc_enc_analyze_4band_tbl_x[i];
		long long int tmp = 0;
		for (int j = 0; j < 4; j++) {
			tmp += (Y[j] * (long long int)t[j]);
		}
		*samples++ = tmp >> 30;
	}
}

static void sbc_filter_4band_analysis_ch(sbc_encoder_t* Encoder, int ch) {
	int blk;
	short* pcm = Encoder->XX[ch];
	int Y[8];
	for (blk = 0; blk < Encoder->block; blk++) {
		sbc_filter_4band_analysis_0(pcm, Y);
		sbc_filter_4band_analysis_1(Y, Encoder->sbSample[blk][ch]);
		pcm += 4;
	}
}

static void sbc_filter_4band_prepair(sbc_encoder_t* Encoder, pcm_s16_t* PcmData) {
	int total = Encoder->block * 4;
	int middle = 36;// Encoder->streamInfo.numBlocks / 2 * 8 / 4;
	///|-------old=36-------|----------new=64---------|
	sbc_info_t* info = &Encoder->streamInfo;
	int i;
	int16_t* ch0 = Encoder->XX[0];
	int16_t* ch1 = Encoder->XX[1];
	int16_t* ch0old = ch0 + Encoder->blockOld * 4;
	int16_t* ch1old = ch1 + Encoder->blockOld * 4;
	int16_t* pcm0 = PcmData->ch0;
	int16_t* pcm1 = PcmData->ch1;
	if (info->numChannels == 1) {
		for (i = 0; i < middle/*36*/; i++) {
			*ch0++ = *ch0old++;///copy old pcm
		}
		for (i = 0; i < total/*64*/; i++) {
			*ch0++ = *pcm0++;///copy left new pcm
		}
	}
	else {
		for (i = 0; i < middle/*36*/; i++) {
			*ch0++ = *ch0old++;///copy old pcm
			*ch1++ = *ch1old++;
		}
		if (!(PcmData->flags & PCM_S16_FLAGS_INTERLACE)) {//non-interlace
			for (i = 0; i < total/*64*/; i++) {
				*ch0++ = *pcm0++;///copy new pcm
				*ch1++ = *pcm1++;
			}
		}
		else {//interlace
			for (i = 0; i < total/*64*/; i++) {
				*ch0++ = *pcm0++;///copy new pcm
				*ch1++ = *pcm0++;
			}
		}
	}
}

/*---------------------------------------------------------------------------
 *			  sbc_filter_4band_analysis()
 *---------------------------------------------------------------------------
 */
void sbc_filter_4band_analysis(sbc_encoder_t* Encoder, pcm_s16_t* PcmData)
{
	sbc_filter_4band_prepair(Encoder, PcmData);
	sbc_filter_4band_analysis_ch(Encoder, 0);
	if (Encoder->streamInfo.numChannels == 2) {
		sbc_filter_4band_analysis_ch(Encoder, 1);
	}
}
