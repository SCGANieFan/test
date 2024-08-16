#include "SbcEnc.h"

static const unsigned int sbc_enc_analyze_4band_tbl_x[4][4] = {
	{0x2D413CCB, 0x3B20D79D, 0x3FFFFFFF, 0x187DE2A5},
	{0xD2BEC335, 0x187DE2A5, 0x3FFFFFFF, 0xC4DF2863},
	{0xD2BEC334, 0xE7821D5B, 0x3FFFFFFF, 0x3B20D79D},
	{0x2D413CCB, 0xC4DF2863, 0x3FFFFFFF, 0xE7821D5B}
};

static const unsigned int sbc_coef_4band_x[40] = {
	0x0008CA72, 0x00187168, 0x002CCA00, 0x003EDE63, 0x003FC471, 0x001E91CC, 0xFFCDDCEC, 0xFF4D305F,
	0x014EDD50, 0x01D9199F, 0x020F771F, 0x01A7F715, 0x0064795D, 0xFE27C8EB, 0xFB07D79D, 0xF7527098,
	0x0C7AAE47, 0x0FC8E522, 0x12097927, 0x12D60FF8, 0x12097927, 0x0FC8E522, 0x0C7AAE47, 0x08AD8F68,
	0xFB07D79D, 0xFE27C8EB, 0x0064795D, 0x01A7F715, 0x020F771F, 0x01D9199F, 0x014EDD50, 0x00B2CFA1,
	0xFFCDDCEC, 0x001E91CC, 0x003FC471, 0x003EDE63, 0x002CCA00, 0x00187168, 0x0008CA72, 0x00000000,
};

STATIC INLINE void sbc_filter_4band_analysis_0(short* pcm, int* Y) {
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

STATIC INLINE void sbc_filter_4band_analysis_1(int* Y,int* samples) {
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

STATIC INLINE void sbc_filter_4band_analysis_ch(u8 ch,u8 block,i16*XX,i32*sbSample, i32 sbSampleStride) {
	int blk;
	//short* pcm = _XX[ch];
	short* pcm = XX;
	int Y[8];
	for (blk = 0; blk < block; blk++) {
		sbc_filter_4band_analysis_0(pcm, Y);
		sbc_filter_4band_analysis_1(Y, sbSample);
		sbSample += sbSampleStride;
		pcm += 4;
	}
}

STATIC INLINE void sbc_filter_4band_prepair(int16_t* pcm0, int16_t* pcm1,u8 channels,i16* XX0,i16 *XX1,u8 block) {
	int total = block * 4;
	int middle = 36;// _streamInfo.numBlocks / 2 * 8 / 4;
	///|-------old=36-------|----------new=64---------|
	int i;
	int16_t* ch0 = XX0;
	int16_t* ch1 = XX1;
	int16_t* ch0old = ch0 + block * 4;
	int16_t* ch1old = ch1 + block * 4;
	if (channels == 1) {
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
		for (i = 0; i < total/*64*/; i++) {
			*ch0++ = *pcm0++;///copy new pcm
			*ch1++ = *pcm0++;
		}
	}
}

/*---------------------------------------------------------------------------
 *			  sbc_filter_4band_analysis()
 *---------------------------------------------------------------------------
 */
void SbcEncoder_c::Filter4bandAnalysis(i16* pcm0, i16* pcm1)
{
	sbc_filter_4band_prepair(pcm0, pcm1, _streamInfo.numChannels, _XX[0], _XX[1], _streamInfo.numBlocks);
	u8 ch = _streamInfo.numChannels;
	u8 block = _streamInfo.numBlocks;
	i32 sbSampleStride = sizeof(_sbSample[0])>>2;
	sbc_filter_4band_analysis_ch(0, block, _XX[0],_sbSample[0][0], sbSampleStride);
	if (_streamInfo.numChannels == 2) {
		sbc_filter_4band_analysis_ch(1, block, _XX[1], _sbSample[0][1], sbSampleStride);
	}
}
