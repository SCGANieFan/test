#pragma once
#include"Algo.AudioSamlpes.h"

namespace Algo{
namespace Audio{


class MusicPlcFillSignal_c
{
public:
	MusicPlcFillSignal_c() {};
	~MusicPlcFillSignal_c() {};
public:
	INLINE void Init(MemoryManger_c* _MM,i32 fillSignalSample,AudioInfo *info) {
		BufferSamples bufferSamples;
		bufferSamples._samples = fillSignalSample;
		bufferSamples._buf = (u8*)_MM->Malloc(bufferSamples._samples * info->_bytesPerSample);
		ALGO_MEM_SET(bufferSamples._buf, 0, bufferSamples._samples * info->_bytesPerSample);
		fillSignal.Init(info, &bufferSamples);
		for (i16 ch = 0; ch < info->_channels; ch++)
			fillSignalSampleIndex[ch] = 0;
	}
	
	INLINE void Input(AudioSamples& src, i32 srcSample, i32 appendSample, i16 ch)
	{
		fillSignal.Clear(fillSignal.GetValidSamples(ch), ch);
		fillSignal.Append(
			src,
			srcSample,
			appendSample,
			ch);
#if 0
		if (fillSignal._info->_width == 2) {
			i16* pDst = (i16*)fillSignal.GetBufInSample(0, ch);
			for (i32 s = 0; s < appendSample; s++) {
				*pDst = 0;
				pDst += fillSignal._info->_channels;
			}
		}
#endif
		fillSignalSampleIndex[ch] = 0;
	}

	INLINE void Input(AudioSamples& src, i32 srcSample, i32 appendSample)
	{
#if 1
		fillSignal.Clear(fillSignal.GetValidSamples());
		fillSignal.Append(
			src,
			srcSample,
			appendSample);
#else
		fillSignal.Clear(fillSignal.GetValidSamples());
		fillSignal.Append(appendSample);
#endif
		fillSignalSampleIndex[0] = 0;
}

#if 0
	INLINE void Output(AudioSamples& dst, i32 appendSample)
	{
		i32 appendSample1 = fillSignal.GetValidSamples() - fillSignalSampleIndex;
		appendSample1 = MIN(appendSample1, appendSample);
		dst.Append(
			fillSignal,
			fillSignalSampleIndex,
			appendSample1);

		fillSignalSampleIndex = (fillSignalSampleIndex + appendSample1) % fillSignal.GetValidSamples();

		i32 appendSample2 = appendSample - appendSample1;
		dst.Append(
			fillSignal,
			fillSignalSampleIndex,
			appendSample2);
		fillSignalSampleIndex = (fillSignalSampleIndex + appendSample2) % fillSignal.GetValidSamples();
	}
#endif

	INLINE void Output(AudioSamples& dst, i32 appendSample, i16 ch)
	{
#if 1
#if 0

		i32 appendSample1 = fillSignal.GetValidSamples(ch) - fillSignalSampleIndex[ch];
		appendSample1 = MIN(appendSample1, appendSample);
		dst.Append(
			fillSignal,
			fillSignalSampleIndex[ch],
			appendSample1,
			ch);

		fillSignalSampleIndex[ch] = (fillSignalSampleIndex[ch] + appendSample1) % fillSignal.GetValidSamples(ch);
		i32 appendSample2 = appendSample - appendSample1;
		dst.Append(
			fillSignal,
			fillSignalSampleIndex[ch],
			appendSample2,
			ch);
		fillSignalSampleIndex[ch] = (fillSignalSampleIndex[ch] + appendSample2) % fillSignal.GetValidSamples(ch);
#else
		i32 appendSampleRem = appendSample;
		while (1) {
			i32 appendSample1 = fillSignal.GetValidSamples(ch) - fillSignalSampleIndex[ch];
			appendSample1 = MIN(appendSample1, appendSampleRem);
			dst.Append(
				fillSignal,
				fillSignalSampleIndex[ch],
				appendSample1,
				ch);

			fillSignalSampleIndex[ch] = (fillSignalSampleIndex[ch] + appendSample1) % fillSignal.GetValidSamples(ch);
			appendSampleRem = appendSampleRem - appendSample1;
			if (appendSampleRem <= 0)
				break;
		}

#endif
#else
		i32 appendSample1 = fillSignal.GetValidSamples() - fillSignalSampleIndex;
		appendSample1 = MIN(appendSample1, appendSample);
		dst.Append(
			fillSignal,
			fillSignalSampleIndex,
			appendSample1);
		i32 stride = fillSignal._info->_channels;
		if (fillSignal._info->_width == 2) {
			i16* pSrc = (i16*)fillSignal.GetBufInSample(fillSignalSampleIndex) + ch;
			i16* pDst = (i16*)dst.GetBufInSample(0) + ch;
			for (i32 s = 0; s < appendSample1; s++) {
				*pDst = *pSrc;
				pDst += stride;
				pSrc += stride;
			}
		}



		for ()

			fillSignalSampleIndex = (fillSignalSampleIndex + appendSample1) % fillSignal.GetValidSamples();
		i32 appendSample2 = appendSample - appendSample1;
		dst.Append(
			fillSignal,
			fillSignalSampleIndex,
			appendSample2);
		fillSignalSampleIndex = (fillSignalSampleIndex + appendSample2) % fillSignal.GetValidSamples();
#endif
	}

	INLINE void Output(AudioSamples& dst, i32 appendSample)
	{
		const int ch = 0;
		i32 appendSampleRem = appendSample;
		while (1) {
			i32 appendSample1 = fillSignal.GetValidSamples() - fillSignalSampleIndex[ch];
			appendSample1 = MIN(appendSample1, appendSampleRem);
			dst.Append(
				fillSignal,
				fillSignalSampleIndex[ch],
				appendSample1);

			fillSignalSampleIndex[ch] = (fillSignalSampleIndex[ch] + appendSample1) % fillSignal.GetValidSamples();
			appendSampleRem = appendSampleRem - appendSample1;
			if (appendSampleRem <= 0)
				break;
		}
	}
private:
	AudioSamples fillSignal;
	i32 fillSignalSampleIndex[16];
};
}
}
