#include"MTF.h"
#include <stdio.h>
#define PATH "../../source/audio/sbc/"

#if 1
enum SbcChannelMode_e {
	SBC_CHNL_MODE_MONO_E = 0,
	SBC_CHNL_MODE_DUAL_CHNL_E = 1,
	SBC_CHNL_MODE_STEREO_E = 2,
	SBC_CHNL_MODE_JOINT_STEREO_E = 3,
};

enum SbcAllocMethod_e {
	SBC_ALLOC_METHOD_LOUDNESS_E = 0,
	SBC_ALLOC_METHOD_SNR_E = 1,
};
enum Sbc_Mode_E {
	SBC_MODE_NONE = 0x00,
	SBC_MODE_NORMAL = 0x9C,
	SBC_MODE_MSBC = 0xAD,
	SBC_MODE_GSBC = 0x8E,
	SBC_MODE_ASBC = 0x7B,
};
#endif

#if 1
//#define FILE_NAME "mbz_48k2h.wav"
//#define FILE_NAME "mbz_48k2h_10ms.wav"
#define FILE_NAME "sin5k48k2ch.wav"
#define RATE 48000
#define CHANNEL 2
#define WIDTH 2
//SBC
#define SBC_BIT_POOL 51
#define SBC_BLOCKS 16
#define SBC_SUB_BAND 8
#define SBC_CHANNEL_MODE SbcChannelMode_e::SBC_CHNL_MODE_STEREO_E
#define SBC_ALLOC_METHOD SbcAllocMethod_e::SBC_ALLOC_METHOD_LOUDNESS_E
#define SBC_MODE Sbc_Mode_E::SBC_MODE_NORMAL
#endif


#if 0
#define FILE_NAME "usedForThd_48k1ch.wav"
#define RATE 48000
#define CHANNEL 1
#define WIDTH 2
//SBC
#define SBC_BIT_POOL 51
#define SBC_BLOCKS 16
#define SBC_SUB_BAND 8
#define SBC_CHANNEL_MODE SbcChannelMode_e::SBC_CHNL_MODE_MONO_E
#define SBC_ALLOC_METHOD SbcAllocMethod_e::SBC_ALLOC_METHOD_LOUDNESS_E
#define SBC_MODE Sbc_Mode_E::SBC_MODE_NORMAL
#endif

#if 1
//PLC
#define FRAME_SAMPLES ((int)SBC_BLOCKS*SBC_SUB_BAND)
#define DECAY_MS 30
#define GAIN_MS 10
#define OVERLAP_MS 4
#endif


void SbcTest()
{
	MultiemdiaTestInit();
	MTF_REGISTER(wav_demuxer);
	MTF_REGISTER(sbc_enc);
	MTF_REGISTER(sbc_dec);
	MTF_REGISTER(music_plc);
	MTF_REGISTER(wav_muxer);
	MTF_REGISTER(pcm_muxer);
	
	char fNameIn[256];
	char fNameOut[256];
	sprintf(fNameIn, "%s%s", PATH, FILE_NAME);

#if 0
	// juse enc
	sprintf(fNameOut, "%s.sbc", fNameIn);
	void* param[] = {
		(void*)(fNameIn),
		(void*)(fNameOut),
		(void*)(FRAME_SAMPLES),
		(void*)(SBC_BIT_POOL),
		(void*)(SBC_BLOCKS),
		(void*)(SBC_SUB_BAND),
		(void*)(SBC_CHANNEL_MODE),
		(void*)(SBC_ALLOC_METHOD),
		(void*)(SBC_MODE),
	};
	const char* str = {
	"|wav_demuxer,url=$0,fSamples=$2|-->"
	"|sbc_enc,bitPool=$3,blocks=$4,subBands=$5,channelMode=$6,allocMethod=$7,sbcMode=$8,|-->"
	"|pcm_muxer,url=$1|"
	};
#endif

#if 1
	//enc and dec
	sprintf(fNameOut, "%s.sbc.wav", fNameIn);
	void* param[] = {
	(void*)(fNameIn),
	(void*)(fNameOut),
	(void*)(FRAME_SAMPLES),
	(void*)(SBC_BIT_POOL),
	(void*)(SBC_BLOCKS),
	(void*)(SBC_SUB_BAND),
	(void*)(SBC_CHANNEL_MODE),
	(void*)(SBC_ALLOC_METHOD),
	(void*)(SBC_MODE),
	(void*)(DECAY_MS),
	(void*)(GAIN_MS),
	(void*)(OVERLAP_MS),
	};
	const char* str = {
	"|wav_demuxer,url=$0,fSamples=$2|-->"
	"|sbc_enc,bitPool=$3,blocks=$4,subBands=$5,channelMode=$6,allocMethod=$7,sbcMode=$8,|-->"
	"|sbc_dec|-->"
	"|wav_muxer,url=$1|"
	};
#endif

#if 0
	//enc, dec and plc
	sprintf(fNameOut, "%s.sbc.wav", fNameIn);
	void* param[] = {
	(void*)(fNameIn),
	(void*)(fNameOut),
	(void*)(FRAME_SAMPLES),
	(void*)(SBC_BIT_POOL),
	(void*)(SBC_BLOCKS),
	(void*)(SBC_SUB_BAND),
	(void*)(SBC_CHANNEL_MODE),
	(void*)(SBC_ALLOC_METHOD),
	(void*)(SBC_MODE),
	(void*)(DECAY_MS),
	(void*)(GAIN_MS),
	(void*)(OVERLAP_MS),
	};
	const char* str = {
	"|wav_demuxer,url=$0,fSamples=$2|-->"
	"|sbc_enc,bitPool=$3,blocks=$4,subBands=$5,channelMode=$6,allocMethod=$7,sbcMode=$8,|-->"
	"|sbc_dec|-->"
	"|music_plc,decayMs=$9,gainMs=$10,overlapMs=$11|-->"
	"|wav_muxer,url=$1|"
	};
#endif


	MultiemdiaApi(str, param);
}
