#include <stdint.h>
#include <string.h>
#include "sbc_dec_priv.h"
#include "sbc_arth.h"

static const int32_t synth_8band_tbl_31[] __ALIGN8__ = {
	///shf=31,max=2106220288,0x7d8a5f00,min=-2147483648,0x80000000
	1518500224,-1518500224,-1518500224,1518500224	,//1518500224,-1518500224,-1518500224,1518500224,
	1193078016,-2106220288,418953280,1785567360		,//-1785567360,-418953280,2106220288,-1193078016,
	821806400,-1984016128,1984016128,-821806400		,//-821806400,1984016128,-1984016128,821806400,
	418953280,-1193078016,1785567360,-2106220288	,//2106220288,-1785567360,1193078016,-418953280,
			0,        0,        0,        0			,//        0,        0,        0,        0,
	-418953280,1193078016,-1785567360,2106220288	,//-2106220288,1785567360,-1193078016,418953280,
	-821806400,1984016128,-1984016128,821806400		,//821806400,-1984016128,1984016128,-821806400,
	-1193078016,2106220288,-418953280,-1785567360	,//1785567360,418953280,-2106220288,1193078016,
	-1518500224,1518500224,1518500224,-1518500224	,//-1518500224,1518500224,1518500224,-1518500224,
	-1785567360,418953280,2106220288,1193078016		,//-1193078016,-2106220288,-418953280,1785567360,
	-1984016128,-821806400,821806400,1984016128		,//1984016128,821806400,-821806400,-1984016128,
	-2106220288,-1785567360,-1193078016,-418953280	,//418953280,1193078016,1785567360,2106220288,
	-2147483648,-2147483648,-2147483648,-2147483648	,//-2147483648,-2147483648,-2147483648,-2147483648,
	-2106220288,-1785567360,-1193078016,-418953280	,//418953280,1193078016,1785567360,2106220288,
	-1984016128,-821806400,821806400,1984016128		,//1984016128,821806400,-821806400,-1984016128,
	-1785567360,418953280,2106220288,1193078016		,//-1193078016,-2106220288,-418953280,1785567360,
};


static void sbc_8band_synth_filter_sub0_0(int* sbSample, int* V0){
    const int32_t*  tbls = synth_8band_tbl_31;
    int *src1 = sbSample;
    int *src2 = sbSample + 7;
    int a0=src1[0], a1=src1[1], a2=src1[2], a3=src1[3];
    int b0=src2[0], b1=src2[-1],b2=src2[-2],b3=src2[-3];
    int sum0 = a0 + b0;
    int sub0 = a0 - b0;
    int sum1 = a1 + b1;
    int sub1 = a1 - b1;
    int sum2 = a2 + b2;
    int sub2 = a2 - b2;
    int sum3 = a3 + b3;
    int sub3 = a3 - b3;
    for (int i = 0; i < 16; i+=2) {
        int tmp1 = 0;
        int tmp2 = 0;
        // for (int j = 0; j < 4; j++) {
        //     tmp1 = __smmla((src1[j] + src2[-j]), *tbls++, tmp1);
        // }
        tmp1 = __smmla(sum0, *tbls++, tmp1);
        tmp1 = __smmla(sum1, *tbls++, tmp1);
        tmp1 = __smmla(sum2, *tbls++, tmp1);
        tmp1 = __smmla(sum3, *tbls++, tmp1);
        // for (int j = 0; j < 4; j++) {
        //     tmp2 = __smmla((src1[j] - src2[-j]), *tbls++, tmp2);
        // }
        tmp2 = __smmla(sub0, *tbls++, tmp2);
        tmp2 = __smmla(sub1, *tbls++, tmp2);
        tmp2 = __smmla(sub2, *tbls++, tmp2);
        tmp2 = __smmla(sub3, *tbls++, tmp2);
        *V0++ = tmp1;
        *V0++ = tmp2;
    }
}

static const int32_t dec_coeff_8band_tbl_21[] __ALIGN8__ = {
    ///shf=21,max=1140833,0x00116861,min=-2465497,0xffda6127
        0,    -2626,    -5758,    -9304,   -13823,   -19124,   -24769,   -29925,
   -33752,   -35294,   -33462,   -27121,   -15135,     2999,    27677,    58672,
   -94950,  -134711,  -175463,  -213863,  -245828,  -266834,  -272140,  -257000,
  -217049,  -148605,   -49057,    82473,   245625,   438051,   655572,   892334,
 -1140833, -1392253, -1637043, -1865570, -2068036, -2235806, -2361452, -2439236,
 -2465497, -2439236, -2361452, -2235806, -2068036, -1865570, -1637043, -1392253,
  1140833,   892334,   655572,   438051,   245625,    82473,   -49057,  -148605,
  -217049,  -257000,  -272140,  -266834,  -245828,  -213863,  -175463,  -134711,
    94950,    58672,    27677,     2999,   -15135,   -27121,   -33462,   -35294,
   -33752,   -29925,   -24769,   -19124,   -13823,    -9304,    -5758,    -2626,
};

static void sbc_8band_synth_filter_sub0_1(int*  V, int16_t* out0)
{
    const int32_t *ts = dec_coeff_8band_tbl_21;
    int index[10] = {0,24,32,56,64,88,96,120,128,152};
    for (int i = 0; i < 8; i++) {
        int tmp = 0;
        int index_ts = 0;
        for (int j = 0; j < 10; j++) {
            tmp = __smmla(V[index[j]], ts[index_ts], tmp);
            index_ts+=8;
        }
        V++;
        ts++;
        *out0++ = SATURATE32(tmp>>2, 16);
    }
}

static void sbc_8band_synth_filter_sub0(sbc_decoder_t* Decoder, int16_t* out, int* V0, unsigned char ch){
    int* VEndNow = V0 + (16 * 16 + 16 * 9 - 1);
    int* VEndOld = VEndNow - 16 * Decoder->blockOld;
    for (int i = 0; i < 144; i++) {
        *VEndNow-- = *VEndOld--;
    }
    int* V1 = V0 + 16 * 16;
    for (int blk = 0; blk < Decoder->block; blk++) {
        V1-=16;
        sbc_8band_synth_filter_sub0_0(Decoder->sbSample[blk][ch], V1);
        sbc_8band_synth_filter_sub0_1(V1, out);
        out+=8;
    }

}

void sbc_8band_synth_filter(sbc_decoder_t* Decoder, pcm_s16_t* pcm)
{//dbgCodecCp();
	if (pcm->ch0)
		sbc_8band_synth_filter_sub0(Decoder, pcm->ch0, Decoder->VV[0], 0);

	if (pcm->ch1)
		sbc_8band_synth_filter_sub0(Decoder, pcm->ch1, Decoder->VV[1], 1);
	
    pcm->samples += Decoder->streamInfo.numSubBands * Decoder->block;
	return;
}

static void sbc_8band_synth_filter_sub0_without_output(sbc_decoder_t* Decoder, int* V0, unsigned char ch)
{
    int* VEndNow = V0 + (16 * 16 + 16 * 9 - 1);
    int* VEndOld = VEndNow - 16 * Decoder->blockOld;
    for (int i = 0; i < 144; i++) {
        *VEndNow-- = *VEndOld--;
    }
    int* V1 = V0 + 16 * 16;

    for (int blk = 0; blk < Decoder->block; blk++) {
        V1-=16;
        sbc_8band_synth_filter_sub0_0(Decoder->sbSample[blk][ch], V1);
    }
}

EXTERNC void sbc_8band_synth_filter_without_output(void *handle)
{
    sbc_decoder_t *Decoder = (sbc_decoder_t *)handle;
    sbc_8band_synth_filter_sub0_without_output(Decoder, Decoder->VV[0], 0);
    if(Decoder->streamInfo.numChannels==2) {
        sbc_8band_synth_filter_sub0_without_output(Decoder, Decoder->VV[1], 1);
    }
}
