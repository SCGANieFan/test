#pragma once
#include"Algo.BasePorting.h"

namespace MusicPlc_ns{
#define MUSIC_PLC_RET_SUCCESS 0
#define MUSIC_PLC_RET_FAIL -1
typedef struct
{
	AlgoBasePorting_c* basePorting;
	int32_t fsHz;
	int16_t channels;
	int16_t width;
	int16_t frameSamples;
	int32_t overlapSamples;
	int32_t holdSamplesAfterLost;
	int32_t attenuateSamplesAfterLost;	//Attenuation after packet loss
	int32_t gainSamplesAfterNoLost;//Gain after obtaining the package
}MusicPlcInitParam_t;


typedef enum {
	MUSIC_PLC_SET_SEEK_SAMPLES = 0,
	MUSIC_PLC_SET_MATCH_SAMPLES,
	MUSIC_PLC_SET_CHANNEL_SELECT,
	MUSIC_PLC_SET_CHOOSE_MAX,
}MusicPlc_SetChhoose_e;

typedef enum {
	MUSIC_PLC_GET_CHOOSE_MAX,
}MusicPlc_GetChhoose_e;


class MusicPlc_c
{
public:
	MusicPlc_c();
	~MusicPlc_c();
public:
	static int32_t GetStateSize();
	static int32_t StateInit(void* pMusicPlcStateIn, MusicPlcInitParam_t* sampleParam);
	static int32_t Set(void* pMusicPlcStateIn, MusicPlc_SetChhoose_e choose, void* val);
	static int32_t Get(void* pMusicPlcStateIn, MusicPlc_GetChhoose_e choose, void* val);
	static int32_t Run(void* pMusicPlcStateIn, uint8_t* in, int32_t inLen, uint8_t* out, int32_t* outLen, uint16_t isLost);
	static int32_t StateDeInit(void* pMusicPlcStateIn);
};


}