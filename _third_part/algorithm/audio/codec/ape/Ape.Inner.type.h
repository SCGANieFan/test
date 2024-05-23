#pragma once
#include"Algo.Type.h"
#include"Algo.BasePorting.h"
#include"Algo.Macro.h"
#include"Algo.Memory.h"
#include"Algo.Data.h"

using namespace Algo;

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
#define APE_FILTER_LEVELS 3
#define APE_MAX_CHANNELS 2

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


typedef i16 APE_RET_t;


/** Filter fraction bits depending on compression level */
static const u8 ape_filter_fracbits[5][APE_FILTER_LEVELS] = {
    {  0,  0,  0 },
    { 11,  0,  0 },
    { 11,  0,  0 },
    { 10, 13,  0 },
    { 11, 13, 15 }
};
/**
 * Probability ranges for symbols in Monkey Audio version 3.97
 */
static const u16 counts_diff_3970[21] = {
    14824, 13400, 11124, 8507, 6139, 4177, 2755, 1756,
    1104, 677, 415, 248, 150, 89, 54, 31,
    19, 11, 7, 4, 2,
};

/**
 * Fixed probabilities for symbols in Monkey Audio version 3.97
 */
static const u16 counts_3970[22] = {
        0, 14824, 28224, 39348, 47855, 53994, 58171, 60926,
    62682, 63786, 64463, 64878, 65126, 65276, 65365, 65419,
    65450, 65469, 65480, 65487, 65491, 65493,
};
static const i32 initial_coeffs[4] = {
    360, 317, -109, 98
};
/**
 * Fixed probabilities for symbols in Monkey Audio version 3.98
 */
static const u16 counts_3980[22] = {
        0, 19578, 36160, 48417, 56323, 60899, 63265, 64435,
    64971, 65232, 65351, 65416, 65447, 65466, 65476, 65482,
    65485, 65488, 65490, 65491, 65492, 65493,
};

/**
 * Probability ranges for symbols in Monkey Audio version 3.98
 */
static const u16 counts_diff_3980[21] = {
    19578, 16582, 12257, 7906, 4576, 2366, 1170, 536,
    261, 119, 65, 31, 19, 10, 6, 3,
    3, 2, 1, 1, 1,
};
static const u16 find_index_counts_3980[32] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 1, 1, 1, 1, 1, 1, 1, 23, 2, 2, 2, 2, 2, 24, 3, 3, 3, 25, 4, 26, 27, 28
};

/** Filter orders depending on compression level */
static const u16 ape_filter_orders[5][APE_FILTER_LEVELS] = {
    {  0,   0,    0 },
    { 16,   0,    0 },
    { 64,   0,    0 },
    { 32, 256,    0 },
    { 16, 256, 1280 }
};

static const u32 K_SUM_MIN_BOUNDARY[32] = {
    0,32,64,128,256,512,1024,2048,
    4096,8192,16384,32768,65536,131072,262144,524288,
    1048576,2097152,4194304,8388608,16777216,33554432,67108864,134217728,
    268435456,536870912,1073741824,2147483648,0,0,0,0
};

//roll buffer
typedef struct {
    u8* buffer;

    u32 bufferLen;
    u32 bufferLenRemMin;

    //inner
    u32 offset;
    u32 bufferLenRem;

}ApeRollBuffer;

typedef struct {
    i64 pos;
    i32 nblocks;
    i32 size;
    i32 skip;
    i64 pts;
} ApeFrame;

typedef struct {
    u8 magic[4];
    i16 fileversion;
    i16 padding;
    u32 descriptorlength;
    u32 headerlength;
    u32 seektablelength;
    u32 wavheaderlength;
    u32 audiodatalength;
    u32 audiodatalength_high;
    u32 wavtaillength;
    u8 md5[16];
} ApeDescriptor;

typedef struct {
    u16 compressiontype;
    u16 formatflags;
    u32 blocksperframe;
    u32 finalframeblocks;
    u32 totalframes;
    u16 bps;
    u16 channels;
    u32 samplerate;
} ApeHeader;

typedef struct {
    u8 cID[4];                         // should equal 'MAC '
    u16 nVersion;                        // version number * 1000 (3.81 = 3810)
    u16 nCompressionLevel;               // the compression level
    u16 nFormatFlags;                    // any format flags (for future use)
    u16 nChannels;                       // the number of channels (1 or 2)
    u32 nSampleRate;                     // the sample rate (typically 44100)
    u32 nHeaderBytes;                    // the bytes after the MAC header that compose the WAV header
    u32 nTerminatingBytes;               // the bytes after that raw data (for extended info)
    u32 nTotalFrames;                    // the number of frames in the file
    u32 nFinalFrameBlocks;               // the number of samples in the final frame
}ApeHeaderOld;

/**************************************************************************************************
WAV header structure
**************************************************************************************************/
typedef struct
{
    // RIFF header
    i8 cRIFFHeader[4];
    u32 nRIFFBytes;

    // data type
    i8 cDataTypeID[4];

    // wave format
    i8 cFormatHeader[4];
    u32 nFormatBytes;

    u16 nFormatTag;
    u16 nChannels;
    u32 nSamplesPerSec;
    u32 nAvgBytesPerSec;
    u16 nBlockAlign;
    u16 nBitsPerSample;

    // data chunk header
    i8 cDataHeader[4];
    u32 nDataBytes;
}WaveHeader;

typedef struct {

    /* Info from Descriptor Block */
    ApeDescriptor descriptor;
    /* Info from Header Block */
    ApeHeader header;


    /* Seektable */
    u32* seektable;
    /* wav header */
    void* waveHeader;
    /* Derived fields */
    u32 junklength;
    u32 firstframe;
    u32 totalsamples;

    /*how many bytes have been readed in bitStream*/
    u32 bitStreamReadedIndex;
} ApeContext;

//demuxer
typedef struct {
    ApeContext* context;
    //int currentframe;
    //ApeFrame* frames;
} ApeDemuxer;


typedef struct {
    u32 k;
    u32 ksum;
} ApeRice;

typedef struct {
    u32 low;           ///< low end of interval
    u32 range;         ///< length of interval
    u32 help;          ///< bytes_to_follow resp. intermediate value
    u32 buffer;    ///< buffer for input/output
} ApeRangecoder;

typedef struct {
    i32 cacheA[4];
    i32 coeffsA[4];
    i32 adatpA[4];

    i32 cacheB[5]__ALIGN8__;
    i32 coeffsB[5]__ALIGN8__;  ///< adaption coefficients
    i32 adatpB[5]__ALIGN8__;

    i32 lastA;
    i32 BOld;
    i32 outOld;
} ApePredictor;

typedef struct {
    ApePredictor predictor[APE_MAX_CHANNELS];
    i32 xOld;
}ApePrediction;



/** Filters applied to the decoded data */
typedef struct {
    ApeRollBuffer coeffs;       ///< actual coefficients used in filtering
    ApeRollBuffer adaptCoeffs;  ///< adaptive filter coefficients used for correcting of actual filter coefficients
    ApeRollBuffer delay;        ///< filtered values
    i32 avg;
}ApeNNFilter_t;




#if 1
class BufferRead
{
public:
    BufferRead() {};
    ~BufferRead() {};

public:
    u32 inline GetBufferUsed() {
        return sizeUsedByte;
    };
    u32 inline GetByteUsed() {
        return sizeUsedByteAlign1;
    }
    u32 inline GetBufferRemByte() {
        return sizeRemByte;
    }
    b1 inline Init()
    {
        offset = -1;
        return true;
    }
    b1 inline SetBufferIn(u8* in, u32 inSize) {
        //check
        if ((!in)
            || (inSize <= 0))
            return false;
        buffer = in;
        sizeRemByte = inSize;
        sizeUsedByte = 0;
        sizeUsedByteAlign1 = 0;

        return true;
    }

    inline b1 BufferCacheReadByte(u8* readedByte)
    {
        if (offset < 0)
            return false;
        *readedByte = data8[offset];
        offset--;
        return true;
    }

    inline b1 BufferCacheUpdata(u8* in, u32 inSize)
    {
        ALGO_MEM_CPY(data8, in, 4);
        offset = 3;
        return true;
    }

    //not saftey
    inline u8 BufferReadByte()
    {
        u8 readedByte;
        if (!BufferCacheReadByte(&readedByte))
        {
            BufferCacheUpdata(buffer + sizeUsedByte, sizeRemByte);
            BufferCacheReadByte(&readedByte);
            sizeUsedByte += 4;
            sizeRemByte -= 4;
        }
        sizeUsedByteAlign1++;
        return readedByte;
    }


    u32 inline Read(u8 readByte)
    {
        //read
        u32 readedDoubleWord = 0;
        for (u8 i = 0; i < readByte; i++)
        {
            readedDoubleWord <<= 8;
            readedDoubleWord += (u32)BufferReadByte();
        }
        return readedDoubleWord;
    }
//private:
public:
    union {
        u32 data32;
        u8 data8[4];
    };
    int8_t offset = -1;
    u8* buffer = 0;
    u32 sizeUsedByte = 0;
    u32 sizeUsedByteAlign1 = 0;
    u32 sizeRemByte = 0;
};
#endif

typedef enum{
    APE_MODE_DMUX=0,
    APE_MODE_DEC,
}ApeMode;

typedef struct {
    AlgoBasePorting* basePorting;
    ApeContext context;
    //ApeContext* pContext;

    //rang decode
    ApeRangecoder rc;                                               ///< rangecoder used to decode actual values
    ApeRice riceX;                                                  ///< rice code parameters for the second channel
    ApeRice riceY;                                                  ///< rice code parameters for the first channel

    //NN Filter
    ApeNNFilter_t filters[APE_MAX_CHANNELS][APE_FILTER_LEVELS];     ///< filters used for reconstruction
    i16* filterbuf[APE_MAX_CHANNELS][APE_FILTER_LEVELS];        ///< filter memory

    //preditor
    ApePrediction prediction;

    //decoded pcm buffer
    i32 ch0[APE_BLOCKS_MAX];                                   ///< decoded data for the first channel
    i32 ch1[APE_BLOCKS_MAX];                                   ///< decoded data for the second channel

    //input buffer read
    BufferRead bufferRead;
    //
    ApeFrame frame;

    //other
    i32 fSet;                                                   ///< which filter set to use (calculated from compression level)
    u32 CRC;                                                   ///< frame CRC
    u32 frameFlags;                                            ///< frame flags
    u16 blocksMax;
    u16 blocksDecoded;
    i32 currentframe;

    //ApeBufferCtr_t inBuffTemp;
    u32 blocksUsed;
    b1 isStartFrame;
    i32 exitByte;
    i32 notEnoughInputAcc;
    b1 isInited;
    ApeMode mode;
    void* mallocPtr[10];
    Data inCache;
} ApeDecoder;