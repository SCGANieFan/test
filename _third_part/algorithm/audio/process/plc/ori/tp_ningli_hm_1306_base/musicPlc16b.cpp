#include<stdio.h>
#include<stdint.h>
#include<string.h>
#include<new>

#include"AudioSamples16.h"
#include"musicPlc16b.h"
#include"musicPlcComInner.h"

EXTERNC int32_t MusicPlc16bGetStateSize(int32_t overlapSamples, int32_t decayTimeSamples, MusicPlcSampleParam* sampleParam)
{
	//check
	if (!sampleParam
		|| overlapSamples < 0)
		return MUSIC_PLC_RET_FAIL;

	int32_t frameSamplesInner = MAX(sampleParam->frameSamples, sampleParam->fsHzMax * MUSIC_PLC_MIN_FRAME_MS / 1000);

	if (!MusicPlcCheckParam(overlapSamples, frameSamplesInner, sampleParam->channels))
		return MUSIC_PLC_RET_FAIL;

	int32_t buffSamples = MusicPlcGetBufSamples(overlapSamples, frameSamplesInner, sampleParam->frameSamples);
	int32_t muteFactorSamples = MusicPlcGetMuteFactorSamples(decayTimeSamples);
	int16_t width = sizeof(int16_t);
	
	return sizeof(MusicPlcState)
		+ (buffSamples * sampleParam->channels + muteFactorSamples) * width;
		
}

EXTERNC int32_t MusicPlc16bStateInit(void* pMusicPlcStateIn, int32_t overlapSamples, int32_t decayTimeSamples, MusicPlcSampleParam* sampleParam)
{
	//check
	if (!sampleParam)
		return MUSIC_PLC_RET_FAIL;

	if (!MusicPlcCheckParam(overlapSamples, sampleParam->frameSamples, sampleParam->channels)
		|| !pMusicPlcStateIn
		|| (decayTimeSamples < 0))
		return MUSIC_PLC_RET_FAIL;
	//
	MusicPlcState* pPlc = (MusicPlcState*)pMusicPlcStateIn;	

	memset((void*)pPlc, 0, MusicPlc16bGetStateSize(overlapSamples, decayTimeSamples, sampleParam));
	return MusicPlcStateInitInner(pMusicPlcStateIn, overlapSamples, decayTimeSamples, sampleParam, sizeof(int16_t));
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

