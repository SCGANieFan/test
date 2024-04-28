#pragma once

#include<stdint.h>
#include"AudioSamples.h"

#if 0
#define STATIC static
#define INLINE inline
#else
#define STATIC 
#define INLINE
#endif

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
	int32_t fsHz;
	int16_t channels;
	int16_t width;

	int32_t lostCount;
	int32_t overlapInSamples;
	int32_t frameSamples;
	int32_t frameSamplesInner;
	int32_t decaySamples;

	AudioSamples* inHistory;
	AudioSamples* infuture;
	AudioSamples* fillSignal;
	AudioSamples* muteFactor;

	int32_t decaySamplesNow;
	int32_t fillSignalSampleIndex;
	int32_t matchSamples;
	int32_t seekSamples;
	bool is24bL;
	bool quickDeal;
	uint8_t reserve[4];
}MusicPlcState;



STATIC INLINE int32_t MusicPlcGetMuteFactorSamples(int32_t frameSamples);

STATIC INLINE int32_t MusicPlcGetFillSignalSamples(int32_t frameSamples);

STATIC INLINE int32_t MusicPlcGetHistorySamples(int32_t overlapInSamples, int32_t frameSamples);

STATIC INLINE int32_t MusicPlcGetFutureSamples(int32_t frameSamples);

STATIC INLINE int32_t MusicPlcGetBufSamples(int32_t overlapInSamples, int32_t frameSamplesInner, int32_t frameSamples);

STATIC INLINE bool MusicPlcCheckParam(int32_t overlapMs, int32_t frameSamples, int32_t fsHz, int16_t channels);


STATIC int32_t MusicPlcStateInitInner(void* pMusicPlcStateIn, int32_t overlapInSamples,
	int32_t decayTimeMs, MusicPlcSampleParam* sampleParam, int16_t width);

STATIC int32_t MusicPlcRun(MusicPlcState* pMusicPlc, AudioSamples* pIn, int32_t* inUsed, AudioSamples* pOut, int32_t* outLen, bool isLost);
