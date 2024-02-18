#pragma once

#include<stdint.h>

typedef struct
{
	void* buff;
	int32_t offset;
	int32_t size;
	int32_t max;
}Frame;

enum TestMode
{
	TestModeAudio = 0,
	TestModeImage,
	TestModeVedio,
	TestModeMultimedia,
	TestModeMax,
};

typedef void* CbHdInit(void* param, bool* isSuccess);
typedef int32_t CbRun(void* hd, Frame* ifrm, Frame* ofrm);

typedef struct
{
	void* param;
	TestMode mode;
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
	}open;

	CbHdInit* HdInit;
	CbRun* Run;

}TestParam;

typedef int32_t MultiMediaTestRet;
#define MMRet_SUCCESS 0
#define MMRet_FAIL -1
MultiMediaTestRet MultiMediaTest(const char* fileInName, TestParam* testParam);
