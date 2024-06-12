#pragma once

#include<stdint.h>
#include"Algo.Type.h"
#include"Algo.BasePorting.h"
using namespace Algo;
//using namespace Algo;
#ifndef EXTERNC
#ifndef __cplusplus
#define EXTERNC
#else
#define EXTERNC extern "C"
#endif
#endif

#ifndef WIN32
#define __ALIGN8__ __attribute__ ((aligned (8)))
#else
#define __ALIGN8__
#endif


#define APE_BLOCKS_MAX   512

#define IN_BUFF_TEMP_SIZE 2*APE_MAX_CHANNELS* APE_BLOCKS_MAX


#define APE_RET_SUCCESS					 0
#define APE_RET_NOT_ENOUGH_INPUT		-1
//#define APE_RET_GET_SYMBOL_OVERFLOW		-2
#define APE_RET_ENTROPY_DECODE_ERROR		-2
#define APE_RET_PRASE_ERROR				-3
#define APE_RET_NOT_ENOUGH_OUTPUT       -4
#define APE_RET_INPUT_ERROR		        -5
#define APE_RET_FINISH		            -6
#define APE_RET_INVALID_INPUT_FILE		-7
#define APE_RET_CONTINUE        		-8


#define APE_MIN_VERSION 3950
#define APE_MAX_VERSION 3990



#define APE_FILTER_LEVELS 3
#define APE_MAX_CHANNELS 2

//#define APE_NN_FILTER_ROLL_BUFFER_EXTRA_LENGTH (0)
#define APE_NN_FILTER_ROLL_BUFFER_EXTRA_LENGTH (30*4)
// #define APE_NN_FILTER_ROLL_BUFFER_EXTRA_LENGTH (256*4)

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


typedef int16_t APE_RET_t;


/** Filter fraction bits depending on compression level */
static const uint8_t ape_filter_fracbits[5][APE_FILTER_LEVELS] = {
    {  0,  0,  0 },
    { 11,  0,  0 },
    { 11,  0,  0 },
    { 10, 13,  0 },
    { 11, 13, 15 }
};
/**
 * Probability ranges for symbols in Monkey Audio version 3.97
 */
static const uint16_t counts_diff_3970[21] = {
    14824, 13400, 11124, 8507, 6139, 4177, 2755, 1756,
    1104, 677, 415, 248, 150, 89, 54, 31,
    19, 11, 7, 4, 2,
};

/**
 * Fixed probabilities for symbols in Monkey Audio version 3.97
 */
static const uint16_t counts_3970[22] = {
        0, 14824, 28224, 39348, 47855, 53994, 58171, 60926,
    62682, 63786, 64463, 64878, 65126, 65276, 65365, 65419,
    65450, 65469, 65480, 65487, 65491, 65493,
};
static const int32_t initial_coeffs[4] = {
    360, 317, -109, 98
};
/**
 * Fixed probabilities for symbols in Monkey Audio version 3.98
 */
static const uint16_t counts_3980[22] = {
        0, 19578, 36160, 48417, 56323, 60899, 63265, 64435,
    64971, 65232, 65351, 65416, 65447, 65466, 65476, 65482,
    65485, 65488, 65490, 65491, 65492, 65493,
};

/**
 * Probability ranges for symbols in Monkey Audio version 3.98
 */
static const uint16_t counts_diff_3980[21] = {
    19578, 16582, 12257, 7906, 4576, 2366, 1170, 536,
    261, 119, 65, 31, 19, 10, 6, 3,
    3, 2, 1, 1, 1,
};
static const uint16_t find_index_counts_3980[32] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 1, 1, 1, 1, 1, 1, 1, 23, 2, 2, 2, 2, 2, 24, 3, 3, 3, 25, 4, 26, 27, 28
};

/** Filter orders depending on compression level */
static const uint16_t ape_filter_orders[5][APE_FILTER_LEVELS] = {
    {  0,   0,    0 },
    { 16,   0,    0 },
    { 64,   0,    0 },
    { 32, 256,    0 },
    { 16, 256, 1280 }
};

static const uint32_t K_SUM_MIN_BOUNDARY[32] = {
    0,32,64,128,256,512,1024,2048,
    4096,8192,16384,32768,65536,131072,262144,524288,
    1048576,2097152,4194304,8388608,16777216,33554432,67108864,134217728,
    268435456,536870912,1073741824,2147483648,0,0,0,0
};

//roll buffer
typedef struct {
    uint8_t* buffer;

    uint32_t bufferLen;
    uint32_t bufferLenRemMin;

    //inner
    uint32_t offset;
    uint32_t bufferLenRem;

}ApeRollBuffer;

typedef struct {
    int64_t pos;
    int32_t nblocks;
    int32_t size;
    int32_t skip;
    int64_t pts;
} ApeFrame;

typedef struct {
    uint8_t magic[4];
    int16_t fileversion;
    int16_t padding;
    uint32_t descriptorlength;
    uint32_t headerlength;
    uint32_t seektablelength;
    uint32_t wavheaderlength;
    uint32_t audiodatalength;
    uint32_t audiodatalength_high;
    uint32_t wavtaillength;
    uint8_t md5[16];
} ApeDescriptor;

typedef struct {
    uint16_t compressiontype;
    uint16_t formatflags;
    uint32_t blocksperframe;
    uint32_t finalframeblocks;
    uint32_t totalframes;
    uint16_t bps;
    uint16_t channels;
    uint32_t samplerate;
} ApeHeader;

typedef struct {
    uint8_t cID[4];                         // should equal 'MAC '
    uint16_t nVersion;                        // version number * 1000 (3.81 = 3810)
    uint16_t nCompressionLevel;               // the compression level
    uint16_t nFormatFlags;                    // any format flags (for future use)
    uint16_t nChannels;                       // the number of channels (1 or 2)
    uint32_t nSampleRate;                     // the sample rate (typically 44100)
    uint32_t nHeaderBytes;                    // the bytes after the MAC header that compose the WAV header
    uint32_t nTerminatingBytes;               // the bytes after that raw data (for extended info)
    uint32_t nTotalFrames;                    // the number of frames in the file
    uint32_t nFinalFrameBlocks;               // the number of samples in the final frame
}ApeHeaderOld;

/**************************************************************************************************
WAV header structure
**************************************************************************************************/
typedef struct
{
    // RIFF header
    char cRIFFHeader[4];
    unsigned int nRIFFBytes;

    // data type
    char cDataTypeID[4];

    // wave format
    char cFormatHeader[4];
    unsigned int nFormatBytes;

    unsigned short nFormatTag;
    unsigned short nChannels;
    unsigned int nSamplesPerSec;
    unsigned int nAvgBytesPerSec;
    unsigned short nBlockAlign;
    unsigned short nBitsPerSample;

    // data chunk header
    char cDataHeader[4];
    unsigned int nDataBytes;
}WaveHeader;

typedef struct {

    /* Info from Descriptor Block */
    ApeDescriptor descriptor;
    /* Info from Header Block */
    ApeHeader header;

    
    /* Seektable */
    uint32_t* seektable;
    /* wav header */
    void* waveHeader;
    /* Derived fields */
    uint32_t junklength;
    uint32_t firstframe;
    uint32_t totalsamples;

    /*how many bytes have been readed in bitStream*/
    //uint32_t bitStreamReadedIndex;
} ApeContext;

//demuxer
typedef struct {
    ApeContext* context;
    //int currentframe;
    //ApeFrame* frames;
} ApeDemuxer;


typedef struct {
    uint32_t k;
    uint32_t ksum;
} ApeRice;

typedef struct {
    uint32_t low;           ///< low end of interval
    uint32_t range;         ///< length of interval
    uint32_t help;          ///< bytes_to_follow resp. intermediate value
    uint32_t buffer;    ///< buffer for input/output
} ApeRangecoder;

typedef struct {
    int32_t cacheA[4];
    int32_t coeffsA[4];
    int32_t adatpA[4];

    int32_t cacheB[5]__ALIGN8__;
    int32_t coeffsB[5]__ALIGN8__;  ///< adaption coefficients
    int32_t adatpB[5]__ALIGN8__;

    int32_t lastA;
    int32_t BOld;
    int32_t outOld;
} ApePredictor;

typedef struct {
    ApePredictor predictor[APE_MAX_CHANNELS];
    int32_t xOld;
}ApePrediction;



/** Filters applied to the decoded data */
typedef struct {
    ApeRollBuffer coeffs;       ///< actual coefficients used in filtering
    ApeRollBuffer adaptCoeffs;  ///< adaptive filter coefficients used for correcting of actual filter coefficients
    ApeRollBuffer delay;        ///< filtered values
    int32_t avg;
}ApeNNFilter_t;


typedef struct {
    AlgoBasePorting* basePorting;
    ApeContext *context;

    //rang decode
    ApeRangecoder rc;                                               ///< rangecoder used to decode actual values
    ApeRice riceX;                                                  ///< rice code parameters for the second channel
    ApeRice riceY;                                                  ///< rice code parameters for the first channel

    //NN Filter
    ApeNNFilter_t filters[APE_MAX_CHANNELS][APE_FILTER_LEVELS];     ///< filters used for reconstruction
    int16_t* filterbuf[APE_MAX_CHANNELS][APE_FILTER_LEVELS];        ///< filter memory

    //preditor
    ApePrediction prediction;

    //decoded pcm buffer
    int32_t ch0[APE_BLOCKS_MAX];                                   ///< decoded data for the first channel
    int32_t ch1[APE_BLOCKS_MAX];                                   ///< decoded data for the second channel

    //input buffer read
    void* bufferRead;
    //
    ApeFrame frame;

    //other
    int32_t fSet;                                                   ///< which filter set to use (calculated from compression level)
    uint32_t CRC;                                                   ///< frame CRC
    uint32_t frameFlags;                                            ///< frame flags
    uint16_t blocksMax;
    uint16_t blocksDecoded;
    int32_t currentframe;
    
    //ApeBufferCtr_t inBuffTemp;
    uint32_t blocksUsed;
    bool isStartFrame;
    int32_t exitByte;
    int32_t notEnoughInputAcc;
    b1 isInited;
} ApeDecoder;


