#pragma once
#include <stdint.h>

namespace Plc_ns{

#define PLC_RET_SUCCESS 0
#define PLC_RET_INPUT_ERROR -1
#define PLC_RET_FAIL -2

class PlcMemory_c {
public:
	PlcMemory_c() {}
	~PlcMemory_c() {}
public:
	virtual void* Malloc(int32_t size) = 0;
	virtual void Free(void* block) = 0;
};

enum PlcMode_e {
	PlcModeMusicPlc = 0,
	PlcModeSbcPlc,
	PlcModeOther,
	PlcModeMax,
};

typedef struct {
	//common
	int32_t fsHz;
	int16_t channels;
	int16_t frameSamples;
	int16_t width;
	enum PlcMode_e mode;
	union {
		//music plc
		struct {
			int32_t overlapSamples;
			int32_t holdSamples;
			int32_t fadeSamples;
			int32_t gainSamples;
			int32_t seekSamples;
			int32_t matchSamples;
			uint16_t channelSelect;
		}MusicPlcParam;
		//other
		struct  {
		}otherParam;
	};
	PlcMemory_c* memory;
	void* other;
}PlcParam_t;


typedef int32_t PlcRet;

class Plc_c{
public:
	Plc_c(){};
	~Plc_c(){};
public:
	static int32_t GetStateSize(PlcParam_t* param);
	static PlcRet Init(void* plcStateIn, PlcParam_t* param);
	static PlcRet Run(void* plcStateIn, uint8_t* in, int32_t inLen, int32_t* inUsed, uint8_t* out, int32_t* pOutLen, uint16_t isLost);
	static PlcRet Deinit(void* plcStateIn);
};

} // namespace Plc_ms
