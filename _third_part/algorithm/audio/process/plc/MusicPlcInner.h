#pragma once
#include"Algo.BasePorting.Inner.h"
#include"Algo.Macro.h"
#include"Algo.Type.h"
#include"Algo.AudioSamlpes.h"
#include"Algo.AudioCal.OverlapAdd.h"
#include"Algo.AudioCal.Muter.h"
#include"MusicPlcFillSignal.h"
#include"Algo.AudioCal.WaveFormMatch.h"
#include"MusicPlc.h"

using namespace Algo;
using namespace Audio;


#define MUSIC_PLC_MIN_FRAME_MS (15)
//#define MUSIC_PLC_SEEK_MS_DEFAULT (8)
//#define MUSIC_PLC_MATCH_MS_DEFAULT (2)

class MusicPlc_c
{
public:
	MusicPlc_c() {};
	~MusicPlc_c() {};
public:
	void GoodFrame()
	{
		if (lostCount > 0){
			_overlapAdd.Start();
			lostCount = 0;
			muterAfterNoLost.Sync(muterAfterLost);
		}

		if (muterAfterNoLost.IsMuteFinish()
			&& _overlapAdd.IsFinish()){
			isQuickDeal = true;
			return;
		}
		
		//infuture
		if (!_overlapAdd.IsFinish()){
			fillSignal.Output(infuture, frameSamples);
			_overlapAdd.DoOverlapAdd(
				infuture.GetBufInSample(0),
				infuture.GetBufInSample(0),
				pIn.GetBufInSample(0),
				frameSamples);
		}
		else
			infuture.Append(pIn, 0, frameSamples);
	}

	void LostFrame()
	{
		lostCount += 1;
		if (lostCount == 1)
		{
#if 1
			i32 bestLag = _waveFormMatch.DoWaveFormMatch(
				inHistory.GetBufInSample(0),
				inHistory.GetBufInSample(inHistory.GetSamplesMax() - overlapSamples),
				_seekSamples,
				_matchSamples);
#else		
			i32 bestLag = 0;
#endif
			//overlap add
#if 1
			_overlapAdd.Start();
			_overlapAdd.DoOverlapAdd(
				inHistory.GetBufInSample(inHistory.GetSamplesMax() - overlapSamples),
				inHistory.GetBufInSample(inHistory.GetSamplesMax() - overlapSamples),
				inHistory.GetBufInSample(bestLag),
				overlapSamples);
#endif
			//fill signal
			fillSignal.Input(
				inHistory,
				bestLag + overlapSamples,
				inHistory.GetSamplesMax() - (bestLag + overlapSamples));

			//state updata
			muterAfterLost.Sync(muterAfterNoLost);
		}
		//infuture
		fillSignal.Output(infuture, infuture.GetSamplesMax());
	}

public:
	i32 Init(AlgoBasePorting *basePorting,i16 channels, i16 width,i32 sampleRate,
			i32 frameSamplesIn, i32 overlapSamplesIn,i32 attenSamplesAfterLostIn,i32 gainSamplesAfterNoLostIn,
			i32 seekSamplesIn, i32 matchSamplesIn) {
		if (!basePorting->Malloc
			|| !basePorting->Free)
			return MUSIC_PLC_RET_FAIL;
		if (channels < 1)
			return MUSIC_PLC_RET_FAIL;
		if (width != 2 && width != 3 && width != 4)
			return MUSIC_PLC_RET_FAIL;
		if (frameSamplesIn < 1
			|| overlapSamplesIn < 0
			|| attenSamplesAfterLostIn < 0
			|| gainSamplesAfterNoLostIn < 0
			|| seekSamplesIn < 0
			|| matchSamplesIn < 0)
			return MUSIC_PLC_RET_FAIL;

		ALGO_MEM_SET(this, 0, sizeof(MusicPlc_c));
		MM.Init(basePorting);
		info.Init(sampleRate, width, channels);
		frameSamples = frameSamplesIn;
		overlapSamples = overlapSamplesIn;
		BufferSamples bufferSamples;
		bufferSamples._samples = overlapSamples + MAX(frameSamples, (i32)(info._rate * MUSIC_PLC_MIN_FRAME_MS / (1000)));
		bufferSamples._buf = (u8*)MM.Malloc(bufferSamples._samples * info._bytesPerSample);
		ALGO_MEM_SET(bufferSamples._buf, 0, bufferSamples._samples * info._bytesPerSample);
		inHistory.Init(&info, &bufferSamples);
		inHistory.Append(inHistory, 0, inHistory.GetSamplesMax());
		bufferSamples._samples = frameSamples;
		bufferSamples._buf = (u8*)MM.Malloc(bufferSamples._samples * info._bytesPerSample);
		ALGO_MEM_SET(bufferSamples._buf, 0, bufferSamples._samples * info._bytesPerSample);
		infuture.Init(&info, &bufferSamples);
		fillSignal.Init(&MM, inHistory.GetSamplesMax() - overlapSamples, &info);
		muterAfterLost.Init(&MM, &info, Muter_c::WindowChoose_e::COSINE, Muter_c::DirChoose_e::ATTENUATION, attenSamplesAfterLostIn);
		muterAfterNoLost.Init(&MM, &info, Muter_c::WindowChoose_e::COSINE, Muter_c::DirChoose_e::AMPLIFICATION, gainSamplesAfterNoLostIn);
		_overlapAdd.Init(&MM,&info, OverlapAdd_c::WindowChoose::Line, overlapSamples);

		_seekSamples = seekSamplesIn;
		_matchSamples = matchSamplesIn;

		_waveFormMatch.Init(WaveFormMatch_c::FuncMode_e::ACCORELATION, &info);
		return MUSIC_PLC_RET_SUCCESS;
	}
	i32 DeInit() {
		MM.FreeAll();
		return MUSIC_PLC_RET_SUCCESS;
	}
	i32 Run(uint8_t* in, int32_t inLen, uint8_t* out, int32_t* outLen, bool isLost)
	{
#if 0
		if (isLost == false) {
			if (!in
				|| inLen < (frameSamples * info._bytesPerSample))
				return MUSIC_PLC_RET_FAIL;
		}
		if (*outLen < frameSamples * info._bytesPerSample)
			return MUSIC_PLC_RET_FAIL;
#endif
		BufferSamples bufferSamples;
		bufferSamples._buf = in;
		bufferSamples._samples = frameSamples;
		pIn.Init(&info, &bufferSamples);

		bufferSamples._buf = out;
		bufferSamples._samples = frameSamples;
		pOut.Init(&info, &bufferSamples);

		if (isLost == false){
			GoodFrame();
		}
		else{
			LostFrame();
		}
		if (isQuickDeal){
			isQuickDeal = false;
			inHistory.Clear(frameSamples);
			inHistory.Append(pIn, 0, frameSamples);
			pOut.Append(inHistory, inHistory.GetSamplesMax() - overlapSamples - frameSamples, frameSamples);
			*outLen = pOut.GetValidSamples() * info._bytesPerSample;
			return MUSIC_PLC_RET_SUCCESS;
		}

		//muting
		if (isLost == false){
			muterAfterNoLost.DoMute(infuture.GetBufInSample(0), frameSamples);
		}
		else{
			muterAfterLost.DoMute(infuture.GetBufInSample(0), frameSamples);
		}

		//out
		inHistory.Clear(frameSamples);
		inHistory.Append(infuture, 0, frameSamples);
		pOut.Append(inHistory, inHistory.GetSamplesMax() - overlapSamples - frameSamples, frameSamples);

		infuture.Clear(infuture.GetValidSamples());
		*outLen = pOut.GetValidSamples() * info._bytesPerSample;
		return MUSIC_PLC_RET_SUCCESS;
	}
private:
	MemoryManger_c MM;
	AudioInfo info;
	i32 frameSamples;
	i32 lostCount;
	i32 overlapSamples;
	i32 _seekSamples;
	i32 _matchSamples;
	AudioSamples pIn;
	AudioSamples pOut;
	AudioSamples inHistory;
	AudioSamples infuture;
	MusicPlcFillSignal_c fillSignal;
	Muter_c muterAfterLost;
	Muter_c muterAfterNoLost;
	OverlapAdd_c _overlapAdd;
	WaveFormMatch_c _waveFormMatch;
	b1 isQuickDeal;
};




