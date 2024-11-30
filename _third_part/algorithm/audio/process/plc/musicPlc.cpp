#if 1
#include"MusicPlcInner.h"
#include"MusicPlc.h"
namespace MusicPlc_ns{

typedef struct
{
	MusicPlcInner_c musicplc;
	b1 isInited;
}MusicPlcState;

int32_t MusicPlc_c::GetStateSize()
{
	return sizeof(MusicPlcState);
}

int32_t MusicPlc_c::StateInit(void* pMusicPlcStateIn, MusicPlcInitParam_t* sampleParam)
{
	//check
	if (!pMusicPlcStateIn
		|| !sampleParam
		|| !sampleParam->basePorting)
		return MUSIC_PLC_RET_FAIL;
	((MusicPlcState*)pMusicPlcStateIn)->isInited = false;
	i32 ret = ((MusicPlcState*)pMusicPlcStateIn)->musicplc.Init(
		sampleParam->basePorting,
		sampleParam->channels,
		sampleParam->width,
		sampleParam->fsHz,
		sampleParam->frameSamples,
		sampleParam->overlapSamples,
		sampleParam->holdSamplesAfterLost,
		sampleParam->attenuateSamplesAfterLost,
		sampleParam->gainSamplesAfterNoLost);
	if (ret == MUSIC_PLC_RET_FAIL)
		return MUSIC_PLC_RET_FAIL;
	((MusicPlcState*)pMusicPlcStateIn)->isInited = true;
	return MUSIC_PLC_RET_SUCCESS;
}

int32_t MusicPlc_c::Set(void* pMusicPlcStateIn, MusicPlc_SetChhoose_e choose, void* val)
{
	//check
	if (!pMusicPlcStateIn
		|| choose >= MUSIC_PLC_SET_CHOOSE_MAX)
		return MUSIC_PLC_RET_FAIL;
	MusicPlcState* pMusicPlc = (MusicPlcState*)pMusicPlcStateIn;
	if (pMusicPlc->isInited == false)
		return MUSIC_PLC_RET_FAIL;

	switch (choose)
	{
	case MUSIC_PLC_SET_SEEK_SAMPLES:
		pMusicPlc->musicplc.Set_SeekSamples((u32)val); break;
	case MUSIC_PLC_SET_MATCH_SAMPLES:
		pMusicPlc->musicplc.Set_MatchSamples((u32)val); break;
	case MUSIC_PLC_SET_CHANNEL_SELECT:
		pMusicPlc->musicplc.Set_ChannelSelect((u32)val); break;
	default:
		break;
	}
	return MUSIC_PLC_RET_SUCCESS;
}

int32_t MusicPlc_c::Get(void* pMusicPlcStateIn, MusicPlc_GetChhoose_e choose, void* val)
{
#if 0
	//check
	if (!pMusicPlcStateIn
		|| choose >= MUSIC_PLC_GET_CHOOSE_MAX
		|| !val)
		return MUSIC_PLC_RET_FAIL;
	MusicPlcState* pMusicPlc = (MusicPlcState*)pMusicPlcStateIn;
	if (pMusicPlc->isInited == false)
		return MUSIC_PLC_RET_FAIL;
	
	switch (choose)
	{
#if 0
	case AUDIO_SPEED_CONTROL_SET_CHOOSE_SPEEDQ8:
		pState->speed = (f32)(u32)val[0] / (1 << 8);
#endif
	default:
		break;
	}
#endif
	return MUSIC_PLC_RET_SUCCESS;
}

int32_t MusicPlc_c::Run(void* pMusicPlcStateIn, uint8_t* in, int32_t inLen, uint8_t* out, int32_t* outLen, uint16_t isLost)
{
	//check
	if (!out
		|| !outLen)
		return MUSIC_PLC_RET_FAIL;

	return ((MusicPlcState*)pMusicPlcStateIn)->musicplc.Run(in, inLen, out, outLen, isLost);
}

int32_t MusicPlc_c::StateDeInit(void* pMusicPlcStateIn)
{
	return ((MusicPlcState*)pMusicPlcStateIn)->musicplc.DeInit();
}
}
#endif