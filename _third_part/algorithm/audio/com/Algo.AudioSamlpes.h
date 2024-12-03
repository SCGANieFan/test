#pragma once
#include"Algo.AudioData.h"

namespace Algo {
namespace Audio {

class BufferSamples
{
public:
	BufferSamples() {};
	~BufferSamples() {};
public:
	u8* _buf = 0;
	i32 _samples = 0;
};

class AudioSamples :public AudioData
{
private:
	class ChannelInfo_c {
	public:
		ChannelInfo_c() {};
		~ChannelInfo_c() {};
	public:
		i32 _usedSample = 0;
		i32 _validSamples = 0;
	};
public:
	AudioSamples() {};
	~AudioSamples() {};
public:
	//set

	//get
	INLINE u8* GetLeftData(i16 ch) { 
		return &_buff[(channelInfos[ch]._usedSample + channelInfos[ch]._validSamples) * _info->_bytesPerSample + ch * _info->_width];
	}
	INLINE u8* GetLeftData() {
		return GetLeftData(0);
	}

	INLINE u8* GetBufInSample(i32 sample, i16 ch) { return &_buff[sample * _info->_bytesPerSample + ch * _info->_width]; }
	INLINE u8* GetBufInSample(i32 sample) { return GetBufInSample(sample, 0); }

	INLINE i32 GetValidSamples(i16 ch) { return channelInfos[ch]._validSamples; }
	INLINE i32 GetValidSamples() { return GetValidSamples(0); }

	INLINE i32 GetUsedSamples(i16 ch) { return channelInfos[ch]._usedSample;}
	INLINE i32 GetUsedSamples() { return GetUsedSamples(0); }

	INLINE i32 GetSamplesMax() { return _samples; };

	INLINE i32 GetLeftSamples(i16 ch) { return _samples - GetUsedSamples(ch) - GetValidSamples(ch); }
	INLINE i32 GetLeftSamples() { return GetLeftSamples(0); }

	INLINE i32 GetSizeMax() { return _samplesTotal * _info->_width; };
	INLINE i16 GetFPNum() { return _fpNum; };
	INLINE b1 IsFull() { return GetValidSamples() == GetSamplesMax(); };

	//
	INLINE b1 Init(const AudioInfo* pInfo, Buffer* buffer, i16 fpNum = 0) {
		_samples = buffer->_max / pInfo->_bytesPerSample;
		// _validSamples = 0;
		_samplesTotal = _samples * pInfo->_channels;
		_fpNum = fpNum;
		return AudioData::Init(pInfo, buffer);
	}

	INLINE b1 Init(const AudioInfo* pInfo, BufferSamples* bufferSamples, i16 fpNum = 0) {
		_samples = bufferSamples->_samples;
		// _validSamples = 0;
		_samplesTotal = _samples * pInfo->_channels;
		_fpNum = fpNum;

		Buffer buffer;
		buffer._buf = bufferSamples->_buf;
		buffer._max = _samples * pInfo->_bytesPerSample;
		return AudioData::Init(pInfo, &buffer);
	}
	INLINE b1 Append(AudioSamples& src, i32 srcSample, i32 appendSample, i16 ch) {
#if 0
		i32 copyByte = appendSample * _info->_bytesPerSample;
		ALGO_MEM_CPY(GetLeftData(), src.GetBufInSample(srcSample), copyByte);
		_validSamples += appendSample;
		_size += copyByte;
		return true;
#else
		i32 stride = _info->_channels;
		if (_info->_width == 2) {
			i16* pDst = (i16*)GetLeftData(ch);
			i16* pSrc = (i16*)src.GetBufInSample(srcSample, ch);
			for (i32 s = 0; s < appendSample; s++) {
				*pDst = *pSrc;
				pDst += stride;
				pSrc += stride;
			}
		}
		else if (_info->_width == 4) {
			i32* pDst = (i32*)GetLeftData(ch);
			i32* pSrc = (i32*)src.GetBufInSample(srcSample, ch);
			for (i32 s = 0; s < appendSample; s++) {
				*pDst = *pSrc;
				pDst += stride;
				pSrc += stride;
			}
		}
		channelInfos[ch]._validSamples += appendSample;
		return true;
#endif
	};
	INLINE b1 Append(AudioSamples& src, i32 srcSample, i32 appendSample) {
		i32 copyByte = appendSample * _info->_bytesPerSample;
		ALGO_MEM_CPY(GetLeftData(), src.GetBufInSample(srcSample), copyByte);
		channelInfos[0]._validSamples += appendSample;
		return true;
	};
	INLINE b1 Append(i32 appendSample,i16 ch) {
#if 0
		_validSamples += appendSample;
		_size += appendSample * _info->_bytesPerSample;
		return true;
#else
		channelInfos[ch]._validSamples += appendSample;
		return true;
#endif
	};

	INLINE b1 Append(i32 appendSample) {
		return Append(appendSample, 0);
	};
	INLINE b1 AppendFully(AudioSamples& src, i32* usedSamples) {
		i32 AppendSamples;
		AppendSamples = GetSamplesMax() - GetValidSamples();
		AppendSamples = AppendSamples < src.GetValidSamples() ? AppendSamples : src.GetValidSamples();
		Append(src, src.GetUsedSamples(), AppendSamples);
		*usedSamples = AppendSamples;
		return true;
	};
	INLINE b1 Used(i32 usedSample, i16 ch) {
#if 0
		_off += _usedSample * _info->_bytesPerSample;
		_size -= _usedSample * _info->_bytesPerSample;
		_validSamples -= _usedSample;
		_usedSamples += _usedSample;
		return true;
#else
		channelInfos[ch]._usedSample += usedSample;
		channelInfos[ch]._validSamples -= usedSample;
		return true;
#endif
	};
	INLINE b1 Used(i32 usedSample) {
		return Used(usedSample, 0);
	};
	INLINE b1 ClearUsed(i16 ch) {
		i32 stride = _info->_channels;
		if (_info->_width == 2) {
			i16* pDst = (i16*)GetBufInSample(0, ch);
			i16* pSrc = (i16*)GetBufInSample(channelInfos[ch]._usedSample, ch);
			for (i32 s = 0; s < channelInfos[ch]._validSamples; s++) {
				*pDst = *pSrc;
				pDst += stride;
				pSrc += stride;
			}
		}
		else if (_info->_width == 4) {
			i32* pDst = (i32*)GetBufInSample(0, ch);
			i32* pSrc = (i32*)GetBufInSample(channelInfos[ch]._usedSample, ch);
			for (i32 s = 0; s < channelInfos[ch]._validSamples; s++) {
				*pDst = *pSrc;
				pDst += stride;
				pSrc += stride;
			}
		}
		channelInfos[ch]._usedSample = 0;
		return true;
	};
	INLINE b1 ClearUsed() {
		ALGO_MEM_MOVE(
			GetBufInSample(0),
			GetBufInSample(channelInfos[0]._usedSample), 
			channelInfos[0]._validSamples * _info->_bytesPerSample);
		channelInfos[0]._usedSample = 0;
		return true;
	};
	INLINE b1 Clear(i32 _usedSample, i16 ch) { Used(_usedSample, ch); ClearUsed(ch); return true; };
	INLINE b1 Clear(i32 usedSample) { Used(usedSample); ClearUsed(); return true; };
protected:
	ChannelInfo_c channelInfos[16];
	i16 _fpNum = 0;
	i32 _samples = 0;
	//i32 _validSamples = 0;
	//i32 _usedSamples = 0;
	i32 _samplesTotal = 0;//_samples * channel
};
}
}






