#include<stdio.h>
#include<stdint.h>
#include<string.h>
#include"musicPlc.h"


#if 1
#define MUSIC_PLC_MIN_FRAME_MS 10

#define MAX(x1,x2) ((x1)>(x2)?(x1):(x2))
#define MIN(x1,x2) ((x1)<(x2)?(x1):(x2))

typedef struct
{
	int32_t fsHz;
	int16_t sampleWidth;
	int16_t channels;

	uint32_t lostCount;

	uint32_t overlapInSamples;
	uint32_t overlapLen;
	uint32_t frameInSamples;
	uint32_t frameLen;
	uint32_t frameInSamplesInner;
	uint32_t frameLenInner;
	int32_t decayTimeNowInSamples;
	int32_t decayTimeInSamples;

	int16_t* buff;
	uint32_t buffSamples;

	int16_t* inHistory;
	uint32_t inHistorySamples;
	uint32_t inHistoryLen;

	int16_t* infuture;
	uint32_t infutureSamples;
	uint32_t infutureLen;

	int16_t* fillSignal;
	uint32_t fillSignalSamples;
	uint32_t fillSignalLen;
	uint32_t fillSignalIndex;

	uint32_t* muteFactorQ31;
	uint32_t muteFactorQ31Samples;
}MusicPlcState;

static inline uint32_t MusicPlcGetMuteFactorQ31Samples(uint32_t frameInSamples)
{
	return frameInSamples;
}

static inline uint32_t MusicPlcGetFillSignalBufSamples(uint32_t frameInSamples)
{
	return frameInSamples;
}

static inline uint32_t MusicPlcGetHistoryBufSamples(uint32_t overlapInSamples, uint32_t frameInSamples)
{
	return frameInSamples + overlapInSamples;
}

static inline uint32_t MusicPlcGetFutureBufSamples(uint32_t frameInSamples)
{
	return frameInSamples;
}

static inline uint32_t MusicPlcGetBufSamples(uint32_t overlapInSamples, uint32_t frameInSamplesInner, uint32_t frameInSamples)
{
	return MusicPlcGetHistoryBufSamples(overlapInSamples, frameInSamplesInner)
		+ MusicPlcGetFutureBufSamples(frameInSamples)
		+ MusicPlcGetFillSignalBufSamples(frameInSamplesInner);
}

static inline bool MusicPlcCheckParam(int32_t overlapMs, int32_t frameInSamples, int32_t fsHz, int16_t channels, int16_t sampleWidth)
{
	if (overlapMs < 1
		|| frameInSamples < 1)
		return false;
	int32_t overlapInSamples = overlapMs * fsHz / 1000;
	if (overlapInSamples > frameInSamples)
		return false;
	if (sampleWidth != 2)
		return false;
	if (channels != 1 && channels != 2)
		return false;
	return true;
}

EXTERNC int32_t MusicPlcGetStateSize(int32_t overlapMs, int32_t frameInSamples, int32_t fsHz, int16_t channels, int16_t sampleWidth)
{
	//check
	uint32_t frameInSamplesInner = MAX(frameInSamples, fsHz * MUSIC_PLC_MIN_FRAME_MS / 1000);
	if (!MusicPlcCheckParam(overlapMs, frameInSamplesInner, fsHz, channels, sampleWidth))
		return -1;

	return sizeof(MusicPlcState)
		+ MusicPlcGetBufSamples((uint32_t)(overlapMs * fsHz / 1000), frameInSamplesInner, frameInSamples) * sampleWidth * channels
		+ MusicPlcGetMuteFactorQ31Samples(frameInSamples) * sizeof(uint32_t);
}

EXTERNC int32_t MusicPlcStateInit(void* pMusicPlcStateIn, int32_t overlapMs, int32_t frameInSamples,
	int32_t decayTimeMs, int32_t fsHz, int16_t channels, int16_t sampleWidth)
{

	//check
	if (!MusicPlcCheckParam(overlapMs, frameInSamples, fsHz, channels, sampleWidth)
		|| !pMusicPlcStateIn
		|| (decayTimeMs < 0))
		return MUSIC_PLC_RET_FAIL;

	//
	MusicPlcState* pPlc = (MusicPlcState*)pMusicPlcStateIn;
	uint32_t overlapInSamples = (uint32_t)(overlapMs * fsHz / 1000);
	memset(pPlc, 0, MusicPlcGetStateSize(overlapMs, frameInSamples, fsHz, channels, sampleWidth));

	pPlc->lostCount = 0;
	pPlc->decayTimeNowInSamples = 0;

	pPlc->overlapInSamples = overlapInSamples;
	pPlc->overlapLen = pPlc->overlapInSamples * channels;

	pPlc->decayTimeInSamples = decayTimeMs * fsHz / 1000;

	pPlc->fsHz = fsHz;
	pPlc->sampleWidth = sampleWidth;
	pPlc->channels = channels;

	pPlc->frameInSamples = frameInSamples;
	pPlc->frameLen = frameInSamples * pPlc->channels;

	pPlc->frameInSamplesInner = MAX(pPlc->frameInSamples, pPlc->fsHz * MUSIC_PLC_MIN_FRAME_MS / 1000);
	pPlc->frameLenInner = pPlc->frameInSamplesInner * pPlc->channels;


	pPlc->buffSamples = MusicPlcGetBufSamples(pPlc->overlapInSamples, pPlc->frameInSamplesInner, frameInSamples);
	pPlc->buff = (int16_t*)((int8_t*)pPlc + sizeof(MusicPlcState));

	pPlc->inHistorySamples = MusicPlcGetHistoryBufSamples(pPlc->overlapInSamples, pPlc->frameInSamplesInner);
	pPlc->inHistoryLen = pPlc->inHistorySamples * pPlc->channels;
	pPlc->inHistory = pPlc->buff;

	pPlc->infutureSamples = MusicPlcGetFutureBufSamples(pPlc->frameInSamples);
	pPlc->infutureLen = pPlc->infutureSamples * pPlc->channels;
	pPlc->infuture = pPlc->inHistory + pPlc->inHistoryLen;

	pPlc->fillSignalSamples = MusicPlcGetFillSignalBufSamples(pPlc->frameInSamplesInner);
	pPlc->fillSignalLen = pPlc->fillSignalSamples * pPlc->channels;
	pPlc->fillSignalIndex = 0;
	pPlc->fillSignal = pPlc->infuture + pPlc->infutureLen;

	pPlc->muteFactorQ31Samples = MusicPlcGetMuteFactorQ31Samples(pPlc->frameInSamples);
	pPlc->muteFactorQ31 = (uint32_t*)(pPlc->fillSignal + pPlc->fillSignalLen);

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
	if (isLost == false)
	{
		if (!in
			|| inLen < ((int32_t)((MusicPlcState*)pMusicPlcStateIn)->frameLen))
			return MUSIC_PLC_RET_FAIL;
	}
	if (!out)
		return MUSIC_PLC_RET_FAIL;

	if (((MusicPlcState*)pMusicPlcStateIn)->sampleWidth != 2)
		return MUSIC_PLC_RET_FAIL;
	//
	MusicPlcState* pMusicPlc = (MusicPlcState*)pMusicPlcStateIn;
	int16_t* pIn = (int16_t*)in;
	int16_t* pOut = (int16_t*)out;

	uint32_t factorQ31;

	if (isLost == true)
	{
		pMusicPlc->lostCount += 1;
		if (pMusicPlc->lostCount == 1)
		{
			int16_t* pSrc = &pMusicPlc->inHistory[pMusicPlc->inHistoryLen - pMusicPlc->overlapLen - pMusicPlc->frameLenInner];
			int16_t* pDst = &pMusicPlc->inHistory[pMusicPlc->inHistoryLen - pMusicPlc->overlapLen];
			//overlap add
			for (uint32_t i = 0; i < pMusicPlc->overlapInSamples; i++)
			{
				factorQ31 = (uint32_t)(((uint64_t)i << 31) / pMusicPlc->overlapInSamples);
				int32_t pos = i * pMusicPlc->channels;
				for (int16_t ch = 0; ch < pMusicPlc->channels; ch++)
				{
					pDst[pos + ch] = (int16_t)(((int64_t)factorQ31 * pSrc[pos + ch] + (((int64_t)1 << 31) - factorQ31) * pDst[pos + ch]) >> 31);
				}
			}

			//fill signal
			memcpy(
				pMusicPlc->fillSignal,
				&pMusicPlc->inHistory[pMusicPlc->inHistoryLen - pMusicPlc->fillSignalLen],
				pMusicPlc->fillSignalLen * pMusicPlc->sampleWidth);
			pMusicPlc->fillSignalIndex = 0;
		}
		//mute factorQ31
		for (uint32_t i = 0; i < pMusicPlc->frameInSamples; i++)
		{
			pMusicPlc->decayTimeNowInSamples++;
			pMusicPlc->decayTimeNowInSamples = MIN(pMusicPlc->decayTimeNowInSamples, pMusicPlc->decayTimeInSamples);
			pMusicPlc->muteFactorQ31[i] = (uint32_t)(((int64_t)(pMusicPlc->decayTimeInSamples - pMusicPlc->decayTimeNowInSamples) << 31) / pMusicPlc->decayTimeInSamples);
		}

		//infuture
		for (uint32_t i = 0; i < pMusicPlc->infutureLen; i++)
		{
			pMusicPlc->infuture[i] = pMusicPlc->fillSignal[pMusicPlc->fillSignalIndex];
			pMusicPlc->fillSignalIndex = (++pMusicPlc->fillSignalIndex) % pMusicPlc->fillSignalLen;
		}
	}
	else
	{
		if (inUsed)
			*inUsed = pMusicPlc->frameLen * pMusicPlc->sampleWidth;

		if (pMusicPlc->lostCount > 0)
		{
			pMusicPlc->lostCount = 0;
			//infuture
			//overlap add
			for (uint32_t i = 0; i < pMusicPlc->overlapInSamples; i++)
			{
				factorQ31 = (uint32_t)(((uint64_t)i << 31) / pMusicPlc->overlapInSamples);
				int32_t pos = i * pMusicPlc->channels;
				for (int16_t ch = 0; ch < pMusicPlc->channels; ch++)
				{
					pMusicPlc->infuture[pos + ch] = (int16_t)(((((int64_t)1 << 31) - factorQ31) * pMusicPlc->fillSignal[pMusicPlc->fillSignalIndex]
						+ (int64_t)factorQ31 * pIn[pos + ch]) >> 31);
					pMusicPlc->fillSignalIndex = (pMusicPlc->fillSignalIndex + 1) % pMusicPlc->fillSignalLen;
				}
			}
			//other
			for (uint32_t i = pMusicPlc->overlapLen; i < pMusicPlc->frameLen; i++)
			{
				pMusicPlc->infuture[i] = pIn[i];
			}
		}
		else
		{
			//infuture
			for (uint32_t i = 0; i < pMusicPlc->frameLen; i++)
			{
				pMusicPlc->infuture[i] = pIn[i];
			}
		}
		//muteFactor
		for (uint32_t i = 0; i < pMusicPlc->frameInSamples; i++)
		{
			pMusicPlc->decayTimeNowInSamples--;
			pMusicPlc->decayTimeNowInSamples = MAX(--pMusicPlc->decayTimeNowInSamples, 0);
			pMusicPlc->muteFactorQ31[i] = (uint32_t)(((int64_t)(pMusicPlc->decayTimeInSamples - pMusicPlc->decayTimeNowInSamples) << 31) / pMusicPlc->decayTimeInSamples);
		}
	}

	//muting
	for (uint32_t i = 0; i < pMusicPlc->frameInSamples; i++)
	{
		int32_t pos = i * pMusicPlc->channels;
		for (int16_t ch = 0; ch < pMusicPlc->channels; ch++)
		{
			pMusicPlc->infuture[pos + ch] = (int16_t)(((int64_t)pMusicPlc->muteFactorQ31[i] * pMusicPlc->infuture[pos + ch]) >> 31);
		}
	}
	//out
	if (outLen)
		*outLen = pMusicPlc->frameLen * pMusicPlc->sampleWidth;
	memcpy(
		pOut,
		&pMusicPlc->inHistory[pMusicPlc->inHistoryLen - pMusicPlc->overlapLen],
		pMusicPlc->frameLen * pMusicPlc->sampleWidth);

	//shift
	memmove(
		pMusicPlc->inHistory,
		pMusicPlc->inHistory + pMusicPlc->frameLen,
		(pMusicPlc->inHistoryLen + pMusicPlc->infutureLen - pMusicPlc->frameLen) * pMusicPlc->sampleWidth);

	return MUSIC_PLC_RET_SUCCESS;
}

#endif