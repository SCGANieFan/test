#include "SbcEnc.h"

STATIC INLINE void sbc_factors_calc_mono(u8 block,i32 *sbSample,i32 sbSampleStide,u8 *scaleFactor){
	int mv1 = 0;
	int mv2 = 0;
	int blk;
	for (blk = 0; blk < block; blk+=2) {
		int v0 = sbSample[0];
		int v1 = sbSample[sbSampleStide];
		sbSample += 2 * sbSampleStide;
		mv1 = ALGO_MAX(ALGO_ABS(v0), ALGO_ABS(mv1));
		mv2 = ALGO_MAX(ALGO_ABS(v1), ALGO_ABS(mv2));
	}
	mv1 = ALGO_MAX(mv1, mv2);
	mv1 = mv1 >> 15;
	int v0 = Logical_c::CLZ32(mv1);
	if (!v0)v0 = 30;
	*scaleFactor = 30-v0;
}

STATIC INLINE void sbc_factors_calc_dual(u8 block, i32* sbSample0, i32* sbSample1, i32 sbSampleStide, u8* scaleFactor0, u8* scaleFactor1){
	int mv1 = 0;
	int mv2 = 0;
	int blk;
	for (blk = 0; blk < block; blk++) {
		int v0 = sbSample0[0];
		int v1 = sbSample1[0];
		sbSample0 += sbSampleStide;
		sbSample1 += sbSampleStide;
		mv1 = ALGO_MAX(ALGO_ABS(v0), ALGO_ABS(mv1));
		mv2 = ALGO_MAX(ALGO_ABS(v1), ALGO_ABS(mv2));
	}
	mv1>>=15;
	mv2>>=15;
	int v0 = Logical_c::CLZ32(mv1);
	int v1 = Logical_c::CLZ32(mv2);
	if (!v0)v0 = 30;
	if (!v1)v1 = 30;
	scaleFactor0[0] =	30-v0;
	scaleFactor1[0] =	30-v1;
}

STATIC INLINE void sbc_factors_calc_join(u8 block, u8 subBands, u32 *joins,i32* sbSample0, i32* sbSample1, i32 sbSampleStide, u8* scaleFactor0, u8* scaleFactor1){
	int mv1 = 0;
	int mv2 = 0;
	int mv3 = 0;
	int mv4 = 0;
	int blk;
	for (blk = 0; blk < block; blk++) {
		int v0 = sbSample0[0];
		int v1 = sbSample1[0];
		sbSample0+= sbSampleStide;
		sbSample1+= sbSampleStide;
		mv1 = ALGO_MAX(ALGO_ABS(v0), ALGO_ABS(mv1));
		mv2 = ALGO_MAX(ALGO_ABS(v1), ALGO_ABS(mv2));
		v0 = v0 >> 1;
		v1 = v1 >> 1;
		int j0 = v0+v1;
		int j1 = v0-v1;
		mv3 = ALGO_MAX(ALGO_ABS(j0), ALGO_ABS(mv3));
		mv4 = ALGO_MAX(ALGO_ABS(j1), ALGO_ABS(mv4));
	}
	mv1>>=15;
	mv2>>=15;
	int v0 = Logical_c::CLZ32(mv1);
	int v1 = Logical_c::CLZ32(mv2);
	if (!v0)v0 = 30;
	if (!v1)v1 = 30;

	mv3>>=15;
	mv4>>=15;
	int j0 = Logical_c::CLZ32(mv3);
	int j1 = Logical_c::CLZ32(mv4);
	if (!j0)j0 = 30;
	if (!j1)j1 = 30;
	if(v0+v1>=j0+j1){
		scaleFactor0[0] = 30 - v0;
		scaleFactor1[0] = 30 - v1;
	}else{
		*joins |= 1u<<(31- subBands);
		scaleFactor0[0] =	30-j0;
		scaleFactor0[0] =	30-j1;
		for (blk = 0; blk < block; blk++) {
			int v0 = sbSample0[0];
			int v1 = sbSample1[0];
			v0>>=1;
			v1>>=1;
			int j0 = v0+v1;
			int j1 = v0-v1;
			sbSample0[0] = j0;
			sbSample1[0] =j1;
			sbSample0 += sbSampleStide;
			sbSample1 += sbSampleStide;
		}
	}
}

void SbcEncoder_c::FactorsCalcX(){
	SbcInfo_t* strInfo = &_streamInfo;
	int sb=0;
	_joins = 0;

	u8 block = strInfo->numBlocks;
	i32 sbSampleStide = sizeof(_sbSample[0]) >> 2;
	switch (_streamInfo.channelMode) {
	case SBC_CHNL_MODE_MONO:
		for (sb = 0; sb < strInfo->numSubBands; sb++) {
			sbc_factors_calc_mono(block, &_sbSample[0][0][sb], sbSampleStide, &_scaleFactors[0][sb]);
		}
		break;
	case SBC_CHNL_MODE_DUAL_CHNL:
	case SBC_CHNL_MODE_STEREO:
		for (sb = 0; sb < strInfo->numSubBands; sb++) {
			sbc_factors_calc_dual(block, &_sbSample[0][0][sb], &_sbSample[0][1][sb], sbSampleStide, &_scaleFactors[0][sb], &_scaleFactors[1][sb]);
		}
		break;
	case SBC_CHNL_MODE_JOINT_STEREO:
		for (sb = 0; sb < strInfo->numSubBands-1; sb++) {
			sbc_factors_calc_join(block, sb, &_joins, &_sbSample[0][0][sb], &_sbSample[0][1][sb], sbSampleStide, &_scaleFactors[0][sb], &_scaleFactors[1][sb]);
		}
		sbc_factors_calc_dual(block, &_sbSample[0][0][sb], &_sbSample[0][1][sb], sbSampleStide, &_scaleFactors[0][sb], &_scaleFactors[1][sb]);
		break;
	}
}
