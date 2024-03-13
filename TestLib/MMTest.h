#pragma once

#include<stdint.h>

#define MMRet_SUCCESS 0
#define MMRet_FAIL -1
#define MMRet_INPUT_ERR -2
#define MMRet_FILE_OPEN_ERR -3
#define MMRet_MEMORY_ERR -4

typedef struct
{
	void* buff;
	int32_t offset;
	int32_t size;
	int32_t max;
}Frame;


typedef int32_t MMTestRet;
typedef void* CbHdInit(void* param, bool* isSuccess);
typedef MMTestRet CbRun(void* hd, Frame* ifrm, Frame* ofrm);
typedef MMTestRet CbEnd(void* hd);

enum MMTestMode
{
	TestModeAudio = 0,
	TestModeImage,
	TestModeVedio,
	TestModeMultimedia,
	TestModeDemux,
	TestModeMax,
};

typedef struct
{
	void* param;
	MMTestMode mode;
	union {
		struct {
			int32_t frameSamples;
			int32_t sampleWidth;
			int32_t channels;
		}audio;

		struct {
			uint8_t reserve[1024];
		}image;

		struct {
			uint8_t reserve[1024];
		}vedio;

		struct {
			uint8_t reserve[1024];
		}multimedia;

		struct {
			uint8_t reserve[1024];
		}demux;
	}open;
	CbHdInit* HdInit;
	CbRun* Run;
	CbEnd* End;
}MMTestParam;

MMTestRet MultiMediaTest(const char* fileInName, MMTestParam* testParam);




