#include"MTF.h"
#include <stdio.h>
#define PATH "../../source/audio/ape/"

#if 0
#define FILE_NAME "Flower_Dance_44p1k2ch.wav.apeOri"
#define RATE 44100
#define CHANNEL 2
#define WIDTH 2
#define FRAME_MS 20
#endif

#if 0
#define FILE_NAME "Inuyasha_48k2ch.wav.apeOri"
#define RATE 48000
#define CHANNEL 2
#define WIDTH 2
#define FRAME_MS 20
#endif


char filenames[][50] = {
#if 1
	"demo1_48k2ch.ape",
	"demo2_44p1k2ch.ape",
	"demo3_96K2ch.ape",
	"demo4_48k2ch.ape",
	"demo5_48k1ch.ape",
	"demo6_48k2ch8b.ape",
	"demo2_44p1k2ch.ape",
	"Flower_Dance_44p1k2ch.wav.ape",
#endif
	"Inuyasha_48k2ch.wav.ape",
};

#define RATE 48000
#define CHANNEL 2
#define WIDTH 2

#define FRAME_MS 20

void ApeTest()
{
	MultiemdiaTestInit();

	MTF_REGISTER(pcm_demuxer);
	MTF_REGISTER(ape);
	MTF_REGISTER(pcm_muxer);

	char fNameIn[256];
	char fNameOut[256];
	for (int i = 0; i < sizeof(filenames) / sizeof(filenames[0]);i++)
	{
		sprintf(fNameIn, "%s%s", PATH, filenames[i]);
		sprintf(fNameOut, "%s.pcm", fNameIn);

		void* param[] = {
			(void*)(fNameIn),
			(void*)(fNameOut),
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
}
