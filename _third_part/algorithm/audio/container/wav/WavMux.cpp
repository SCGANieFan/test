#if 1
#include"Algo.AudioSamlpes.h"
#include"Algo.AudioCal.Basic.h"
#if 0
#include"Algo.AudioCal.Product.h"
#include"Algo.AudioCal.OverlapAdd.h"
#include"Algo.AudioCal.WaveFormMatch.h"
#endif
#include"WavMux.h"

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
	AudioInfo info;
	AudioInfo muteInfo;

	i32 frameSamples;
	i32 lostCount;
	i32 overlapInSamples;
	i32 frameSamplesInner;
	i32 decaySamples;
#if 0
	FuncList* funcList;
#endif
	AudioSamples inHistory;
	AudioSamples infuture;
	AudioSamples fillSignal;
	AudioSamples muteFactor;

	i32 decaySamplesNow;
	i32 fillSignalSampleIndex;
	i32 matchSamples;
	i32 seekSamples;

	b1 isInited;
}WavMuxState;

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
	int32_t WavMux_GetStateSize()
	{
		return sizeof(WavMuxState);
	}

	int32_t WavMux_StateInit(void* pWavMuxStateIn, WavMuxInitParam* paramIn)
	{
		//check
		if (!pWavMuxStateIn
			|| !paramIn
			|| !paramIn->basePorting
			|| paramIn->overlapMs < 0
			|| paramIn->decayTimeMs < 0)
			return WAV_MUX_RET_FAIL;

		//
		WavMuxState* pState = (WavMuxState*)pWavMuxStateIn;
		ALGO_MEM_SET((u8*)pState, 0, WavMux_GetStateSize());
		
		pState->basePorting = paramIn->basePorting;

		pState->info.Init(paramIn->fsHz, paramIn->width, paramIn->channels);

		pState->info.Init(paramIn->fsHz, paramIn->width, paramIn->channels);
		pState->muteInfo.Init(pState->info._rate, pState->info._width, 1);
		
#if 0
		pState->frameSamples = paramIn->frameSamples;
		pState->overlapInSamples = paramIn->overlapMs * pState->info._rate / 1000;
		pState->frameSamplesInner = MAX(pState->frameSamples, pState->info._rate * WAV_MUX_MIN_FRAME_MS / 1000);
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
		BufferSamples bufferSamples;
#if 0
		bufferSamples._samples = WavMuxGetHistorySamples(pState->overlapInSamples, pState->frameSamplesInner);
		bufferSamples._buf = (u8*)pState->basePorting->Malloc(bufferSamples._samples * pState->info._bytesPerSample);
		pState->inHistory.Init(&pState->info, &bufferSamples);
		pState->inHistory.Append(pState->inHistory, 0, pState->inHistory.GetSamplesMax());
#endif

		pState->isInited = true;

		return WAV_MUX_RET_SUCCESS;
	}

	int32_t WavMux_Set(void* pWavMuxStateIn, WavMux_SetChhoose_e choose, void** val)
	{
		//check
		if (!pWavMuxStateIn
			|| choose >= WAV_MUX_SET_CHOOSE_MAX
			|| !val)
			return WAV_MUX_RET_FAIL;
		WavMuxState* pWavMux = (WavMuxState*)pWavMuxStateIn;
		if (pWavMux->isInited == false)
			return WAV_MUX_RET_FAIL;

		switch (choose)
		{
#if 0
		case AUDIO_SPEED_CONTROL_SET_CHOOSE_SPEEDQ8:
			pState->speed = (f32)(u32)val[0] / (1 << 8);
#endif
		default:
			break;
		}
		return WAV_MUX_RET_SUCCESS;
	}

	int32_t WavMux_Get(void* pWavMuxStateIn, WavMux_GetChhoose_e choose, void** val)
	{
		//check
		if (!pWavMuxStateIn
			|| choose >= WAV_MUX_GET_CHOOSE_MAX
			|| !val)
			return WAV_MUX_RET_FAIL;
		WavMuxState* pWavMux = (WavMuxState*)pWavMuxStateIn;
		if (pWavMux->isInited == false)
			return WAV_MUX_RET_FAIL;
		
		switch (choose)
		{
#if 0
		case AUDIO_SPEED_CONTROL_SET_CHOOSE_SPEEDQ8:
			pState->speed = (f32)(u32)val[0] / (1 << 8);
#endif
		default:
			break;
		}
		return WAV_MUX_RET_SUCCESS;
	}

	int32_t WavMux_Run(void* pWavMuxStateIn, uint8_t* in, int32_t inLen, uint8_t* out, int32_t* outLen, bool isLost)
	{
		//check
		if (!pWavMuxStateIn
			||!out
			||!outLen)
			return WAV_MUX_RET_FAIL;
		WavMuxState* pWavMux = (WavMuxState*)pWavMuxStateIn;
		if (isLost == false) {
			if (!in
				|| inLen != (pWavMux->frameSamples * pWavMux->info._bytesPerSample))
				return WAV_MUX_RET_FAIL;
		}
		if (*outLen < pWavMux->frameSamples * pWavMux->info._bytesPerSample)
			return WAV_MUX_RET_FAIL;

		BufferSamples bufferSamples;
#if 0
		AudioSamples pIn;
		bufferSamples._buf = in;
		bufferSamples._samples = pWavMux->frameSamples;
		pIn.Init(&((WavMuxState*)pWavMuxStateIn)->info, &bufferSamples);

		AudioSamples pOut;
		bufferSamples._buf = out;
		bufferSamples._samples = pWavMux->frameSamples;
		pOut.Init(&((WavMuxState*)pWavMuxStateIn)->info, &bufferSamples);
#endif
		return WAV_MUX_RET_SUCCESS;
	}

	int32_t WavMux_StateDeInit(void* pWavMuxStateIn)
	{
		if (!pWavMuxStateIn)
			return WAV_MUX_RET_FAIL;
		WavMuxState* pWavMux = (WavMuxState*)pWavMuxStateIn;
		pWavMux->basePorting->Free(pWavMux->inHistory.GetBufInSample(0));
		pWavMux->basePorting->Free(pWavMux->infuture.GetBufInSample(0));
		pWavMux->basePorting->Free(pWavMux->fillSignal.GetBufInSample(0));
		pWavMux->basePorting->Free(pWavMux->muteFactor.GetBufInSample(0));
		return WAV_MUX_RET_SUCCESS;
	}
}

#endif