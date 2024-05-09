#if 1
#include<new>
#include"Algo.AS_Calculator32.h"
#include"musicPlc32b.h"
#include"musicPlcComInner.h"

EXTERNC int32_t MusicPlc32bGetStateSize(int32_t overlapMs, MusicPlcSampleParam* sampleParam)
{
	return sizeof(MusicPlcState);
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

	int32_t overlapInSamples = overlapMs * sampleParam->fsHz / 1000;
	MusicPlcState* pPlc = (MusicPlcState*)pMusicPlcStateIn;
	memset(pPlc, 0, MusicPlc32bGetStateSize(overlapMs, sampleParam));
	pPlc->info.Init(sampleParam->fsHz, sizeof(i32), sampleParam->channels);
	pPlc->frameSamples = sampleParam->frameSamples;
	pPlc->asCalculator = new(MusicplcMalloc(sizeof(AS_Calculator32))) AS_Calculator32();
	return MusicPlcStateInitInner(pPlc, overlapInSamples, decayTimeMs);
}

EXTERNC int32_t MusicPlc32b(void* pMusicPlcStateIn, uint8_t* in, int32_t inLen, int32_t* inUsed, uint8_t* out, int32_t* outLen, bool isLost)
{
	//check
	if (!outLen)
		return MUSIC_PLC_RET_FAIL;
	if (inUsed)
		*inUsed = 0;
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

	Buffer buffer;

	AudioSamples pIn;
	buffer._buf = in;
	buffer._max = inLen;
	pIn.Init(&((MusicPlcState*)pMusicPlcStateIn)->info, &buffer);

	AudioSamples pOut;
	buffer._buf = out;
	buffer._max = *outLen;
	pOut.Init(&((MusicPlcState*)pMusicPlcStateIn)->info, &buffer);

	return MusicPlcRun((MusicPlcState*)pMusicPlcStateIn, &pIn, inUsed, &pOut, outLen, isLost);
}

int32_t MusicPlc32bStateDeInit(void* pMusicPlcStateIn)
{
	if (!pMusicPlcStateIn)
		return MUSIC_PLC_RET_FAIL;
	return MusicPlcStateDeInitInner((MusicPlcState*)pMusicPlcStateIn);
}

#endif