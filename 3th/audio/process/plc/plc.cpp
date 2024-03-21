#include<stdint.h>
#include<string.h>

#define USE_MUSIC_PLC
//#define USE_SBC_PLC

#ifdef USE_MUSIC_PLC
#include"musicPlc.h"
#endif

#ifdef USE_SBC_PLC
#include"sbcplc.h"
#endif

#include"plc.h"

typedef int32_t(*PlcProcess)(void*, int8_t*, int32_t, int32_t*, int8_t*, int32_t*, bool);

typedef struct {
	PlcMode_e mode;
	void* state;
	PlcProcess plcProcess;
}PlcState;

EXTERNC int32_t GetPlcStateSize(PlcParam* param)
{
	//check
	if (!param)
		return -1;

	int32_t size;
	switch (param->mode)
	{
#ifdef USE_MUSIC_PLC
	case PlcMode_e::PlcModeMusicPlc:
		MusicPlcSampleParam musicParam;
		memset(&musicParam, 0, sizeof(MusicPlcSampleParam));
		musicParam.fsHz = param->fsHz;
		musicParam.sampleStoreFormat.validWidthPerSample = param->sampleFormat.validBits >> 3;
		musicParam.sampleStoreFormat.storedWidthPerSample = param->sampleFormat.storedBits >> 3;
		if (param->sampleFormat.alignMode == PlcSampleAlignMode_e::PlcSampleHighByteAlign) {
			musicParam.sampleStoreFormat.alignMode = MusicPlcSampleAlignMode_e::MusicPlcSampleHighByteAlign;
		}
		else {
			musicParam.sampleStoreFormat.alignMode = MusicPlcSampleAlignMode_e::MusicPlcSampleLowByteAlign;
		}
		musicParam.channels = param->channels;
		size = MusicPlcGetStateSize(param->MusicPlcParam.overlapMs, param->frameSamples, &musicParam);
		break;
#endif
#ifdef USE_SBC_PLC
	case PlcMode_e::PlcModeSbcPlc:
		size = SbcPlcGetStateSize(param->targetChannels, (SbcPlcCodecType_e)param->SbcPlcParam.codecType, param->SbcPlcParam.olal, param->frameSamples);
		break;
#endif

	default:
		size = -1;
		break;
	}
	if (size < 0)
		return -1;

	return sizeof(PlcState) + size;
}

EXTERNC PlcRet PlcInit(void* plcStateIn, PlcParam* param)
{
	//check
	if (!plcStateIn
		|| !param)
		return PLC_RET_INPUT_ERROR;


	PlcState* plcState = (PlcState*)plcStateIn;
	memset(plcState, 0, sizeof(PlcState));
	plcState->mode = param->mode;
	plcState->state = ((uint8_t*)plcState + sizeof(PlcState));
	plcState->plcProcess = NULL;

	int32_t ret;
	switch (plcState->mode)
	{
#ifdef USE_MUSIC_PLC
	case PlcMode_e::PlcModeMusicPlc:
		MusicPlcSampleParam musicParam;
		memset(&musicParam, 0, sizeof(MusicPlcSampleParam));
		musicParam.fsHz = param->fsHz;
		musicParam.sampleStoreFormat.validWidthPerSample = param->sampleFormat.validBits >> 3;
		musicParam.sampleStoreFormat.storedWidthPerSample = param->sampleFormat.storedBits >> 3;
		if (param->sampleFormat.alignMode == PlcSampleAlignMode_e::PlcSampleHighByteAlign) {
			musicParam.sampleStoreFormat.alignMode = MusicPlcSampleAlignMode_e::MusicPlcSampleHighByteAlign;
		}
		else {
			musicParam.sampleStoreFormat.alignMode = MusicPlcSampleAlignMode_e::MusicPlcSampleLowByteAlign;
		}
		musicParam.channels = param->channels;
		ret = MusicPlcStateInit(plcState->state, param->MusicPlcParam.overlapMs, param->frameSamples, param->MusicPlcParam.decayTimeMs, &musicParam);
		if (ret != MUSIC_PLC_RET_SUCCESS)
			return PLC_RET_FAIL;
		plcState->plcProcess = MusicPlc;
		break;
#endif
#ifdef USE_SBC_PLC
	case PlcMode_e::PlcModeSbcPlc:
		ret = SbcPlcStateInit(plcState->state, (SbcPlcCodecType_e)(param->SbcPlcParam.codecType), param->frameSamples, param->targetChannels, param->sampleBits >> 3,
			param->SbcPlcParam.searchSize, param->SbcPlcParam.matchSize, param->SbcPlcParam.olal, param->SbcPlcParam.recover);

		if (ret != SBC_PLC_RET_SUCCESS)
			return PLC_RET_FAIL;
		plcState->plcProcess = SbcPlc;

		break;
#endif
	default:
		return PLC_RET_FAIL;
		break;
	}
	return PLC_RET_SUCCESS;
}

EXTERNC PlcRet PlcRun(void* plcStateIn, int8_t* in, int32_t inLen, int32_t* inUsed, int8_t* out, int32_t* pOutLen, bool isLost)
{
	//check
	if (!plcStateIn)
		return PLC_RET_INPUT_ERROR;
	if (!isLost)
	{
		if (!in
			|| inLen < 1)
			return PLC_RET_INPUT_ERROR;
	}
	if (!out)
		return PLC_RET_INPUT_ERROR;

	//
	int32_t ret = ((PlcState*)plcStateIn)->plcProcess(((PlcState*)plcStateIn)->state, in, inLen, inUsed, out, pOutLen, isLost);
	switch (((PlcState*)plcStateIn)->mode)
	{
#ifdef USE_MUSIC_PLC
	case PlcMode_e::PlcModeMusicPlc:
		if (ret != MUSIC_PLC_RET_SUCCESS)
			return PLC_RET_FAIL;
		break;
#endif
#ifdef USE_SBC_PLC
	case PlcMode_e::PlcModeSbcPlc:
		if (ret != SBC_PLC_RET_SUCCESS)
			return PLC_RET_FAIL;
		break;
#endif
	default:
		return PLC_RET_FAIL;
		break;
	}
	return PLC_RET_SUCCESS;
}


