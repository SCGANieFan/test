#pragma once

#include"plcCommon.h"

#define PLC_RET_SUCCESS 0
#define PLC_RET_INPUT_ERROR -1
#define PLC_RET_FAIL -2


enum PlcMode_e {
	PlcModeMusicPlc = 0,
	PlcModeSbcPlc,
	PlcModeOther,
	PlcModeMax,
};


enum PlcSbcCodecType_e {
	PlcSbcCodecTypeSbc = 0,
	PlcSbcCodecTypeAac,
	PlcSbcCodecTypeLc3,
	PlcSbcCodecTypeCvsd,
	PlcSbcCodecTypeAmrNb,
	PlcSbcCodecTypeAmrWb,
	PlcSbcCodecTypeMax,
};

typedef struct {
	//common
	int32_t fsHz;
	int32_t channels;
	int32_t targetChannels;
	int32_t targetChannel;
	int32_t sampleBits;
	int32_t frameSamples;
	enum PlcMode_e mode;

	union {
		//music plc
		struct {
			int32_t overlapMs;
			int32_t decayTimeMs;
		}MusicPlcParam;

		//sbcplc
		struct {
			union {
				enum PlcSbcCodecType_e codecType;

				uint32_t searchSize;
				uint32_t matchSize;
				uint32_t olal;
				uint32_t recover;
			};
		}SbcPlcParam;
	};
	void* other;
}PlcParam;

typedef int32_t PlcRet;


EXTERNC int32_t GetPlcStateSize(PlcParam* param);
EXTERNC PlcRet PlcInit(void* plcStateIn, PlcParam* param);
EXTERNC PlcRet PlcRun(void* plcStateIn, int8_t* in, int32_t inLen, int32_t* inUsed, int8_t* out, int32_t* pOutLen, bool isLost);

