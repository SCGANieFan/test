#include"SbcCom.h"
int SbcCom_c::SbcGetVlcSize(uint8_t channelMode, uint8_t subBands, uint8_t blocks, uint8_t channels, uint8_t bitPool) {
	int bits = 4 * 8;///header
	if (SBC_CHNL_MODE_JOINT_STEREO == channelMode) {
		bits += subBands;
	}
	//scale factor	
	uint8_t gain = (blocks + 15) / 16;
	bits += channels * subBands * 4 * gain;
	//data
	bits += blocks * bitPool;
	if (SBC_CHNL_MODE_DUAL_CHNL == channelMode) {
		bits += blocks * bitPool;
	}
	int n = (bits + 7) >> 3;
	return n;
}

int SbcCom_c::SbcGetPcmSize(SbcInfo_t* info) {
	return info->numChannels * info->numSubBands * info->numBlocks * 2;
}
