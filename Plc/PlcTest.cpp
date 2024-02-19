#pragma once
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdint.h>
#include"plc.h"
#include"MMTest.h"


void* cbHdInit(void *param0,bool *isSuccess)
{
	PlcParam* param = (PlcParam*)param0;

	bool ret = true;
	int32_t size = GetPlcStateSize(param);
	if (size < 0)
		ret = false;
	void* plcState = malloc(size);
	if (!plcState)
		ret = false;
	PlcRet plcRet;
	plcRet = PlcInit(plcState, param);
	if (plcRet != PLC_RET_SUCCESS)
		ret = false;
	if (isSuccess)
		*isSuccess = ret;
	return plcState;
}

#define NUM_LOST 1
#define NUM_ALL 50
MMTestRet cbRun(void* hd, Frame *ifrm, Frame* ofrm)
{
	bool isLost = false;
	static uint32_t frames = 0;
	frames++;
	if ((frames % NUM_ALL) < (NUM_ALL - NUM_LOST))
		isLost = false;
	else
		isLost = true;
	int8_t* iBuff = (int8_t*)ifrm->buff + ifrm->offset;
	int32_t inByte = ifrm->size;
	int32_t inUsed = 0;

	int8_t* oBuff = (int8_t*)ofrm->buff + ofrm->offset + ofrm->size;
	int32_t outByte = ofrm->max- ofrm->offset- ofrm->size;

	PlcRet ret = PlcRun(hd, iBuff, inByte, &inUsed, oBuff, &outByte, isLost);

	//printf("[%d] inLen:%d,outByte:%d\n", ii++, readNum,outByte);
	if (ret != PLC_RET_SUCCESS)
	{
		printf("plc error\n");
		return MMRet_FAIL;
	}

	ifrm->size = 0;
	ifrm->offset = 0;
	ofrm->size += outByte;
	return MMRet_SUCCESS;
}


#define SOURCE_PATH "../../source/audio/plc/"
#define FILE_NAME "chirp_sin_16k1ch.wav"
#define FRAME_LEN_MS 20
#define SAMPLE_RATE 16000
#define CHANNELS 1
#define DECAY_TIME_MS 30
#define OVERLAP_MS 2
#define FRAME_LEN (FRAME_LEN_MS*SAMPLE_RATE/1000)

void PlcTest()
{
	MMTestParam testParam = { 0 };
	testParam.mode = MMTestMode::TestModeAudio;
	testParam.open.audio.channels = CHANNELS;
	testParam.open.audio.frameSamples = FRAME_LEN;
	testParam.open.audio.sampleWidth = sizeof(int16_t);

	testParam.HdInit = cbHdInit;
	testParam.Run = cbRun;

	PlcParam param = { 0 };
	param.fsHz = SAMPLE_RATE;
	param.channels = CHANNELS;
	param.targetChannels = param.channels;
	param.targetChannel = param.channels;
	param.sampleBits = sizeof(int16_t) << 3;
	param.frameSamples = FRAME_LEN;
	param.mode = PlcMode_e::PlcModeMusicPlc;
	param.MusicPlcParam.overlapMs = OVERLAP_MS;
	param.MusicPlcParam.decayTimeMs = DECAY_TIME_MS;
	testParam.param = (void*)(&param);
	MultiMediaTest(SOURCE_PATH FILE_NAME, &testParam);
}


