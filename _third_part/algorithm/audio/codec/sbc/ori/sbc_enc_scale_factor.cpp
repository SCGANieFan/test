#include "sbc_enc_priv.h"
#include "string.h"
#include "bs.h"
#include "sbc_arth.h"

static void sbc_factors_calc_mono(sbc_encoder_t* Encoder,int sb,int ch){
	int mv1 = 0;
	int mv2 = 0;
	int blk;
	for (blk = 0; blk < Encoder->block; blk+=2) {
		int v0 = Encoder->sbSample[blk][ch][sb];
		int v1 = Encoder->sbSample[blk+1][ch][sb];
		mv1 = SBC_MAX(SBC_ABS(v0), SBC_ABS(mv1));
		mv2 = SBC_MAX(SBC_ABS(v1), SBC_ABS(mv2));
	}
	mv1 = SBC_MAX(mv1, mv2);
	mv1 = mv1 >> 15;
	int v0 = CLZ(mv1);
	if (!v0)v0 = 30;
	Encoder->scale_factors[ch][sb] = 30-v0;
}

static void sbc_factors_calc_dual(sbc_encoder_t* Encoder,int sb){
	int mv1 = 0;
	int mv2 = 0;
	int blk;
	for (blk = 0; blk < Encoder->block; blk++) {
		int v0 = Encoder->sbSample[blk][0][sb];
		int v1 = Encoder->sbSample[blk][1][sb];
		mv1 = SBC_MAX(SBC_ABS(v0), SBC_ABS(mv1));
		mv2 = SBC_MAX(SBC_ABS(v1), SBC_ABS(mv2));
	}
	mv1>>=15;
	mv2>>=15;
	int v0 = CLZ(mv1);
	int v1 = CLZ(mv2);
	if (!v0)v0 = 30;
	if (!v1)v1 = 30;
	Encoder->scale_factors[0][sb] =	30-v0;
	Encoder->scale_factors[1][sb] =	30-v1;
}

static void sbc_factors_calc_join(sbc_encoder_t* Encoder,int sb){
	int mv1 = 0;
	int mv2 = 0;
	int mv3 = 0;
	int mv4 = 0;
	int blk;
	for (blk = 0; blk < Encoder->block; blk++) {
		int v0 = Encoder->sbSample[blk][0][sb];
		int v1 = Encoder->sbSample[blk][1][sb];
		mv1 = SBC_MAX(SBC_ABS(v0), SBC_ABS(mv1));
		mv2 = SBC_MAX(SBC_ABS(v1), SBC_ABS(mv2));
		v0 = v0 >> 1;
		v1 = v1 >> 1;
		int j0 = v0+v1;
		int j1 = v0-v1;
		mv3 = SBC_MAX(SBC_ABS(j0), SBC_ABS(mv3));
		mv4 = SBC_MAX(SBC_ABS(j1), SBC_ABS(mv4));
	}
	mv1>>=15;
	mv2>>=15;
	int v0 = CLZ(mv1);
	int v1 = CLZ(mv2);
	if (!v0)v0 = 30;
	if (!v1)v1 = 30;

	mv3>>=15;
	mv4>>=15;
	int j0 = CLZ(mv3);
	int j1 = CLZ(mv4);
	if (!j0)j0 = 30;
	if (!j1)j1 = 30;
	if(v0+v1>=j0+j1){
		Encoder->scale_factors[0][sb] =	30-v0;
		Encoder->scale_factors[1][sb] =	30-v1;
	}else{
		Encoder->joins |= 1u<<(31-sb);
		Encoder->scale_factors[0][sb] =	30-j0;
		Encoder->scale_factors[1][sb] =	30-j1;
		for (blk = 0; blk < Encoder->block; blk++) {
			int v0 = Encoder->sbSample[blk][0][sb];
			int v1 = Encoder->sbSample[blk][1][sb];
			v0>>=1;
			v1>>=1;
			int j0 = v0+v1;
			int j1 = v0-v1;
			Encoder->sbSample[blk][0][sb]=j0;
			Encoder->sbSample[blk][1][sb]=j1;
		}
	}
}

void sbc_factors_calc_x(sbc_encoder_t* Encoder){//dbgCodecCp();
	sbc_info_t* strInfo = &Encoder->streamInfo;
	int sb=0;
	Encoder->joins = 0;
	switch (Encoder->streamInfo.channelMode) {
	case SBC_CHNL_MODE_MONO:
		for (sb = 0; sb < strInfo->numSubBands; sb++) {
			sbc_factors_calc_mono(Encoder,sb,0);
		}
		break;
	case SBC_CHNL_MODE_DUAL_CHNL:
	case SBC_CHNL_MODE_STEREO:
		for (sb = 0; sb < strInfo->numSubBands; sb++) {
			sbc_factors_calc_dual(Encoder,sb);
		}
		break;
	case SBC_CHNL_MODE_JOINT_STEREO:
		for (sb = 0; sb < strInfo->numSubBands-1; sb++) {
			sbc_factors_calc_join(Encoder,sb);
		}
		sbc_factors_calc_dual(Encoder,sb);
		break;
	}
}
