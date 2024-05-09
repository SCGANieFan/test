#include"Algo.Memory.h"
#include"Algo.AudioData.h"
#include"audiodemo.h"

AudioInfo info;

void AudioDemoInit()
{
	printf("comInit\n");
	//info.Init(48000, 2, 1, 20 * 1000);
}



void AudioDemoRun()
{
	printf("comRun\n");

	i32 bufSize = 1024;
	u8* buf = (u8*)ALGO_MALLOC(bufSize);
	AudioData data;
	//data.Init(&info, buf, bufSize);
	ALGO_FREE(buf);
}



