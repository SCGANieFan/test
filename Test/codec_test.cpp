#if 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include"M2Fapi.h"



#define FILE_PATH "./"
#define FILE_NAME "Female1_1st_16k1ch.pcm"
#define PCM_LEN 2*1024
#define BIT_LEN 1024

#define ENC
//#define DEC
//#define CODEC

#define OPUS
//#define LC3
//#define SILK



typedef struct {
	int16_t* inBuf;
	int16_t* outBuf;
	uint8_t codecControl[128];
	uint8_t codec[30*1024];
}EncParam;

void EncInit(EncParam* encParam, int16_t* inBuf, int16_t* outBuf)
{
	memset(encParam, 0, sizeof(EncParam));
	encParam->inBuf = inBuf;
	encParam->outBuf = outBuf;
	M2F* m2f = new(encParam->codec)M2F(encParam->codec + sizeof(M2F), 30 * 1024 - sizeof(M2F));
	m2f->AudioRegister();
#ifdef OPUS
	M2FOpusEncInfo_t* encInfo = (M2FOpusEncInfo_t*)encParam->codecControl;
	encInfo->sampling_rate_hz = 16000;
	encInfo->channels = 1;
	encInfo->application = 2048;
	encInfo->bitrate_bps = 11000;
	encInfo->bandwidth = 2;
	encInfo->use_vbr = 0;
	encInfo->cvbr = 0;
	encInfo->complexity = 0;
	encInfo->use_inbandfec = 0;
	encInfo->forcechannels = 1;
	encInfo->use_dtx = 0;
	encInfo->lsb_depth = 0;
	encInfo->variable_duration = 0;
	encInfo->frame_duration_0p1ms = 200;
	m2f->CodecCreate("Opusx");
	m2f->EncoderInit("Opusx", encInfo);
#endif
#ifdef LC3

#endif
#ifdef SILK

#endif

	m2f->UseMipsMonitor();
	m2f->UseMemMonitor();
}

int32_t Encode(EncParam* encParam, int16_t* pcmIn, uint32_t pcmInLen, uint8_t* bitOut, uint32_t* bitOutLen)
{
	M2F* m2f = (M2F*)encParam->codec;
	int32_t ret;
#ifdef OPUS
	ret = m2f->AudioEncode("Opusx", pcmIn, pcmInLen, bitOut, bitOutLen);
#endif

	return ret;
}

typedef struct {


}DecParam;
void DecInit(DecParam* decParam, )
{
	//
	//	typedef struct {
	//		int sampling_rate_hz;
	//		unsigned char channels;
	//	}M2FOpusDecInfo_t;

	decParam

}

void Decode(DecParam* decParam, )
{
	decParam
}


void CodecTest()
{
		char nameTemp[256] = { 0 };
	FILE* fin = NULL;
	FILE* fencoded = NULL;
	FILE* fdecoded = NULL;

	//in
	sprintf(nameTemp, "%s", FILE_PATH FILE_NAME);
	fopen_s(&fin, nameTemp, "rb");

	//encoded
	sprintf(nameTemp + strlen(nameTemp), ".%s", codecType);
	fopen_s(&fencoded, nameTemp, "wb");

	//decoded
	//sprintf(nameTemp + strlen(nameTemp), ".pcm", bps / 1000, complexity);
	//sprintf(nameTemp + strlen(nameTemp), "Ori_fl%dkc%d.pcm", bps / 1000, complexity);
	sprintf(nameTemp + strlen(nameTemp), "Ori_fp%dkc%d.pcm", bps / 1000, complexity);
	fopen_s(&fdecoded, nameTemp, "wb");

	//M2FAudioCodec* audioCodec = M2FCreateAudioCodec(codecType);
	M2F m2f;
	m2f.AudioRegister();
	m2f.CodecCreate(codecType);
	m2f.UseMipsMonitor();
	m2f.UseMemMonitor();

	unsigned int pcmRemLen = 0;
	short pcmIn[PCM_LEN] = { 0 };

	unsigned int bitOutLen = 0;
	char bitOut[BIT_LEN] = { 0 };

	unsigned int pcmOutLen = 0;
	short pcmOut[PCM_LEN] = { 0 };

	void* pEncInfo = (void*)malloc(sizeof(char) * 512);
	memset(pEncInfo, 0, sizeof(char) * 512);

	

	EncParam encParam = { 0 };
	DecParam decParam = { 0 };
	EncInit(&encParam, );
	DecInit

	while (true)
	{
		Encode(&encParam, int16_t * pcmIn, uint32_t pcmInLen, uint8_t * bitOut, uint32_t * bitOutLen)


		M2FAudioCodecEncodeIn encIn = { 0 };
		M2FAudioCodecDecodeIn decIn = { 0 };

		encIn.pcmIn = pcmIn;
		encIn.bitOut = (unsigned char*)bitOut;
		encIn.bitOutLen = &bitOutLen;

		decIn.bitIn = encIn.bitOut;
		decIn.pcmOut = pcmOut;
		decIn.pcmOutLen = &pcmOutLen;

		int duration_ms = 20;
		int samplerate = 16000;
		int frameSize = samplerate * duration_ms / 1000;
		int readNum = 0;

		while (1)
		{
			readNum = fread_s(pcmIn, sizeof(pcmIn[0]) * frameSize, sizeof(pcmIn[0]), frameSize, fin);
			if (readNum < frameSize)
				break;
			encIn.pcmInLen = readNum;
			bitOutLen = BIT_LEN;
			encIn.bitOutLen = &bitOutLen;

			m2f.AudioEncode(codecType, pcmIn, readNum, (unsigned char*)bitOut, &bitOutLen);
			fwrite(bitOut, sizeof(bitOut[0]), bitOutLen, fencoded);

			decIn.bitInLen = *encIn.bitOutLen;
			pcmOutLen = PCM_LEN;
			decIn.pcmOutLen = &pcmOutLen;

			m2f.AudioDecode(codecType, (unsigned char*)bitOut, bitOutLen, pcmOut, &pcmOutLen);
			fwrite(pcmOut, sizeof(pcmOut[0]), pcmOutLen, fdecoded);
		}


		Decode
	}


}


void CodecTest()
{
	char nameTemp[256] = { 0 };
	FILE* fin = NULL;
	FILE* fencoded = NULL;
	FILE* fdecoded = NULL;

	//in
	sprintf(nameTemp, "%s", FILE_PATH FILE_NAME);
	fopen_s(&fin, nameTemp, "rb");

	//encoded
	sprintf(nameTemp + strlen(nameTemp), ".%s", codecType);
	fopen_s(&fencoded, nameTemp, "wb");

	//decoded
	//sprintf(nameTemp + strlen(nameTemp), ".pcm", bps / 1000, complexity);
	//sprintf(nameTemp + strlen(nameTemp), "Ori_fl%dkc%d.pcm", bps / 1000, complexity);
	sprintf(nameTemp + strlen(nameTemp), "Ori_fp%dkc%d.pcm", bps / 1000, complexity);
	fopen_s(&fdecoded, nameTemp, "wb");

	//M2FAudioCodec* audioCodec = M2FCreateAudioCodec(codecType);
	M2F m2f;
	m2f.AudioRegister();
	m2f.CodecCreate(codecType);
	m2f.UseMipsMonitor();
	m2f.UseMemMonitor();

	unsigned int pcmRemLen = 0;
	short pcmIn[PCM_LEN] = { 0 };

	unsigned int bitOutLen = 0;
	char bitOut[BIT_LEN] = { 0 };

	unsigned int pcmOutLen = 0;
	short pcmOut[PCM_LEN] = { 0 };

	void* pEncInfo = (void*)malloc(sizeof(char) * 512);
	memset(pEncInfo, 0, sizeof(char) * 512);

	void* pDecInfo = (void*)malloc(sizeof(char) * 512);
	memset(pDecInfo, 0, sizeof(char) * 512);



}


#if 0




#define FILE_PATH "./"
//#define FILE_NAME "chirp_sin_16k1ch.pcm" 
#define FILE_NAME "Female1_1st_16k1ch.pcm" 
#define PCM_LEN 2*1024
#define BIT_LEN 1024

void CodecTest(const char* codecType, int complexity, int bps)
{
	char nameTemp[256] = { 0 };
	FILE* fin = NULL;
	FILE* fencoded = NULL;
	FILE* fdecoded = NULL;

	//in
	sprintf(nameTemp, "%s", FILE_PATH FILE_NAME);
	fopen_s(&fin, nameTemp, "rb");

	//encoded
	sprintf(nameTemp + strlen(nameTemp), ".%s", codecType);
	fopen_s(&fencoded, nameTemp, "wb");

	//decoded
	//sprintf(nameTemp + strlen(nameTemp), ".pcm", bps / 1000, complexity);
	//sprintf(nameTemp + strlen(nameTemp), "Ori_fl%dkc%d.pcm", bps / 1000, complexity);
	sprintf(nameTemp + strlen(nameTemp), "Ori_fp%dkc%d.pcm", bps / 1000, complexity);
	fopen_s(&fdecoded, nameTemp, "wb");

	//M2FAudioCodec* audioCodec = M2FCreateAudioCodec(codecType);
	M2F m2f;
	m2f.AudioRegister();
	m2f.CodecCreate(codecType);
	m2f.UseMipsMonitor();
	m2f.UseMemMonitor();

	unsigned int pcmRemLen = 0;
	short pcmIn[PCM_LEN] = { 0 };

	unsigned int bitOutLen = 0;
	char bitOut[BIT_LEN] = { 0 };

	unsigned int pcmOutLen = 0;
	short pcmOut[PCM_LEN] = { 0 };

	void* pEncInfo = (void*)malloc(sizeof(char) * 512);
	memset(pEncInfo, 0, sizeof(char) * 512);

	void* pDecInfo = (void*)malloc(sizeof(char) * 512);
	memset(pDecInfo, 0, sizeof(char) * 512);

	if (!strcmp(codecType, "AudioCodecTest"))
	{
		pEncInfo = NULL;
		pDecInfo = NULL;
	}

	else if (!strcmp(codecType, "Opusx"))
	{
		M2FOpusEncInfo_t* encInfo = (M2FOpusEncInfo_t*)pEncInfo;
		encInfo->sampling_rate_hz = 16000;
		encInfo->channels = 1;
		encInfo->application = 2048;
		encInfo->bitrate_bps = bps;
		encInfo->complexity = complexity;
		encInfo->frame_duration_0p1ms = 200;

		M2FOpusDecInfo_t* decInfo = (M2FOpusDecInfo_t*)pDecInfo;
		decInfo->sampling_rate_hz = encInfo->sampling_rate_hz;
		decInfo->channels = encInfo->channels;

	}

	else if (!strcmp(codecType, "Silk"))
	{
		M2FSilkEncInfo_t* encInfo = (M2FSilkEncInfo_t*)pEncInfo;
		encInfo->sample_rate = 16000;
		encInfo->max_internal_fs_Hz = encInfo->sample_rate;
		encInfo->bit_rate = bps;
		encInfo->packetSize_ms = 20;
		encInfo->complexity = complexity;

		M2FSilkDecInfo_t* decInfo = (M2FSilkDecInfo_t*)pDecInfo;
		decInfo->sample_rate = encInfo->sample_rate;
	}

	m2f.EncoderInit(codecType, pEncInfo);
	m2f.DecoderInit(codecType, pDecInfo);

	M2FAudioCodecEncodeIn encIn = { 0 };
	M2FAudioCodecDecodeIn decIn = { 0 };

	encIn.pcmIn = pcmIn;
	encIn.bitOut = (unsigned char*)bitOut;
	encIn.bitOutLen = &bitOutLen;

	decIn.bitIn = encIn.bitOut;
	decIn.pcmOut = pcmOut;
	decIn.pcmOutLen = &pcmOutLen;

	int duration_ms = 20;
	int samplerate = 16000;
	int frameSize = samplerate * duration_ms / 1000;
	int readNum = 0;

	while (1)
	{
		readNum = fread_s(pcmIn, sizeof(pcmIn[0]) * frameSize, sizeof(pcmIn[0]), frameSize, fin);
		if (readNum < frameSize)
			break;
		encIn.pcmInLen = readNum;
		bitOutLen = BIT_LEN;
		encIn.bitOutLen = &bitOutLen;

		m2f.AudioEncode(codecType, pcmIn, readNum, (unsigned char*)bitOut, &bitOutLen);
		fwrite(bitOut, sizeof(bitOut[0]), bitOutLen, fencoded);

		decIn.bitInLen = *encIn.bitOutLen;
		pcmOutLen = PCM_LEN;
		decIn.pcmOutLen = &pcmOutLen;

		m2f.AudioDecode(codecType, (unsigned char*)bitOut, bitOutLen, pcmOut, &pcmOutLen);
		fwrite(pcmOut, sizeof(pcmOut[0]), pcmOutLen, fdecoded);
	}
	m2f.PrintCodecMips(codecType);
	free(pEncInfo);
	free(pDecInfo);
	fclose(fin);
	fclose(fencoded);
	fclose(fdecoded);
}



void test1()
{
	//int complexity[] = { 0 };
	//int bps[] = { 6000 };
	int complexity[] = { 0 };
	int bps[] = { 6000,8000,10000,12000,15000,20000,30000 };
	for (int i = 0; i < sizeof(complexity) / sizeof(complexity[0]); i++)
	{
		for (int j = 0; j < sizeof(bps) / sizeof(bps[0]); j++)
		{
			printf("complexity:%d,bps:%d\n", complexity[i], bps[j]);
			CodecTest("Opusx", complexity[i], bps[j]);
		}
	}
	//CodecTest("Silk");
}
#endif

#endif