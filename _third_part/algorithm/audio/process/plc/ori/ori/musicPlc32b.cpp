#include<stdio.h>
#include<stdint.h>
#include<string.h>
#include<new>

#include"AudioSamples32.h"
#include"musicPlc32b.h"
#include"musicPlcComInner.h"

EXTERNC int32_t MusicPlc32bGetStateSize(int32_t overlapMs, MusicPlcSampleParam* sampleParam)
{
	//check
	if (!sampleParam
		|| overlapMs < 0)
		return MUSIC_PLC_RET_FAIL;

	int32_t frameSamplesInner = MAX(sampleParam->frameSamples, sampleParam->fsHz * MUSIC_PLC_MIN_FRAME_MS / 1000);

	if (!MusicPlcCheckParam(overlapMs, frameSamplesInner, sampleParam->fsHz, sampleParam->channels))
		return MUSIC_PLC_RET_FAIL;

	int32_t overlapSamples = overlapMs * sampleParam->fsHz / 1000;
	int32_t buffSamples = MusicPlcGetBufSamples(overlapSamples, frameSamplesInner, sampleParam->frameSamples);
	int32_t muteFactorSamples = MusicPlcGetMuteFactorSamples(sampleParam->frameSamples);
	int16_t width = sizeof(int32_t);

	return sizeof(MusicPlcState)
		+ sizeof(AudioSamples32) * 4
		+ (buffSamples * sampleParam->channels + muteFactorSamples) * width;

}

EXTERNC int32_t MusicPlc32bStateInit(void* pMusicPlcStateIn, int32_t overlapMs, int32_t decayTimeMs, MusicPlcSampleParam* sampleParam)
{
	//check
	if (!sampleParam)
		return MUSIC_PLC_RET_FAIL;

	if (!MusicPlcCheckParam(overlapMs, sampleParam->frameSamples, sampleParam->fsHz, sampleParam->channels)
		|| !pMusicPlcStateIn
		|| (decayTimeMs < 0))
		return MUSIC_PLC_RET_FAIL;
	//
	MusicPlcState* pPlc = (MusicPlcState*)pMusicPlcStateIn;
	int32_t overlapInSamples = overlapMs * sampleParam->fsHz / 1000;

	memset(pPlc, 0, MusicPlc32bGetStateSize(overlapMs, sampleParam));

	pPlc->inHistory = new((uint8_t*)pPlc + sizeof(MusicPlcState)) AudioSamples32();
	pPlc->infuture = new((uint8_t*)pPlc->inHistory + sizeof(AudioSamples32)) AudioSamples32();
	pPlc->fillSignal = new((uint8_t*)pPlc->infuture + sizeof(AudioSamples32)) AudioSamples32();
	pPlc->muteFactor = new((uint8_t*)pPlc->fillSignal + sizeof(AudioSamples32)) AudioSamples32();
	return MusicPlcStateInitInner(pMusicPlcStateIn, overlapInSamples, decayTimeMs, sampleParam, sizeof(int32_t));
}

EXTERNC int32_t MusicPlc32b(void* pMusicPlcStateIn, uint8_t* in, int32_t inLen, int32_t* inUsed, uint8_t* out, int32_t* outLen, bool isLost)
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

	int16_t channels = ((MusicPlcState*)pMusicPlcStateIn)->channels;
	int16_t width = ((MusicPlcState*)pMusicPlcStateIn)->width;
	int32_t frameSamples = ((MusicPlcState*)pMusicPlcStateIn)->frameSamples;

	AudioSamples32 pIn;
	pIn.Init((uint8_t*)in, channels, inLen / channels / width);

	AudioSamples32 pOut;
	pOut.Init((uint8_t*)out, channels, frameSamples);

	int32_t ret = MusicPlcRun((MusicPlcState*)pMusicPlcStateIn, &pIn, inUsed, &pOut, outLen, isLost);

	if (ret != MUSIC_PLC_RET_SUCCESS)
		return MUSIC_PLC_RET_FAIL;

	return MUSIC_PLC_RET_SUCCESS;
}

