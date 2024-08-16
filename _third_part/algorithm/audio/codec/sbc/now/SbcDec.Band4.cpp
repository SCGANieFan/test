#include"SbcDec.h"
static const int32_t synth_4band_tbl_31[] = {
    ///shf=31,max=1984016128,0x7641af00,min=-2147483648,0x80000000
    1518500224,-1518500224,//-1518500224,1518500224,
    821806400,-1984016128,//1984016128,-821806400,
            0,        0,   //     0,        0,
    -821806400,1984016128,//-1984016128,821806400,
    -1518500224,1518500224,//1518500224,-1518500224,
    -1984016128,-821806400,//821806400,1984016128,
    -2147483648,-2147483648,//-2147483648,-2147483648,
    -1984016128,-821806400,//821806400,1984016128,
};

static void sbc_4band_synth_filter_sub0_0(int* sbSample, int* V0)
{
    const int32_t*  tbls = synth_4band_tbl_31;
    int *src1 = sbSample;
    int *src2 = sbSample + 3;
    int a0=src1[0], a1=src1[1];
    int b0=src2[0], b1=src2[-1];
    int sum0 = a0 + b0;
    int sub0 = a0 - b0;
    int sum1 = a1 + b1;
    int sub1 = a1 - b1;
    for (int i = 0; i < 8; i+=2) {
        int tmp1 = 0;
        int tmp2 = 0;
        tmp1 = Logical_c::MAQ32(sum0, *tbls++, tmp1);
        tmp1 = Logical_c::MAQ32(sum1, *tbls++, tmp1);
        tmp2 = Logical_c::MAQ32(sub0, *tbls++, tmp2);
        tmp2 = Logical_c::MAQ32(sub1, *tbls++, tmp2);
        *V0++ = tmp1;
        *V0++ = tmp2;
    }
}

static const int32_t dec_coeff_4band_tbl_21[] = {
    ///shf=21,max=1137438,0x00115b1e,min=-2468896,0xffda53e0
        0,    -4500,   -12514,   -22932,
   -32188,   -32648,   -15651,    25670,
   -91551,  -171450,  -242227,  -270062,
  -217070,   -51442,   241774,   651344,
 -1137438, -1635676, -2068938, -2364146,
 -2468896, -2364146, -2068938, -1635676,
  1137438,   651344,   241774,   -51442,
  -217070,  -270062,  -242227,  -171450,
    91551,    25670,   -15651,   -32648,
   -32188,   -22932,   -12514,    -4500,
};

static void sbc_4band_synth_filter_sub0_1(int*  V, int16_t* out0)
{
    const int32_t *ts = dec_coeff_4band_tbl_21;
    int index[10] = {0,12,16,28,32,44,48,60,64,76};
    for (int i = 0; i < 4; i++) {
        int tmp = 0;
        int index_ts = 0;
        for (int j = 0; j < 10; j++) {
            tmp = Logical_c::MAQ32(V[index[j]], ts[index_ts], tmp);
            index_ts+=4;
        }
        V++;
        ts++;
        *out0++ = Logical_c::SaturateFrom32To16(tmp>>2);
    }
}

static void sbc_4band_synth_filter_sub0(u8 block,i32*sbSample,i32 sbSampleStride,int16_t* out, int* V0)
{
    int* VEndNow = V0 + (8 * 16 + 8 * 9 - 1);
    int* VEndOld = VEndNow - 8 * block;
    for (int i = 0; i < 72; i++) {
        *VEndNow-- = *VEndOld--;
    }
    int* V1 = V0 + 8 * 16;
    for (int blk = 0; blk < block; blk++) {
        V1-=8;
        sbc_4band_synth_filter_sub0_0(sbSample, V1);
        sbSample += sbSampleStride;
        sbc_4band_synth_filter_sub0_1(V1, out);
        out+=4;
    }
}

void SbcDec_c::BandSynthFilter4(i16 *ch0, i16* ch1)
{
    u8 block = _streamInfo.numBlocks;
    u8 ch = _streamInfo.numChannels;
    i32 sbSampleStride= sizeof(_sbSample[0]) >> 2;
    sbc_4band_synth_filter_sub0(block, _sbSample[0][0],sbSampleStride, ch0, _VV[0]);
    if (ch)
        sbc_4band_synth_filter_sub0(block, _sbSample[0][1], sbSampleStride, ch1, _VV[1]);
    return;
}


