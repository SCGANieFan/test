#pragma once
#if 1
#include"Algo.Type.h"
#include"Algo.BasePorting.h"
#include"Algo.Macro.h"
#include"Algo.Memory.h"
#include"Algo.Data.h"
#include"Algo.Printer.h"
#endif
#include"Ape.h"

using namespace Algo;


#ifndef WIN32
#define __ALIGN8__ __attribute__ ((aligned (8)))
#else
#define __ALIGN8__
#endif


#define APE_RET_SUCCESS					 0
#define APE_RET_NOT_ENOUGH_INPUT		-1
#define APE_RET_ENTROPY_DECODE_ERROR		-2
#define APE_RET_PRASE_ERROR				-3
#define APE_RET_NOT_ENOUGH_OUTPUT       -4
#define APE_RET_INPUT_ERROR		        -5
#define APE_RET_FINISH		            -6
#define APE_RET_INVALID_INPUT_FILE		-7
#define APE_RET_CONTINUE        		-8


#define APE_BLOCKS_MAX   512
#define APE_FILTER_LEVELS 3
#define APE_MAX_CHANNELS 2

#define APE_MIN_VERSION 3950
#define APE_MAX_VERSION 3990

#define APE_NN_FILTER_ROLL_BUFFER_EXTRA_LENGTH (30*4)

#define APE_FRAMECODE_MONO_SILENCE    1
#define APE_FRAMECODE_STEREO_SILENCE  3
#define APE_FRAMECODE_PSEUDO_STEREO   4


#define APE_COMPRESSION_LEVEL_FAST          1000
#define APE_COMPRESSION_LEVEL_NORMAL        2000
#define APE_COMPRESSION_LEVEL_HIGH          3000
#define APE_COMPRESSION_LEVEL_EXTRA_HIGH    4000
#define APE_COMPRESSION_LEVEL_INSANE        5000

#define APE_FORMAT_FLAG_8_BIT               (1 << 0)    // is 8-bit [OBSOLETE]
#define APE_FORMAT_FLAG_CRC                 (1 << 1)    // uses the new CRC32 error detection [OBSOLETE]
#define APE_FORMAT_FLAG_HAS_PEAK_LEVEL      (1 << 2)    // uint32 nPeakLevel after the header [OBSOLETE]
#define APE_FORMAT_FLAG_24_BIT              (1 << 3)    // is 24-bit [OBSOLETE]
#define APE_FORMAT_FLAG_HAS_SEEK_ELEMENTS   (1 << 4)    // has the number of seek elements after the peak level
#define APE_FORMAT_FLAG_CREATE_WAV_HEADER   (1 << 5)    // create the wave header on decompression (not stored)
#define APE_FORMAT_FLAG_AIFF                (1 << 6)    // the file is an AIFF that was compressed (instead of WAV)
#define APE_FORMAT_FLAG_W64                 (1 << 7)    // the file is a W64 (instead of WAV)
#define APE_FORMAT_FLAG_SND                 (1 << 8)    // the file is a SND (instead of WAV)
#define APE_FORMAT_FLAG_BIG_ENDIAN          (1 << 9)    // flags that the file uses big endian encoding
#define APE_FORMAT_FLAG

#define MODEL_ELEMENTS 64
#define ONE_MILLION                  1000000

#define APE_MAX(a,b) ((a) > (b) ? (a) : (b))
#define APE_MIN(a,b) ((a) > (b) ? (b) : (a))
#define APE_SIGN(x)  ((x < 0) - (x > 0))
#define APE_ABS(a) ((a) >= 0 ? (a) : (-(a)))


typedef i16 APE_RET_t;

