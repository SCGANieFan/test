#if 1
#include<new>
#include"PlcApi.h"
#include"Algo.Memory.h"
using namespace Algo;

#ifdef WIN32
#define USE_MUSIC_PLC 1
#define LOG(func,fmt,...) if(func) func("[%s](%d)" fmt "\n",__func__, __LINE__, ##__VA_ARGS__)
#define PLC_DATA_TYPE_SHORT_16	(1)
#define PLC_DATA_TYPE_INT_32	(0&(!PLC_DATA_TYPE_SHORT_16))
#define PLC_DATA_TYPE_FLOAT_32  (1&(!PLC_DATA_TYPE_SHORT_16)&(!PLC_DATA_TYPE_INT_32))
#else
#define LOG(func,fmt,...) if(func) func("[%s](%d)" fmt, __func__, __LINE__, ##__VA_ARGS__)
#endif

#if USE_MUSIC_PLC
#include"MusicPlc.h"
#endif

class PlcApiBasePort_c : public AlgoBasePorting_c {
public:
	PlcApiBasePort_c() {}
	~PlcApiBasePort_c() {}
public:
	void *Malloc(int size){
		return malloc_cb(size);
	}
	void Free(void *ptr){
		free_cb(ptr);
	}
public:
	void *(*malloc_cb)(int size);
	void (*free_cb)(void *ptr);
};


class PlcApiCom_c {
public:
	PlcApiCom_c() {}
	~PlcApiCom_c() {}
public:
	virtual PlcApiRet Create(void** pHd, PlcApiParam_t* param, PlcApiBasePort_c *plcMemory) const {
		return PLC_RET_NOT_SUPPORT;
	}
	virtual PlcApiRet Run(void* hd, uint8_t* in, int32_t inLen, int32_t* inUsed, uint8_t* out, int32_t* pOutLen, uint16_t isLost) const {
		return PLC_RET_NOT_SUPPORT;
	}
	virtual PlcApiRet Set(void* hd, PlcApiSet_e choose, void* val) const {
		return PLC_RET_NOT_SUPPORT;
	}
	virtual PlcApiRet Get(void* hd, PlcApiGet_e choose, void* val) const {
		return PLC_RET_NOT_SUPPORT;
	}
	virtual PlcApiRet Destory(void* hd) const {
		return PLC_RET_NOT_SUPPORT;
	}
};

template<class T>
class PlcApiMusic_c : public PlcApiCom_c {
public:
	PlcApiMusic_c() {}
	~PlcApiMusic_c() {}
#if USE_MUSIC_PLC
public:
	virtual PlcApiRet Create(void** pHd, PlcApiParam_t* param, PlcApiBasePort_c* plcMemory) const {
		MusicPlc_ns::MusicPlcParam_t musicplcParam;
		ALGO_MEM_SET(&musicplcParam, 0, sizeof(MusicPlc_ns::MusicPlcParam_t));
		musicplcParam.basePorting = (AlgoBasePorting_c*)plcMemory;
		musicplcParam.fsHz = param->fsHz;
		musicplcParam.channels = param->channels;
		musicplcParam.width = param->width;
		musicplcParam.frameSamples = param->frameSamples;
		musicplcParam.overlapSamples = param->MusicPlcParam.overlapSamples;
		musicplcParam.holdSamplesAfterLost = param->MusicPlcParam.holdSamples;
		musicplcParam.attenuateSamplesAfterLost = param->MusicPlcParam.fadeSamples;
		musicplcParam.gainSamplesAfterNoLost = param->MusicPlcParam.gainSamples;
		musicplcParam.seekSamples = param->MusicPlcParam.seekSamples;
		musicplcParam.matchSamples = param->MusicPlcParam.matchSamples;
		void* plc;
		int32_t ret = MusicPlc_ns::MusicPlc_c<T>::Create(&plc, &musicplcParam);
		if (!plc) {
			LOG(plcMemory->print_cb, "plc api create fail, %d", ret);
			return PLC_RET_FAIL;
		}
		MusicPlc_ns::MusicPlc_c<T>::Set(plc, MusicPlc_ns::MusicPlc_SetChhoose_e::MUSIC_PLC_SET_CHANNEL_SELECT, (void*)(int32_t)(param->MusicPlcParam.channelSelect));
		*pHd = plc;
		return PLC_RET_SUCCESS;
	}
	virtual PlcApiRet Run(void* hd, uint8_t* in, int32_t inLen, int32_t* inUsed, uint8_t* out, int32_t* pOutLen, uint16_t isLost) const {
		int32_t ret = MusicPlc_ns::MusicPlc_c<T>::Run(hd, in, inLen, out, pOutLen, isLost);
		if (ret != MUSIC_PLC_RET_SUCCESS) {
			return PLC_RET_FAIL;
		}
		if (inUsed)
			*inUsed = inLen;
		return PLC_RET_SUCCESS;
	}
	virtual PlcApiRet Set(void* hd, PlcApiSet_e choose, void* val) const {
		return PLC_RET_SUCCESS;
	}
	virtual PlcApiRet Get(void* hd, PlcApiGet_e choose, void* val) const {
		switch (choose)
		{
		case PLC_API_GET_HEAP_SIZE: {
			int* heapSize = (int*)((void**)val)[0];
#if 0
			PlcParam* param = (PlcParam*)((void**)val)[1];
			void* pp[] = {
				(void*)param->fsHz,
				(void*)param->channels,
				(void*)param->width,
				(void*)param->frameSamples,
				(void*)param->MusicPlcParam.overlapSamples,
				(void*)param->MusicPlcParam.holdSamples,
				(void*)param->MusicPlcParam.fadeSamples,
				(void*)param->MusicPlcParam.gainSamples,
				(void*)param->MusicPlcParam.seekSamples,
				(void*)param->MusicPlcParam.matchSamples,
				param,
			};
			MusicPlc_ns::MusicPlc_c<T>::Get(0, MusicPlc_GetChhoose_e::MUSIC_PLC_GET_HEAP_SIZE, &pp);
#endif
			*heapSize = 55 * 1024;
			break;
		}
		case PLC_API_GET_MAX:
			return PLC_RET_SUCCESS;
			break;
		default:
			break;
		}
		return PLC_RET_SUCCESS;
	}
	virtual PlcApiRet Destory(void* hd) const {
		MusicPlc_ns::MusicPlc_c<T>::Destory(hd);
		return PLC_RET_SUCCESS;
	}
#endif
};

template<>
class PlcApiMusic_c<bool> : public PlcApiCom_c {
public:
	PlcApiMusic_c() {}
	~PlcApiMusic_c() {}
};

#if PLC_DATA_TYPE_SHORT_16
const static PlcApiMusic_c<int16_t> plcApiMusic;
#elif PLC_DATA_TYPE_INT_32
const static PlcApiMusic_c<int32_t> plcApiMusic;
#elif PLC_DATA_TYPE_FLOAT_32
const static PlcApiMusic_c<float> plcApiMusic;
#else
const static PlcApiMusic_c<bool> plcApiMusic;
#endif

class PlcApi_c {
public:
	PlcApi_c() {}
	~PlcApi_c() {}
public:
	static inline PlcApiRet Create(void** pHd, PlcApiParam_t* param) {
		if (!pHd
			|| !param)
			return PLC_RET_INPUT_ERROR;
		LOG(param->cb_printf, "plc api, (%p,%p)", pHd, param);
		int32_t size = sizeof(PlcApi_c);
		PlcApi_c* plcApi = (PlcApi_c*)param->cb_malloc(size);
		if (!plcApi) {
			LOG(param->cb_printf, "plc api malloc fail, (%p,%d)", param->cb_malloc, size);
			return PLC_RET_FAIL;
		}
		new(plcApi) PlcApi_c();
		plcApi->_basePort.malloc_cb = param->cb_malloc;
		plcApi->_basePort.free_cb = param->cb_free;
		plcApi->_basePort.print_cb = param->cb_printf;

		switch (param->mode) {
		case PlcApiMode_e::PLC_API_MODE_MUSIC_PLC:
			plcApi->_plcCom = &plcApiMusic;
			break;
		default:
			LOG(plcApi->_basePort.print_cb, "plc api mode error, %d", param->mode);
			Destory(plcApi);
			return PLC_RET_FAIL;
			break;
		}
		plcApi->_plcCom->Create(&plcApi->_plc, param, &plcApi->_basePort);
		*pHd = plcApi;
		LOG(param->cb_printf, "plc api create success, (%p,%p)", plcApi, plcApi->_plc);
		return PLC_RET_SUCCESS;
	}

	static PlcApiRet Run(void* hd, uint8_t* in, int32_t inLen, int32_t* inUsed, uint8_t* out, int32_t* pOutLen, uint16_t isLost) {
		if (!hd)
			return PLC_RET_INPUT_ERROR;
		if (!isLost){
			if (!in
				|| inLen < 1)
				return PLC_RET_INPUT_ERROR;
		}
		if (!out)
			return PLC_RET_INPUT_ERROR;
		PlcApi_c *plcApi = (PlcApi_c*)hd;
		return plcApi->_plcCom->Run(plcApi->_plc, in, inLen, inUsed, out, pOutLen, isLost);
	}
	static PlcApiRet Set(void* hd, PlcApiSet_e choose, void* val) {
		if (!hd) {
			return PLC_RET_INPUT_ERROR;
		}
		PlcApi_c* plcApi = (PlcApi_c*)hd;
		LOG(plcApi->_basePort.print_cb, "plc api set, (%p,%d,%d)", hd, choose, val);
		return plcApi->_plcCom->Set(plcApi->_plc, choose, val);
	}
	static PlcApiRet Get(void* hd, PlcApiGet_e choose, void* val) {
		if (!hd) {
			return PLC_RET_INPUT_ERROR;
		}
		PlcApi_c* plcApi = (PlcApi_c*)hd;
		LOG(plcApi->_basePort.print_cb, "plc api get, (%p,%d,%d)", hd, choose, val);
		return plcApi->_plcCom->Get(plcApi->_plc, choose, val);
	}
	static PlcApiRet Destory(void* hd) {
		if (!hd)
			return PLC_RET_INPUT_ERROR;
		PlcApi_c* plcApi = (PlcApi_c*)hd;
		LOG(plcApi->_basePort.print_cb, "plc api destory");
		plcApi->_plcCom->Destory(plcApi->_plc);
		PlcApiBasePort_c basePort = plcApi->_basePort;
		plcApi->~PlcApi_c();
		basePort.Free(hd);
		return PLC_RET_SUCCESS;
	}
public:
	PlcApiBasePort_c _basePort;
	const PlcApiCom_c* _plcCom;
	void* _plc;
};



EXTERNC{

PlcApiRet PlcApiCreate(void** pHd, PlcApiParam_t* param){
	return PlcApi_c::Create(pHd,param);
}

PlcApiRet PlcApiRun(void* hd, uint8_t* in, int32_t inLen, int32_t* inUsed, uint8_t* out, int32_t* pOutLen, uint16_t isLost){
	return PlcApi_c::Run(hd,in,inLen,inUsed,out,pOutLen,isLost);
}

PlcApiRet PlcApiSet(void* hd, PlcApiSet_e choose, void* val) {
	return PlcApi_c::Set(hd,choose,val);
}

PlcApiRet PlcApiGet(void* hd, PlcApiGet_e choose, void* val) {
	return PlcApi_c::Get(hd,choose,val);
}

PlcApiRet PlcApiDestory(void* hd){
	return PlcApi_c::Destory(hd);
}

}

#endif