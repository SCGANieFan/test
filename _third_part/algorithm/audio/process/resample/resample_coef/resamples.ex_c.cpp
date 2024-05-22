#if 1
#include<stdint.h>
#include<string.h>
#include"Algo.Type.h"
#include"resamples.h"
#include"resamples.inner.h"

#if 1
typedef int8_t int08_t;

struct int24_t {
	uint8_t x[3];
	int24_t(int32_t in) { *this = in; }
	inline int32_t operator+(int24_t& in) { return (int32_t)*this + (int32_t)in; }
	inline int32_t operator<<(int32_t shf) { return (int32_t)*this << shf; }
	inline int32_t operator>>(int32_t shf) { return (int32_t)*this >> shf; }
	inline int32_t operator<<=(int32_t shf) { return *this = (int32_t)*this << shf; }
	inline int32_t operator>>=(int32_t shf) { return *this = (int32_t)*this >> shf; }
	inline int32_t operator=(int32_t in) { x[0] = ((uint8_t*)&in)[0]; x[1] = ((uint8_t*)&in)[1]; x[2] = ((uint8_t*)&in)[2]; return in; }
	inline int32_t operator=(int24_t& in) { x[0] = in.x[0]; x[1] = in.x[1]; x[2] = in.x[2]; return in; }
	inline operator int32_t()const { return (int32_t)((x[2] << 24) | (x[1] << 16) | (x[0] << 8)) >> 8; }
};
#endif


template<class T> static inline
T Ssat(int64_t in)
{
	const uint8_t bits = sizeof(T) << 3;
	const int64_t vMax = ((int64_t)1 << (bits - 1)) - 1;
	const int64_t vMin = -((int64_t)1 << (bits - 1));
	int64_t ret = in > vMax ? vMax : (in < vMin ? vMin : in);
	return (T)ret;
}

template<class To, class Tc, class Ti> static inline
void MultiplyAccumulate(To* dst, Ti* src, const Tc* coef, int count)
{
	int64_t temp = 0;
	for (unsigned i = 0; i < count; i++) {
		temp += (int64_t)src[i] * coef[i];
	}
	temp = temp >> 15;
	*dst = Ssat<To>(temp);
}

typedef RESAMPLE_STATUS_T(*ResampleInner_CB)(ResampleState* pState, const void* in, int* inByte, void* out, int* outByte);
template<class Ti, class To> static inline
RESAMPLE_STATUS_T ResampleInner(ResampleState* pState, const void* in, int* inByte, void* out, int* outByte)
{
	uint32_t coefChoose = pState->coefChoose;
	const ResampleCoef* coef = pState->coef;
	int16_t inBytesPerSample = pState->iInfo.ch * sizeof(Ti);
	int16_t outBytesPerSample = pState->oInfo.ch * sizeof(To);
	int32_t inRemSamples = *inByte / inBytesPerSample;
	int32_t outRemSize = *outByte;
	Ti* pIn = (Ti*)in;
	To* pOut = (To*)out;
	uint32_t i = pState->inStartSamples;
	int16_t chCom = MIN(pState->iInfo.ch, pState->oInfo.ch);

	while (inRemSamples > 0)
	{
		//exit check
		if (outRemSize < outBytesPerSample) {
			break;
		}

		//prepare inSamplesCache
		int inSamples = MIN(inRemSamples, SAMPLE_NUM);
		for (int16_t ch = 0; ch < chCom; ch++)
		{
			for (int16_t s = 0; s < inSamples; s++)
			{
				((Ti*)(pState->inSamplesCache[ch]))[coef->coefNumPerGroup + s] = pIn[s * pState->iInfo.ch + ch];
			}
		}

		//deal per ch
		while (outRemSize >= outBytesPerSample)
		{
			if (i >= inSamples) {
				break;
			}
			//deal per sample
#if 1
			for (int16_t ch = 0; ch < chCom; ch++)
			{
				MultiplyAccumulate<To, int16_t, Ti>(
					pOut + ch,
					&((Ti*)pState->inSamplesCache[ch])[(1 + i)],
					&coef->coefGroups[coef->coefNumPerGroup * coefChoose],
					coef->coefNumPerGroup);
			}
#else
			for (int16_t ch = 0; ch < chCom; ch++)
			{

				pOut[ch] = ((Ti*)(pState->inSamplesCache[ch]))[(1 + i)];
				if (pOut[ch] != 0)
					int a = 1;
			}
#endif
			for (int16_t ch = chCom; ch < pState->oInfo.ch; ch++)
			{
				if (pState->isChCopy)
					pOut[ch] = pOut[ch - chCom];
				else
					pOut[ch] = 0;
			}

			pOut = pOut + pState->oInfo.ch;
			outRemSize -= outBytesPerSample;

			//coefChoose
			coefChoose += coef->downsampleFactor;
			if (coefChoose >= coef->upsampleFactor) {
				i += coefChoose / coef->upsampleFactor;
				coefChoose = coefChoose % coef->upsampleFactor;//coefChoose = [0,upsampleFactor-1]
			}
		}
		int inUsedSamples;
		if (i >= inSamples) {
			inUsedSamples = inSamples;
			i -= inSamples;
		}
		else {
			inUsedSamples = i;
			i = 0;
		}
		for (int16_t ch = 0; ch < chCom; ch++)
		{
			uint8_t* p0 = (uint8_t*)&pState->inSamplesCache[ch];
			uint8_t* p1 = (uint8_t*)&((Ti*)(pState->inSamplesCache[ch]))[inUsedSamples];
			memcpy(
				&pState->inSamplesCache[ch],
				&((Ti*)(pState->inSamplesCache[ch]))[inUsedSamples],
				coef->coefNumPerGroup * sizeof(Ti));
		}
		pIn = pIn + inUsedSamples * pState->iInfo.ch;
		inRemSamples -= inUsedSamples;
	}
	pState->inStartSamples = i;
	pState->coefChoose = coefChoose;
	*inByte -= inRemSamples * inBytesPerSample;
	*outByte -= outRemSize;
	return RESAMPLE_STATUS_OK;
}


ResampleInner_CB ResampleInnerList[4][4] = {
	ResampleInner<int08_t,int08_t>,ResampleInner<int08_t,int16_t>,ResampleInner<int08_t,int24_t>,ResampleInner<int08_t,int32_t>,
	ResampleInner<int16_t,int08_t>,ResampleInner<int16_t,int16_t>,ResampleInner<int16_t,int24_t>,ResampleInner<int16_t,int32_t>,
	ResampleInner<int24_t,int08_t>,ResampleInner<int24_t,int16_t>,ResampleInner<int24_t,int24_t>,ResampleInner<int24_t,int32_t>,
	ResampleInner<int32_t,int08_t>,ResampleInner<int32_t,int16_t>,ResampleInner<int32_t,int24_t>,ResampleInner<int32_t,int32_t>,
};

enum RESAMPLE_STATUS_T audio_resample_run(void* hd, const void* in, int* inSize, void* out, int* outSize)
{
	if (!hd
		|| !in
		|| !inSize
		|| !out
		|| !outSize)
		return RESAMPLE_STATUS_ERROR;

	ResampleState* pState = (ResampleState*)hd;
	int8_t iIdx = (pState->iInfo.bits >> 3) - 1;
	int8_t oIdx = (pState->oInfo.bits >> 3) - 1;
	(ResampleInnerList[iIdx][oIdx])(pState, in, inSize, out, outSize);
	return RESAMPLE_STATUS_OK;
}

#endif
