#if 1
#define USE_MUSIC_PLC
#include"plc.h"
#include"Algo.Memory.h"
using namespace Algo;

#ifdef USE_MUSIC_PLC
#include"MusicPlc.h"
using namespace MusicPlc_ns;
#endif


namespace Plc_ns{

typedef int32_t (*PlcProcess)(void*, uint8_t*, int32_t, uint8_t*, int32_t*, uint16_t);
typedef struct {
	PlcMode_e mode;
	void* state;
	PlcProcess plcProcess;
}PlcState_t;

int32_t Plc_c::GetStateSize(PlcParam_t* param)
{
	//check
	if (!param)
		return -1;

	int32_t size;
	switch (param->mode)
	{
#ifdef USE_MUSIC_PLC
	case PlcMode_e::PlcModeMusicPlc:
		size = MusicPlc_c::GetStateSize();
		break;
#endif
	default:
		size = -1;
		break;
	}
	if (size < 0)
		return -1;

	return sizeof(PlcState_t) + size;
}

PlcRet Plc_c::Init(void* plcStateIn, PlcParam_t* param)
{
	//check
	if (!plcStateIn
		|| !param)
		return PLC_RET_INPUT_ERROR;


	PlcState_t* plcState = (PlcState_t*)plcStateIn;
	ALGO_MEM_SET(plcState, 0, sizeof(PlcState_t));
	plcState->mode = param->mode;
	plcState->state = ((uint8_t*)plcState + sizeof(PlcState_t));
	plcState->plcProcess = NULL;

	int32_t ret;
	switch (plcState->mode)
	{
#ifdef USE_MUSIC_PLC
	case PlcMode_e::PlcModeMusicPlc:
		MusicPlcInitParam_t sampleParam;
		ALGO_MEM_SET(&sampleParam, 0, sizeof(MusicPlcInitParam_t));
		sampleParam.basePorting = (AlgoBasePorting_c*)param->memory;
		sampleParam.fsHz = param->fsHz;
		sampleParam.channels = param->channels;
		sampleParam.width = param->width;
		sampleParam.frameSamples = param->frameSamples;
		sampleParam.overlapSamples = param->MusicPlcParam.overlapSamples;
		sampleParam.holdSamplesAfterLost = param->MusicPlcParam.holdSamples;
		sampleParam.attenuateSamplesAfterLost = param->MusicPlcParam.fadeSamples;
		sampleParam.gainSamplesAfterNoLost = param->MusicPlcParam.gainSamples;
		ret = MusicPlc_c::StateInit(plcState->state, &sampleParam);
		if (ret != MUSIC_PLC_RET_SUCCESS)
			return PLC_RET_FAIL;
		MusicPlc_c::Set(plcState->state, MusicPlc_SetChhoose_e::MUSIC_PLC_SET_SEEK_SAMPLES, (void*)param->MusicPlcParam.seekSamples);
		MusicPlc_c::Set(plcState->state, MusicPlc_SetChhoose_e::MUSIC_PLC_SET_MATCH_SAMPLES, (void*)param->MusicPlcParam.matchSamples);
		MusicPlc_c::Set(plcState->state, MusicPlc_SetChhoose_e::MUSIC_PLC_SET_CHANNEL_SELECT, (void*)(int32_t)(param->MusicPlcParam.channelSelect));
		plcState->plcProcess = MusicPlc_c::Run;
		break;
#endif
	default:
		return PLC_RET_FAIL;
		break;
	}
	return PLC_RET_SUCCESS;
}

PlcRet Plc_c::Run(void* plcStateIn, uint8_t* in, int32_t inLen, int32_t* inUsed, uint8_t* out, int32_t* pOutLen, uint16_t isLost)
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
	if(inUsed)
		*inUsed = inLen;
	int32_t ret = ((PlcState_t*)plcStateIn)->plcProcess(((PlcState_t*)plcStateIn)->state, in, inLen, out, pOutLen, isLost);
	switch (((PlcState_t*)plcStateIn)->mode)
	{
#ifdef USE_MUSIC_PLC
	case PlcMode_e::PlcModeMusicPlc:
		if (ret != MUSIC_PLC_RET_SUCCESS)
			return PLC_RET_FAIL;
		break;
#endif
	default:
		return PLC_RET_FAIL;
		break;
	}
	return PLC_RET_SUCCESS;
}

PlcRet Plc_c::Deinit(void* plcStateIn){
		//check
	if (!plcStateIn)
		return PLC_RET_INPUT_ERROR;
	PlcState_t* plcState = (PlcState_t*)plcStateIn;
	switch (plcState->mode)
	{
#ifdef USE_MUSIC_PLC
	case PlcMode_e::PlcModeMusicPlc:
		MusicPlc_c::StateDeInit(plcState->state);
		break;
#endif
	default:
		break;
	}
	return PLC_RET_SUCCESS;
}

}

#endif