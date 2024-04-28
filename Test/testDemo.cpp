#if 1
#include<string.h>
#include"MultimeidaTest.h"
//#include"MTF.Object.h"


#define PATH "../../source/audio/plc/"
#define FILE_NAME "chirp_sin_16k1ch.wav"



bool testDemo()
{

#if 1
	MultiemdiaTestInit();
	void* param[] = {
		(void*)(PATH FILE_NAME),
		(void*)(PATH FILE_NAME ".plc.pcm"),
	};
	const char* str = {
	"|pcm_demuxer,url=$0|-->"
	"|music_plc|-->"
	"|pcm_muxer,url=$1|"
	};
	MultiemdiaApi(str, param);
	return 1;
#else

#endif
}

#endif