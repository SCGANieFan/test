#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include"testLib.h"
MultiMediaTestRet MultiMediaTest(const char* fileInName, TestParam* testParam)
{

#if 1
	//in file
	FILE* fRead = fopen(fileInName, "rb+");
	if (!fRead)
	{
		printf("open fial\n");
		return MMRet_FAIL;
	}

	//out file
	char fileOutName[512];
	sprintf(fileOutName, "%s.pcm", fileInName);
	FILE* fWrite = fopen(fileOutName, "wb+");
	if (!fWrite)
	{
		printf("open fial\n");
		return MMRet_FAIL;
	}

	int32_t frameByte = testParam->open.audio.channels * testParam->open.audio.frameSamples * testParam->open.audio.sampleWidth;

	Frame ifrm = { 0 };
	ifrm.max = frameByte;
	ifrm.buff = malloc(ifrm.max);
	if (!ifrm.buff)
		return MMRet_FAIL;

	Frame ofrm = { 0 };
	ofrm.max = frameByte;
	ofrm.buff = malloc(ofrm.max);
	if (!ofrm.buff)
		return MMRet_FAIL;

	void* plcState = testParam->HdInit(testParam->param, NULL);

	int32_t outLen;
	uint32_t readByte;
	while (1)
	{
		readByte = fread((uint8_t*)ifrm.buff + ifrm.offset, 1, frameByte, fRead);
		if (readByte < frameByte)
			break;
		ifrm.size += readByte;

		testParam->Run(plcState, &ifrm, &ofrm);
		if (ifrm.offset) {
			memmove(ifrm.buff, (uint8_t*)ifrm.buff + ifrm.offset, ifrm.size);
			ifrm.offset = 0;
		}
		fwrite((uint8_t*)ofrm.buff + ofrm.offset, 1, ofrm.size, fWrite);
		ofrm.size = 0;
		ofrm.offset = 0;
	}
	return MMRet_SUCCESS;
#endif

}