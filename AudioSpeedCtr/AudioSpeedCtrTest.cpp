#include"MTF.h"
#define PATH "../../source/audio/speedCtr/"

#define FILE_NAME "stSection_16k1ch.wav"
//#define FILE_NAME "chirp_sin_16k1ch.wav"
#define RATE 16000
#define CHANNEL 1
#define WIDTH 2
#define FRAME_MS 20
#define SPEED (1.5f)

void AudioSpeedCtrTest()
{
	MultiemdiaTestInit();

	MTF_REGISTER(pcm_demuxer);
	MTF_REGISTER(auio_speedCtr);
	MTF_REGISTER(pcm_muxer);

	float speedQ8 = SPEED * (1 << 8);
	void* param[] = {
		(void*)(PATH FILE_NAME),
		(void*)(PATH FILE_NAME ".speed.pcm"),
		(void*)RATE,
		(void*)CHANNEL,
		(void*)WIDTH,
		(void*)FRAME_MS,
		(void*)(uint32_t)speedQ8,
	};

	const char* str = {
	"|pcm_demuxer,url=$0,rate=$2,ch=$3,width=$4,fMs=$5|-->"
	"|auio_speedCtr,speedQ8=$6|-->"
	"|pcm_muxer,url=$1|"
	};

	MultiemdiaApi(str, param);
}
