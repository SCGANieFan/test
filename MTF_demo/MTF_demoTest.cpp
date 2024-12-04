#include"MTF.h"
#define PATH "../../source/audio/plc/"

#define FILE_NAME "chirp_sin_16k1ch.wav"
#define RATE 16000
#define CHANNEL 1
#define WIDTH 2
#define FRAME_MS 20

void MTF_demo_Test()
{
	MTF_REGISTER(pcm_demuxer);
	MTF_REGISTER(audio_demo);
	MTF_REGISTER(pcm_muxer);
#if 0
	MultiemdiaTestInit();
	void* param[] = {
		(void*)(PATH FILE_NAME),
		(void*)(PATH FILE_NAME ".audioDemo.pcm"),
		(void*)RATE,
		(void*)CHANNEL,
		(void*)WIDTH,
		(void*)FRAME_MS,
	};

	const char* str = {
	"|pcm_demuxer,url=$0,rate=$2,ch=$3,width=$4,fMs=$5|-->"
	"|audio_demo|-->"
	"|pcm_muxer,url=$1|"
	};

	MultiemdiaApi(str, param);
#endif
}
