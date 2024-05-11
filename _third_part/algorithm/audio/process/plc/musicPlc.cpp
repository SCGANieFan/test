#if 1
#include"Algo.AS_Calculator.h"
#include"MusicPlc.h"

#define MUSIC_PLC_MIN_FRAME_MS 10
#define MUSIC_PLC_SEEK_MS_DEFAULT 2
#define MUSIC_PLC_MATCH_MS_DEFAULT 2

typedef enum {
	MUTE_MODE_NOMUTE = 0,
	MUTE_MODE_DOMUTE,
	MUTE_MODE_DOMUTE_ZERO,
}MUTE_MODE;

typedef struct
{
	BasePorting* basePorting;
	AudioInfo info;
	AudioInfo muteInfo;

	i32 frameSamples;
	i32 lostCount;
	i32 overlapInSamples;
	i32 frameSamplesInner;
	i32 decaySamples;

	AS_Calculator asCalculator;
	AudioSamples inHistory;
	AudioSamples infuture;
	AudioSamples fillSignal;
	AudioSamples muteFactor;

	i32 decaySamplesNow;
	i32 fillSignalSampleIndex;
	i32 matchSamples;
	i32 seekSamples;

	b1 isInited;
}MusicPlcState;



STATIC INLINE i32 MusicPlcGetMuteFactorSamples(i32 frameSamples)
{
	return frameSamples;
}

STATIC INLINE i32 MusicPlcGetFillSignalSamples(i32 frameSamples)
{
	return frameSamples;
}

STATIC INLINE i32 MusicPlcGetHistorySamples(i32 overlapInSamples, i32 frameSamples)
{
	return frameSamples + overlapInSamples;
}

STATIC INLINE i32 MusicPlcGetFutureSamples(i32 frameSamples)
{
	return frameSamples;
}

STATIC b1 MusicPlcCheckParam(i32 overlapMs, i32 frameSamples, i32 fsHz, i16 channels, i16 width)
{
	if (overlapMs < 1
		|| frameSamples < 1)
		return false;
	i32 overlapInSamples = overlapMs * fsHz / 1000;
	if (overlapInSamples > frameSamples)
		return false;
	if (channels != 1 && channels != 2 && channels != 4)
		return false;
	if (width != 2 && channels != 4)
		return false;

	return true;
}

EXTERNC {
	int32_t MusicPlc_GetStateSize()
	{
		return sizeof(MusicPlcState);
	}

	int32_t MusicPlc_StateInit(void* pMusicPlcStateIn, MusicPlcInitParam* sampleParam)
	{
		//check
		if (!pMusicPlcStateIn
			|| !sampleParam
			|| !sampleParam->basePorting
			|| sampleParam->overlapMs < 0
			|| sampleParam->decayTimeMs < 0)
			return MUSIC_PLC_RET_FAIL;

		if (!MusicPlcCheckParam(sampleParam->overlapMs, sampleParam->frameSamples, sampleParam->fsHz, sampleParam->channels, sampleParam->width))
			return MUSIC_PLC_RET_FAIL;

		//
		MusicPlcState* pPlc = (MusicPlcState*)pMusicPlcStateIn;
		sampleParam->basePorting->MemSet((u8*)pPlc, 0, MusicPlc_GetStateSize());

		pPlc->basePorting = sampleParam->basePorting;

		pPlc->info.Init(sampleParam->fsHz, sampleParam->width, sampleParam->channels);

		pPlc->info.Init(sampleParam->fsHz, sampleParam->width, sampleParam->channels);
		pPlc->muteInfo.Init(pPlc->info._rate, pPlc->info._width, 1);
		
		pPlc->frameSamples = sampleParam->frameSamples;
		pPlc->overlapInSamples = sampleParam->overlapMs * pPlc->info._rate / 1000;
		pPlc->frameSamplesInner = MAX(pPlc->frameSamples, pPlc->info._rate * MUSIC_PLC_MIN_FRAME_MS / 1000);
		pPlc->decaySamples = sampleParam->decayTimeMs * pPlc->info._rate / 1000;
		pPlc->asCalculator.Init(pPlc->info._width);
		BufferSamples bufferSamples;
		
		bufferSamples._samples = MusicPlcGetHistorySamples(pPlc->overlapInSamples, pPlc->frameSamplesInner);
		bufferSamples._buf = (u8*)pPlc->basePorting->Malloc(bufferSamples._samples * pPlc->info._bytesPerSample);
		pPlc->inHistory.Init(&pPlc->info, &bufferSamples);
		pPlc->inHistory.Append(pPlc->inHistory, 0, pPlc->inHistory.GetSamplesMax());

		bufferSamples._samples = MusicPlcGetFutureSamples(pPlc->frameSamples);
		bufferSamples._buf = (u8*)pPlc->basePorting->Malloc(bufferSamples._samples * pPlc->info._bytesPerSample);
		pPlc->infuture.Init(&pPlc->info, &bufferSamples);

		bufferSamples._samples = MusicPlcGetFillSignalSamples(pPlc->frameSamplesInner);
		bufferSamples._buf = (u8*)pPlc->basePorting->Malloc(bufferSamples._samples * pPlc->info._bytesPerSample);
		pPlc->fillSignal.Init(&pPlc->info, &bufferSamples);

		bufferSamples._samples = MusicPlcGetMuteFactorSamples(pPlc->frameSamples);
		bufferSamples._buf = (u8*)pPlc->basePorting->Malloc(bufferSamples._samples * pPlc->muteInfo._bytesPerSample);
		pPlc->muteFactor.Init(&pPlc->muteInfo, &bufferSamples, 14);

		pPlc->fillSignalSampleIndex = 0;
		pPlc->matchSamples = MUSIC_PLC_MATCH_MS_DEFAULT * pPlc->info._rate / 1000;
		pPlc->matchSamples = pPlc->matchSamples < pPlc->overlapInSamples ? pPlc->matchSamples : pPlc->overlapInSamples;
		pPlc->seekSamples = MUSIC_PLC_SEEK_MS_DEFAULT * pPlc->info._rate / 1000;
		
		pPlc->isInited = true;

		return MUSIC_PLC_RET_SUCCESS;
	}

	int32_t MusicPlc_Set(void* pMusicPlcStateIn, MusicPlc_SetChhoose_e choose, void** val)
	{
		//check
		if (!pMusicPlcStateIn
			|| choose >= MUSIC_PLC_SET_CHOOSE_MAX
			|| !val)
			return MUSIC_PLC_RET_FAIL;
		MusicPlcState* pMusicPlc = (MusicPlcState*)pMusicPlcStateIn;
		if (pMusicPlc->isInited == false)
			return MUSIC_PLC_RET_FAIL;

		switch (choose)
		{
#if 0
		case AUDIO_SPEED_CONTROL_SET_CHOOSE_SPEEDQ8:
			pState->speed = (f32)(u32)val[0] / (1 << 8);
#endif
		default:
			break;
		}
		return MUSIC_PLC_RET_SUCCESS;
	}

	int32_t MusicPlc_Get(void* pMusicPlcStateIn, MusicPlc_GetChhoose_e choose, void** val)
	{
		//check
		if (!pMusicPlcStateIn
			|| choose >= MUSIC_PLC_GET_CHOOSE_MAX
			|| !val)
			return MUSIC_PLC_RET_FAIL;
		MusicPlcState* pMusicPlc = (MusicPlcState*)pMusicPlcStateIn;
		if (pMusicPlc->isInited == false)
			return MUSIC_PLC_RET_FAIL;
		
		switch (choose)
		{
#if 0
		case AUDIO_SPEED_CONTROL_SET_CHOOSE_SPEEDQ8:
			pState->speed = (f32)(u32)val[0] / (1 << 8);
#endif
		default:
			break;
		}
		return MUSIC_PLC_RET_SUCCESS;
	}

	int32_t MusicPlc_Run(void* pMusicPlcStateIn, uint8_t* in, int32_t inLen, uint8_t* out, int32_t* outLen, bool isLost)
	{
		//check
		if (!pMusicPlcStateIn
			||!out
			||!outLen)
			return MUSIC_PLC_RET_FAIL;
		MusicPlcState* pMusicPlc = (MusicPlcState*)pMusicPlcStateIn;
		if (isLost == false) {
			if (!in
				|| inLen != (pMusicPlc->frameSamples * pMusicPlc->info._bytesPerSample))
				return MUSIC_PLC_RET_FAIL;
		}
		if (*outLen < pMusicPlc->frameSamples * pMusicPlc->info._bytesPerSample)
			return MUSIC_PLC_RET_FAIL;

		BufferSamples bufferSamples;

		AudioSamples pIn;
		bufferSamples._buf = in;
		bufferSamples._samples = pMusicPlc->frameSamples;
		pIn.Init(&((MusicPlcState*)pMusicPlcStateIn)->info, &bufferSamples);

		AudioSamples pOut;
		bufferSamples._buf = out;
		bufferSamples._samples = pMusicPlc->frameSamples;
		pOut.Init(&((MusicPlcState*)pMusicPlcStateIn)->info, &bufferSamples);

		MUTE_MODE muteMode = MUTE_MODE_NOMUTE;
		if (isLost == false)
		{
			if (pMusicPlc->lostCount == 0)
			{
				//infuture
				if ((pMusicPlc->decaySamplesNow - 1) < 0)
				{
					//quickly Deal
					pOut.Append(pMusicPlc->inHistory, pMusicPlc->inHistory.GetSamplesMax() - pMusicPlc->overlapInSamples, pMusicPlc->overlapInSamples);
					pOut.Append(pIn, 0, pMusicPlc->frameSamples - pMusicPlc->overlapInSamples);

					//shift
					pMusicPlc->inHistory.Clear(pMusicPlc->frameSamples);
					pMusicPlc->inHistory.Append(pIn, 0, pMusicPlc->frameSamples);
					*outLen = pOut.GetValidSamples() * pMusicPlc->info._bytesPerSample;
					return MUSIC_PLC_RET_SUCCESS;
				}
				else
					pMusicPlc->infuture.Append(pIn, 0, pMusicPlc->frameSamples);
			}
			else
			{
				pMusicPlc->lostCount = 0;
				//infuture
				for (i32 i = 0; i < pMusicPlc->overlapInSamples; i++)
				{
					pMusicPlc->infuture.Append(pMusicPlc->fillSignal, pMusicPlc->fillSignalSampleIndex, 1);
					pMusicPlc->fillSignalSampleIndex = (pMusicPlc->fillSignalSampleIndex + 1) % pMusicPlc->fillSignal.GetValidSamples();
				}
				pMusicPlc->asCalculator.OverlapAdd(pMusicPlc->infuture, 0, pIn, 0, pMusicPlc->overlapInSamples);
				pMusicPlc->infuture.Append(pIn, pMusicPlc->overlapInSamples, pMusicPlc->frameSamples - pMusicPlc->overlapInSamples);
			}

			//muteFactor
			muteMode = MUTE_MODE_DOMUTE;
			for (i32 i = 0; i < pMusicPlc->frameSamples; i++)
			{
				pMusicPlc->decaySamplesNow--;
				pMusicPlc->decaySamplesNow = MAX(pMusicPlc->decaySamplesNow, 0);
				pMusicPlc->asCalculator.AppendInFixPoint(pMusicPlc->muteFactor, pMusicPlc->decaySamples - pMusicPlc->decaySamplesNow, pMusicPlc->decaySamples);

			}
		}
		else
		{
			pMusicPlc->lostCount += 1;
			if (pMusicPlc->lostCount == 1)
			{
#if 1
				i32 bestLag = pMusicPlc->asCalculator.WaveFormMatch(
					AS_Calculator::WaveformMatchChoose_e::WAVEFORM_MATCH_SUM,
					pMusicPlc->inHistory,
					0,
					pMusicPlc->inHistory,
					pMusicPlc->inHistory.GetSamplesMax() - pMusicPlc->overlapInSamples,
					pMusicPlc->seekSamples,
					pMusicPlc->matchSamples);
#else		
				i32 bestLag = 0;
#endif
				//overlap add
				pMusicPlc->asCalculator.OverlapAdd(
					pMusicPlc->inHistory,
					pMusicPlc->inHistory.GetSamplesMax() - pMusicPlc->overlapInSamples,
					pMusicPlc->inHistory,
					bestLag,
					pMusicPlc->overlapInSamples);
				//fill signal
				pMusicPlc->fillSignal.Clear(pMusicPlc->fillSignal.GetValidSamples());
				pMusicPlc->fillSignal.Append(
					pMusicPlc->inHistory,
					bestLag + pMusicPlc->overlapInSamples,
					pMusicPlc->inHistory.GetSamplesMax() - (bestLag + pMusicPlc->overlapInSamples));
				pMusicPlc->fillSignalSampleIndex = 0;
			}
			//mute
			if ((pMusicPlc->decaySamplesNow + 1) <= pMusicPlc->decaySamples) {
				muteMode = MUTE_MODE_DOMUTE;
				for (i32 i = 0; i < pMusicPlc->frameSamples; i++) {
					pMusicPlc->decaySamplesNow++;
					pMusicPlc->decaySamplesNow = MIN(pMusicPlc->decaySamplesNow, pMusicPlc->decaySamples);
					pMusicPlc->asCalculator.AppendInFixPoint(pMusicPlc->muteFactor, pMusicPlc->decaySamples - pMusicPlc->decaySamplesNow, pMusicPlc->decaySamples);
				}
			}
			else {
				muteMode = MUTE_MODE_DOMUTE_ZERO;
			}
			//infuture
			for (i32 i = 0; i < pMusicPlc->infuture.GetSamplesMax(); i++) {
				pMusicPlc->infuture.Append(pMusicPlc->fillSignal, pMusicPlc->fillSignalSampleIndex, 1);
				pMusicPlc->fillSignalSampleIndex = (pMusicPlc->fillSignalSampleIndex + 1) % pMusicPlc->fillSignal.GetValidSamples();
			}
		}
		//muting
		if (muteMode != MUTE_MODE_NOMUTE) {
			if (muteMode == MUTE_MODE_DOMUTE) {
				pMusicPlc->asCalculator.Product(pMusicPlc->infuture, 0, pMusicPlc->muteFactor, 0, pMusicPlc->frameSamples);
			}
			else {
				pMusicPlc->basePorting->MemSet(&(pMusicPlc->infuture)[0], 0, pMusicPlc->infuture.GetSizeMax());
			}
		}
		//out
		pOut.Append(pMusicPlc->inHistory, pMusicPlc->inHistory.GetSamplesMax() - pMusicPlc->overlapInSamples, pMusicPlc->overlapInSamples);
		pOut.Append(pMusicPlc->infuture, 0, pMusicPlc->frameSamples - pMusicPlc->overlapInSamples);

		//shift
		pMusicPlc->inHistory.Clear(pMusicPlc->frameSamples);
		pMusicPlc->inHistory.Append(pMusicPlc->infuture, 0, pMusicPlc->frameSamples);
		pMusicPlc->infuture.Clear(pMusicPlc->infuture.GetValidSamples());

		*outLen = pOut.GetValidSamples() * pMusicPlc->info._bytesPerSample;
		pMusicPlc->muteFactor.Clear(pMusicPlc->muteFactor.GetValidSamples());
		return MUSIC_PLC_RET_SUCCESS;
	}

	int32_t MusicPlc_StateDeInit(void* pMusicPlcStateIn)
	{
		if (!pMusicPlcStateIn)
			return MUSIC_PLC_RET_FAIL;
		MusicPlcState* pMusicPlc = (MusicPlcState*)pMusicPlcStateIn;
		pMusicPlc->basePorting->Free(&pMusicPlc->inHistory[0]);
		pMusicPlc->basePorting->Free(&pMusicPlc->infuture[0]);
		pMusicPlc->basePorting->Free(&pMusicPlc->fillSignal[0]);
		pMusicPlc->basePorting->Free(&pMusicPlc->muteFactor[0]);
		return MUSIC_PLC_RET_SUCCESS;
	}
}

#endif