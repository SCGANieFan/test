#pragma once
#include <stdint.h>

#ifndef EXTERNC
#ifndef __cplusplus
#define EXTERNC
#else
#define EXTERNC extern "C"
#endif
#endif

#define SBC_RET_SUCCESS 0
#define SBC_RET_FAIL -1

enum SbcInterfaceChannelMode_e {
	SBC_INTERFACE_CHNL_MODE_MONO_E = 0,
	SBC_INTERFACE_CHNL_MODE_DUAL_CHNL_E = 1,
	SBC_INTERFACE_CHNL_MODE_STEREO_E = 2,
	SBC_INTERFACE_CHNL_MODE_JOINT_STEREO_E = 3,
};

enum SbcInterfaceAllocMethod_e {
	SBC_INTERFACE_ALLOC_METHOD_LOUDNESS_E =0,
	SBC_INTERFACE_ALLOC_METHOD_SNR_E =1,
};

enum SbcInterfaceSubBandS_e {
	SBC_INTERFACE_SUB_BANDS_4_E= 4,
	SBC_INTERFACE_SUB_BANDS_8_E= 8,
};

enum SbcInterfaceBlocks_e {
	SBC_INTERFACE_BLOCKS_4_E = 4,
	SBC_INTERFACE_BLOCKS_8_E = 8,
	SBC_INTERFACE_BLOCKS_12_E = 12,
	SBC_INTERFACE_BLOCKS_16_E = 16,
};

typedef enum {
	SBC_ENC_SET_E_HAS_IN_CACHE = 0,
	SBC_ENC_SET_E_MAX,
}SbcEncSet_e;

typedef enum {
	SBC_ENC_GET_PCM_SIZE = 0,
	SBC_ENC_GET_VLC_SIZE,
	SBC_ENC_GET_E_MAX,
}SbcEncGet_e;

typedef struct {
	uint32_t sampleRate;
	uint8_t channels;
	uint8_t bitPool;
	SbcInterfaceChannelMode_e channelMode;
	SbcInterfaceAllocMethod_e allocMethod;
	SbcInterfaceBlocks_e blocks;
	SbcInterfaceSubBandS_e subBands;
	uint8_t reserve[2];
}SbcEncInitParam_t;

EXTERNC int32_t SbcEnc_GetSize();
EXTERNC bool SbcEnc_Init(void* hd, SbcEncInitParam_t* initParam);
EXTERNC int32_t SbcEnc_Run(void* hd, uint8_t* in, int32_t inLen, uint8_t* out, int32_t outLen);
EXTERNC int32_t SbcEnc_Set(void* hd, SbcEncSet_e choose, void* val);
EXTERNC int32_t SbcEnc_Get(void* hd, SbcEncGet_e choose, void* val);
EXTERNC bool SbcEnc_DeInit(void* hd);

EXTERNC int32_t sbc_bps2bitPool(int32_t sampleRate, uint8_t channels, SbcInterfaceChannelMode_e channelMode, uint8_t blocks, uint8_t subBands, int32_t bps);
EXTERNC int32_t sbc_bitPool2bps(int32_t sampleRate, uint8_t channels, SbcInterfaceChannelMode_e channelMode, uint8_t blocks, uint8_t subBands, uint8_t bitPool);
