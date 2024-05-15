#if 1
#include"Algo.AudioSamlpes.h"
#include"Algo.AudioCal.Basic.h"
#if 0
#include"Algo.AudioCal.Product.h"
#include"Algo.AudioCal.OverlapAdd.h"
#include"Algo.AudioCal.WaveFormMatch.h"
#endif
#include"WavDemux.h"
#include"WavDemuxInner.h"

using namespace Algo;
using namespace Audio;



#if 0
typedef struct {
	ALGO_PRODUCT_CB product;
	ALGO_OVERLAP_ADD_CB Overlapadd;
	ALGO_WAVE_FORM_MATCH_CB WaveFormMatch;
	ALGO_APPEND_IN_FIXPOINT AppendInFixpoint;
}FuncList;
#endif

typedef struct
{
	AlgoBasePorting* basePorting;
#if 0
	AudioInfo info;
	AudioInfo muteInfo;
#endif
	WavHead head;

	i32 frameSamples;
#if 0
	i32 lostCount;
	i32 overlapInSamples;
	i32 frameSamplesInner;
	i32 decaySamples;
	FuncList* funcList;
	AudioSamples inHistory;
	AudioSamples infuture;
	AudioSamples fillSignal;
	AudioSamples muteFactor;

	i32 decaySamplesNow;
	i32 fillSignalSampleIndex;
	i32 matchSamples;
	i32 seekSamples;
#endif

	b1 isInited;
	b1 hasDemuxHead;
}WavDemuxState;

#if 0
static FuncList funcList16_g = {
	Algo_Product<i16,i16>,
	Algo_OverlapAdd<i16>,
	Get_Algo_WaveFormMatch(sizeof(i16)),
	Algo_AppendInFixPoint<i16,i32>,
};

static FuncList funcList24_g = {
	Algo_Product<i24,i24>,
	Algo_OverlapAdd<i24>,
	Get_Algo_WaveFormMatch(sizeof(i24)),
	Algo_AppendInFixPoint<i24,i32>,
};

static FuncList funcList32_g = {
	Algo_Product<i32,i32>,
	Algo_OverlapAdd<i32>,
	Get_Algo_WaveFormMatch(sizeof(i32)),
	Algo_AppendInFixPoint<i32,i32>,
};
#endif


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

#if 0

		pState->info.Init(paramIn->fsHz, paramIn->width, paramIn->channels);

		pState->info.Init(paramIn->fsHz, paramIn->width, paramIn->channels);
		pState->muteInfo.Init(pState->info._rate, pState->info._width, 1);
		
		pState->frameSamples = paramIn->frameSamples;
		pState->overlapInSamples = paramIn->overlapMs * pState->info._rate / 1000;
		pState->frameSamplesInner = MAX(pState->frameSamples, pState->info._rate * WAV_DEMUX_MIN_FRAME_MS / 1000);
		pState->decaySamples = paramIn->decayTimeMs * pState->info._rate / 1000;
		if (pState->info._width == 2){
			pState->funcList = &funcList16_g;
		}
		else if (pState->info._width == 3) {
			pState->funcList = &funcList24_g;
		}
		else{
			pState->funcList = &funcList32_g;
		}
#endif
#if 0
		BufferSamples bufferSamples;
		bufferSamples._samples = WavDemuxGetHistorySamples(pState->overlapInSamples, pState->frameSamplesInner);
		bufferSamples._buf = (u8*)pState->basePorting->Malloc(bufferSamples._samples * pState->info._bytesPerSample);
		pState->inHistory.Init(&pState->info, &bufferSamples);
		pState->inHistory.Append(pState->inHistory, 0, pState->inHistory.GetSamplesMax());
#endif

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
#if 0
		case AUDIO_SPEED_CONTROL_SET_CHOOSE_SPEEDQ8:
			pState->speed = (f32)(u32)val[0] / (1 << 8);
#endif
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
		u8* pIn = in;
		if (pWavDemux->hasDemuxHead)
		{
		}
		else
		{
			const i8* riffKey = "RIFF";
			i32 skipByte = 0;
			i32 inRemByte = inLen;
			for (i32 b = 0; b < inLen; b++)
			{
				if (*((u32*)(pIn + b)) == (*(u32*)riffKey))
					break;
				skipByte++;
			}
			pIn += skipByte;
			int a = 1;
		}
#endif


#if 0
		BufferSamples bufferSamples;
		AudioSamples pIn;
		bufferSamples._buf = in;
		bufferSamples._samples = pWavDemux->frameSamples;
		pIn.Init(&((WavDemuxState*)pWavDemuxStateIn)->info, &bufferSamples);

		AudioSamples pOut;
		bufferSamples._buf = out;
		bufferSamples._samples = pWavDemux->frameSamples;
		pOut.Init(&((WavDemuxState*)pWavDemuxStateIn)->info, &bufferSamples);
#endif
		return WAV_DEMUX_RET_SUCCESS;
	}

	int32_t WavDemux_StateDeInit(void* pWavDemuxStateIn)
	{
		if (!pWavDemuxStateIn)
			return WAV_DEMUX_RET_FAIL;
#if 0
		WavDemuxState* pWavDemux = (WavDemuxState*)pWavDemuxStateIn;
		pWavDemux->basePorting->Free(pWavDemux->inHistory.GetBufInSample(0));
		pWavDemux->basePorting->Free(pWavDemux->infuture.GetBufInSample(0));
		pWavDemux->basePorting->Free(pWavDemux->fillSignal.GetBufInSample(0));
		pWavDemux->basePorting->Free(pWavDemux->muteFactor.GetBufInSample(0));
#endif
		return WAV_DEMUX_RET_SUCCESS;
	}
}

#endif