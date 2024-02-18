#include<stdint.h>
#include<string.h>
#include"plc.h"
#include"musicPlc.h"



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
	case PlcMode_e::PlcModeMusicPlc:
		size = MusicPlcGetStateSize(param->MusicPlcParam.overlapMs, param->frameSamples, param->fsHz, param->channels, param->sampleBits >> 3);
		break;
	case PlcMode_e::PlcModeSbcPlc:
		//size = SbcPlcGetStateSize(param->targetChannels, (SbcPlcCodecType_e)param->SbcPlcParam.codecType, param->SbcPlcParam.olal, param->frameSamples);
		break;

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
	case PlcMode_e::PlcModeMusicPlc:
		ret = MusicPlcStateInit(plcState->state, param->MusicPlcParam.overlapMs, param->frameSamples, param->MusicPlcParam.decayTimeMs, param->fsHz, param->channels, param->sampleBits >> 3);
		if (ret != MUSIC_PLC_RET_SUCCESS)
			return PLC_RET_FAIL;
		plcState->plcProcess = MusicPlc;
		break;
	case PlcMode_e::PlcModeSbcPlc:
#if 0
		ret = SbcPlcStateInit(plcState->state, (SbcPlcCodecType_e)(param->SbcPlcParam.codecType), param->frameSamples, param->targetChannels, param->sampleBits >> 3,
			param->SbcPlcParam.searchSize, param->SbcPlcParam.matchSize, param->SbcPlcParam.olal, param->SbcPlcParam.recover);

		if (ret != SBC_PLC_RET_SUCCESS)
			return PLC_RET_FAIL;
		plcState->plcProcess = SbcPlc;
#endif
		break;

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
	case PlcMode_e::PlcModeMusicPlc:
		if (ret != MUSIC_PLC_RET_SUCCESS)
			return PLC_RET_FAIL;
		break;
	case PlcMode_e::PlcModeSbcPlc:
		//if (ret != SBC_PLC_RET_SUCCESS)
			return PLC_RET_FAIL;
		break;

	default:
		return PLC_RET_FAIL;
		break;
	}
	return PLC_RET_SUCCESS;
}
