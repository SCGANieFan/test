#if 1
#include<new>
#include"Algo.AS_Calculator16.h"
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
		+ sizeof(AS_Calculator16)
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
	
	pPlc->info.Init(sampleParam->fsHz, sizeof(i16), sampleParam->channels, (i32)((u64)sampleParam->frameSamples * 1000 * 1000 / sampleParam->fsHz));
	pPlc->frameSamples = sampleParam->frameSamples;

	pPlc->asCalculator = new((uint8_t*)pPlc + sizeof(MusicPlcState)) AS_Calculator16();
	return MusicPlcStateInitInner(pMusicPlcStateIn, overlapInSamples, decayTimeMs);
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

	MusicPlcState* plcState = (MusicPlcState*)pMusicPlcStateIn;
	if (isLost == false) {
		if (!in
			|| inLen < (plcState->frameSamples * plcState->info._bytesPerSample))
			return MUSIC_PLC_RET_FAIL;
	}
	if (!out)
		return MUSIC_PLC_RET_FAIL;

	AudioSamples pIn;
	pIn.Init(&((MusicPlcState*)pMusicPlcStateIn)->info, (u8*)in, inLen / plcState->info._bytesPerSample);
	
	AudioSamples pOut;
	pOut.Init(&((MusicPlcState*)pMusicPlcStateIn)->info, (u8*)out, plcState->frameSamples);

	return MusicPlcRun((MusicPlcState*)pMusicPlcStateIn, &pIn, inUsed, &pOut, outLen, isLost);
}
#endif