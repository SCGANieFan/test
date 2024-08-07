#pragma once
#include "sbc_com.h"

/***********************macro**************************/
#ifdef WIN32
#define __ALIGN8__
#else
#define __ALIGN8__ __attribute__ ((aligned (8)))
#endif


#ifndef dbgCodecCp
#define dbgCodecCp()
#endif

#define ONE_F       (0x00008000)
#define ONE_F_P     (ONE_F << 15)

#define SBC_MAX_NUM_BLK     16
#define SBC_MAX_NUM_SB       8
#define SBC_MAX_NUM_CHNL     2

//samele rate
#define SBC_CHNL_SAMPLE_FREQ_16    0
#define SBC_CHNL_SAMPLE_FREQ_32    1
#define SBC_CHNL_SAMPLE_FREQ_44_1  2
#define SBC_CHNL_SAMPLE_FREQ_48    3

#define ASBC_CHNL_SAMPLE_FREQ_32    0
#define ASBC_CHNL_SAMPLE_FREQ_48    1
#define ASBC_CHNL_SAMPLE_FREQ_88_2  2
#define ASBC_CHNL_SAMPLE_FREQ_96    3

//SYNC
#define SBC_SYNC_WORD 0x9C
#define MSBC_SYNC_WORD	0xAD
#define GSBC_SYNC_WORD	0x8E
#define ASBC_SYNC_WORD	0x7B

//BLOCKS
#define SBC_BLOCKS_4	4
#define SBC_BLOCKS_8	8
#define SBC_BLOCKS_12	12
#define SBC_BLOCKS_16	16

#define MSBC_BLOCKS	15
#define GSBC_BLOCKS	15

#define ASBC_BLOCKS_15	15
#define ASBC_BLOCKS_20	20
#define ASBC_BLOCKS_30	30
#define ASBC_BLOCKS_60	60

/***********************functions**************************/
void sbc_info_print(sbc_info_t*info);

//crc
bool sbc_crc_check(sbc_info_t *sbc,void*buff);
void sbc_crc_calc(sbc_info_t *sbc,void*buff);

//bit alloc
void sbc_bits_for_mono_alloc(sbc_info_t * StreamInfo, uint8_t* bits,uint8_t* scale_factors);
void sbc_bits_for_stereo_alloc(sbc_info_t * StreamInfo, uint8_t (*bits)[8],uint8_t (*scale_factors)[8]);

//info
void msbc_info_init(sbc_info_t* info);
void gsbc_info_init(sbc_info_t* info);
