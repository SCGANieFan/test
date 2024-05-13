#include"MTF.h"
#define PATH "../../source/audio/resample/"

#if 1
//16k1ch
#define FILE_NAME "chirp_sin_16k1ch.wav"
#define RATE 16000
#define CHANNEL 1
#define WIDTH 2
#endif


#define FRAME_MS 20
#define SPEED (1.5f)

void AudioResampleTest()
{
	MultiemdiaTestInit();

	MTF_REGISTER(pcm_demuxer);
	MTF_REGISTER(auio_resample);
	MTF_REGISTER(pcm_muxer);

	uint32_t targetRate = 2 * RATE;
	//uint32_t targetRate = RATE / 2;
	void* param[] = {
		(void*)(PATH FILE_NAME),
		(void*)(PATH FILE_NAME ".speed.pcm"),
		(void*)RATE,
		(void*)CHANNEL,
		(void*)WIDTH,
		(void*)FRAME_MS,
		(void*)targetRate,
	};

	const char* str = {
	"|pcm_demuxer,url=$0,rate=$2,ch=$3,width=$4,fMs=$5|-->"
	"|auio_resample,targetRate=$6|-->"
	"|pcm_muxer,url=$1|"
	};

	MultiemdiaApi(str, param);
}
