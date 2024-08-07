#include "sbc_priv.h"
#include <stdint.h>
#include <stdio.h>

#define SBC_PRINTF(...) printf(##__VA_ARGS__)
void sbc_info_print(sbc_info_t*info){
#if 0
	SBC_PRINTF("bitPool:%d\n",info->bitPool);
	SBC_PRINTF("sampleFreq:%d\n",info->sampleFreq);
	SBC_PRINTF("channelMode:%d\n",info->channelMode);
	SBC_PRINTF("allocMethod:%d\n",info->allocMethod);
	SBC_PRINTF("numBlocks:%d\n",info->numBlocks);
	SBC_PRINTF("numSubBands:%d\n",info->numSubBands);
	SBC_PRINTF("numChannels:%d\n",info->numChannels);
	SBC_PRINTF("sampleRate:%d\n",info->sampleRate);
	SBC_PRINTF("vlc_size:%d\n",info->vlc_size);
	SBC_PRINTF("pcm_size:%d\n",info->pcm_size);
	SBC_PRINTF("pcm_samples:%d\n",info->pcm_samples);
#endif
}

void msbc_info_init(sbc_info_t*info){
	info->bitPool = 26;
	info->sampleFreq = SBC_CHNL_SAMPLE_FREQ_16;
	info->sampleRate = 16000;
	info->channelMode = SBC_CHNL_MODE_MONO;
	info->allocMethod = SBC_ALLOC_METHOD_LOUDNESS;
	info->numBlocks = MSBC_BLOCKS;
	info->numSubBands = 8;
	info->numChannels = 1;
	info->sbcMode = Sbc_Mode_E::SBC_MODE_MSBC;
	info->pcm_size = 120*2;
	info->pcm_samples = 120;
	info->vlc_size = 57;
}

void gsbc_info_init(sbc_info_t* info) {
	info->bitPool = 18;
	info->sampleFreq = SBC_CHNL_SAMPLE_FREQ_48;
	info->sampleRate = 48000;
	info->channelMode = SBC_CHNL_MODE_MONO;
	info->allocMethod = SBC_ALLOC_METHOD_LOUDNESS;
	info->numBlocks = GSBC_BLOCKS;
	info->numSubBands = 8;
	info->numChannels = 1;
	info->sbcMode = Sbc_Mode_E::SBC_MODE_GSBC;
	info->pcm_size = 15 * 8 * 2;
	info->pcm_samples = 15 * 8;
	info->vlc_size = 42;//4 + numChannels*numSubBands/2 + ceil(bitPool*numBlocks/8)
}

EXTERNC int sbc_get_vlc_size(sbc_info_t* info){
	int bits=4*8;///header
	if(SBC_CHNL_MODE_JOINT_STEREO == info->channelMode){
		bits+=info->numSubBands;
	}
	//scale factor	
	uint8_t gain = (info->numBlocks + 15) / 16;
	bits += info->numChannels * info->numSubBands * 4 * gain;
	//data
	bits+=info->numBlocks * info->bitPool;
	if (SBC_CHNL_MODE_DUAL_CHNL == info->channelMode) {
		bits += info->numBlocks * info->bitPool;
	}
	int n = (bits+7)>>3;
	return n;
}

EXTERNC int sbc_get_pcm_size(sbc_info_t* info){
	return info->numChannels*info->numSubBands*info->numBlocks*2;
}

EXTERNC bool sbc_get_info_from_mode(sbc_info_t* info, Sbc_Mode_E sbcMode){
    if (sbcMode == Sbc_Mode_E::SBC_MODE_MSBC) {
        msbc_info_init(info);
    }
    else if (sbcMode == Sbc_Mode_E::SBC_MODE_GSBC) {
        gsbc_info_init(info);
    }
    else
        return false;
    return true;
}

EXTERNC bool sbc_uninit() {
	return true;
}
