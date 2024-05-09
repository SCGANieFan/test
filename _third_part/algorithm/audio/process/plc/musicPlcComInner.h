#pragma once
#include"Algo.AudioSamlpes.h"
#include"Algo.AS_Calculator.h"


#define MUSIC_PLC_MIN_FRAME_MS 10
#define MUSIC_PLC_SEEK_MS_DEFAULT 2
#define MUSIC_PLC_MATCH_MS_DEFAULT 2

#define MAX(x1,x2) ((x1)>(x2)?(x1):(x2))
#define MIN(x1,x2) ((x1)<(x2)?(x1):(x2))

typedef enum {
	MUTE_MODE_NOMUTE = 0,
	MUTE_MODE_DOMUTE,
	MUTE_MODE_DOMUTE_ZERO,
}MUTE_MODE;

typedef struct
{
	AudioInfo info;
	AudioInfo muteInfo;
	i32 frameSamples;
	
	i32 lostCount;
	i32 overlapInSamples;	
	i32 frameSamplesInner;
	i32 decaySamples;

	AS_Calculator *asCalculator;
	AudioSamples inHistory;
	AudioSamples infuture;
	AudioSamples fillSignal;
	AudioSamples muteFactor;

	i32 decaySamplesNow;
	i32 fillSignalSampleIndex;
	i32 matchSamples;
	i32 seekSamples;
	b1 quickDeal;
}MusicPlcState;


void* MusicplcMalloc(i32 size);

void MusicplcFree(void* ptr);

b1 MusicPlcCheckParam(i32 overlapMs, i32 frameSamples, i32 fsHz, i16 channels);

i32 MusicPlcStateInitInner(MusicPlcState* pMusicPlcStateIn, i32 overlapInSamples, i32 decayTimeMs);
	
i32 MusicPlcRun(MusicPlcState* pMusicPlc, AudioSamples* pIn, i32* inUsed, AudioSamples* pOut, i32* outLen, b1 isLost);

i32 MusicPlcStateDeInitInner(MusicPlcState* pMusicPlc);
