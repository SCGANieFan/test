#include"MTF.h"
#define PATH "../../source/audio/plc/"

#if 1
#if 0
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

#define FILE_NAME "sin4ch48k32b.wav"
#define RATE 48000
#define CHANNEL 4
#define WIDTH 4
#define FRAME_MS 2
#define DECAY_MS 40
#define OVERLAP_MS 2

#endif
void PlcTest()
{
	MultiemdiaTestInit();

	MTF_REGISTER(pcm_demuxer);
	MTF_REGISTER(music_plc);
	MTF_REGISTER(pcm_muxer);

	void* param[] = {
		(void*)(PATH FILE_NAME),
		(void*)(PATH FILE_NAME ".plc.pcm"),
		(void*)RATE,
		(void*)CHANNEL,
		(void*)WIDTH,
		(void*)FRAME_MS,
		(void*)DECAY_MS,
		(void*)OVERLAP_MS,
	};

	const char* str = {
	"|pcm_demuxer,url=$0,rate=$2,ch=$3,width=$4,fMs=$5|-->"
	"|music_plc,decayMs=$6,overlapMs=$7|-->"
	"|pcm_muxer,url=$1|"
	};

	MultiemdiaApi(str, param);
}
