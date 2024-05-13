#include"Algo.Memory.h"
#include"Algo.AudioData.h"
#include"Algo.AudioSamlpes.h"
#include"audiodemo.h"

AudioInfo info;

void AudioDemoInit()
{
	printf("comInit\n");
}



void AudioDemoRun()
{
	printf("comRun\n");
	AudioSamples samples;

	//samples.Init(0, 0);
	AudioInfo info;
	Buffer buf;
	samples.Init(&info, &buf);


}



