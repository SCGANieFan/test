#include "SbcEnc.h"
static const unsigned int sbc_enc_analyze_8band_tbl_x[8][8] = {
    ///shf=30,max=1073741824,0x40000000,min=-1053110144,0xc13ad080
    {0x2d413ccb, 0x3536cc51, 0x3b20d79d, 0x3ec52f9e, 0x3fffffff, 0x238e7672, 0x187de2a5, 0x0c7c5c1d},
    {0xd2bec335, 0xf383a3e3, 0x187de2a5, 0x3536cc51, 0x3fffffff, 0xc13ad062, 0xc4df2863, 0xdc71898e},
    {0xd2bec334, 0xc13ad062, 0xe7821d5b, 0x238e7672, 0x3fffffff, 0x0c7c5c1d, 0x3b20d79d, 0x3536cc51},
    {0x2d413ccb, 0xdc71898e, 0xc4df2863, 0x0c7c5c1d, 0x3fffffff, 0x3536cc51, 0xe7821d5b, 0xc13ad062},
    {0x2d413ccc, 0x238e7672, 0xc4df2863, 0xf383a3e3, 0x3fffffff, 0xcac933af, 0xe7821d5b, 0x3ec52f9e},
    {0xd2bec335, 0x3ec52f9e, 0xe7821d5b, 0xdc71898e, 0x3fffffff, 0xf383a3e3, 0x3b20d79d, 0xcac933af},
    {0xd2bec334, 0x0c7c5c1d, 0x187de2a5, 0xcac933af, 0x3fffffff, 0x3ec52f9e, 0xc4df2863, 0x238e7672},
    {0x2d413ccb, 0xcac933af, 0x3b20d79d, 0xc13ad062, 0x3fffffff, 0xdc71898e, 0x187de2a5, 0xf383a3e3},
};

static const unsigned int sbc_coef_8band_x[] = {
    ///shf=30,max=157791808,0x0967b640,min=-73013312,0xfba5e7c0
    0x000290b8, 0x00059fb7, 0x0009163d, 0x000d7fc3, 0x0012ad30, 0x00183079, 0x001d3972, 0x0020f634,
    0x002277a3, 0x0020adb9, 0x001a7c5a, 0x000ec7e8, 0xfffd120b, 0xffe4f888, 0xffc6b3cb, 0xffa3465d,
    0x00838dca, 0x00ab59e4, 0x00d0d9c4, 0x00f01125, 0x0104948e, 0x0109c328, 0x00fafa11, 0x00d3f676,
    0x00911f5c, 0x002fe87e, 0xffaf75bb, 0xff1021b0, 0xfe543740, 0xfd7fcaff, 0xfc98944a, 0xfba5e7c4,
    0x054f9f4c, 0x063eacda, 0x071dd8b6, 0x07e390fb, 0x08876777, 0x09021afc, 0x094e1136, 0x0967b639,
    0x094e1136, 0x09021afc, 0x08876777, 0x07e390fb, 0x071dd8b6, 0x063eacda, 0x054f9f4c, 0x045a183c,
    0xfc98944a, 0xfd7fcaff, 0xfe543740, 0xff1021b0, 0xffaf75bb, 0x002fe87e, 0x00911f5c, 0x00d3f676,
    0x00fafa11, 0x0109c328, 0x0104948e, 0x00f01125, 0x00d0d9c4, 0x00ab59e4, 0x00838dca, 0x005cb9a3,
    0xffc6b3cb, 0xffe4f888, 0xfffd120b, 0x000ec7e8, 0x001a7c5a, 0x0020adb9, 0x002277a3, 0x0020f634,
    0x001d3972, 0x00183079, 0x0012ad30, 0x000d7fc3, 0x0009163d, 0x00059fb7, 0x000290b8, 0x00000000,
};

STATIC INLINE void sbc_filter_8band_analysis_0(short * pcm, int * Y){
    int *out = Y + 15;
    const int * tb = (const int *)sbc_coef_8band_x;
    for (int i = 0; i < 16; i++){
        long long int tmp = 0;
        for (int j = 0; j < 5; j++){
            tmp += (((long long int)tb[j * 16] * (long long int)pcm[j * 16])>>15);
        }
        *out-- = tmp;
        tb++;
        pcm++;
    }
}

STATIC INLINE void sbc_filter_8band_analysis_1(int*Y,int* samples){
    Y[0] = Y[0] + Y[8];
    Y[1] = Y[1] + Y[7];
    Y[2] = Y[2] + Y[6];
    Y[3] = Y[3] + Y[5];
    //Y[4] = Y[4];
    Y[5] = Y[9] - Y[15];
    Y[6] = Y[10] - Y[14];
    Y[7] = Y[11] - Y[13];
    for (int i = 0; i < 8; i++){
        const int *t = (const int *)sbc_enc_analyze_8band_tbl_x[i];
        long long int tmp = 0;
        for (int j = 0; j < 8; j++){
            tmp += (Y[j] * (long long int)t[j]);
        }
        *samples++ = tmp >> 30;
    }
}

STATIC INLINE void sbc_filter_8band_analysis_ch( u8 ch, u8 block, i16* XX, i32* sbSample, i32 sbSampleStride){
    int blk;
    short* pcm = XX;
    int Y[16];
    for (blk = 0; blk < block; blk++) {
        sbc_filter_8band_analysis_0(pcm,Y);
        sbc_filter_8band_analysis_1(Y, sbSample);
        sbSample += sbSampleStride;
        pcm+=8;
    }
}
STATIC INLINE void sbc_filter_8band_prepair( int16_t* pcm0, int16_t* pcm1, u8 channels, i16* XX0, i16* XX1, u8 block){
    int total = block * 8;
    int middle = 72;// _streamInfo.numBlocks / 2 * 8 / 4;
    ///|-------old=72-------|----------new=128---------|
    int i;
    int16_t * ch0 = XX0;
    int16_t * ch1 = XX1;
    int16_t * ch0old = ch0 + block * 8;
    int16_t * ch1old = ch1 + block * 8;
    if(channels ==1) {
        for(i=0;i< middle/*72*/;i++) {
            *ch0++=*ch0old++;///copy old pcm
        }
        for(i=0;i< total/*128*/;i++) {
            *ch0++=*pcm0++;///copy left new pcm
        }
    } else {
        for(i=0;i< middle/*72*/;i++) {
            *ch0++=*ch0old++;///copy old pcm
            *ch1++=*ch1old++;
        }
        for (i = 0; i < total/*128*/; i++) {
            *ch0++ = *pcm0++;///copy new pcm
            *ch1++ = *pcm0++;
        }
    }
}

void SbcEncoder_c::Filter8bandAnalysis( int16_t* pcm0, int16_t* pcm1){//dbgCodecCp();    
    u8 ch = _streamInfo.numChannels;
    u8 block = _streamInfo.numBlocks;
    i32 sbSampleStride = sizeof(_sbSample[0])>>2;
    sbc_filter_8band_prepair(pcm0, pcm1,ch,_XX[0],_XX[1],block);
    sbc_filter_8band_analysis_ch(0,block,_XX[0],_sbSample[0][0], sbSampleStride);
    if (_streamInfo.numChannels == 2) {
        sbc_filter_8band_analysis_ch(1, block, _XX[1], _sbSample[0][1], sbSampleStride);
    }
}

