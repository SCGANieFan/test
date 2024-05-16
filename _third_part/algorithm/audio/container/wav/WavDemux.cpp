#if 1
#include"Algo.Data.h"
#include"Algo.AudioCal.Basic.h"
#include"WavDemux.h"
#include"WavDemuxInner.h"

using namespace Algo;
using namespace Audio;

typedef enum {
	WavDemuxStage_SearchRIFF=0,
	WavDemuxStage_RIFF,
	WavDemuxStage_Format,
	WavDemuxStage_ExpandFormat,
	WavDemuxStage_SearchFact,
	WavDemuxStage_Fact,
	WavDemuxStage_DataHead,
	WavDemuxStage_Data,
}WavDemuxStage;


typedef struct
{
	AlgoBasePorting* basePorting;
	WavHead head;
	i32 frameSamples;
	Data iCache;
	WavDemuxStage stage;
	b1 isInited;
	b1 hasDemuxHead;
}WavDemuxState;


EXTERNC {
	int32_t WavDemux_GetStateSize()
	{
		return sizeof(WavDemuxState);
	}

	int32_t WavDemux_StateInit(void* pWavDemuxStateIn, WavDemuxInitParam* paramIn)
	{
		//check
		if (!pWavDemuxStateIn
			|| !paramIn
			|| !paramIn->basePorting)
			return WAV_DEMUX_RET_FAIL;

		//
		WavDemuxState* pState = (WavDemuxState*)pWavDemuxStateIn;
		ALGO_MEM_SET((u8*)pState, 0, WavDemux_GetStateSize());
		
		pState->basePorting = paramIn->basePorting;
		pState->frameSamples = paramIn->frameSamples;

		i32 bufLen = 1024;
		u8* pBuf= (u8*)pState->basePorting->Malloc(bufLen);
		Buffer buff;
		buff.Init(pBuf, bufLen);
		pState->iCache.Init(&buff);
		pState->isInited = true;

		return WAV_DEMUX_RET_SUCCESS;
	}

	int32_t WavDemux_Set(void* pWavDemuxStateIn, WavDemux_SetChhoose_e choose, void** val)
	{
		//check
		if (!pWavDemuxStateIn
			|| choose >= WAV_DEMUX_SET_CHOOSE_MAX
			|| !val)
			return WAV_DEMUX_RET_FAIL;
		WavDemuxState* pWavDemux = (WavDemuxState*)pWavDemuxStateIn;
		if (pWavDemux->isInited == false)
			return WAV_DEMUX_RET_FAIL;

		switch (choose)
		{
#if 0
		case AUDIO_SPEED_CONTROL_SET_CHOOSE_SPEEDQ8:
			pState->speed = (f32)(u32)val[0] / (1 << 8);
#endif
		default:
			break;
		}
		return WAV_DEMUX_RET_SUCCESS;
	}

	int32_t WavDemux_Get(void* pWavDemuxStateIn, WavDemux_GetChhoose_e choose, void** val)
	{
		//check
		if (!pWavDemuxStateIn
			|| choose >= WAV_DEMUX_GET_CHOOSE_MAX
			|| !val)
			return WAV_DEMUX_RET_FAIL;
		WavDemuxState* pWavDemux = (WavDemuxState*)pWavDemuxStateIn;
		if (pWavDemux->isInited == false)
			return WAV_DEMUX_RET_FAIL;

		switch (choose)
		{
		case WAV_DEMUX_GET_CHOOSE_HAS_HEAD:
			if(pWavDemux->hasDemuxHead)
				*(u32*)val[0] = 1;
			else
				*(u32*)val[0] = 0;
			break;
#if 0
		case WAV_DEMUX_GET_CHOOSE_RATE:
			*(u32*)val[0] = pWavDemux->head.fmt.sampleRate;
			break;
		case WAV_DEMUX_GET_CHOOSE_CH:
			*(u32*)val[0] = pWavDemux->head.fmt.numChannels;
			break;
		case WAV_DEMUX_GET_CHOOSE_WIDTH:
			*(u32*)val[0] = pWavDemux->head.fmt.blockAlign / pWavDemux->head.fmt.numChannels;
#endif
		case WAV_DEMUX_GET_CHOOSE_BASIC_INFO:
			*(u32*)val[0] = pWavDemux->head.fmt.sampleRate;
			*(u32*)val[1] = pWavDemux->head.fmt.numChannels;
			*(u32*)val[2] = pWavDemux->head.fmt.blockAlign / pWavDemux->head.fmt.numChannels;
			break;
		default:
			break;
		}
		return WAV_DEMUX_RET_SUCCESS;
	}

	int32_t WavDemux_Run(void* pStateIn, uint8_t* in, int32_t inLen, uint8_t* out, int32_t* outLen)
	{
		//check
		if (!pStateIn
			||!out
			||!outLen)
			return WAV_DEMUX_RET_FAIL;
		WavDemuxState* pWavDemux = (WavDemuxState*)pStateIn;
#if 1
		Buffer buff;

		Data pIn;
		buff.Init(in, inLen);
		pIn.Init(&buff);
		pIn.Append(inLen);

		Data pOut;
		buff.Init(out, *outLen);
		pOut.Init(&buff);


		while (pIn.GetSize() > 0)
		{
			switch (pWavDemux->stage)
			{
			case WavDemuxStage_SearchRIFF:
			{
				const i8* riffKey = "RIFF";
				//i32 skipByte = 0;
				i32 inRemByte = inLen;
				while (pIn.GetSize() > 0)
				{
					if (*(u32*)pIn.GetData() == (*(u32*)riffKey))
					{
						pWavDemux->stage = WavDemuxStage_RIFF;
						pWavDemux->iCache.Used(pWavDemux->iCache.GetSize());
						pWavDemux->iCache.ClearUsed();
						break;
					}
					pIn.Used(1);
				}
				break;
			}
			case WavDemuxStage_RIFF:
			{
				i32 size = sizeof(RiffChunk);
				if ((pIn.GetSize() + pWavDemux->iCache.GetSize()) > size)
				{
					i32 appendSize = size - pWavDemux->iCache.GetSize();
					if (appendSize > 0)
					{
						pWavDemux->iCache.Append(pIn.GetData(), appendSize);
						pIn.Used(appendSize);
					}

					pWavDemux->head.riff = *(RiffChunk*)pWavDemux->iCache.GetData();
					pWavDemux->iCache.Used(size);
					pWavDemux->stage = WavDemuxStage_Format;
				}
				else
				{
					pWavDemux->iCache.Append(pIn.GetData(), pIn.GetSize());
					pIn.Used(pIn.GetSize());
				}
				break;
			}
			case WavDemuxStage_Format:
			{
				i32 size = sizeof(FmtChunk);
				if ((pIn.GetSize() + pWavDemux->iCache.GetSize()) > size)
				{
					i32 appendSize = size - pWavDemux->iCache.GetSize();
					if (appendSize > 0)
					{
						pWavDemux->iCache.Append(pIn.GetData(), appendSize);
						pIn.Used(appendSize);
					}

					pWavDemux->head.fmt = *(FmtChunk*)pWavDemux->iCache.GetData();
					pWavDemux->iCache.Used(size);
					pWavDemux->stage = WavDemuxStage_ExpandFormat;
				}
				else
				{
					pWavDemux->iCache.Append(pIn.GetData(), pIn.GetSize());
					pIn.Used(pIn.GetSize());
				}
				break;
			}
			case WavDemuxStage_ExpandFormat:
			{
				i32 size = (pWavDemux->head.fmt.chunkSize + 8) - sizeof(FmtChunk);
				if (size > 0)
				{
					if ((pIn.GetSize() + pWavDemux->iCache.GetSize()) > size)
					{
						i32 appendSize = size - pWavDemux->iCache.GetSize();
						if (appendSize > 0)
						{
							pWavDemux->iCache.Append(pIn.GetData(), appendSize);
							pIn.Used(appendSize);
						}
						pWavDemux->head.expandFmt = (ExpandFormatChunk*)pWavDemux->basePorting->Malloc(size);
						ALGO_MEM_CPY(pWavDemux->head.expandFmt, pWavDemux->iCache.GetData(), size);
						pWavDemux->iCache.Used(size);
						pWavDemux->stage = WavDemuxStage_SearchFact;
					}
					else
					{
						pWavDemux->iCache.Append(pIn.GetData(), pIn.GetSize());
						pIn.Used(pIn.GetSize());
					}
				}
				else
				{
					pWavDemux->stage = WavDemuxStage_SearchFact;
				}
				break;
			}
			case WavDemuxStage_SearchFact:
			{
				i32 size = 4;
				if ((pIn.GetSize() + pWavDemux->iCache.GetSize()) > size)
				{
					i32 appendSize = size - pWavDemux->iCache.GetSize();
					if (appendSize > 0)
					{
						pWavDemux->iCache.Append(pIn.GetData(), appendSize);
						pIn.Used(appendSize);
					}
					const i8* factKey = "fact";
					if (*(u32*)pWavDemux->iCache.GetData() == *(u32*)factKey)
					{
						pWavDemux->stage = WavDemuxStage_Fact;
					}
					else
					{
						pWavDemux->stage = WavDemuxStage_DataHead;
					}
				}
				else
				{
					pWavDemux->iCache.Append(pIn.GetData(), pIn.GetSize());
					pIn.Used(pIn.GetSize());
				}
				break;
			}
			case WavDemuxStage_Fact:
			{
				i32 size = sizeof(FactChunk);
				if ((pIn.GetSize() + pWavDemux->iCache.GetSize()) > size)
				{
					i32 appendSize = size - pWavDemux->iCache.GetSize();
					if (appendSize > 0)
					{
						pWavDemux->iCache.Append(pIn.GetData(), appendSize);
						pIn.Used(appendSize);
					}

					pWavDemux->head.fact = (FactChunk*)pWavDemux->basePorting->Malloc(sizeof(FactChunk));
					*pWavDemux->head.fact = *(FactChunk*)pWavDemux->iCache.GetData();
					pWavDemux->iCache.Used(size);
					pWavDemux->stage = WavDemuxStage_DataHead;
				}
				else
				{
					pWavDemux->iCache.Append(pIn.GetData(), pIn.GetSize());
					pIn.Used(pIn.GetSize());
				}
				break;
			}
			case WavDemuxStage_DataHead:
			{
				i32 size = sizeof(DataChunk);
				if ((pIn.GetSize() + pWavDemux->iCache.GetSize()) > size)
				{
					i32 appendSize = size - pWavDemux->iCache.GetSize();
					if (appendSize > 0)
					{
						pWavDemux->iCache.Append(pIn.GetData(), appendSize);
						pIn.Used(appendSize);
					}

					pWavDemux->head.data = *(DataChunk*)pWavDemux->iCache.GetData();
					pWavDemux->iCache.Used(size);
					pWavDemux->hasDemuxHead = true;
					pWavDemux->stage = WavDemuxStage_Data;
				}
				else
				{
					pWavDemux->iCache.Append(pIn.GetData(), pIn.GetSize());
					pIn.Used(pIn.GetSize());
				}
				break;
			}
			case WavDemuxStage_Data:
			{
				i32 size;
				//pOut
				if (pWavDemux->iCache.GetSize())
				{
					pOut.AppendFully(pWavDemux->iCache.GetData(), pWavDemux->iCache.GetSize(), &size);
					pWavDemux->iCache.Used(size);
				}
				if (pOut.GetLeftSize())
				{
					pOut.AppendFully(pIn.GetData(), pIn.GetSize(), &size);
					pIn.Used(size);
				}
				if (pIn.GetSize())
				{
					pWavDemux->iCache.Append(pIn.GetData(), pIn.GetSize());
					pIn.Used(pIn.GetSize());
				}
				break;
			default:
				break;
			}
			}
		}
#endif
		*outLen = pOut.GetSize();
		return WAV_DEMUX_RET_SUCCESS;
	}

	int32_t WavDemux_StateDeInit(void* pWavDemuxStateIn)
	{
		if (!pWavDemuxStateIn)
			return WAV_DEMUX_RET_FAIL;
		WavDemuxState* pWavDemux = (WavDemuxState*)pWavDemuxStateIn;
		u8* ptr = (u8*)pWavDemux->head.expandFmt;
		if (ptr)
			pWavDemux->basePorting->Free(ptr);
		ptr = (u8*)pWavDemux->head.fact;
		if (ptr)
			pWavDemux->basePorting->Free(ptr);
		ptr = (u8*)pWavDemux->iCache.GetBuf();
		if (ptr)
			pWavDemux->basePorting->Free(ptr);

		return WAV_DEMUX_RET_SUCCESS;
	}
}

#endif