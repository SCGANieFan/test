#include"MTF.h"
#define PATH "../../source/audio/plc/"

#if 1
#if 1
#define FILE_NAME "chirp_sin_16k1ch.wav"
#define RATE 16000
#define CHANNEL 1
#define WIDTH 2
#define FRAME_MS 20
#define DECAY_MS 50
#define OVERLAP_MS 2
#else
#define FILE_NAME "sin4ch48k16b.wav"
#define RATE 48000
#define CHANNEL 4
#define WIDTH 2
#define FRAME_MS 20
#define DECAY_MS 50
#define OVERLAP_MS 2
#endif
#else

#if 0
#define FILE_NAME "sin1k_48k1ch24b.pcm"
#define RATE 48000
#define CHANNEL 1
#define WIDTH 3
#define FRAME_MS 2
#define DECAY_MS 50
#define OVERLAP_MS 2
#else
#define FILE_NAME "sin4ch48k32b.wav"
#define RATE 48000
#define CHANNEL 4
#define WIDTH 4
#define FRAME_MS 2
#define DECAY_MS 50
#define OVERLAP_MS 2
#endif
#endif
void WavTest()
{
	MultiemdiaTestInit();

	MTF_REGISTER(wav_demuxer);
	MTF_REGISTER(wav_muxer);

	void* param[] = {
		(void*)(PATH FILE_NAME),
		(void*)(PATH FILE_NAME ".plc.pcm"),
	};

	const char* str = {
	"|wav_demuxer,url=$0|-->"
	"|wav_muxer,url=$1|"
	};

	MultiemdiaApi(str, param);
}
