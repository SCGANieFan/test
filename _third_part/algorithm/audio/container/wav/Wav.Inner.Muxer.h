#if 1
#include"Algo.Memory.h"
#include"Wav.Inner.HeadType.h"
#include"Wav.Inner.Com.h"

#if 1
class WavMuxer_c
{
public:
	WavMuxer_c() {};
	~WavMuxer_c() {};

public:
	INLINE void SetSampleRate(u32 sampleRate) {
		_head.fmt.sampleRate = sampleRate;
		_head.fmt.byteRate = _head.fmt.blockAlign * _head.fmt.sampleRate;
	}

	INLINE void SetNumChannels(u16 numChannels) {
		_head.fmt.numChannels = numChannels;
		u16 width = _head.fmt.bitsPerSample >> 3;
		_head.fmt.blockAlign = _head.fmt.numChannels * width;
		_head.fmt.byteRate = _head.fmt.blockAlign * _head.fmt.sampleRate;
	}

	INLINE void SetWidth(u16 width) {
		_head.fmt.bitsPerSample = width << 3;
		_head.fmt.blockAlign = _head.fmt.numChannels * width;
		_head.fmt.byteRate = _head.fmt.blockAlign * _head.fmt.sampleRate;
	}

	INLINE void GetHead(void* headOut) {
		ALGO_MEM_CPY(headOut, &_head, sizeof(WavHead));
	}
public:
	b1 Init(AlgoBasePorting* basePorting);
	INLINE void DeInit() { _wavMem.FreeAll(); };
	b1 Process(u8* inOut, i32 inLen);

private:
	WavHead _head;
	WavMem_c _wavMem;
};

#endif

#endif