#include<stdio.h>
#include<stdint.h>
#include<string.h>
#include<stdlib.h>


int32_t pcm24to32(int8_t *in,int32_t inLen, int32_t *inUsed, int8_t* out, int32_t *outLen)
{
#if 1
	uint8_t* pIn = (uint8_t*)in;
	*outLen = 0;
	*inUsed = 0;
	int32_t* pOut = (int32_t*)out;
	while (inLen > 2)
	{
		int32_t outTemp = (((uint32_t)pIn[0]
			+ (((uint32_t)pIn[1]) << 8)
			+ (((uint32_t)pIn[2]) << 16)) << 8);
		*pOut = outTemp>>8;
		pIn += 3;
		*inUsed += 3;
		inLen -= 3;
		pOut++;
		(*outLen)+=4;
	}
#else
	memcpy(out, in, inLen);
	*inUsed = inLen;
	*outLen = inLen;
#endif


	return 0;
}


#define TEST_PATH "./"
//#define TEST_FILE "mbz_48k2h24bit.pcm"
#define TEST_FILE "stSection_16k1ch24bit.pcm"
#define TEST_FILE_SRC TEST_PATH TEST_FILE
#define TEST_FILE_DST TEST_FILE_SRC ".32bit.pcm"

bool test1()
{
	FILE* fIn = fopen(TEST_FILE_SRC, "rb");
	FILE* fOut = fopen(TEST_FILE_DST, "wb");
	int8_t inBuff[1024] = { 0 };

	int8_t outBuff[2*1024] = { 0 };
	int32_t inSize = 0;
	while (1)
	{
		int readNum = fread(inBuff+ inSize, 1, 1024 - inSize, fIn);
		if (!readNum)
			break;
		inSize += readNum;
		int32_t inUsed;
		int32_t outLen;
		pcm24to32(inBuff, inSize, &inUsed, outBuff, &outLen);
		fwrite(outBuff, 1, outLen, fOut);
		//printf("inSize:%d,inUsed:%d,outLen:%d\n", inSize, inUsed, outLen);
		inSize -= inUsed;

		memmove(inBuff, inBuff + inUsed, inSize);
	}
	fclose(fIn);
	fclose(fOut);
	return 0;
}

