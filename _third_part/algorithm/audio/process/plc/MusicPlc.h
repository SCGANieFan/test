#pragma once
#include"Algo.BasePorting.h"
#include"Algo.BasePorting.Inner.h"
#include"Algo.Macro.h"
#include"Algo.Type.h"
#include"Algo.AudioSamlpes.h"
#include"Algo.AudioCal.OverlapAdd.h"
#include"Algo.AudioCal.Muter.h"
#include"Algo.AudioCal.WaveFormMatch.h"
#include"MusicPlcFillSignal.h"

namespace MusicPlc_ns{
using namespace Algo;
using namespace Audio;

#define MUSIC_PLC_RET_SUCCESS 0
#define MUSIC_PLC_RET_FAIL -1

#ifdef WIN32
#define MUSIC_PLC_SUPPORT_CH_SELECT 0
#endif
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
	int32_t seekSamples;
	int32_t matchSamples;
}MusicPlcParam_t;


typedef enum {
	MUSIC_PLC_SET_SEEK_SAMPLES = 0,
	MUSIC_PLC_SET_MATCH_SAMPLES,
	MUSIC_PLC_SET_CHANNEL_SELECT,
	MUSIC_PLC_SET_CHOOSE_MAX,
}MusicPlc_SetChhoose_e;

typedef enum {
	MUSIC_PLC_GET_CHOOSE_MAX,
}MusicPlc_GetChhoose_e;


template<class T, i8 type = (TypeIdentify_c::IsI16<T>() ? 0 : (TypeIdentify_c::IsI32<T>() ? 1 : 2))>
struct WaveFormMatchPlc_c : WaveFormMatch_c<i16, i16, u64>{};
template<class T>
struct WaveFormMatchPlc_c<T, 0> : WaveFormMatch_c<i16, i16, i64, 15> {};
template<class T>
struct WaveFormMatchPlc_c<T, 1> : WaveFormMatch_c<i32, i32, i64, 47> {};
template<class T>
struct WaveFormMatchPlc_c<T, 2> : WaveFormMatch_c<f32, f32, f32> {};

#if 1

template<class T>
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
	Muter_c<T> muterAfterLost;
	Muter_c<T> muterAfterNoLost;
	OverlapAdd_c<T> _overlapAdd;
	WaveFormMatchPlc_c<T> _waveFormMatch;
};
#endif

#if MUSIC_PLC_SUPPORT_CH_SELECT
template<class T>
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
			i32 bestLag = _com->_waveFormMatch.DoWaveFormMatchCh(
				(T*)_com->inHistory.GetBufInSample(0, 0),
				(T*)_com->inHistory.GetBufInSample(_com->inHistory.GetSamplesMax() - _com->overlapSamples, 0),
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
	INLINE void Init(MusicPlcCom_c<T>* com) {
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
	MusicPlcCom_c<T>* _com;
};
#else
template<class T>
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
			i32 bestLag = _com->_waveFormMatch.DoWaveFormMatchAllCh(
				(T*)_com->inHistory.GetBufInSample(0),
				(T*)_com->inHistory.GetBufInSample(_com->inHistory.GetSamplesMax() - _com->overlapSamples),
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
	INLINE void Init(MusicPlcCom_c<T>* com) {
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
			return MUSIC_PLC_RET_SUCCESS;
		}

#if 1
		//muting
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
		return MUSIC_PLC_RET_SUCCESS;
#endif
	}
public:
private:
	i32 _lostCount;
	b1 _isQuickDeal;
	i32 _holdAfterLostSamplesNow;
	MusicPlcCom_c<T>* _com;
};
#endif

template<class T>
class MusicPlc_c
{
public:
	MusicPlc_c() {}
	~MusicPlc_c() {}
public:
	static inline i32 Create(void **pHd, MusicPlcParam_t* param) {
		if (!param
			|| !param->basePorting
			|| param->channels < 1)
			return MUSIC_PLC_RET_FAIL;
#if 0
		if (param->width != 2 && param->width != 3 && param->width != 4)
			return MUSIC_PLC_RET_FAIL;
#endif
		if (param->frameSamples < 1
			|| param->overlapSamples < 0
			|| param->holdSamplesAfterLost < 0
			|| param->attenuateSamplesAfterLost < 0
			|| param->gainSamplesAfterNoLost < 0
			|| param->seekSamples < 0
			|| param->matchSamples < 0)
			return MUSIC_PLC_RET_FAIL;

#if 1
		int size = sizeof(MusicPlc_c);
		MusicPlc_c* hd = (MusicPlc_c*)param->basePorting->Malloc(size);
		if (!hd) {
			return MUSIC_PLC_RET_FAIL;
		}
		new(hd) MusicPlc_c();
#endif
		hd->_basePorting = param->basePorting;
		MusicPlcCom_c<T>* pCom = &hd->_com;
		pCom->MM.Init(hd->_basePorting);
		pCom->info.Init(param->fsHz, param->width, param->channels);
		pCom->frameSamples = param->frameSamples;
		pCom->overlapSamples = param->overlapSamples;
		pCom->_seekSamples = param->seekSamples;
		pCom->_matchSamples = param->matchSamples;

		BufferSamples bufferSamples;
#if 0
		bufferSamples._samples =pCom->overlapSamples + MAX(pCom->frameSamples, (i32)(pCom->info._rate * MUSIC_PLC_MIN_FRAME_MS / (1000)));
#else
		bufferSamples._samples = pCom->_seekSamples + pCom->_matchSamples + (5 * pCom->info._rate) / 1000;
		bufferSamples._samples = MAX(bufferSamples._samples, pCom->frameSamples + pCom->overlapSamples);
#endif
		bufferSamples._buf = (u8*)pCom->MM.Malloc(bufferSamples._samples *pCom->info._bytesPerSample);
		if (!bufferSamples._buf) {
			return MUSIC_PLC_RET_FAIL;
		}
		ALGO_MEM_SET(bufferSamples._buf, 0, bufferSamples._samples *pCom->info._bytesPerSample);
		pCom->inHistory.Init(&pCom->info, &bufferSamples);
		for (i16 ch = 0; ch <pCom->info._channels; ch++)
			pCom->inHistory.Append(pCom->inHistory.GetSamplesMax(), ch);
		bufferSamples._samples = pCom->frameSamples;
		bufferSamples._buf = (u8*)pCom->MM.Malloc(bufferSamples._samples * pCom->info._bytesPerSample);
		if (!bufferSamples._buf) {
			return MUSIC_PLC_RET_FAIL;
		}
		ALGO_MEM_SET(bufferSamples._buf, 0, bufferSamples._samples *pCom->info._bytesPerSample);
		pCom->infuture.Init(&pCom->info, &bufferSamples);
		pCom->fillSignal.Init(&pCom->MM,pCom->inHistory.GetSamplesMax() -pCom->overlapSamples, &pCom->info);
		pCom->muterAfterLost.Init(
			&pCom->MM, &pCom->info, 
			MuterWindowChoose_e::MUTER_WINDOW_CHOOSE_COSINE, MuterDirChoose_e::MUTER_DIR_CHOOSE_ATTENUATION, 
			param->attenuateSamplesAfterLost);
		pCom->muterAfterNoLost.Init(
			&pCom->MM, &pCom->info, 
			MuterWindowChoose_e::MUTER_WINDOW_CHOOSE_COSINE, MuterDirChoose_e::MUTER_DIR_CHOOSE_AMPLIFICATION, 
			param->gainSamplesAfterNoLost);
		pCom->_overlapAdd.Init(&pCom->MM, &pCom->info, OverlapAddWindowChoose_e::Line,pCom->overlapSamples);
		pCom->_holdSamplesAfterLost = param->holdSamplesAfterLost;
		pCom->_waveFormMatch.Init(WaveFormMatchFuncMode_e::WAVE_FORM_MATCH_FUNC_MODE_ACCORELATION, &pCom->info);
		hd->_channelSelect = 0xffff;
#if MUSIC_PLC_SUPPORT_CH_SELECT
		hd->_musicPlcCh.Init(pCom);
#else
		hd->_musicPlcChAll.Init(pCom);
#endif
		*pHd = hd;
		return MUSIC_PLC_RET_SUCCESS;
	}
	static inline int32_t Set(void* hd, MusicPlc_SetChhoose_e choose, void* val) {
		if (!hd
			|| choose >= MUSIC_PLC_SET_CHOOSE_MAX)
			return MUSIC_PLC_RET_FAIL;
		MusicPlc_c<T>* pMusicPlc = (MusicPlc_c<T>*)hd;
		switch (choose)
		{
#if 0
		case MUSIC_PLC_SET_SEEK_SAMPLES:
			pMusicPlc->_musicplc.Set_SeekSamples((u32)val); break;
		case MUSIC_PLC_SET_MATCH_SAMPLES:
			pMusicPlc->_musicplc.Set_MatchSamples((u32)val); break;
		case MUSIC_PLC_SET_CHANNEL_SELECT:
			pMusicPlc->_musicplc->Set_ChannelSelect((u32)val); break;
#endif
		default:
			break;
		}
		return MUSIC_PLC_RET_SUCCESS;
	}
	static inline int32_t Get(void* hd, MusicPlc_GetChhoose_e choose, void* val) {
		return MUSIC_PLC_RET_SUCCESS;
	}
	static inline i32 Run(void* hd, uint8_t* in, int32_t inLen, uint8_t* out, int32_t* outLen, uint16_t isLost)
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
		MusicPlc_c* plc = (MusicPlc_c*)hd;
		MusicPlcCom_c<T>* pCom = &plc->_com;
		
		BufferSamples bufferSamples;
		bufferSamples._buf = in;
		bufferSamples._samples = pCom->frameSamples;
		AudioSamples pIn;
		pIn.Init(&pCom->info, &bufferSamples);

		bufferSamples._buf = out;
		bufferSamples._samples = pCom->frameSamples;
		AudioSamples pOut;
		pOut.Init(&pCom->info, &bufferSamples);
#if MUSIC_PLC_SUPPORT_CH_SELECT
		for (i16 ch = 0; ch < pCom->info._channels; ch++) {
			b1 isSelectCh = plc->_channelSelect & (1 << ch);
			b1 isLostCh = isLost & (1 << ch);
			plc->_musicPlcCh.Run(pIn, pOut, ch, isLostCh, isSelectCh);
		}
#else
		plc->_musicPlcChAll.Run(pIn, pOut, isLost ? true : false);
#endif
		*outLen = pOut.GetValidSamples(0) * pCom->info._bytesPerSample;
		return MUSIC_PLC_RET_SUCCESS;
	}

	static inline i32 Destory(void* hd) {
		MusicPlc_c* plc = (MusicPlc_c*)hd;
		plc->_com.MM.FreeAll();
		AlgoBasePorting_c* basePorting = plc->_basePorting;
		plc->~MusicPlc_c();
		basePorting->Free(plc);
		return MUSIC_PLC_RET_SUCCESS;
	}

private:
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

	MusicPlcCom_c<T> _com;
#if MUSIC_PLC_SUPPORT_CH_SELECT
	MusicPlcCh_c<T> _musicPlcCh;
#else
	MusicPlcChAll_c<T> _musicPlcChAll;
#endif
	u16 _channelSelect;
	AlgoBasePorting_c* _basePorting;
};

}