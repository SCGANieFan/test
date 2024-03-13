#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdint.h>
#include"OggDemux2Opus.h"
#include"MMTest.h"

void* cbHdInit(void* param0, bool* isSuccess)
{
	int hdSize = GetOggDemux2OpusStateSize();
	void* hd = malloc(hdSize);
	if (!hd)
	{
		if (isSuccess)
			*isSuccess = false;
		return NULL;
	}
	if (OggDemux2OpusStateInit(hd) != OGG_DMUX_TO_OPUS_RET_SUCCESS)
	{
		if (isSuccess)
			*isSuccess = false;
		return NULL;
	}
	if (isSuccess)
		*isSuccess = true;
	return hd;

}
int num = 0;
int inputByteAcc = 0;
MMTestRet cbRun(void* hd, Frame* ifrm, Frame* ofrm)
{
	OggDmux2OpusRet ret;

	int8_t* iBuff = (int8_t*)ifrm->buff + ifrm->offset;
	int32_t inByte = ifrm->size;
	int32_t inUsed = 0;
	ret = OggDemuxReceive(hd, (char*)iBuff, inByte);
	if (ret != OGG_DMUX_TO_OPUS_RET_SUCCESS)
		return MMRet_FAIL;
	ifrm->size = 0;
	ifrm->offset = 0;
#if 1
	int8_t* oBuff = (int8_t*)ofrm->buff + ofrm->offset + ofrm->size;
	int32_t outByte = ofrm->max - ofrm->offset - ofrm->size;
	ret = OggDemuxGenerate(hd, (char*)oBuff, &outByte);
	num++;
	printf("[%d] buff:%x,ret:%d, iByte:%d,iByteAcc:%d, oByte:%d\n", num, ((int*)iBuff)[0], ret, inByte, inputByteAcc, outByte);
	inputByteAcc += inByte;
	ofrm->size += outByte;
#else
	while (1)
	{
		int8_t* oBuff = (int8_t*)ofrm->buff + ofrm->offset + ofrm->size;
		int32_t outByte = ofrm->max - ofrm->offset - ofrm->size;
		ret = OggDemuxGenerate(hd, (char*)oBuff, &outByte);
		if (outByte < 1)
			break;
		num++;
		printf("[%d] buff:%x,ret:%d, iByte:%d,iByteAcc:%d, oByte:%d\n", num, ((int*)buffer)[0], ret, bytes, inputByteAcc, outByte);
		inputByteAcc += bytes;
		ofrm->size += outByte;
	}
#endif
	return MMRet_SUCCESS;
}

MMTestRet cbEnd(void* hd)
{
	OggDemux2OpusEnd(hd);
	return MMRet_SUCCESS;
}

#define SOURCE_PATH "../../source/demux/ogg/"
#define FILE_NAME "test.opusx.ogg"
void OggTest()
{
	MMTestParam testParam = { 0 };
	testParam.mode = MMTestMode::TestModeDemux;
	//testParam.open.demux.reserve=0;
	testParam.HdInit = cbHdInit;
	testParam.Run = cbRun;
	testParam.End = cbEnd;

	testParam.param = NULL;
	MultiMediaTest(SOURCE_PATH FILE_NAME, &testParam);
}



