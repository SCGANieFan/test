#include<string.h>
#include"musicPlc16b.h"
#include"musicPlcComInner.h"

STATIC INLINE int32_t MusicPlcGetMuteFactorSamples(int32_t frameSamples)
{
	return frameSamples;
}

STATIC INLINE int32_t MusicPlcGetFillSignalSamples(int32_t frameSamples)
{
	return frameSamples;
}

STATIC INLINE int32_t MusicPlcGetHistorySamples(int32_t overlapInSamples, int32_t frameSamples)
{
	return frameSamples + overlapInSamples;
}

STATIC INLINE int32_t MusicPlcGetFutureSamples(int32_t frameSamples)
{
	return frameSamples;
}

STATIC INLINE int32_t MusicPlcGetBufSamples(int32_t overlapInSamples, int32_t frameSamplesInner, int32_t frameSamples)
{
	return MusicPlcGetHistorySamples(overlapInSamples, frameSamplesInner)
		+ MusicPlcGetFutureSamples(frameSamples)
		+ MusicPlcGetFillSignalSamples(frameSamplesInner);
}

STATIC INLINE bool MusicPlcCheckParam(int32_t overlapSamples, int32_t frameSamples, int16_t channels)
{
	if (overlapSamples < 1
		|| frameSamples < 1)
		return false;
	if (overlapSamples > frameSamples)
		return false;
	if (channels != 1 && channels != 2 && channels != 4)
		return false;
	return true;
}


STATIC int32_t MusicPlcStateInitInner(void* pMusicPlcStateIn, int32_t overlapInSamples,
	int32_t decayTimeSamples, MusicPlcSampleParam* sampleParam, int16_t width)
{
	MusicPlcState* pPlc = (MusicPlcState*)pMusicPlcStateIn;

	pPlc->fsHz = sampleParam->fsHzMax;
	pPlc->width = width;
	pPlc->channels = sampleParam->channels;

	pPlc->overlapInSamples = overlapInSamples;
	pPlc->decaySamples = decayTimeSamples;

	pPlc->frameSamples = sampleParam->frameSamples;
	pPlc->frameSamplesInner = MAX(pPlc->frameSamples, pPlc->fsHz * MUSIC_PLC_MIN_FRAME_MS / 1000);

	uint8_t* ptr = (uint8_t*)pPlc + sizeof(MusicPlcState);

	pPlc->inHistory.Init(
		ptr,
		pPlc->channels,
		MusicPlcGetHistorySamples(pPlc->overlapInSamples, pPlc->frameSamplesInner));

	pPlc->inHistory.Append(&(pPlc->inHistory)[0], pPlc->inHistory.GetSamples());
	pPlc->inHistory.used(pPlc->inHistory.GetSamples() - pPlc->overlapInSamples);

	pPlc->infuture.Init(
		&(pPlc->inHistory)[pPlc->inHistory.GetSamples()],
		pPlc->channels,
		MusicPlcGetFutureSamples(pPlc->frameSamples));

	pPlc->fillSignal.Init(
		&(pPlc->infuture)[pPlc->infuture.GetSamples()],
		pPlc->channels,
		MusicPlcGetFillSignalSamples(pPlc->frameSamplesInner));
	pPlc->fillSignalSampleIndex = 0;
	pPlc->muteFactor.Init(
		&(pPlc->fillSignal)[pPlc->fillSignal.GetSamples()],
		1,
		pPlc->decaySamples,
		14);
	for (int32_t i = 0; i < pPlc->decaySamples; i++)
	{
		pPlc->muteFactor.AppendInFixPoint(pPlc->decaySamples - i, pPlc->decaySamples);//1 --> 0
	}
	pPlc->matchSamples = MUSIC_PLC_MATCH_MS_DEFAULT * pPlc->fsHz / 1000;
	pPlc->matchSamples = pPlc->matchSamples < pPlc->overlapInSamples ? pPlc->matchSamples : pPlc->overlapInSamples;

	pPlc->seekSamples = MUSIC_PLC_SEEK_MS_DEFAULT * pPlc->fsHz / 1000;

	return MUSIC_PLC_RET_SUCCESS;
}

STATIC int32_t MusicPlcRun(MusicPlcState* pMusicPlc, AudioSamples16* pIn, int32_t* inUsed, AudioSamples16* pOut, int32_t* outLen, bool isLost)
{
	MUTE_MODE muteMode = MUTE_MODE_DOMUTE;
	if (isLost == false)
	{
		if (inUsed)
			*inUsed = pMusicPlc->frameSamples * pMusicPlc->channels * pMusicPlc->width;

		if (pMusicPlc->lostCount > 0)
		{
			pMusicPlc->lostCount = 0;
			//infuture
			for (int32_t i = 0; i < pMusicPlc->overlapInSamples; i++)
			{
				pMusicPlc->infuture.Append(&(pMusicPlc->fillSignal)[pMusicPlc->fillSignalSampleIndex], 1);
				pMusicPlc->fillSignalSampleIndex = (pMusicPlc->fillSignalSampleIndex + 1) % pMusicPlc->fillSignal.GetValidSamples();
			}
			pMusicPlc->infuture.OverlapAdd(&(*pIn)[0], pMusicPlc->overlapInSamples);
			pMusicPlc->infuture.Append(&(*pIn)[pMusicPlc->overlapInSamples], pMusicPlc->frameSamples - pMusicPlc->overlapInSamples);
			pMusicPlc->infuture.Append(&(*pIn)[0], pMusicPlc->frameSamples);
		}
		else
		{
			//infuture
			if ((pMusicPlc->decaySamplesNow - 1) < 0)
			{
				pMusicPlc->inHistory.Move(-pMusicPlc->frameSamples);
				pMusicPlc->inHistory.Append(&(*pIn)[0], pMusicPlc->frameSamples);
				pOut->Append(&(pMusicPlc->inHistory)[pMusicPlc->inHistory.GetSamples() - pMusicPlc->overlapInSamples - pMusicPlc->frameSamples], pMusicPlc->frameSamples);
				pMusicPlc->inHistory.used(pMusicPlc->frameSamples);
				if (outLen)
					*outLen = pMusicPlc->frameSamples * pMusicPlc->channels * pMusicPlc->width;
				return MUSIC_PLC_RET_SUCCESS;
			}
			else
				pMusicPlc->infuture.Append(&(*pIn)[0], pMusicPlc->frameSamples);
		}
		//muteFactor
		if ((pMusicPlc->decaySamplesNow - 1) < 0)
		{
			muteMode = MUTE_MODE_NOMUTE;
		}
		pMusicPlc->decayDir = -1;
	}
	else
	{
		pMusicPlc->lostCount += 1;
		if (pMusicPlc->lostCount == 1)
		{
			pMusicPlc->decaySamplesNow = 0;
#if 0
			int32_t bestLag = pMusicPlc->inHistory.WaveFormMatch(
				&(pMusicPlc->inHistory)[pMusicPlc->inHistory.GetSamples() - pMusicPlc->overlapInSamples],
				pMusicPlc->seekSamples,
				pMusicPlc->matchSamples);
			bestLag += 0;
#else		
			int32_t bestLag = 0;
#endif
			//overlap add
			pMusicPlc->inHistory.OverlapAdd(
				&(pMusicPlc->inHistory)[bestLag],
				pMusicPlc->overlapInSamples);

			//fill signal
			pMusicPlc->fillSignal.Clear();
			pMusicPlc->fillSignal.Append(
				&(pMusicPlc->inHistory)[bestLag + pMusicPlc->overlapInSamples],
				pMusicPlc->inHistory.GetSamples() - (bestLag + pMusicPlc->overlapInSamples));
			pMusicPlc->fillSignalSampleIndex = 0;
		}
		//mute
		if ((pMusicPlc->decaySamplesNow + 1) > pMusicPlc->decaySamples)
		{
			muteMode = MUTE_MODE_DOMUTE_ZERO;
		}
		pMusicPlc->decayDir = 1;
		//infuture
		for (int32_t i = 0; i < pMusicPlc->infuture.GetSamples(); i++) {
			pMusicPlc->infuture.Append(&(pMusicPlc->fillSignal)[pMusicPlc->fillSignalSampleIndex], 1);
			pMusicPlc->fillSignalSampleIndex = (pMusicPlc->fillSignalSampleIndex + 1) % pMusicPlc->fillSignal.GetValidSamples();
		}
	}

	//muting
	if (muteMode == MUTE_MODE_DOMUTE)
	{
		pMusicPlc->infuture.ProductWithEverySamlpe(pMusicPlc->muteFactor, pMusicPlc->decaySamplesNow, pMusicPlc->decayDir, pMusicPlc->frameSamples);
		if (pMusicPlc->decayDir == 1)
		{
			pMusicPlc->decaySamplesNow += pMusicPlc->frameSamples;
			pMusicPlc->decaySamplesNow = MIN(pMusicPlc->decaySamplesNow, pMusicPlc->decaySamples);
		}
		else
		{
			pMusicPlc->decaySamplesNow -= pMusicPlc->frameSamples;
			pMusicPlc->decaySamplesNow = MAX(pMusicPlc->decaySamplesNow, 0);
		}
	}
	else if (muteMode == MUTE_MODE_DOMUTE_ZERO)
	{
		memset(&(pMusicPlc->infuture)[0], 0, pMusicPlc->infuture.GetSizeInByte());
	}

	//out
	if (outLen)
		*outLen = pMusicPlc->frameSamples * pMusicPlc->channels * pMusicPlc->width;
	pOut->Append(&(pMusicPlc->inHistory)[pMusicPlc->inHistory.GetSamples() - pMusicPlc->overlapInSamples], pMusicPlc->overlapInSamples);
	pOut->Append(&(pMusicPlc->infuture)[0], pMusicPlc->frameSamples - pMusicPlc->overlapInSamples);
	pMusicPlc->inHistory.Move(-pMusicPlc->frameSamples);
	pMusicPlc->inHistory.Append(&(pMusicPlc->infuture)[0], pMusicPlc->frameSamples);
	pMusicPlc->inHistory.used(pMusicPlc->frameSamples);
	pMusicPlc->infuture.Clear();
	return MUSIC_PLC_RET_SUCCESS;
}






