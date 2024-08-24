#pragma once
#include <stdint.h>

#ifndef EXTERNC
#ifndef __cplusplus
#define EXTERNC
#else
#define EXTERNC extern "C"
#endif
#endif

#define H265_DEC_RET_SUCCESS 0
#define H265_DEC_RET_FAIL -1

#if 0
enum H265InterfaceChannelMode_e {
	H265_INTERFACE_CHNL_MODE_MONO_E = 0,
	H265_INTERFACE_CHNL_MODE_DUAL_CHNL_E = 1,
	H265_INTERFACE_CHNL_MODE_STEREO_E = 2,
	H265_INTERFACE_CHNL_MODE_JOINT_STEREO_E = 3,
};

enum H265InterfaceAllocMethod_e {
	H265_INTERFACE_ALLOC_METHOD_LOUDNESS_E =0,
	H265_INTERFACE_ALLOC_METHOD_SNR_E =1,
};

enum H265InterfaceSubBandS_e {
	H265_INTERFACE_SUB_BANDS_4_E= 4,
	H265_INTERFACE_SUB_BANDS_8_E= 8,
};

enum H265InterfaceBlocks_e {
	H265_INTERFACE_BLOCKS_4_E = 4,
	H265_INTERFACE_BLOCKS_8_E = 8,
	H265_INTERFACE_BLOCKS_12_E = 12,
	H265_INTERFACE_BLOCKS_16_E = 16,
};
#endif

typedef enum {
	H265_DEC_SET_E_HAS_IN_CACHE = 0,
	H265_DEC_SET_E_MAX,
}H265DecSet_e;

typedef enum {
	H265_DEC_GET_PCM_SIZE = 0,
	H265_DEC_GET_VLC_SIZE,
	H265_DEC_GET_E_MAX,
}H265DecGet_e;

typedef struct {
#if 0
	uint32_t sampleRate;
	uint8_t channels;
	uint8_t bitPool;
#endif
}H265DecInitParam_t;

EXTERNC int32_t H265Dec_GetSize();
EXTERNC bool H265Dec_Init(void* hd, H265DecInitParam_t* initParam);
EXTERNC int32_t H265Dec_Receive(void* hd, uint8_t* in, int32_t inLen);
EXTERNC int32_t H265Dec_Generate(void* hd, uint8_t* out, int32_t *outLen);
EXTERNC int32_t H265Dec_Set(void* hd, H265DecSet_e choose, void* val);
EXTERNC int32_t H265Dec_Get(void* hd, H265DecGet_e choose, void* val);
EXTERNC bool H265Dec_DeInit(void* hd);


