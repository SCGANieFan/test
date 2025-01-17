#pragma once
#include"Algo.AudioSamlpes.h"
#include"Algo.BasePorting.Inner.h"
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
	
	INLINE void Input(AudioSamples& src, i32 srcSample, i32 appendSample, i16 ch) {
		fillSignal.Clear(fillSignal.GetValidSamples(ch), ch);
		fillSignal.Append(
			src,
			srcSample,
			appendSample,
			ch);
		fillSignalSampleIndex[ch] = 0;
	}

	INLINE void Input(AudioSamples& src, i32 srcSample, i32 appendSample) {
		fillSignal.Clear(fillSignal.GetValidSamples());
		fillSignal.Append(
			src,
			srcSample,
			appendSample);
		fillSignalSampleIndex[0] = 0;
	}

	INLINE void Output(AudioSamples& dst, i32 appendSample, i16 ch) {
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
	}

	INLINE void Output(AudioSamples& dst, i32 appendSample){
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
public:
	AudioSamples fillSignal;
	i32 fillSignalSampleIndex[16];
};
}
}
