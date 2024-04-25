#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdint.h>
#include"MMTest.h"

#include"AudioAlgorithm.h"
#include"MAF.Interface.Opus.h"

#define SOURCE_PATH "../../source/audio/opus/"


//#define FILE_NAME "Female1_1st_16k1ch_1s.wav"
#define FILE_NAME "stSection_16k1ch.wav"
//#define FILE_NAME "sin4k_16k1ch.wav"
#define CHANNELS 1
#define SAMPLE_RATE 16000
#define SAMPLE_WIDTH 2
#define FRAME_LEN_MS 20
#define FRAME_LEN (FRAME_LEN_MS*SAMPLE_RATE/1000)




#define MAFAA_REGISTER(name) \
extern void maf_algorithm_audio_##name##_register();\
			maf_algorithm_audio_##name##_register();

#define PRINT(s,...)		printf(s"\n",##__VA_ARGS__)



typedef struct {
	MAF_InterfaceOpusEnc encParam;
	MAF_InterfaceOpusDec decParam;
}OpusParam;

typedef struct {
	void* hdEnc;
	void* hdDec;
}HD_s;

static inline void OpusInit(void **hd,const char *codec,void *param)
{
	int32_t size = 0;
	AudioAlgorithmRet ret;
	ret = AudioAlgorithmGetHandleSize(codec, &size);
	if (ret != AUDIO_ALGORITHM_RET_SUCCESS)
	{
		PRINT("AudioAlgorithmGetHandleSize fail, ret:%d", ret);
		return;
	}
	PRINT("%s size:%d", codec, size);
	*hd = malloc(size);
	ret = AudioAlgorithmCreate(codec, *hd);
	if (ret != AUDIO_ALGORITHM_RET_SUCCESS)
	{
		PRINT("AudioAlgorithmCreate fail, ret:%d", ret);
		return;
	}
	PRINT("%s hd:%x", codec,(unsigned)*hd);


	ret = AlgorithmSetMemory(*hd, (Malloc_t*)malloc, (Realloc_t*)realloc, (Calloc_t*)calloc, (Free_t*)free);
	if (ret != AUDIO_ALGORITHM_RET_SUCCESS)
	{
		PRINT("AlgorithmSetMemory fail, ret:%d", ret);
		return;
	}

	ret = AlgorithmInit(*hd, param);
	if (ret != AUDIO_ALGORITHM_RET_SUCCESS)
	{
		PRINT("AlgorithmInit fail, ret:%d", ret);
		return;
	}
}


void* cbHdInit(void* param0, bool* isSuccess)
{



	*isSuccess = false;
	MAFAA_REGISTER(opus_enc);
	MAFAA_REGISTER(opus_dec);

	
	HD_s *hd = (HD_s*)malloc(sizeof(HD_s));
	AudioAlgorithmRet ret;
	OpusParam* param = (OpusParam*)param0;

	
	OpusInit(&hd->hdEnc, "opus_enc", &param->encParam);
	OpusInit(&hd->hdDec, "opus_dec", &param->decParam);


	*isSuccess = true;
	return hd;
}

#define NUM_TOTAL 50
#define NUM_LOST 5

MMTestRet cbRun(void* hd, Frame* ifrm, Frame* ofrm)
{
	HD_s* pHd = (HD_s*)hd;

	static uint32_t frames = 0;
	frames++;


	MAFA_Frame frameIn;
	memset(&frameIn, 0, sizeof(MAFA_Frame));
	frameIn.buff = (uint8_t*)ifrm->buff + ifrm->offset;
	frameIn.max = frameIn.size = ifrm->size;


	if (frames % NUM_TOTAL > (NUM_TOTAL - NUM_LOST))
	{
		memset(frameIn.buff, 0, frameIn.max);
	}


	uint8_t encodedBuf[1024];
	MAFA_Frame frameEncoded;
	memset(&frameEncoded, 0, sizeof(MAFA_Frame));
	frameEncoded.buff = encodedBuf;
	frameEncoded.max = 1024;

	MAFA_Frame frameOut;
	memset(&frameOut, 0, sizeof(MAFA_Frame));
	frameOut.buff = (uint8_t*)ofrm->buff + ofrm->offset + ofrm->size;
	frameOut.max = ofrm->max - ofrm->offset - ofrm->size;

	AudioAlgorithmRet ret;
	
	ret = AudioAlgorithmRun(pHd->hdEnc, &frameIn, &frameEncoded);
	int32_t encodedSize = frameEncoded.size;
	ret = AudioAlgorithmRun(pHd->hdDec, &frameEncoded, &frameOut);
	PRINT("[%d] inByte:%d,encodedByte:%d,outByte:%d", frames, ifrm->size, encodedSize, frameOut.size);

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
	HD_s* pHd = (HD_s*) hd;
	ret = AlgorithmDeinit(pHd->hdDec);
	ret = AudioAlgorithmDestory(pHd->hdDec);

	ret = AlgorithmDeinit(pHd->hdEnc);
	ret = AudioAlgorithmDestory(pHd->hdEnc);


	free(pHd->hdDec);
	free(pHd->hdEnc);
	free(pHd);
	return MMRet_SUCCESS;
}



void OpusTest()
{
	MMTestParam testParam = { 0 };
	testParam.mode = MMTestMode::TestModeAudio;
	testParam.open.audio.channels = CHANNELS;
	testParam.open.audio.frameSamples = FRAME_LEN;
	testParam.open.audio.sampleWidth = SAMPLE_WIDTH;

	testParam.HdInit = cbHdInit;
	testParam.Run = cbRun;
	testParam.End = cbEnd;

	OpusParam param = {0};

	param.encParam.fsHz = SAMPLE_RATE;
	param.encParam.channels = CHANNELS;
	param.encParam.frameSamples = FRAME_LEN_MS * param.encParam.fsHz / 1000;
	param.encParam.bitrate = 12000;
	
	param.decParam.fsHz = SAMPLE_RATE;
	param.decParam.channels = CHANNELS;
	param.decParam.frameSamples = FRAME_LEN_MS * param.decParam.fsHz / 1000;


	testParam.param = (void*)(&param);
	MultiMediaTest(SOURCE_PATH FILE_NAME, &testParam);
}
