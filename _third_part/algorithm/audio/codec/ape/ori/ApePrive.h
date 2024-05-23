#pragma once

#include<stdio.h>
#include"ApeComInner.h"

#define APE_ALIGN_4(x) ((((x)+3)>>2)<<2)
#define APE_ALIGN_8(x) ((((x)+7)>>3)<<3)

#define APE_IS_ALIGN_4(x) (((x)&0x03)==0)
#define APE_IS_ALIGN_8(x) (((x)&0x07)==0)

#if 1
#define APE_PRINTF(s,...) printf("%s[%d] " s,__func__,__LINE__,##__VA_ARGS__)
#else
#define APE_PRINTF(...)
#endif

//read
uint32_t GetBufferReadSize();
uint32_t GetBufferUsed(void* bufferRead);
uint32_t GetByteUsed(void* bufferRead);
uint32_t GetBufferRemByte(void* bufferRead);

bool BufferReadInit(void* bufferRead);
bool SetBufferIn(void* bufferRead, uint8_t* in, uint32_t inSize);
uint32_t BufferRead(void* bufferRead, uint8_t readByte);

//roll buffer
bool ApeRollBufferInit(ApeRollBuffer* rollBuffer, uint8_t* buffer, uint32_t bufferLen, uint32_t bufferLenRemMin);
void ApeRollBufferIncrement(ApeRollBuffer* rollBuffer, uint32_t bytes);
static inline uint8_t* ApeRollBufferGetBuffer(ApeRollBuffer* rollBuffer)
{
	return rollBuffer->buffer + rollBuffer->offset;
}

//context
#if 0
static inline uint32_t ApeGetWaveHeaderSize()
{
	return APE_ALIGN_8(128);
}
static inline uint32_t ApeGetSeekTableSize()
{
    return APE_ALIGN_8(4 * 1000);
}
#endif
static inline int32_t ApeGetContextSize(uint8_t* in, int32_t inLen)
{
	//if (inLen < sizeof(ApeDescriptor))
	//	return -1;
    ApeDescriptor* descriptor = (ApeDescriptor*)in;
	return APE_ALIGN_8(sizeof(ApeContext)) + APE_ALIGN_8(descriptor->seektablelength) + APE_ALIGN_8(descriptor->wavheaderlength);
}
static inline int32_t ApeGetContextSize(uint32_t seektablelength, uint32_t wavheaderlength)
{
	return APE_ALIGN_8(sizeof(ApeContext)) + APE_ALIGN_8(seektablelength) + APE_ALIGN_8(wavheaderlength);
}

//entropy
bool ApeEntropyDecode(ApeRice* rice, ApeRangecoder* rc, void* bufferRead, int32_t* out, uint16_t version);



//NN filter
static inline int16_t ApeClipInt16(int32_t a)
{
	if ((a + 0x8000) & ~0xFFFF)
		return (a >> 31) ^ 0x7FFF;
	else
		return a;
}
void ApeNNFilter(ApeNNFilter_t filter[APE_FILTER_LEVELS], int32_t* data, uint16_t version, uint16_t fSet);

//prediction filter
void ApePredictionFilterMono(ApePrediction* prediction, int32_t* Y);

void ApePredictionFilterStero(ApePrediction* prediction, int32_t* Y, int32_t* X);

//other
