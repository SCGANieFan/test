#include"testInner.h"





void fread()
{
	FILE* fRead = NULL;
	uint32_t readLen = FRAME_LEN * CHANNELS;
	int16_t readBuffer[FRAME_LEN * CHANNELS];


	FILE* fWrite = NULL;
	int16_t outBuffer[FRAME_LEN * CHANNELS];

	fRead = fopen(PATH FILE_NAME, "rb+");
	if (!fRead)
	{
		printf("open fial\n");
		return false;
	}

	fWrite = fopen(PATH FILE_NAME ".plc.pcm", "wb+");
	if (!fWrite)
	{
		printf("open fial\n");
		return false;
	}
}



bool FrameTest(const char* fileName,void *param)
{
	char fileOutName[512];
	sprintf(fileOutName, "%s.test", fileName);



	return true;
}


bool FrameTest0()
{
#define FILE_NAME "chirp_sin_16k1ch.wav"


	//prarame init
	void* param = NULL;
	return FrameTest(FILE_NAME, param);

}



