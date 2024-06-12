#pragma once
#include"ApeCom.h"

EXTERNC int32_t ApeDemuxerGetHeaderSize();
EXTERNC int32_t ApeDemuxerGetPraseInBufferSize(uint8_t* in, int32_t inLen);


//get the size of demuxer should be alloc
EXTERNC int32_t ApeGetDemuxerSize(uint8_t* in, int32_t inLen);


//get compress level from demuxer
EXTERNC uint16_t ApeGetDemuxerCompressLevel(void* demuxer);
EXTERNC uint32_t ApeGetDemuxerSeektableLength(void* demuxer);
EXTERNC uint32_t ApeGetDemuxerWavheaderLength(void* demuxer);

EXTERNC uint32_t ApeGetDemuxerChannels(void* demuxer);
EXTERNC uint32_t ApeGetDemuxerSampleRate(void* demuxer);
EXTERNC uint32_t ApeGetDemuxerSampleBits(void* demuxer);

//get heaedr info (context) from demuxer
EXTERNC void* ApeGetDemuxerContext(void* demuxer);

//initial demuxer
EXTERNC APE_RET_t ApeDemuxerInit(void* demuxer, int32_t demuxerSize);

//prase header info
EXTERNC APE_RET_t ApeDemuxerPrase(void* demuxer, uint8_t* in, uint32_t inSize, uint32_t* inUsed);




