#include<string.h>
#include"musicPlcCom.h"
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

STATIC INLINE bool MusicPlcCheckParam(int32_t overlapMs, int32_t frameSamples, int32_t fsHz, int16_t channels)
{
	if (overlapMs < 1
		|| frameSamples < 1)
		return false;
	int32_t overlapInSamples = overlapMs * fsHz / 1000;
	if (overlapInSamples > frameSamples)
		return false;
	if (channels != 1 && channels != 2 && channels != 4)
		return false;
	return true;
}


STATIC int32_t MusicPlcStateInitInner(void* pMusicPlcStateIn, int32_t overlapInSamples,
	int32_t decayTimeMs, MusicPlcSampleParam* sampleParam, int16_t width)
{
	MusicPlcState* pPlc = (MusicPlcState*)pMusicPlcStateIn;

	pPlc->fsHz = sampleParam->fsHz;
	pPlc->width = width;
	pPlc->channels = sampleParam->channels;

	pPlc->overlapInSamples = overlapInSamples;
	pPlc->decaySamples = decayTimeMs * sampleParam->fsHz / 1000;

	pPlc->frameSamples = sampleParam->frameSamples;
	pPlc->frameSamplesInner = MAX(pPlc->frameSamples, pPlc->fsHz * MUSIC_PLC_MIN_FRAME_MS / 1000);

	pPlc->inHistory->Init(
		(uint8_t*)(pPlc->muteFactor) + ((uint8_t*)pPlc->muteFactor - (uint8_t*)pPlc->fillSignal),
		pPlc->channels,
		MusicPlcGetHistorySamples(pPlc->overlapInSamples, pPlc->frameSamplesInner));

	pPlc->inHistory->Append(&(*pPlc->inHistory)[0], pPlc->inHistory->GetSamples());
	pPlc->inHistory->used(pPlc->inHistory->GetSamples() - pPlc->overlapInSamples);

	pPlc->infuture->Init(
		&(*pPlc->inHistory)[pPlc->inHistory->GetSamples()],
		pPlc->channels,
		MusicPlcGetFutureSamples(pPlc->frameSamples));

	pPlc->fillSignal->Init(
		&(*pPlc->infuture)[pPlc->infuture->GetSamples()],
		pPlc->channels,
		MusicPlcGetFillSignalSamples(pPlc->frameSamplesInner));
	pPlc->fillSignalSampleIndex = 0;
	pPlc->muteFactor->Init(
		&(*pPlc->fillSignal)[pPlc->fillSignal->GetSamples()],
		1,
		MusicPlcGetMuteFactorSamples(pPlc->frameSamples),
		14);

	pPlc->matchSamples = MUSIC_PLC_MATCH_MS_DEFAULT * pPlc->fsHz / 1000;
	pPlc->matchSamples = pPlc->matchSamples < pPlc->overlapInSamples ? pPlc->matchSamples : pPlc->overlapInSamples;

	pPlc->seekSamples = MUSIC_PLC_SEEK_MS_DEFAULT * pPlc->fsHz / 1000;

	return MUSIC_PLC_RET_SUCCESS;
}

STATIC int32_t MusicPlcRun(MusicPlcState* pMusicPlc, AudioSamples* pIn, int32_t* inUsed, AudioSamples* pOut, int32_t* outLen, bool isLost)
{
	MUTE_MODE muteMode = MUTE_MODE_DOMUTE;
	if (isLost == true)
	{
		pMusicPlc->lostCount += 1;
		if (pMusicPlc->lostCount == 1)
		{
			if(pMusicPlc->is24bL)
				pMusicPlc->inHistory->LeftShift(8, false);
#if 1
			int32_t bestLag = pMusicPlc->inHistory->WaveFormMatch(
				&(*pMusicPlc->inHistory)[pMusicPlc->inHistory->GetSamples() - pMusicPlc->overlapInSamples],
				pMusicPlc->seekSamples,
				pMusicPlc->matchSamples);
			bestLag += 1;
#else		
			int32_t bestLag = 1;
#endif
			//overlap add
			pMusicPlc->inHistory->OverlapAdd(
				&(*pMusicPlc->inHistory)[bestLag],
				pMusicPlc->overlapInSamples);

			if (pMusicPlc->is24bL)
				pMusicPlc->inHistory->RightShift(8, false);

			//fill signal
			pMusicPlc->fillSignal->Clear();
			pMusicPlc->fillSignal->Append(
				&(*pMusicPlc->inHistory)[bestLag + pMusicPlc->overlapInSamples],
				pMusicPlc->inHistory->GetSamples() - (bestLag + pMusicPlc->overlapInSamples));
			pMusicPlc->fillSignalSampleIndex = 0;
		}
		//mute
		if ((pMusicPlc->decaySamplesNow + 1) > pMusicPlc->decaySamples)
		{
			muteMode = MUTE_MODE_DOMUTE_ZERO;
		}
		else
		{
			for (int32_t i = 0; i < pMusicPlc->frameSamples; i++) {
				pMusicPlc->decaySamplesNow++;
				pMusicPlc->decaySamplesNow = MIN(pMusicPlc->decaySamplesNow, pMusicPlc->decaySamples);
				pMusicPlc->muteFactor->AppendInFixPoint(pMusicPlc->decaySamples - pMusicPlc->decaySamplesNow, pMusicPlc->decaySamples);
			}
		}

		//infuture
		for (int32_t i = 0; i < pMusicPlc->infuture->GetSamples(); i++) {
			pMusicPlc->infuture->Append(&(*pMusicPlc->fillSignal)[pMusicPlc->fillSignalSampleIndex], 1);
			pMusicPlc->fillSignalSampleIndex = (pMusicPlc->fillSignalSampleIndex + 1) % pMusicPlc->fillSignal->GetValidSamples();
		}
	}
	else
	{
		if (inUsed)
			*inUsed = pMusicPlc->frameSamples * pMusicPlc->channels * pMusicPlc->width;

		if (pMusicPlc->lostCount > 0)
		{
			pMusicPlc->lostCount = 0;
			//infuture
			for (int32_t i = 0; i < pMusicPlc->overlapInSamples; i++)
			{
				pMusicPlc->infuture->Append(&(*pMusicPlc->fillSignal)[pMusicPlc->fillSignalSampleIndex], 1);
				pMusicPlc->fillSignalSampleIndex = (pMusicPlc->fillSignalSampleIndex + 1) % pMusicPlc->fillSignal->GetValidSamples();
			}
			pMusicPlc->infuture->OverlapAdd(&(*pIn)[0], pMusicPlc->overlapInSamples);
			pMusicPlc->infuture->Append(&(*pIn)[pMusicPlc->overlapInSamples], pMusicPlc->frameSamples - pMusicPlc->overlapInSamples);
			pMusicPlc->infuture->Append(&(*pIn)[0], pMusicPlc->frameSamples);
		}
		else
		{
			//infuture
			pMusicPlc->infuture->Append(&(*pIn)[0], pMusicPlc->frameSamples);
		}
		//muteFactor
		if ((pMusicPlc->decaySamplesNow - 1) < 0)
		{
			muteMode = MUTE_MODE_NOMUTE;
		}
		else
		{
			for (int32_t i = 0; i < pMusicPlc->frameSamples; i++)
			{
				pMusicPlc->decaySamplesNow--;
				pMusicPlc->decaySamplesNow = MAX(pMusicPlc->decaySamplesNow, 0);
				pMusicPlc->muteFactor->AppendInFixPoint(pMusicPlc->decaySamples - pMusicPlc->decaySamplesNow, pMusicPlc->decaySamples);
			}
		}
	}
	//muting
	if (muteMode == MUTE_MODE_DOMUTE)
	{
		if (pMusicPlc->is24bL)
			pMusicPlc->infuture->LeftShift(8, false);
		pMusicPlc->infuture->ProductWithEverySamlpe(*pMusicPlc->muteFactor, pMusicPlc->frameSamples);
		if (pMusicPlc->is24bL)
			pMusicPlc->infuture->RightShift(8, false);
	}
	else if (muteMode == MUTE_MODE_DOMUTE_ZERO)
	{
		memset(&(*pMusicPlc->infuture)[0], 0, pMusicPlc->infuture->GetSizeInByte());
	}

	//out
	if (outLen)
		*outLen = pMusicPlc->frameSamples * pMusicPlc->channels * pMusicPlc->width;
	pOut->Append(&(*pMusicPlc->inHistory)[pMusicPlc->inHistory->GetSamples() - pMusicPlc->overlapInSamples], pMusicPlc->overlapInSamples);
	pOut->Append(&(*pMusicPlc->infuture)[0], pMusicPlc->frameSamples - pMusicPlc->overlapInSamples);

	//shift
	pMusicPlc->inHistory->Move(-pMusicPlc->frameSamples);
	pMusicPlc->inHistory->Append(&(*pMusicPlc->infuture)[0], pMusicPlc->frameSamples);
	pMusicPlc->inHistory->used(pMusicPlc->frameSamples);
	pMusicPlc->infuture->Clear();

	//
	pMusicPlc->muteFactor->Clear();
	return MUSIC_PLC_RET_SUCCESS;
}






