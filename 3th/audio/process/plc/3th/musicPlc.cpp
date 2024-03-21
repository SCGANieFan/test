#include<stdio.h>
#include<stdint.h>
#include<string.h>

#include"AudioSample.h"
#include"musicPlc.h"


#if 1
#define MUSIC_PLC_MIN_FRAME_MS 10
#define MUSIC_PLC_SEEK_MS_DEFAULT 2
#define MUSIC_PLC_MATCH_MS_DEFAULT 2

#define MAX(x1,x2) ((x1)>(x2)?(x1):(x2))
#define MIN(x1,x2) ((x1)<(x2)?(x1):(x2))

using namespace PlcCom;

typedef struct
{
	int32_t fsHz;
	MusicPlcSampleStoreFormat sampleFormat;
	int16_t channels;

	int32_t lostCount;
	int32_t overlapInSamples;
	int32_t frameSamples;
	int32_t frameSamplesInner;
	int32_t decaySamples;

	AudioSample inHistory;
	AudioSample infuture;
	AudioSample fillSignal;
	AudioSample muteFactor;

	int32_t decaySamplesNow;
	int32_t fillSignalSampleIndex;
	int32_t matchSamples;
	int32_t seekSamples;

}MusicPlcState;

static inline int32_t MusicPlcGetMuteFactorSamples(int32_t frameSamples)
{
	return frameSamples;
}

static inline int32_t MusicPlcGetFillSignalSamples(int32_t frameSamples)
{
	return frameSamples;
}

static inline int32_t MusicPlcGetHistorySamples(int32_t overlapInSamples, int32_t frameSamples)
{
	return frameSamples + overlapInSamples;
}

static inline int32_t MusicPlcGetFutureSamples(int32_t frameSamples)
{
	return frameSamples;
}

static inline int32_t MusicPlcGetBufSamples(int32_t overlapInSamples, int32_t frameSamplesInner, int32_t frameSamples)
{
	return MusicPlcGetHistorySamples(overlapInSamples, frameSamplesInner)
		+ MusicPlcGetFutureSamples(frameSamples)
		+ MusicPlcGetFillSignalSamples(frameSamplesInner);
}

static inline bool MusicPlcCheckParam(int32_t overlapMs, int32_t frameSamples, int32_t fsHz, int16_t channels, int16_t sampleWidth)
{
	if (overlapMs < 1
		|| frameSamples < 1)
		return false;
	int32_t overlapInSamples = overlapMs * fsHz / 1000;
	if (overlapInSamples > frameSamples)
		return false;
	if (sampleWidth != 2 && sampleWidth != 4)
		return false;
	if (channels != 1 && channels != 2 && channels != 4)
		return false;
	return true;
}

int16_t WaveFormMatch(AudioSample& templ, int32_t templSample, AudioSample& comp, int32_t compSample, int32_t seekSample, int32_t matchSample)
{
#if 0
	if (!(&dst)
		|| !(&src1)
		|| !(&src2))
		return;
	if (dstSample < 0
		|| src1Sample < 0
		|| src2Sample < 0)
		return;
	if (overlapSample < 1)
		return;
#endif

	int16_t width = templ.GetWidth();
	int16_t channels = templ.GetChannels();

	int32_t matchFactor = 0;
	int32_t matchFactorOpt = 0;
	int16_t lagOpt = 0;

	switch (width)
	{
	case 2:
	{
		int16_t* pTempl = (int16_t*)(&templ[templSample]);
		int16_t* pComp = (int16_t*)(&comp[compSample]);

		for (int16_t s = 0; s < seekSample; s++) {
			matchFactor = 0;
			for (int16_t m = 0; m < matchSample; m++) {
				for (int16_t ch = 0; ch < channels; ch++) {
					matchFactor += ((int32_t)pTempl[m * channels + ch] * pComp[(s + m) * channels + ch]) >> 8;
				}
			}
			if (matchFactorOpt < matchFactor)
			{
				matchFactorOpt = matchFactor;
				lagOpt = s;
			}
		}
		break;
	}
	case 4:
	{
		int32_t* pTempl = (int32_t*)(&templ[templSample]);
		int32_t* pComp = (int32_t*)(&comp[compSample]);
		for (int16_t s = 0; s < seekSample; s++) {
			matchFactor = 0;
			for (int16_t m = 0; m < matchSample; m++) {
				for (int16_t ch = 0; ch < channels; ch++) {
					matchFactor += ((int32_t)((int16_t)(pTempl[m * channels + ch] >> 16)) * (int16_t)(pComp[(s + m) * channels + ch] >> 16)) >> 8;
				}
			}
			if (matchFactorOpt < matchFactor)
			{
				matchFactorOpt = matchFactor;
				lagOpt = s;
			}
		}
		break;
	}
	}
	return lagOpt;
}

int32_t MusicPlcRun(MusicPlcState* pMusicPlc, int8_t* in, int32_t inLen, int32_t* inUsed, int8_t* out, int32_t* outLen, bool isLost)
{
	MusicPlcSampleStoreFormat* sampleFormat = &pMusicPlc->sampleFormat;
	AudioSample pIn;
	pIn.Init((uint8_t*)in, sampleFormat->storedWidthPerSample,
		pMusicPlc->channels, inLen / pMusicPlc->channels / sampleFormat->storedWidthPerSample);
	AudioSample pOut;
	pOut.Init((uint8_t*)out, sampleFormat->storedWidthPerSample, pMusicPlc->channels, pMusicPlc->frameSamples);

	//
	if (sampleFormat->validWidthPerSample < sampleFormat->storedWidthPerSample)
	{
		int16_t shiftBits = (sampleFormat->storedWidthPerSample - sampleFormat->validWidthPerSample) << 3;
		if (sampleFormat->alignMode == MusicPlcSampleHighByteAlign)
		{
			//...
		}
		else
		{
			pIn.LeftShift(shiftBits, false);
		}
	}
	if (isLost == true)
	{
		pMusicPlc->lostCount += 1;
		if (pMusicPlc->lostCount == 1)
		{
#if 1
			int32_t bestLag = WaveFormMatch(
				pMusicPlc->inHistory, pMusicPlc->inHistory.GetSamples() - pMusicPlc->overlapInSamples,
				pMusicPlc->inHistory, 0,
				pMusicPlc->seekSamples, pMusicPlc->matchSamples);
#else
			int32_t bestLag = 0;
#endif
			//overlap add
			pMusicPlc->inHistory.OverlapAdd(
				&pMusicPlc->inHistory[bestLag],
				pMusicPlc->overlapInSamples);

			//fill signal
			pMusicPlc->fillSignal.Clear();
			pMusicPlc->fillSignal.Append(
				&pMusicPlc->inHistory[bestLag + pMusicPlc->overlapInSamples],
				pMusicPlc->inHistory.GetSamples() - (bestLag + pMusicPlc->overlapInSamples));
			pMusicPlc->fillSignalSampleIndex = 0;
		}
		//mute
		for (int32_t i = 0; i < pMusicPlc->frameSamples; i++) {
			pMusicPlc->decaySamplesNow++;
			pMusicPlc->decaySamplesNow = MIN(pMusicPlc->decaySamplesNow, pMusicPlc->decaySamples);
			pMusicPlc->muteFactor.AppendInFixPoint(pMusicPlc->decaySamples - pMusicPlc->decaySamplesNow, pMusicPlc->decaySamples);
		}

		//infuture
		for (int32_t i = 0; i < pMusicPlc->infuture.GetSamples(); i++) {
			pMusicPlc->infuture.Append(&pMusicPlc->fillSignal[pMusicPlc->fillSignalSampleIndex], 1);
			pMusicPlc->fillSignalSampleIndex = (pMusicPlc->fillSignalSampleIndex + 1) % pMusicPlc->fillSignal.GetValidSamples();
		}
	}
	else
	{
		if (inUsed)
			*inUsed = pMusicPlc->frameSamples * pMusicPlc->channels * pMusicPlc->sampleFormat.storedWidthPerSample;

		if (pMusicPlc->lostCount > 0)
		{
			pMusicPlc->lostCount = 0;
			//infuture
			for (int32_t i = 0; i < pMusicPlc->overlapInSamples; i++)
			{
				pMusicPlc->infuture.Append(&pMusicPlc->fillSignal[pMusicPlc->fillSignalSampleIndex], 1);
				pMusicPlc->fillSignalSampleIndex = (pMusicPlc->fillSignalSampleIndex + 1) % pMusicPlc->fillSignal.GetValidSamples();
			}
			pMusicPlc->infuture.OverlapAdd(&pIn[0], pMusicPlc->overlapInSamples);
			pMusicPlc->infuture.Append(&pIn[pMusicPlc->overlapInSamples], pMusicPlc->frameSamples - pMusicPlc->overlapInSamples);
		}
		else
		{
			//infuture
			pMusicPlc->infuture.Append(&pIn[0], pMusicPlc->frameSamples);
		}
		//muteFactor
		for (int32_t i = 0; i < pMusicPlc->frameSamples; i++)
		{
			pMusicPlc->decaySamplesNow--;
			pMusicPlc->decaySamplesNow = MAX(pMusicPlc->decaySamplesNow, 0);
			pMusicPlc->muteFactor.AppendInFixPoint(pMusicPlc->decaySamples - pMusicPlc->decaySamplesNow, pMusicPlc->decaySamples);
		}
	}
	//muting
	pMusicPlc->infuture.ProductWithEverySamlpe(pMusicPlc->muteFactor, pMusicPlc->frameSamples);

	//out
	if (outLen)
		*outLen = pMusicPlc->frameSamples * pMusicPlc->channels * pMusicPlc->sampleFormat.storedWidthPerSample;
	pOut.Append(&pMusicPlc->inHistory[pMusicPlc->inHistory.GetSamples() - pMusicPlc->overlapInSamples], pMusicPlc->overlapInSamples);
	pOut.Append(&pMusicPlc->infuture[0], pMusicPlc->frameSamples - pMusicPlc->overlapInSamples);

	if (sampleFormat->validWidthPerSample < sampleFormat->storedWidthPerSample)
	{
		int16_t shiftBits = (sampleFormat->storedWidthPerSample - sampleFormat->validWidthPerSample) << 3;
		if (sampleFormat->alignMode == MusicPlcSampleHighByteAlign)
		{
			//...
		}
		else
		{
			pOut.RightShift(shiftBits, false);
		}
	}
	//shift
	pMusicPlc->inHistory.Move(-pMusicPlc->frameSamples);
	pMusicPlc->inHistory.Append(&pMusicPlc->infuture[0], pMusicPlc->frameSamples);
	pMusicPlc->inHistory.used(pMusicPlc->frameSamples);
	pMusicPlc->infuture.Clear();

	//
	pMusicPlc->muteFactor.Clear();
	return MUSIC_PLC_RET_SUCCESS;
}

EXTERNC int32_t MusicPlcGetStateSize(int32_t overlapMs, int32_t frameSamples, MusicPlcSampleParam* sampleParam)
{
	//check
	if (!sampleParam)
		return MUSIC_PLC_RET_FAIL;
	MusicPlcSampleStoreFormat* pSampleStoreFormat = &sampleParam->sampleStoreFormat;
	int32_t frameSamplesInner = MAX(frameSamples, sampleParam->fsHz * MUSIC_PLC_MIN_FRAME_MS / 1000);

	if (!MusicPlcCheckParam(overlapMs, frameSamplesInner, sampleParam->fsHz, sampleParam->channels, pSampleStoreFormat->storedWidthPerSample))
		return -1;

	return sizeof(MusicPlcState)
		+ MusicPlcGetBufSamples((int32_t)(overlapMs * sampleParam->fsHz / 1000), frameSamplesInner, frameSamples) * pSampleStoreFormat->storedWidthPerSample * sampleParam->channels
		+ MusicPlcGetMuteFactorSamples(frameSamples) * sizeof(uint32_t);
}

EXTERNC int32_t MusicPlcStateInit(void* pMusicPlcStateIn, int32_t overlapMs, int32_t frameSamples, int32_t decayTimeMs, MusicPlcSampleParam* sampleParam)
{

	//check
	if (!sampleParam)
		return MUSIC_PLC_RET_FAIL;

	if (!MusicPlcCheckParam(overlapMs, frameSamples, sampleParam->fsHz, sampleParam->channels, sampleParam->sampleStoreFormat.storedWidthPerSample)
		|| !pMusicPlcStateIn
		|| (decayTimeMs < 0))
		return MUSIC_PLC_RET_FAIL;

	//
	MusicPlcState* pPlc = (MusicPlcState*)pMusicPlcStateIn;
	MusicPlcSampleStoreFormat* pSampleStoreFormat = &sampleParam->sampleStoreFormat;
	int32_t overlapInSamples = (int32_t)(overlapMs * sampleParam->fsHz / 1000);
	memset(pPlc, 0, MusicPlcGetStateSize(overlapMs, frameSamples, sampleParam));

	pPlc->lostCount = 0;
	pPlc->decaySamplesNow = 0;


	pPlc->overlapInSamples = overlapInSamples;

	pPlc->decaySamples = decayTimeMs * sampleParam->fsHz / 1000;

	pPlc->fsHz = sampleParam->fsHz;
	pPlc->sampleFormat = *pSampleStoreFormat;
	pPlc->channels = sampleParam->channels;

	pPlc->frameSamples = frameSamples;
	pPlc->frameSamplesInner = MAX(pPlc->frameSamples, pPlc->fsHz * MUSIC_PLC_MIN_FRAME_MS / 1000);

	pPlc->inHistory.Init(
		(uint8_t*)pPlc + sizeof(MusicPlcState),
		pPlc->sampleFormat.storedWidthPerSample,
		pPlc->channels,
		MusicPlcGetHistorySamples(pPlc->overlapInSamples, pPlc->frameSamplesInner));

	pPlc->inHistory.Append(&pPlc->inHistory[0], pPlc->inHistory.GetSamples());
	pPlc->inHistory.used(pPlc->inHistory.GetSamples() - pPlc->overlapInSamples);

	pPlc->infuture.Init(
		&pPlc->inHistory[pPlc->inHistory.GetSamples()],
		pPlc->sampleFormat.storedWidthPerSample,
		pPlc->channels,
		MusicPlcGetFutureSamples(pPlc->frameSamples));

	pPlc->fillSignal.Init(
		&pPlc->infuture[pPlc->infuture.GetSamples()],
		pPlc->sampleFormat.storedWidthPerSample,
		pPlc->channels,
		MusicPlcGetFillSignalSamples(pPlc->frameSamplesInner));
	pPlc->fillSignalSampleIndex = 0;
	pPlc->muteFactor.Init(
		&pPlc->fillSignal[pPlc->fillSignal.GetSamples()],
		4,
		1,
		MusicPlcGetMuteFactorSamples(pPlc->frameSamples),
		15);

	pPlc->matchSamples = MUSIC_PLC_MATCH_MS_DEFAULT * pPlc->fsHz / 1000;
	pPlc->matchSamples = pPlc->matchSamples < pPlc->overlapInSamples ? pPlc->matchSamples : pPlc->overlapInSamples;

	pPlc->seekSamples = MUSIC_PLC_SEEK_MS_DEFAULT * pPlc->fsHz / 1000;

	return MUSIC_PLC_RET_SUCCESS;
}

EXTERNC int32_t MusicPlc(void* pMusicPlcStateIn, int8_t* in, int32_t inLen, int32_t* inUsed, int8_t* out, int32_t* outLen, bool isLost)
{
	//check
	if (inUsed)
		*inUsed = 0;
	if (outLen)
		*outLen = 0;
	if (!pMusicPlcStateIn)
		return MUSIC_PLC_RET_FAIL;
	if (isLost == false) {
		if (!in
			|| inLen < ((int32_t)((MusicPlcState*)pMusicPlcStateIn)->frameSamples * ((MusicPlcState*)pMusicPlcStateIn)->channels))
			return MUSIC_PLC_RET_FAIL;
	}
	if (!out)
		return MUSIC_PLC_RET_FAIL;
#if 0
	if (((MusicPlcState*)pMusicPlcStateIn)->sampleWidth != 2
		&& ((MusicPlcState*)pMusicPlcStateIn)->sampleWidth != 4)
		return MUSIC_PLC_RET_FAIL;
#endif
	int32_t ret = MUSIC_PLC_RET_FAIL;
	ret = MusicPlcRun((MusicPlcState*)pMusicPlcStateIn, in, inLen, inUsed, out, outLen, isLost);

	if (ret != MUSIC_PLC_RET_SUCCESS)
		return MUSIC_PLC_RET_FAIL;

	return MUSIC_PLC_RET_SUCCESS;
}

#endif