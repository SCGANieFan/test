#include"Algo.Memory.h"
#include"Algo.AudioData.h"
#include"Algo.AudioSamlpes.h"
#include"audiodemo.h"

using namespace Algo;
using namespace Audio;
AudioInfo info;

void AudioDemoInit()
{
	printf("comInit\n");
}



void AudioDemoRun()
{
	printf("comRun\n");
	AudioSamples samples;
	//samples[0]
	//samples()
	//samples.GetBufInSample(sample)
	//samples.Sample(sample)


#if 0
	//samples.Init(0, 0);
	AudioInfo info;
	Buffer buf;
	samples.Init(&info, &buf);
#endif

}



