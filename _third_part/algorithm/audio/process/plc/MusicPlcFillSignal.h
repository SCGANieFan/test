#pragma once
#include"MusicPlcMem.h"
#include"Algo.AudioSamlpes.h"

using namespace Algo;
using namespace Audio;


class MusicPlcFillSignal_c
{
public:
	MusicPlcFillSignal_c() {};
	~MusicPlcFillSignal_c() {};
public:
	INLINE void Init(MusicPlcMem_c* _MM,i32 fillSignalSample,AudioInfo *info) {
		BufferSamples bufferSamples;
		bufferSamples._samples = fillSignalSample;
		bufferSamples._buf = (u8*)_MM->Malloc(bufferSamples._samples * info->_bytesPerSample);
		ALGO_MEM_SET(bufferSamples._buf, 0, bufferSamples._samples * info->_bytesPerSample);
		fillSignal.Init(info, &bufferSamples);
		fillSignalSampleIndex = 0;
	}
	INLINE void Input(AudioSamples& src, i32 srcSample, i32 appendSample)
	{
	
		fillSignal.Clear(fillSignal.GetValidSamples());
		fillSignal.Append(
			src,
			srcSample,
			appendSample);
		fillSignalSampleIndex = 0;
	}

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
private:
	AudioSamples fillSignal;
	i32 fillSignalSampleIndex;
};

