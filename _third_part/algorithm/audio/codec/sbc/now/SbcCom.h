#pragma once
#include"Algo.Type.h"
#include"Algo.Macro.h"
#include"Algo.Memory.h"
#include"Algo.Logical.h"
#include"Algo.Math.h"

using namespace Algo;



#if 1

#include <stdint.h>
#include <stdbool.h>

#ifdef WIN32
#define __ALIGN8__
#else
#define __ALIGN8__ __attribute__ ((aligned (8)))
#endif

/***********************macro**************************/
//channels mode
#define SBC_CHNL_MODE_MONO          0
#define SBC_CHNL_MODE_DUAL_CHNL     1
#define SBC_CHNL_MODE_STEREO        2
#define SBC_CHNL_MODE_JOINT_STEREO  3

//alloc method
#define SBC_ALLOC_METHOD_LOUDNESS   0
#define SBC_ALLOC_METHOD_SNR        1

//return flags
#define SBC_RET_STATUS_SUCCESS 			0
#define SBC_RET_STATUS_FAILED 			1
#define SBC_RET_STATUS_NO_RESOURCES 	12
#define SBC_RET_STATUS_NOT_FOUND 		13
#define SBC_RET_STATUS_INVALID_PARM 	18
#define SBC_RET_STATUS_CONTINUE 		24


#define SBC_MAX_NUM_BLK     16
#define SBC_MAX_NUM_SB       8
#define SBC_MAX_NUM_CHNL     2


//SYNC
#define SBC_SYNC_WORD 0x9C

/***********************structs**************************/

#if 1
enum SbcSampleRate_e {
	SBC_SAMPLE_FREQ_16K = 0,
	SBC_SAMPLE_FREQ_32K = 1,
	SBC_SAMPLE_FREQ_44K1 = 1,
	SBC_SAMPLE_FREQ_48K = 3,
};
#endif

//
typedef struct SbcInfo_t {
	uint8_t bitPool;
	uint8_t sampleFreq;
	uint8_t channelMode;
	uint8_t allocMethod;
	uint8_t numBlocks;
	uint8_t numSubBands;
	uint8_t numChannels;
	uint32_t sampleRate;
	uint16_t vlc_size;
	uint16_t pcm_size;
	uint16_t pcm_samples;
} SbcInfo_t;


class SbcCom_c
{
public:
	SbcCom_c() {}
	~SbcCom_c() {}
public:
	STATIC void BitAllocMono(SbcInfo_t* _streamInfo, uint8_t* bits, uint8_t* scale_factors);
	STATIC void BitAllocStereo(SbcInfo_t* _streamInfo, uint8_t(*bits)[8], uint8_t(*scale_factors)[8]);
	STATIC int SbcGetVlcSize(uint8_t channelMode, uint8_t subBands, uint8_t blocks, uint8_t channels, uint8_t bitPool);
	STATIC int SbcGetPcmSize(SbcInfo_t* info);
	STATIC b1 CRCCheck(SbcInfo_t* info, void* buff);
	STATIC void CRCCalc(SbcInfo_t* info, void* buff);
};



#endif


