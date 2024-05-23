#include"MTF.h"
#define PATH "../../source/audio/ape/"

#define FILE_NAME "Flower_Dance_44p1k2ch.wav.apeOri"
#define RATE 44100
#define CHANNEL 2
#define WIDTH 2
#define FRAME_MS 20


void ApeTest()
{
	MultiemdiaTestInit();

	MTF_REGISTER(pcm_demuxer);
	MTF_REGISTER(ape);
	MTF_REGISTER(pcm_muxer);

	void* param[] = {
		(void*)(PATH FILE_NAME),
		(void*)(PATH FILE_NAME ".ape.pcm"),
		(void*)RATE,
		(void*)CHANNEL,
		(void*)WIDTH,
		(void*)FRAME_MS,
	};

	const char* str = {
	"|pcm_demuxer,url=$0,rate=$2,ch=$3,width=$4,fMs=$5|-->"
	"|ape|-->"
	"|pcm_muxer,url=$1|"
	};

	MultiemdiaApi(str, param);
}
