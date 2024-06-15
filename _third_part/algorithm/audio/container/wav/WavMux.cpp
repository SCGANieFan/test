#if 1
#include"Algo.Data.h"
#include"WavMux.h"

using namespace Algo;
//using namespace Audio;


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
	u32 chunkId;//"fmt "
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
	DataChunk data;
}WavHead;


typedef struct
{
	AlgoBasePorting* basePorting;
	WavHead head;
	b1 isInited;
}WavMuxState;



EXTERNC {
	int32_t WavMux_GetStateSize()
	{
		return sizeof(WavMuxState);
	}

	int32_t WavMux_StateInit(void* pStateIn, WavMuxInitParam* paramIn)
	{
		//check
		if (!pStateIn
			|| !paramIn
			|| !paramIn->basePorting)
			return WAV_MUX_RET_FAIL;

		//
		WavMuxState* pState = (WavMuxState*)pStateIn;
		ALGO_MEM_SET((u8*)pState, 0, WavMux_GetStateSize());
		
		pState->basePorting = paramIn->basePorting;
		
		pState->head.riff.chunkID = *(u32*)"RIFF";
		//pState->head.riff.chunkSize
		pState->head.riff.format = *(u32*)"WAVE";

		pState->head.fmt.chunkId = *(u32*)"fmt ";//
		pState->head.fmt.chunkSize = 16;
		pState->head.fmt.audioFormat = 1;
#if 0
		pState->head.fmt.numChannels = paramIn->channels;
		pState->head.fmt.sampleRate = paramIn->fsHz;
		pState->head.fmt.byteRate = paramIn->channels * paramIn->fsHz * widht;
		pState->head.fmt.blockAlign = paramIn->channels * widht;
		pState->head.fmt.bitsPerSample = widht << 3;
#endif
		pState->head.data.chunkId = *(u32*)"data";
		//pState->head.data.chunkSize

		pState->isInited = true;

		return WAV_MUX_RET_SUCCESS;
	}

	int32_t WavMux_Set(void* pStateIn, WavMux_SetChhoose_e choose, void* val)
	{
		//check
		if (!pStateIn
			|| choose >= WAV_MUX_SET_CHOOSE_MAX
			|| !val)
			return WAV_MUX_RET_FAIL;
		WavMuxState* pWavMux = (WavMuxState*)pStateIn;
		if (pWavMux->isInited == false)
			return WAV_MUX_RET_FAIL;

		switch (choose)
		{
		case WAV_MUX_SET_CHOOSE_BASIC_INFO:
			i32 rate = (i32)((void**)val)[0];
			i16 ch = (i16)((void**)val)[1];
			i16 widht = (i16)((void**)val)[2];
			pWavMux->head.fmt.numChannels = ch;
			pWavMux->head.fmt.sampleRate = rate;
			pWavMux->head.fmt.byteRate = ch * rate * widht;
			pWavMux->head.fmt.blockAlign = ch * widht;
			pWavMux->head.fmt.bitsPerSample = widht << 3;
#if 0
		default:
			break;
#endif
		}
		return WAV_MUX_RET_SUCCESS;
	}

	int32_t WavMux_Get(void* pStateIn, WavMux_GetChhoose_e choose, void* val)
	{
		//check
		if (!pStateIn
			|| choose >= WAV_MUX_GET_CHOOSE_MAX
			|| !val)
			return WAV_MUX_RET_FAIL;
		WavMuxState* pWavMux = (WavMuxState*)pStateIn;
		if (pWavMux->isInited == false)
			return WAV_MUX_RET_FAIL;
		
		switch (choose)
		{
#if 1
		case WAV_MUX_GET_CHOOSE_HEAD_SIZE:
			*(u32*)val = sizeof(WavHead);
			break;
		case WAV_MUX_GET_CHOOSE_HEAD:
			ALGO_MEM_CPY(val, &pWavMux->head, sizeof(WavHead));
			break;
#endif
		default:
			break;
		}
		return WAV_MUX_RET_SUCCESS;
	}

	int32_t WavMux_Run(void* pStateIn, uint8_t* in, int32_t inLen, uint8_t* out, int32_t* outLen)
	{
		//check
		if (!pStateIn
			||!out
			||!outLen)
			return WAV_MUX_RET_FAIL;
		WavMuxState* pWavMux = (WavMuxState*)pStateIn;

		pWavMux->head.data.chunkSize += inLen;
		pWavMux->head.riff.chunkSize = pWavMux->head.data.chunkSize + sizeof(WavHead) - 8;
		ALGO_MEM_CPY(out, in, inLen);
		if (pWavMux->head.fmt.bitsPerSample == 8)
		{
			for (i32 i = 0; i < inLen; i++)
			{
				out[i] += 0x80;
			}
		}

		*outLen = inLen;
		
		return WAV_MUX_RET_SUCCESS;
	}

	int32_t WavMux_StateDeInit(void* pStateIn)
	{
		if (!pStateIn)
			return WAV_MUX_RET_FAIL;
		WavMuxState* pWavMux = (WavMuxState*)pStateIn;
#if 0
		pWavMux->basePorting->Free(pWavMux->inHistory.GetBufInSample(0));
		pWavMux->basePorting->Free(pWavMux->infuture.GetBufInSample(0));
		pWavMux->basePorting->Free(pWavMux->fillSignal.GetBufInSample(0));
		pWavMux->basePorting->Free(pWavMux->muteFactor.GetBufInSample(0));
#endif
		return WAV_MUX_RET_SUCCESS;
	}
}

#endif