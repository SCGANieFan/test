#if 1
#include"musicPlcCom.h"
#include"musicPlcComInner.h"

#if 1
#include"Algo.Printer.h"
#include"Algo.Memory.h"

void* MusicplcMalloc(i32 size) {
#if 1
	static i32 sizeTotal = 0;
	sizeTotal += size;
	void *ptr= ALGO_MALLOC(size);
	ALGO_PRINT("malloc, ptr:%x, size:%d, sizeTotal:%d,", (u32)ptr, size, sizeTotal);
	return ptr;
#else
	return ALGO_MALLOC(size);
#endif
}

void MusicplcFree(void* ptr) {
#if 1
	ALGO_PRINT("free, ptr:%x", (u32)ptr);
	return ALGO_FREE(ptr);
#else
	return ALGO_FREE(ptr);
#endif
}
#endif

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

b1 MusicPlcCheckParam(i32 overlapMs, i32 frameSamples, i32 fsHz, i16 channels)
{
	if (overlapMs < 1
		|| frameSamples < 1)
		return false;
	i32 overlapInSamples = overlapMs * fsHz / 1000;
	if (overlapInSamples > frameSamples)
		return false;
	if (channels != 1 && channels != 2 && channels != 4)
		return false;
	return true;
}


i32 MusicPlcStateInitInner(MusicPlcState* pMusicPlc, i32 overlapInSamples, i32 decayTimeMs)
{
	MusicPlcState* pPlc = (MusicPlcState*)pMusicPlc;
	pPlc->overlapInSamples = overlapInSamples;
	pPlc->decaySamples = decayTimeMs * pPlc->info._rate / 1000;
	
	pPlc->frameSamplesInner = MAX(pPlc->frameSamples, pPlc->info._rate * MUSIC_PLC_MIN_FRAME_MS / 1000);

	BufferSamples bufferSamples;

	bufferSamples._samples = MusicPlcGetHistorySamples(pPlc->overlapInSamples, pPlc->frameSamplesInner);
	bufferSamples._buf= (u8*)MusicplcMalloc(bufferSamples._samples * pPlc->info._bytesPerSample);
	pPlc->inHistory.Init(&pPlc->info, &bufferSamples);
	pPlc->inHistory.Append(pPlc->inHistory, 0, pPlc->inHistory.GetSamplesMax());

	bufferSamples._samples = MusicPlcGetFutureSamples(pPlc->frameSamples);
	bufferSamples._buf = (u8*)MusicplcMalloc(bufferSamples._samples * pPlc->info._bytesPerSample);
	pPlc->infuture.Init(&pPlc->info, &bufferSamples);

	bufferSamples._samples = MusicPlcGetFillSignalSamples(pPlc->frameSamplesInner);
	bufferSamples._buf = (u8*)MusicplcMalloc(bufferSamples._samples * pPlc->info._bytesPerSample);
	pPlc->fillSignal.Init(&pPlc->info, &bufferSamples);

	pPlc->muteInfo.Init(pPlc->info._rate, pPlc->info._width, 1);
	bufferSamples._samples = MusicPlcGetMuteFactorSamples(pPlc->frameSamples);
	bufferSamples._buf = (u8*)MusicplcMalloc(bufferSamples._samples * pPlc->muteInfo._bytesPerSample);
	pPlc->muteFactor.Init(&pPlc->muteInfo, &bufferSamples, 14);

	pPlc->fillSignalSampleIndex = 0;
	pPlc->matchSamples = MUSIC_PLC_MATCH_MS_DEFAULT * pPlc->info._rate / 1000;
	pPlc->matchSamples = pPlc->matchSamples < pPlc->overlapInSamples ? pPlc->matchSamples : pPlc->overlapInSamples;
	pPlc->seekSamples = MUSIC_PLC_SEEK_MS_DEFAULT * pPlc->info._rate / 1000;

	return MUSIC_PLC_RET_SUCCESS;
}

i32 MusicPlcRun(MusicPlcState* pMusicPlc, AudioSamples* pIn, i32* inUsed, AudioSamples* pOut, i32* outLen, b1 isLost)
{
	MUTE_MODE muteMode = MUTE_MODE_NOMUTE;
	if (isLost == true)
	{
		pMusicPlc->lostCount += 1;
		if (pMusicPlc->lostCount == 1)
		{
#if 1
			i32 bestLag = pMusicPlc->asCalculator->WaveFormMatch(
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
			pMusicPlc->asCalculator->OverlapAdd(
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
		if ((pMusicPlc->decaySamplesNow + 1) > pMusicPlc->decaySamples)
		{
			muteMode = MUTE_MODE_DOMUTE_ZERO;
		}
		else
		{
			muteMode = MUTE_MODE_DOMUTE;
			for (i32 i = 0; i < pMusicPlc->frameSamples; i++) {
				pMusicPlc->decaySamplesNow++;
				pMusicPlc->decaySamplesNow = MIN(pMusicPlc->decaySamplesNow, pMusicPlc->decaySamples);
				pMusicPlc->asCalculator->AppendInFixPoint(pMusicPlc->muteFactor, pMusicPlc->decaySamples - pMusicPlc->decaySamplesNow, pMusicPlc->decaySamples);
			}
		}

		//infuture
		for (i32 i = 0; i < pMusicPlc->infuture.GetSamplesMax(); i++) {
			pMusicPlc->infuture.Append(pMusicPlc->fillSignal, pMusicPlc->fillSignalSampleIndex, 1);
			pMusicPlc->fillSignalSampleIndex = (pMusicPlc->fillSignalSampleIndex + 1) % pMusicPlc->fillSignal.GetValidSamples();
		}
	}
	else
	{
		if (inUsed)
			*inUsed = pMusicPlc->frameSamples * pMusicPlc->info._bytesPerSample;
		if (pMusicPlc->lostCount > 0)
		{
			pMusicPlc->lostCount = 0;
			//infuture
			for (i32 i = 0; i < pMusicPlc->overlapInSamples; i++)
			{
				pMusicPlc->infuture.Append(pMusicPlc->fillSignal, pMusicPlc->fillSignalSampleIndex, 1);
				pMusicPlc->fillSignalSampleIndex = (pMusicPlc->fillSignalSampleIndex + 1) % pMusicPlc->fillSignal.GetValidSamples();
			}
			pMusicPlc->asCalculator->OverlapAdd(pMusicPlc->infuture, 0, *pIn, 0, pMusicPlc->overlapInSamples);
			pMusicPlc->infuture.Append(*pIn, pMusicPlc->overlapInSamples, pMusicPlc->frameSamples - pMusicPlc->overlapInSamples);			
		}
		else
		{
			//infuture
			if ((pMusicPlc->decaySamplesNow - 1) < 0)
				pMusicPlc->quickDeal = true;
			else
				pMusicPlc->infuture.Append(*pIn, 0, pMusicPlc->frameSamples);
		}

		//muteFactor
		if ((pMusicPlc->decaySamplesNow - 1) >= 0)
		{
			muteMode = MUTE_MODE_DOMUTE;
			for (i32 i = 0; i < pMusicPlc->frameSamples; i++)
			{
				pMusicPlc->decaySamplesNow--;
				pMusicPlc->decaySamplesNow = MAX(pMusicPlc->decaySamplesNow, 0);
				pMusicPlc->asCalculator->AppendInFixPoint(pMusicPlc->muteFactor, pMusicPlc->decaySamples - pMusicPlc->decaySamplesNow, pMusicPlc->decaySamples);

			}
		}
	}
	//muting
	if (muteMode != MUTE_MODE_NOMUTE) {
		if (muteMode == MUTE_MODE_DOMUTE) {
			pMusicPlc->asCalculator->Product(pMusicPlc->infuture, 0, pMusicPlc->muteFactor, 0, pMusicPlc->frameSamples);
		}
		else {
			memset(&(pMusicPlc->infuture)[0], 0, pMusicPlc->infuture.GetSizeMax());
		}
	}
	//out
	if (outLen)
		*outLen = pMusicPlc->frameSamples * pMusicPlc->info._bytesPerSample;
	if (pMusicPlc->quickDeal)
	{
		pOut->Append(pMusicPlc->inHistory,pMusicPlc->inHistory.GetSamplesMax() - pMusicPlc->overlapInSamples, pMusicPlc->overlapInSamples);
		pOut->Append(*pIn, 0, pMusicPlc->frameSamples - pMusicPlc->overlapInSamples);

		//shift
		pMusicPlc->inHistory.Clear(pMusicPlc->frameSamples);
		pMusicPlc->inHistory.Append(*pIn, 0, pMusicPlc->frameSamples);
		pMusicPlc->quickDeal = false;
	}
	else
	{
		pOut->Append(pMusicPlc->inHistory,pMusicPlc->inHistory.GetSamplesMax() - pMusicPlc->overlapInSamples, pMusicPlc->overlapInSamples);
		pOut->Append(pMusicPlc->infuture, 0, pMusicPlc->frameSamples - pMusicPlc->overlapInSamples);

		//shift
		pMusicPlc->inHistory.Clear(pMusicPlc->frameSamples);
		pMusicPlc->inHistory.Append(pMusicPlc->infuture, 0, pMusicPlc->frameSamples);
		pMusicPlc->infuture.Clear(pMusicPlc->infuture.GetValidSamples());
	}
	pMusicPlc->muteFactor.Clear(pMusicPlc->muteFactor.GetValidSamples());
	return MUSIC_PLC_RET_SUCCESS;
}


i32 MusicPlcStateDeInitInner(MusicPlcState* pMusicPlc)
{
	pMusicPlc->asCalculator->~AS_Calculator();
	MusicplcFree(pMusicPlc->asCalculator);

	MusicplcFree(&pMusicPlc->inHistory[0]);
	pMusicPlc->inHistory.~AudioSamples();

	MusicplcFree(&pMusicPlc->infuture[0]);
	pMusicPlc->infuture.~AudioSamples();

	MusicplcFree(&pMusicPlc->fillSignal[0]);
	pMusicPlc->fillSignal.~AudioSamples();

	MusicplcFree(&pMusicPlc->muteFactor[0]);
	pMusicPlc->muteFactor.~AudioSamples();
	return MUSIC_PLC_RET_SUCCESS;
}


#endif