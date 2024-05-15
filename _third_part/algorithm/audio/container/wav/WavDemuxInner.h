#pragma once

#include"Algo.Type.h"

using namespace Algo;
//using namespace Audio;

typedef struct {
	//ExpandFormatChunk, size = FmtChunk.chunkSize - 16
	u16 expandLen;//22
	u16 validSamples;
	u32 speakerPos;
	u16 encFormat;
	u8 xx[14];//\x00\x00\x00\x00\x10\x00\x80\x00\x00\xAA\x00\x38\x9B\x71
}ExpandFormatChunk;

typedef struct {
	//FactChunk,size = 24,optional
	u32 ID;//"fact"
	u32 chunkSize;// >4
	u32 samplesTotalPerChannel;
}FactChunk;

#if 0
typedef struct {
	//the RIFF chunk descriptor
	//the format of concern here is WAVE,
	//which requires tow sub-chunks:fmt and data
	u32 chunkID;//"RIFF"
	u32 chunkSize;//(size of file) - (chunkID + chunkSize)
	u32 format;//"WAVE"

	//the fmt sub-chunk
	//describes the format of the sound information in the sub-chunk
	u32 subChunkId1;//"fmt"
	/*
	* audioFormat:	pcm:1,	micro ADPCM:2,	IEEE float:3,	a-law:6,	u-law:7,	GSM 6.10:49,	ITU G.721 ADPCM:64,	...:65534
	* subChunkSize1:16,		18,				18,				18,			18,			20,				...,				40
	* has fac chunk:0,		1,				1,				1,			1,			1,				1,					0
	*/
	u32 subChunkSize1;
	u16 audioFormat;
	u16 numChannels;
	u32 sampleRate;
	u32 byteRate;//channels*samplerate*width
	u16 blockAlign;//channels*width
	u16 bitsPerSample;//width*8, 4,8,12,16,24,32
	//ExpandFormatChunk, size =  subChunkSize1 - 16

	//FactChunk,size = 24,optional

	//the data sub-chunk
	//indicates the size of the sound infomation and contains the raw sound data
	u32 subChunkId2;
	u32 subChunkSize2;
	
	//data...
}WavHead;
#endif
typedef struct {
	//the RIFF chunk descriptor
	//the format of concern here is WAVE,
	//which requires tow sub-chunks:fmt and data
	u32 chunkID;//"RIFF"
	u32 chunkSize;//(size of file) - (chunkID + chunkSize)
	u32 format;//"WAVE"
}RiffChunk;

typedef struct {
	//the fmt sub-chunk
	//describes the format of the sound information in the sub-chunk
	u32 chunkId;//"fmt"
	/*
	* audioFormat:	pcm:1,	micro ADPCM:2,	IEEE float:3,	a-law:6,	u-law:7,	GSM 6.10:49,	ITU G.721 ADPCM:64,	...:65534
	* subChunkSize:	16,		18,				18,				18,			18,			20,				...,				40
	* has fac chunk:0,		1,				1,				1,			1,			1,				1,					0
	*/
	u32 chunkSize;
	u16 audioFormat;
	u16 numChannels;
	u32 sampleRate;
	u32 byteRate;//channels*samplerate*width
	u16 blockAlign;//channels*width
	u16 bitsPerSample;//width*8, 4,8,12,16,24,32
}FmtChunk;

typedef struct {
	//the data sub-chunk
	//indicates the size of the sound infomation and contains the raw sound data
	u32 chunkId;
	u32 chunkSize;
	//data...
}DataChunk;


typedef struct {
	RiffChunk riff;
	FmtChunk fmt;
	ExpandFormatChunk* expandFmt;
	FactChunk* fact;
	DataChunk data;
}WavHead;
