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

namespace MusicPlc_ns{

#define MUSIC_PLC_SUPPORT_CH_SELECT 0
#define MUSIC_PLC_SEEK_MS_DEFAULT (8)
#define MUSIC_PLC_MATCH_MS_DEFAULT (2)
#define MUSIC_PLC_MIN_FRAME_MS (MUSIC_PLC_SEEK_MS_DEFAULT+MUSIC_PLC_MATCH_MS_DEFAULT+5)

#if 1
class MusicPlcCom_c {
public:
	MusicPlcCom_c() {}
	~MusicPlcCom_c() {}
public:
	MemoryManger_c MM;
	AudioInfo info;
	i32 frameSamples;
	i32 overlapSamples;
	i32 _holdSamplesAfterLost;
	i32 _seekSamples;
	i32 _matchSamples;
	AudioSamples inHistory;
	AudioSamples infuture;
	MusicPlcFillSignal_c fillSignal;
	Muter_c muterAfterLost;
	Muter_c muterAfterNoLost;
	OverlapAdd_c _overlapAdd;
	WaveFormMatch_c _waveFormMatch;
};
#endif

#if MUSIC_PLC_SUPPORT_CH_SELECT
class MusicPlcCh_c {
public:
	MusicPlcCh_c() {};
	~MusicPlcCh_c() {};

private:
	void GoodFrame(AudioSamples& pIn, AudioSamples& pOut, i16 ch)
	{
		if (_lostCount[ch] > 0) {
			_com->_overlapAdd.Start(ch);
			_lostCount[ch] = 0;
			_com->muterAfterNoLost.Sync(_com->muterAfterLost, ch);
		}

		if (_com->muterAfterNoLost.IsMuteFinish(ch)
			&& _com->_overlapAdd.IsFinish(ch)) {
			_isQuickDeal[ch] = true;
			return;
		}

		//infuture
		if (!_com->_overlapAdd.IsFinish(ch)) {
			_com->fillSignal.Output(_com->infuture, _com->frameSamples, ch);
#if 1
			_com->_overlapAdd.DoOverlapAdd(
				_com->infuture.GetBufInSample(0),
				_com->infuture.GetBufInSample(0),
				pIn.GetBufInSample(0),
				_com->frameSamples,
				ch);
#endif
		}
		else
			_com->infuture.Append(pIn, 0, _com->frameSamples, ch);
	}
	void LostFrame(AudioSamples& pIn, AudioSamples& pOut, i16 ch)
	{
		_lostCount[ch] += 1;
		if (_lostCount[ch] == 1)
		{
			_holdAfterLostSamplesNow[ch] = 0;
#if 1
			i32 bestLag = _com->_waveFormMatch.DoWaveFormMatch(
				_com->inHistory.GetBufInSample(0, 0),
				_com->inHistory.GetBufInSample(_com->inHistory.GetSamplesMax() - _com->overlapSamples, 0),
				1 << ch,
				_com->_seekSamples,
				_com->_matchSamples);
#else		
			i32 bestLag = 0;
#endif
			//overlap add
#if 1
			_com->_overlapAdd.Start(ch);
			_com->_overlapAdd.DoOverlapAdd(
				_com->inHistory.GetBufInSample(_com->inHistory.GetSamplesMax() - _com->overlapSamples),
				_com->inHistory.GetBufInSample(_com->inHistory.GetSamplesMax() - _com->overlapSamples),
				_com->inHistory.GetBufInSample(bestLag),
				_com->overlapSamples,
				ch);
#endif
			//fill signal
#if 1
			_com->fillSignal.Input(
				_com->inHistory,
				bestLag + _com->overlapSamples,
				_com->inHistory.GetSamplesMax() - (bestLag + _com->overlapSamples),
				ch);
#endif
			//state updata
			_com->muterAfterLost.Sync(_com->muterAfterNoLost, ch);
		}
		//infuture
		_com->fillSignal.Output(_com->infuture, _com->infuture.GetSamplesMax(), ch);
	}

public:
	INLINE void Init(MusicPlcCom_c* com) {
		_com = com;
		for (i32 ch = 0; ch < 16; ch++) {
			_lostCount[ch] = 0;
			_isQuickDeal[ch] = 0;
			_holdAfterLostSamplesNow[ch] = 0;
		}
	}

	INLINE i32 Run(AudioSamples& pIn, AudioSamples& pOut, i16 ch, b1 isLost, b1 isSelected) {
#if 1
		if (!isSelected) {
			_com->inHistory.Clear(_com->frameSamples, ch);
			_com->inHistory.Append(pIn, 0, _com->frameSamples, ch);
			pOut.Append(
				_com->inHistory,
				_com->inHistory.GetSamplesMax() - _com->overlapSamples - _com->frameSamples,
				_com->frameSamples,
				ch);
			return MUSIC_PLC_RET_SUCCESS;
		}
		if (isLost == false) {
			GoodFrame(pIn, pOut, ch);
		}
		else {
			LostFrame(pIn, pOut, ch);
		}
		if (_isQuickDeal[ch]) {
			_isQuickDeal[ch] = false;
			_com->inHistory.Clear(_com->frameSamples, ch);
			_com->inHistory.Append(pIn, 0, _com->frameSamples, ch);
			pOut.Append(
				_com->inHistory,
				_com->inHistory.GetSamplesMax() - _com->overlapSamples - _com->frameSamples,
				_com->frameSamples,
				ch);
			//*outLen = pOut.GetValidSamples(ch) * _com->info._bytesPerSample;
			return MUSIC_PLC_RET_SUCCESS;
		}

		//muting
#if 1
		if (isLost == false) {
			_com->muterAfterNoLost.DoMute(_com->infuture.GetBufInSample(0, ch), _com->frameSamples, ch);
		}
		else { 
			i32 holdSamples = _com->_holdSamplesAfterLost - _holdAfterLostSamplesNow[ch];
			holdSamples = MIN(holdSamples, _com->frameSamples);
			i32 doMuteSamples = _com->frameSamples - holdSamples;
			_com->muterAfterLost.DoMute(_com->infuture.GetBufInSample(holdSamples, ch), doMuteSamples, ch);
			_holdAfterLostSamplesNow[ch] += holdSamples;
		}
#endif
		//out
		_com->inHistory.Clear(_com->frameSamples, ch);
		_com->inHistory.Append(_com->infuture, 0, _com->frameSamples, ch);
		pOut.Append(
			_com->inHistory,
			_com->inHistory.GetSamplesMax() - _com->overlapSamples - _com->frameSamples,
			_com->frameSamples,
			ch);

		_com->infuture.Clear(_com->infuture.GetValidSamples(ch), ch);
		//*outLen = pOut.GetValidSamples() * info._bytesPerSample;
		return MUSIC_PLC_RET_SUCCESS;
#endif
	}
public:
private:
	i32 _lostCount[16];
	b1 _isQuickDeal[16];
	i32 _holdAfterLostSamplesNow[16];
	MusicPlcCom_c* _com;
};
#else

class MusicPlcChAll_c {
public:
	MusicPlcChAll_c() {};
	~MusicPlcChAll_c() {};

private:
	void GoodFrame(AudioSamples& pIn, AudioSamples& pOut)
	{
		if (_lostCount > 0) {
			_com->_overlapAdd.Start();
			_lostCount = 0;
			_com->muterAfterNoLost.Sync(_com->muterAfterLost);
		}

		if (_com->muterAfterNoLost.IsMuteFinish()
			&& _com->_overlapAdd.IsFinish()) {
			_isQuickDeal = true;
			return;
		}

		//infuture
		if (!_com->_overlapAdd.IsFinish()) {
			_com->fillSignal.Output(_com->infuture, _com->frameSamples);
#if 1
			_com->_overlapAdd.DoOverlapAdd(
				_com->infuture.GetBufInSample(0),
				_com->infuture.GetBufInSample(0),
				pIn.GetBufInSample(0),
				_com->frameSamples);
#endif
		}
		else
			_com->infuture.Append(pIn, 0, _com->frameSamples);
	}
	void LostFrame(AudioSamples& pIn, AudioSamples& pOut)
	{
		_lostCount += 1;
		if (_lostCount == 1)
		{
			_holdAfterLostSamplesNow = 0;
#if 1
			i32 bestLag = _com->_waveFormMatch.DoWaveFormMatch(
				_com->inHistory.GetBufInSample(0),
				_com->inHistory.GetBufInSample(_com->inHistory.GetSamplesMax() - _com->overlapSamples),
				_com->_seekSamples,
				_com->_matchSamples);
#else		
			i32 bestLag = 0;
#endif
			//overlap add
#if 1
			_com->_overlapAdd.Start();
			_com->_overlapAdd.DoOverlapAdd(
				_com->inHistory.GetBufInSample(_com->inHistory.GetSamplesMax() - _com->overlapSamples),
				_com->inHistory.GetBufInSample(_com->inHistory.GetSamplesMax() - _com->overlapSamples),
				_com->inHistory.GetBufInSample(bestLag),
				_com->overlapSamples);
#endif
			//fill signal
			_com->fillSignal.Input(
				_com->inHistory,
				bestLag + _com->overlapSamples,
				_com->inHistory.GetSamplesMax() - (bestLag + _com->overlapSamples));
			//state updata
			_com->muterAfterLost.Sync(_com->muterAfterNoLost);
		}
		//infuture
		_com->fillSignal.Output(_com->infuture, _com->infuture.GetSamplesMax());
	}
public:
	INLINE void Init(MusicPlcCom_c* com) {
		_com = com;
		_lostCount = 0;
		_isQuickDeal = 0;
		_holdAfterLostSamplesNow = 0;
	}

	INLINE i32 Run(AudioSamples& pIn, AudioSamples& pOut, b1 isLost) {
#if 1
		if (isLost == false) {
			GoodFrame(pIn, pOut);
		}
		else {
			LostFrame(pIn, pOut);
		}
		if (_isQuickDeal) {
			_isQuickDeal = false;
			_com->inHistory.Clear(_com->frameSamples);
			_com->inHistory.Append(pIn, 0, _com->frameSamples);
			pOut.Append(
				_com->inHistory,
				_com->inHistory.GetSamplesMax() - _com->overlapSamples - _com->frameSamples,
				_com->frameSamples);
			//*outLen = pOut.GetValidSamples(ch) * _com->info._bytesPerSample;
			return MUSIC_PLC_RET_SUCCESS;
		}

		//muting
#if 1
		if (isLost == false) {
			_com->muterAfterNoLost.DoMute(_com->infuture.GetBufInSample(0), _com->frameSamples);
		}
		else {
			i32 holdSamples = _com->_holdSamplesAfterLost - _holdAfterLostSamplesNow;
			holdSamples = MIN(holdSamples, _com->frameSamples);
			i32 doMuteSamples = _com->frameSamples - holdSamples;
			_com->muterAfterLost.DoMute(_com->infuture.GetBufInSample(holdSamples), doMuteSamples);
			_holdAfterLostSamplesNow += holdSamples;
		}
#endif
		//out
		_com->inHistory.Clear(_com->frameSamples);
		_com->inHistory.Append(_com->infuture, 0, _com->frameSamples);
		pOut.Append(
			_com->inHistory,
			_com->inHistory.GetSamplesMax() - _com->overlapSamples - _com->frameSamples,
			_com->frameSamples);

		_com->infuture.Clear(_com->infuture.GetValidSamples());
		//*outLen = pOut.GetValidSamples() * info._bytesPerSample;
		return MUSIC_PLC_RET_SUCCESS;
#endif
	}
public:
private:
	i32 _lostCount;
	b1 _isQuickDeal;
	i32 _holdAfterLostSamplesNow;
	MusicPlcCom_c* _com;
};
#endif

class MusicPlcInner_c
{
public:
	MusicPlcInner_c() {};
	~MusicPlcInner_c() {};
public:
	i32 Init(AlgoBasePorting_c* basePorting, i16 channels, i16 width, i32 sampleRate,
		i32 frameSamplesIn, i32 overlapSamplesIn, i32 holdSamplesAfterLostIn, i32 attenSamplesAfterLostIn, i32 gainSamplesAfterNoLostIn,
		i32 seekSamplesIn, i32 matchSamplesIn
	) {
		if (!basePorting)
			return MUSIC_PLC_RET_FAIL;
		if (channels < 1)
			return MUSIC_PLC_RET_FAIL;
		if (width != 2 && width != 3 && width != 4)
			return MUSIC_PLC_RET_FAIL;
		if (frameSamplesIn < 1
			|| overlapSamplesIn < 0
			|| attenSamplesAfterLostIn < 0
			|| gainSamplesAfterNoLostIn < 0
			|| holdSamplesAfterLostIn < 0
			|| seekSamplesIn < 0
			|| matchSamplesIn < 0)
			return MUSIC_PLC_RET_FAIL;

		ALGO_MEM_SET(this, 0, sizeof(MusicPlcInner_c));
		_com.MM.Init(basePorting);
		_com.info.Init(sampleRate, width, channels);
		_com.frameSamples = frameSamplesIn;
		_com.overlapSamples = overlapSamplesIn;
		_com._seekSamples = seekSamplesIn;
		_com._matchSamples = matchSamplesIn;

		BufferSamples bufferSamples;
#if 0
		bufferSamples._samples = _com.overlapSamples + MAX(_com.frameSamples, (i32)(_com.info._rate * MUSIC_PLC_MIN_FRAME_MS / (1000)));
#else
		bufferSamples._samples = _com.frameSamples + _com.overlapSamples;
		bufferSamples._samples = MAX(bufferSamples._samples, _com._seekSamples + _com._matchSamples + (5 * _com.info._rate) / 1000);
#endif
		bufferSamples._buf = (u8*)_com.MM.Malloc(bufferSamples._samples * _com.info._bytesPerSample);
		ALGO_MEM_SET(bufferSamples._buf, 0, bufferSamples._samples * _com.info._bytesPerSample);
		_com.inHistory.Init(&_com.info, &bufferSamples);
		//inHistory.Append(inHistory, 0, inHistory.GetSamplesMax());
		for (i16 ch = 0; ch < _com.info._channels; ch++)
			_com.inHistory.Append(_com.inHistory.GetSamplesMax(), ch);
		bufferSamples._samples = _com.frameSamples;
		bufferSamples._buf = (u8*)_com.MM.Malloc(bufferSamples._samples * _com.info._bytesPerSample);
		ALGO_MEM_SET(bufferSamples._buf, 0, bufferSamples._samples * _com.info._bytesPerSample);
		_com.infuture.Init(&_com.info, &bufferSamples);
		_com.fillSignal.Init(&_com.MM, _com.inHistory.GetSamplesMax() - _com.overlapSamples, &_com.info);
#if 1
		_com.muterAfterLost.Init(&_com.MM, &_com.info, Muter_c::WindowChoose_e::COSINE, Muter_c::DirChoose_e::ATTENUATION, attenSamplesAfterLostIn);
		_com.muterAfterNoLost.Init(&_com.MM, &_com.info, Muter_c::WindowChoose_e::COSINE, Muter_c::DirChoose_e::AMPLIFICATION, gainSamplesAfterNoLostIn);
#else
		_com.muterAfterLost.Init(&_com.MM, &_com.info, Muter_c::WindowChoose_e::LINE, Muter_c::DirChoose_e::ATTENUATION, attenSamplesAfterLostIn);
		_com.muterAfterNoLost.Init(&_com.MM, &_com.info, Muter_c::WindowChoose_e::LINE, Muter_c::DirChoose_e::AMPLIFICATION, gainSamplesAfterNoLostIn);
#endif
		_com._overlapAdd.Init(&_com.MM, &_com.info, OverlapAdd_c::WindowChoose::Line, _com.overlapSamples);
		_com._holdSamplesAfterLost = holdSamplesAfterLostIn;
		//Set_SeekSamples(MUSIC_PLC_SEEK_MS_DEFAULT * sampleRate / 1000);
		//Set_MatchSamples(MUSIC_PLC_MATCH_MS_DEFAULT * sampleRate / 1000);
		_com._waveFormMatch.Init(
			WaveFormMatch_c::FuncMode_e::ACCORELATION,
			&_com.info);
		_channelSelect = 0xffff;
#if MUSIC_PLC_SUPPORT_CH_SELECT
		_musicPlcCh.Init(&_com);
#else
		_musicPlcChAll.Init(&_com);
#endif
		return MUSIC_PLC_RET_SUCCESS;
	}
	i32 DeInit() {
		_com.MM.FreeAll();
		return MUSIC_PLC_RET_SUCCESS;
	}
	i32 Run(uint8_t* in, int32_t inLen, uint8_t* out, int32_t* outLen, u16 isLost)
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
		bufferSamples._samples = _com.frameSamples;
		AudioSamples pIn;
		pIn.Init(&_com.info, &bufferSamples);

		bufferSamples._buf = out;
		bufferSamples._samples = _com.frameSamples;
		AudioSamples pOut;
		pOut.Init(&_com.info, &bufferSamples);
#if MUSIC_PLC_SUPPORT_CH_SELECT
		for (i16 ch = 0; ch < _com.info._channels; ch++) {
			b1 isSelectCh = _channelSelect & (1 << ch);
			b1 isLostCh = isLost & (1 << ch);
			_musicPlcCh.Run(pIn, pOut, ch, isLostCh, isSelectCh);
		}
#else
		b1 isLost0 = isLost ? true : false;
		_musicPlcChAll.Run(pIn, pOut, isLost0);
#endif
		* outLen = pOut.GetValidSamples(0) * _com.info._bytesPerSample;
		return MUSIC_PLC_RET_SUCCESS;
	}
public:
#if 0
	void Set_SeekSamples(i32 seekSamples) {
		if (seekSamples < 0)
			return;
		_com._seekSamples = seekSamples;
		if ((_com._matchSamples + _com._seekSamples) > _com.inHistory.GetSamplesMax())
			_com._seekSamples = _com.inHistory.GetSamplesMax() - _com._matchSamples;
	}
	void Set_MatchSamples(i32 matchSamples) {
		if (matchSamples < 0)
			return;
		_com._matchSamples = matchSamples;
		if (_com.overlapSamples > 0)
			_com._matchSamples = MIN(_com._matchSamples, _com.overlapSamples);
		if ((_com._matchSamples + _com._seekSamples) > _com.inHistory.GetSamplesMax())
			_com._matchSamples = _com.inHistory.GetSamplesMax() - _com._seekSamples;
	}
#endif
	void Set_ChannelSelect(u16 channelSelect) {
		_channelSelect = channelSelect;
	}
private:

	MusicPlcCom_c _com;
#if MUSIC_PLC_SUPPORT_CH_SELECT
	MusicPlcCh_c _musicPlcCh;
#else
	MusicPlcChAll_c _musicPlcChAll;
#endif
	u16 _channelSelect;
};


}
