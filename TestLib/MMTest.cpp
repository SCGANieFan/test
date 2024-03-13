#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"MMTest.h"

#define MMTestPrint(fmt,...)		printf("%s [%d] " fmt "\n", __func__, __LINE__, ##__VA_ARGS__)
void PrintErr(MMTestRet ret)
{
	switch (ret)
	{
	case MMRet_SUCCESS:
		break;
	case MMRet_FAIL:
		MMTestPrint("fail");
		break;
	case MMRet_INPUT_ERR:
		MMTestPrint("input error");
		break;
	case MMRet_FILE_OPEN_ERR:
		MMTestPrint("file open error");
		break;
	case MMRet_MEMORY_ERR:
		MMTestPrint("memory error");
		break;
	default:
		break;
	}
}


MMTestRet AudioTest(const char* fileInName, MMTestParam* testParam)
{
	MMTestRet ret = MMRet_SUCCESS;
	FILE* fRead = NULL;
	char fileOutName[512];
	FILE* fWrite = NULL;
	int32_t frameByte = 0;
	Frame ifrm = { 0 };
	Frame ofrm = { 0 };
	void* hd = NULL;

	//check
	if (!fileInName
		|| !testParam){
		ret = MMRet_INPUT_ERR;
		goto exitAudio;
	}

	//in file
	fRead = fopen(fileInName, "rb+");
	if (!fRead){
		ret = MMRet_FILE_OPEN_ERR;
		goto exitAudio;
	}

	//out file	
	sprintf(fileOutName, "%s.pcm", fileInName);
	fWrite = fopen(fileOutName, "wb+");
	if (!fWrite){
		ret = MMRet_FILE_OPEN_ERR;
		goto exitAudio;
	}

	frameByte = testParam->open.audio.channels * testParam->open.audio.frameSamples * testParam->open.audio.sampleWidth;

	
	ifrm.max = frameByte;
	ifrm.buff = malloc(ifrm.max);
	if (!ifrm.buff){
		ret = MMRet_MEMORY_ERR;
		goto exitAudio;
	}

	
	ofrm.max = frameByte;
	ofrm.buff = malloc(ofrm.max);
	if (!ofrm.buff) {
		ret = MMRet_MEMORY_ERR;
		goto exitAudio;
	}
	bool initRet;
	hd = testParam->HdInit(testParam->param, &initRet);
	if (!initRet)
	{
		ret = MMRet_FAIL;
		goto exitAudio;
	}
	//int32_t outLen;
	int32_t readByte;
	while (1)
	{
		readByte = fread((uint8_t*)ifrm.buff + ifrm.offset, 1, frameByte, fRead);
		if (readByte < frameByte)
			break;
		ifrm.size += readByte;

		MMTestRet runRet = testParam->Run(hd, &ifrm, &ofrm);
		if (runRet != MMRet_SUCCESS){
			ret = runRet;
			goto exitAudio;
		}
		if (ifrm.offset) {
			memmove(ifrm.buff, (uint8_t*)ifrm.buff + ifrm.offset, ifrm.size);
			ifrm.offset = 0;
		}
		fwrite((uint8_t*)ofrm.buff + ofrm.offset, 1, ofrm.size, fWrite);
		ofrm.size = 0;
		ofrm.offset = 0;
	}
exitAudio:
	PrintErr(ret);
	return ret;
}

MMTestRet DemuxTest(const char* fileInName, MMTestParam* testParam)
{
	MMTestRet ret = MMRet_SUCCESS;
	FILE* fRead = NULL;
	char fileOutName[512];
	FILE* fWrite = NULL;
	//int32_t frameByte = 0;
	Frame ifrm = { 0 };
	Frame ofrm = { 0 };
	void* hd = NULL;

	//check
	if (!fileInName
		|| !testParam) {
		ret = MMRet_INPUT_ERR;
		goto exitAudio;
	}

	//in file
	fRead = fopen(fileInName, "rb+");
	if (!fRead) {
		ret = MMRet_FILE_OPEN_ERR;
		goto exitAudio;
	}

	//out file	
	sprintf(fileOutName, "%s.demux", fileInName);
	fWrite = fopen(fileOutName, "wb+");
	if (!fWrite) {
		ret = MMRet_FILE_OPEN_ERR;
		goto exitAudio;
	}

	//frameByte = testParam->open.audio.channels * testParam->open.audio.frameSamples * testParam->open.audio.sampleWidth;


	ifrm.max = 40;
	ifrm.buff = malloc(ifrm.max);
	if (!ifrm.buff) {
		ret = MMRet_MEMORY_ERR;
		goto exitAudio;
	}


	ofrm.max = 5*1024;
	ofrm.buff = malloc(ofrm.max);
	if (!ofrm.buff) {
		ret = MMRet_MEMORY_ERR;
		goto exitAudio;
	}
	bool initRet;
	hd = testParam->HdInit(testParam->param, &initRet);
	if (!initRet)
	{
		ret = MMRet_FAIL;
		goto exitAudio;
	}
	//int32_t outLen;
	int32_t readByte;
	while (1)
	{
		readByte = fread((uint8_t*)ifrm.buff + ifrm.offset, 1, ifrm.max, fRead);
		if (readByte < ifrm.max)
			break;
		ifrm.size += readByte;

		MMTestRet runRet = testParam->Run(hd, &ifrm, &ofrm);
		if (runRet != MMRet_SUCCESS) {
			ret = runRet;
			goto exitAudio;
		}
		if (ifrm.offset) {
			memmove(ifrm.buff, (uint8_t*)ifrm.buff + ifrm.offset, ifrm.size);
			ifrm.offset = 0;
		}
		fwrite((uint8_t*)ofrm.buff + ofrm.offset, 1, ofrm.size, fWrite);
		ofrm.size = 0;
		ofrm.offset = 0;
	}
exitAudio:
	testParam->End(hd);
	PrintErr(ret);
	return ret;
}

MMTestRet MultiMediaTest(const char* fileInName, MMTestParam* testParam)
{

	if (!fileInName
		|| !testParam)
	{
		return MMRet_INPUT_ERR;
	}

	MMTestRet ret;
	switch (testParam->mode)
	{
	case MMTestMode::TestModeAudio:
		ret = AudioTest(fileInName, testParam);
		break;
	case MMTestMode::TestModeImage:
	case MMTestMode::TestModeVedio:
	case MMTestMode::TestModeMultimedia:
	case MMTestMode::TestModeDemux:
	case MMTestMode::TestModeMax:
		ret = DemuxTest(fileInName, testParam);
		break;
	default:
		ret = MMRet_SUCCESS;
		break;
	}
	return ret;
}







