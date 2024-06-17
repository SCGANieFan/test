#pragma once

#include<stdint.h>
#include<string.h>

#define AS_MCM_CPY(dst,src,size) memcpy(dst,src,size)
#define AS_MCM_SET(dst,val,size) memset(dst,val,size)
#define AS_MCM_MOVE(dsr,src,size) memmove(dsr,src,size)

class AudioSamples16
{
public:
	AudioSamples16(){};
	~AudioSamples16(){};

public:
	inline uint8_t& operator[](int32_t sample){return _buff[sample * _width * _channels];}
public:
	//get
	inline int32_t GetSizeInByte(){return _sizeInByte;}

	inline int32_t GetSamples(){return _samples;}
	inline int16_t GetWidth(){return _width;}
	inline int16_t GetFixPointNum(){return _fixPointNum;}
	inline int32_t GetValidSamples(){return _validSamples;}
	inline int16_t GetChannels(){return _channels;}


	inline bool Move(int32_t moveSample){
		//check
		if(moveSample > 0)
			return false;

		//left move
		moveSample = -moveSample;
		int32_t copyByte = (_startSamples + _validSamples - moveSample) * _width * _channels;
		uint8_t* dst = _buff;
		uint8_t* src = _buff + moveSample * _width * _channels;

		if (copyByte > 0) {
			AS_MCM_MOVE(dst, src, copyByte);
		}

		if (moveSample > _startSamples)
		{
			_startSamples = 0;
			_validSamples -= (moveSample - _startSamples);
			_validSamples = _validSamples > 0 ? _validSamples : 0;
		}
		else
		{
			_startSamples -= moveSample;
		}
		return true;
	}

	inline void Clear(){
		_startSamples = 0;
		_validSamples = 0;
	}
	inline bool Append(uint8_t* src, int32_t srcSample){
		//check
		uint8_t* pBuf = _buff + (_startSamples + _validSamples) * _width * _channels;
		int32_t copySize = srcSample * _width * _channels;
		AS_MCM_CPY(pBuf, src, copySize);
		_validSamples += srcSample;
		return true;
	}
	inline void used(int32_t usedSample){
		_validSamples -= usedSample;
		_validSamples = _validSamples > 0 ? _validSamples : 0;
		_startSamples += usedSample;
		_startSamples = _startSamples < _samples ? _startSamples : _samples;
	}

	bool Init(uint8_t* buff, int16_t channels, int32_t samples, int16_t fixPointNum = 0);	
	inline bool ProductWithEverySamlpe(AudioSamples16& src, int32_t startSample,int32_t dir, int32_t productSample){
		if(dir==1)
		{
			int32_t remSample= src.GetSamples()-startSample;
			if(productSample<remSample)
			{
				for (int32_t s = 0; s < productSample; s++)
				{
					int16_t* p = (int16_t*)(_buff + (_startSamples + s) * _width * _channels);
					for (int16_t ch = 0; ch < _channels; ch++)
					{
						p[ch] = (((int64_t)p[ch]) * *((int16_t*)&src[startSample + s])) >> src.GetFixPointNum();
					}
					
				}
			}
			else
			{
				for (int32_t s = 0; s < remSample; s++)
				{
					int16_t* p = (int16_t*)(_buff + (_startSamples + s) * _width * _channels);
					for (int16_t ch = 0; ch < _channels; ch++)
					{
						p[ch] = (((int64_t)p[ch]) * *((int16_t*)&src[startSample + s])) >> src.GetFixPointNum();
					}
					
				}
				for (int32_t s = remSample; s < productSample; s++)
				{
					int16_t* p = (int16_t*)(_buff + (_startSamples + s) * _width * _channels);
					for (int16_t ch = 0; ch < _channels; ch++)
					{
						p[ch] = 0;
					}
					
				}
			}
		}
		else
		{
			int32_t remSample= startSample;
			if(productSample<remSample)
			{
				for (int32_t s = 0; s < productSample; s++)
				{
					int16_t* p = (int16_t*)(_buff + (_startSamples + s) * _width * _channels);
					for (int16_t ch = 0; ch < _channels; ch++)
					{
						p[ch] = (((int64_t)p[ch]) * *((int16_t*)&src[remSample-1-s])) >> src.GetFixPointNum();
					}
				}
			}
			else
			{
				for (int32_t s = 0; s < remSample; s++)
				{
					int16_t* p = (int16_t*)(_buff + (_startSamples + s) * _width * _channels);
					for (int16_t ch = 0; ch < _channels; ch++)
					{
						p[ch] = (((int64_t)p[ch]) * *((int16_t*)&src[remSample-1-s])) >> src.GetFixPointNum();
					}
					
				}
			}	
		}

		return true;
	}
#if 0
	inline bool LeftShift(int16_t bits, bool isSigned){
		if (isSigned)
		{
			int16_t* pSrc = (int16_t*)_buff;
			for (int32_t s = 0; s < _samples * _channels; s++)
			{
				pSrc[s] = pSrc[s] << bits;
			}
		}
		else
		{
			uint16_t* pSrc = (uint16_t*)_buff;
			for (int32_t s = 0; s < _samples * _channels; s++)
			{
				pSrc[s] = pSrc[s] << bits;
			}
		}
		return true;
	}
	inline bool RightShift(int16_t bits, bool isSigned){
		if (isSigned)
		{
			int16_t* pSrc = (int16_t*)_buff;
			for (int32_t s = 0; s < _samples * _channels; s++)
			{
				pSrc[s] = pSrc[s] >> bits;
			}
		}
		else
		{
			uint16_t* pSrc = (uint16_t*)_buff;
			for (int32_t s = 0; s < _samples * _channels; s++)
			{
				pSrc[s] = pSrc[s] >> bits;
			}
		}
		return true;
	}
#endif
	inline bool AppendInFixPoint(int32_t num, int32_t den){
		uint8_t* pBuff = _buff + _validSamples * _width * _channels;
		_validSamples++;
		if (den == 0)
		{
			AS_MCM_SET(pBuff, 0, _width * _channels);
			return true;
		}
		int16_t* psrc = (int16_t*)pBuff;
		int64_t fac = ((int64_t)num << _fixPointNum) / den;
		for (int16_t ch = 0; ch < _channels; ch++) {
			psrc[ch] = fac;
		}
		return true;
	}
	inline bool OverlapAdd(uint8_t* src, int32_t overlapSample){
		//check
		int32_t fixNum = 15;
		int32_t factor;

		int16_t* pBuf = (int16_t*)(_buff + _startSamples * _width * _channels);
		int16_t* pSrc = (int16_t*)src;
		for (int32_t s = 0; s < overlapSample; s++)
		{
			factor = ((int32_t)s << fixNum) / overlapSample;
			for (int16_t ch = 0; ch < _channels; ch++)
			{
				pBuf[s * _channels + ch] = ((int64_t)pBuf[s * _channels + ch] * (((int32_t)1 << fixNum) - factor)
					+ (int64_t)pSrc[s * _channels + ch] * factor) >> fixNum;
			}
		}
		return true;
	}
	inline int16_t WaveFormMatch(uint8_t* comp, int32_t seekSample, int32_t matchSample){
		int64_t matchFactor = 0;
		int64_t matchFactorOpt = 0;
		int16_t lagOpt = 0;

		int16_t* pTempl = (int16_t*)_buff;
		int16_t* pComp = (int16_t*)comp;

		for (int16_t s = 0; s < seekSample; s++) {
			matchFactor = 0;
			for (int16_t m = 0; m < matchSample; m++) {
				for (int16_t ch = 0; ch < _channels; ch++) {
					matchFactor += ((int32_t)pTempl[(s + m) * _channels + ch] * pComp[m * _channels + ch]) >> 8;
				}
			}
			if (matchFactorOpt < matchFactor)
			{
				matchFactorOpt = matchFactor;
				lagOpt = s;
			}
		}

		return lagOpt;
	}

protected:
	bool Init(uint8_t* buff, int16_t width, int16_t channels, int32_t samples, int16_t fixPointNum);

protected:
	uint8_t* _buff;
	int16_t _width;
	int16_t _channels;
	int16_t _fixPointNum;
	int32_t _samples;
	int32_t _startSamples;
	int32_t _validSamples;
	int32_t _sizeInByte;
};




