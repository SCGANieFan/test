#if 1
#include"MusicPlcInner.h"
#include"MusicPlc.h"
typedef struct
{
	MusicPlc_c musicplc;
}MusicPlcState;

EXTERNC {
	int32_t MusicPlc_GetStateSize()
	{
		return sizeof(MusicPlcState);
	}

	int32_t MusicPlc_StateInit(void* pMusicPlcStateIn, MusicPlcInitParam* sampleParam)
	{
		//check
		if (!pMusicPlcStateIn
			|| !sampleParam
			|| !sampleParam->basePorting)
			return MUSIC_PLC_RET_FAIL;
		return ((MusicPlcState*)pMusicPlcStateIn)->musicplc.Init(
			sampleParam->basePorting,
			sampleParam->channels,
			sampleParam->width,
			sampleParam->fsHz,
			sampleParam->frameSamples,
			sampleParam->overlapSamples,
			sampleParam->attenuateSamplesAfterLost,
			sampleParam->gainSamplesAfterNoLost,
			sampleParam->seekSamples,
			sampleParam->matchSamples);
	}

	int32_t MusicPlc_Set(void* pMusicPlcStateIn, MusicPlc_SetChhoose_e choose, void** val)
	{
#if 0
		//check
		if (!pMusicPlcStateIn
			|| choose >= MUSIC_PLC_SET_CHOOSE_MAX
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

	int32_t MusicPlc_Get(void* pMusicPlcStateIn, MusicPlc_GetChhoose_e choose, void** val)
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

	int32_t MusicPlc_Run(void* pMusicPlcStateIn, uint8_t* in, int32_t inLen, uint8_t* out, int32_t* outLen, bool isLost)
	{
		//check
		if (!out
			|| !outLen)
			return MUSIC_PLC_RET_FAIL;

		return ((MusicPlcState*)pMusicPlcStateIn)->musicplc.Run(in, inLen, out, outLen, isLost);
	}

	int32_t MusicPlc_StateDeInit(void* pMusicPlcStateIn)
	{
		return ((MusicPlcState*)pMusicPlcStateIn)->musicplc.DeInit();
	}
}

#endif