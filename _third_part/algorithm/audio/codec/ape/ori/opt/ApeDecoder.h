#pragma once


#include"ApeCom.h"


//get decoder size should be alloc based on compressLevel,
//compressLevel should be get from header info
EXTERNC uint32_t ApeGetDecoderSize(uint16_t compressLevel, uint32_t seektablelength, uint32_t wavheaderlength);

//initial decoder based on context which come from demux header
EXTERNC APE_RET_t ApeDecoderInit(void* decoder, void* context);

//get channels
EXTERNC uint16_t ApeGetDecoderChannels(void* decoder);

//get sample rate in Hz
EXTERNC uint32_t ApeGetDecoderSampleRate(void* decoder);

//get samlpe bits,includ 8,16,24,32
EXTERNC uint16_t ApeGetDecoderSampleBits(void* decoder);

//get the all blocks in current frame
EXTERNC uint32_t ApeGetDecoderNblocks(void* decoder);

//get the decoded blocks after use ApeDecodeBlocks()
EXTERNC uint16_t ApeGetDecodedBlocks(void* decoder);



//decode
//until has decoded the number of blocksMax setted by ApeSetDecoderBlocksMax()
//if inSize is not enough, the decoded blocks should be get by ApeGetDecodedBlocks()
EXTERNC APE_RET_t ApeDecode(void* decoder, uint8_t* in, int32_t inSize, int32_t* inUsed, uint8_t* out, int32_t* outSize);

