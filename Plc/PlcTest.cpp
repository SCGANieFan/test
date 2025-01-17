#include"MTF.h"
#define PATH "../../source/audio/plc/"

#if 0
#if 0
//#define FILE_NAME "chirp_sin_16k1ch.wav"
#define FILE_NAME "stSection_16k1ch.wav"
#define RATE 16000
#define CHANNEL 1
#define WIDTH 2
#define FRAME_MS 10
#define DECAY_MS 20
#define OVERLAP_MS 5 //OVERLAP_MS < FRANME_MS,
#else
#define FILE_NAME "sin4ch48k16b.wav"
#define RATE 48000
#define CHANNEL 4
#define WIDTH 2
#define FRAME_MS 10
#define DECAY_MS 30
#define GAIN_MS 10
#define OVERLAP_MS 4
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

//#define FILE_NAME "mbz_48k2h.wav"
//#define FILE_NAME "sin4ch48k16b.wav"
//#define FILE_NAME "sin4ch48k32b.wav"
//#define FILE_NAME "sin1ch48k32b_1.wav"
//#define FILE_NAME "sin1ch48k32b_2.wav"
//#define FILE_NAME "sin1ch48k32b_3.wav"
#define FILE_NAME "sin1ch48k32b_4.wav"
//#define FILE_NAME "sin4ch48k32bFloat.wav"
//#define FILE_NAME "Walking01_Burn(1).wav"
#define RATE 48000
#define CHANNEL 2
#define WIDTH 4
#define FRAME_MS 2
#define OVERLAP_MS 4
#define DECAY_MS 30
#define GAIN_MS 30
#endif
#endif

#define FRAME_LEN ((FRAME_MS*RATE/1000))

void PlcTest()
{
	MultiemdiaTestInit();

	MTF_REGISTER(wav_demuxer);
	MTF_REGISTER(music_plc);
	MTF_REGISTER(wav_muxer);

	void* param[] = {
		(void*)(PATH FILE_NAME),
		(void*)(PATH FILE_NAME ".plc.wav"),		
		//(void*)(FRAME_MS),
		(void*)(FRAME_LEN),
		(void*)DECAY_MS,
		(void*)GAIN_MS,
		(void*)OVERLAP_MS,
	};
#if 0
	const char* str = {
	"|wav_demuxer,url=$0,rate=$2,ch=$3,width=$4,fMs=$5|-->"
	"|music_plc,decayMs=$6,overlapMs=$7|-->"
	"|wav_muxer,url=$1|"
	};
#else
	const char* str = {
	"|wav_demuxer,url=$0,fSamples=$2|-->"
	"|music_plc,decayMs=$3,gainMs=$4,overlapMs=$5|-->"
	"|wav_muxer,url=$1|"
	};
#endif

	MultiemdiaApi(str, param);
}
