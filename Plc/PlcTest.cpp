#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdint.h>
#include"MMTest.h"

#include"AudioAlgorithm.h"
#include"AA_MusicPlc.h"

#define SOURCE_PATH "../../source/audio/plc/"
//#define FILE_NAME "chirp_sin_16k1ch.wav"
//#define FILE_NAME "sin4k_16k1ch.wav"

#if 1
#define FILE_NAME "sin4ch48k32b.wav"
#define SAMPLE_STORED_WIDTH 4
#define SAMPLE_VALID_WIDTH 4
#define CHANNELS 4
#define SAMPLE_RATE 48000
#elif 0
#define FILE_NAME "sin4ch48k16b.wav"
#define SAMPLE_STORED_WIDTH 2
#define SAMPLE_VALID_WIDTH 2
#define CHANNELS 4
#define SAMPLE_RATE 48000
#else
#define FILE_NAME "sin1k_48k1ch24bS32AlignLow.pcm"
#define SAMPLE_STORED_WIDTH 4
#define SAMPLE_VALID_WIDTH 3
#define SAMPLE_ALIGN_LOW 1
#define CHANNELS 1
#define SAMPLE_RATE 48000
#endif




#define FRAME_LEN_MS 20
#define DECAY_TIME_MS 30
#define OVERLAP_MS 2
#define FRAME_LEN (FRAME_LEN_MS*SAMPLE_RATE/1000)

#define NUM_LOST 10
#define NUM_ALL 50



#define MAFAA_REGISTER(name) \
extern void maf_algorithm_audio_##name##_register();\
			maf_algorithm_audio_##name##_register();

#define PRINT(s,...)		printf(s"\n",##__VA_ARGS__)

void* cbHdInit(void *param0,bool *isSuccess)
{

	* isSuccess = false;
	

#if (SAMPLE_STORED_WIDTH == 2)&&(SAMPLE_VALID_WIDTH == 2)
	MAFAA_REGISTER(music_plc16);
	const char* algoName = "music_plc16";
#elif (SAMPLE_STORED_WIDTH == 4)&&(SAMPLE_VALID_WIDTH == 4)
	MAFAA_REGISTER(music_plc32);
	const char* algoName = "music_plc32";
#elif (SAMPLE_STORED_WIDTH == 4)&&(SAMPLE_VALID_WIDTH == 3)&&(SAMPLE_ALIGN_LOW == 1)
	MAFAA_REGISTER(music_plc24L);
	const char* algoName = "music_plc24L";
#endif

	AudioAlgorithmRet ret;


	int32_t size = 0;
	ret = AudioAlgorithmGetHandleSize(algoName, &size);
	if (ret != AUDIO_ALGORITHM_RET_SUCCESS)
	{
		PRINT("AudioAlgorithmGetHandleSize fail, ret:%d", ret);
		return NULL;
	}
	PRINT("size:%d", size);

	void* hd = malloc(size);
	ret = AudioAlgorithmCreate(algoName, hd);
	if (ret != AUDIO_ALGORITHM_RET_SUCCESS)
	{
		PRINT("AudioAlgorithmCreate fail, ret:%d", ret);
		return NULL;
	}
	PRINT("hd:%x", (unsigned)hd);


	ret = AlgorithmSetMemory(hd, (Malloc_t*)malloc, (Realloc_t*)realloc, (Calloc_t*)calloc, (Free_t*)free);
	if (ret != AUDIO_ALGORITHM_RET_SUCCESS)
	{
		PRINT("AlgorithmSetMemory fail, ret:%d", ret);
		return NULL;
	}

	ret = AlgorithmInit(hd, param0);
	if (ret != AUDIO_ALGORITHM_RET_SUCCESS)
	{
		PRINT("AlgorithmInit fail, ret:%d", ret);
		return NULL;
	}
	*isSuccess = true;
	return hd;
}


MMTestRet cbRun(void* hd, Frame *ifrm, Frame* ofrm)
{

	bool isLost = false;
	static uint32_t frames = 0;
	frames++;
	if ((frames % NUM_ALL) < (NUM_ALL - NUM_LOST))
		isLost = false;
	else
		isLost = true;

	MAFA_Frame frameIn;
	memset(&frameIn, 0, sizeof(MAFA_Frame));
	frameIn.buff = (uint8_t*)ifrm->buff + ifrm->offset;
	frameIn.max = frameIn.size = ifrm->size;

	MAFA_Frame frameOut;
	memset(&frameOut, 0, sizeof(MAFA_Frame));
	frameOut.buff = (uint8_t*)ofrm->buff + ofrm->offset + ofrm->size;
	frameOut.max = ofrm->max - ofrm->offset - ofrm->size;
	if (isLost)
		frameIn.flags = MAFA_FRAME_IS_EMPTY;
	AudioAlgorithmRet ret = AudioAlgorithmRun(hd, &frameIn, &frameOut);

	PRINT("[%d] inLen:%d,outByte:%d", frames, ifrm->size, frameOut.size);

	if (ret != AUDIO_ALGORITHM_RET_SUCCESS)
	{
		PRINT("AudioAlgorithmRun fail, ret:%d", ret);
		return MMRet_FAIL;
	}

	ifrm->size = 0;
	ifrm->offset = 0;
	ofrm->size += frameOut.size;
	return MMRet_SUCCESS;
}

MMTestRet cbEnd(void* hd)
{
	AudioAlgorithmRet ret;
	ret = AlgorithmDeinit(hd);
	if (ret != AUDIO_ALGORITHM_RET_SUCCESS)
	{
		PRINT("AlgorithmDeinit fail, ret:%d", ret);
		return MMRet_FAIL;
	}

	ret = AudioAlgorithmDestory(hd);
	if (ret != AUDIO_ALGORITHM_RET_SUCCESS)
	{
		PRINT("AudioAlgorithmDestory fail, ret:%d", ret);
		return MMRet_FAIL;
	}

	free(hd);
	return MMRet_SUCCESS;
}



void PlcTest()
{
	MMTestParam testParam = { 0 };
	testParam.mode = MMTestMode::TestModeAudio;
	testParam.open.audio.channels = CHANNELS;
	testParam.open.audio.frameSamples = FRAME_LEN;
	testParam.open.audio.sampleWidth = SAMPLE_STORED_WIDTH;

	testParam.HdInit = cbHdInit;
	testParam.Run = cbRun;
	testParam.End = cbEnd;

	AA_MusicPlcParam param = { 0 };
	param.fsHz = SAMPLE_RATE;
	param.channels = CHANNELS;
	param.frameSamples = FRAME_LEN_MS * param.fsHz / 1000;
	param.overlapMs = OVERLAP_MS;
	param.decayTimeMs = DECAY_TIME_MS;

	testParam.param = (void*)(&param);
	MultiMediaTest(SOURCE_PATH FILE_NAME, &testParam);
}

#if 0

#define MAFAA_REGISTER(name) \
extern void maf_algorithm_audio_##name##_register();\
			maf_algorithm_audio_##name##_register();


#include<stdio.h>
#include<stdlib.h>
#include"AudioAlgorithm.h"

#define PRINT(s,...)		printf(s"\n",##__VA_ARGS__)
void PlcTest()
{
	MAFAA_REGISTER(musicplc);

	const char* algoName = "music_plc";
	AudioAlgorithmRet ret;


	int size = 0;
	ret = AudioAlgorithmGetHandleSize(algoName, &size);
	if (ret != AUDIO_ALGORITHM_RET_SUCCESS)
	{
		PRINT("AudioAlgorithmGetHandleSize fail, ret:%d", ret);
		return;
	}
	PRINT("size:%d", size);


	void* hd = malloc(size);
	ret = AudioAlgorithmCreate(algoName, hd);
	if (ret != AUDIO_ALGORITHM_RET_SUCCESS)
	{
		PRINT("AudioAlgorithmCreate fail, ret:%d", ret);
		return;
	}
	PRINT("hd:%x", (unsigned)hd);


	ret= AlgorithmSetMemory(hd,(Malloc_t*)malloc, (Realloc_t*)realloc, (Calloc_t*)calloc, (Free_t*)free);
	if (ret != AUDIO_ALGORITHM_RET_SUCCESS)
	{
		PRINT("AlgorithmSetMemory fail, ret:%d", ret);
		return;
	}

	void* param = 0;
	ret = AlgorithmInit(hd, param);
	if (ret != AUDIO_ALGORITHM_RET_SUCCESS)
	{
		PRINT("AlgorithmInit fail, ret:%d", ret);
		return;
	}

	
	uint8_t* in = 0;
	int32_t inByte = 0;
	uint8_t* out = 0;
	int32_t* outByte = 0;
	ret = AudioAlgorithmRun(hd, in, inByte, out, outByte);
	if (ret != AUDIO_ALGORITHM_RET_SUCCESS)
	{
		PRINT("AudioAlgorithmRun fail, ret:%d", ret);
		return;
	}
	
	ret = AlgorithmDeinit(hd);
	if (ret != AUDIO_ALGORITHM_RET_SUCCESS)
	{
		PRINT("AlgorithmDeinit fail, ret:%d", ret);
		return;
	}

	ret = AudioAlgorithmDestory(hd);
	if (ret != AUDIO_ALGORITHM_RET_SUCCESS)
	{
		PRINT("AudioAlgorithmDestory fail, ret:%d", ret);
		return;
	}

	free(hd);
}
#endif