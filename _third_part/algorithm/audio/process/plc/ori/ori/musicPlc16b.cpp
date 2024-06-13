#include<stdio.h>
#include<stdint.h>
#include<string.h>
#include<new>

#include"AudioSamples16.h"
#include"musicPlc16b.h"
#include"musicPlcComInner.h"

EXTERNC int32_t MusicPlc16bGetStateSize(int32_t overlapMs, MusicPlcSampleParam* sampleParam)
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
	int16_t width = sizeof(int16_t);
	
	return sizeof(MusicPlcState)
		+ sizeof(AudioSamples16) * 4
		+ (buffSamples * sampleParam->channels + muteFactorSamples) * width;
		
}

EXTERNC int32_t MusicPlc16bStateInit(void* pMusicPlcStateIn, int32_t overlapMs, int32_t decayTimeMs, MusicPlcSampleParam* sampleParam)
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

	memset(pPlc, 0, MusicPlc16bGetStateSize(overlapMs, sampleParam));

	pPlc->inHistory = new((uint8_t*)pPlc + sizeof(MusicPlcState)) AudioSamples16();
	pPlc->infuture = new((uint8_t*)pPlc->inHistory + sizeof(AudioSamples16)) AudioSamples16();
	pPlc->fillSignal = new((uint8_t*)pPlc->infuture + sizeof(AudioSamples16)) AudioSamples16();
	pPlc->muteFactor = new((uint8_t*)pPlc->fillSignal + sizeof(AudioSamples16)) AudioSamples16();
	return MusicPlcStateInitInner(pMusicPlcStateIn, overlapInSamples, decayTimeMs, sampleParam, sizeof(int16_t));
}

EXTERNC int32_t MusicPlc16b(void* pMusicPlcStateIn, uint8_t* in, int32_t inLen, int32_t* inUsed, uint8_t* out, int32_t* outLen, bool isLost)
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

	AudioSamples16 pIn;
	pIn.Init((uint8_t*)in, channels, inLen / channels / width);

	AudioSamples16 pOut;
	pOut.Init((uint8_t*)out, channels, frameSamples);

	int32_t ret = MusicPlcRun((MusicPlcState*)pMusicPlcStateIn, &pIn, inUsed, &pOut, outLen, isLost);

	if (ret != MUSIC_PLC_RET_SUCCESS)
		return MUSIC_PLC_RET_FAIL;

	return MUSIC_PLC_RET_SUCCESS;
}

