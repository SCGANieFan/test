#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifndef EXTERNC
#ifndef __cplusplus
#define EXTERNC
#else
#define EXTERNC extern "C"
#endif
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

/***********************structs**************************/
//
enum pcm_s16_flags_e{
	PCM_S16_FLAGS_INTERLACE = 1<<0,
	PCM_S16_FLAGS_MUTE_FRAME = 1<<1,
	PCM_S16_FLAGS_PLC_BAD_FRAME = 1<<2,
	PCM_S16_FLAGS_FIRST_FRAME = 1<<3,
};

enum Sbc_Mode_E{
	SBC_MODE_NONE= 0x00,
	SBC_MODE_NORMAL = 0x9C,
	SBC_MODE_MSBC = 0xAD,
	SBC_MODE_GSBC = 0x8E,
	SBC_MODE_ASBC = 0x7B,
};

//
typedef struct pcm_s16_t {
	int16_t *ch0,*ch1;
    uint16_t samples;
    uint16_t flags;//pcm_s16_flags_e
} pcm_s16_t;

//
typedef struct sbc_info_t {
	uint8_t bitPool;
	uint8_t sampleFreq;
	uint8_t channelMode;
	uint8_t allocMethod;
	uint8_t numBlocks;
	uint8_t numSubBands;
	uint8_t numChannels;
	enum Sbc_Mode_E sbcMode;
	uint32_t sampleRate;
	uint16_t vlc_size;
	uint16_t pcm_size;
	uint16_t pcm_samples;
} sbc_info_t;


/***********************functions**************************/

/** get the size of encoded frame data size in bytes
 * @param info[in], the next param need be set:
 * 							info->channelMode
 * 							info->numSubBands
 * 							info->numBlocks
 * 							info->numChannels
 * 							info->bitPool
 * @return: encoded frame size
 */
EXTERNC int sbc_get_vlc_size(sbc_info_t* info);


/** get the size of pcm data size per channel before sbc encoded start
 * @param info[in], the next param need be set:
 * 							info->numChannels
 * 							info->numSubBands
 * 							info->numBlocks
 * @return: pcm data size per channel
 */
EXTERNC int sbc_get_pcm_size(sbc_info_t* info);

/** get the info data just based on sbcmode, just support msbc and gsbc
 * @param info[in]
 * @param sbcMode[in], just support msbc and gsbc
 * @param info[out]
 * @return: if successfully get info, reture true
 */
EXTERNC bool sbc_get_info_from_mode(sbc_info_t* info, enum Sbc_Mode_E sbcMode);

EXTERNC bool sbc_uninit();
