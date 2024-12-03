#if 1
#include "MAFA.MusicPlc.h"
#include "MAF.Objects.h"
#include "MAF.String.h"
#include "Plc.h"
using namespace Plc_ns;

maf_void maf_music_plc_register()
{
	MAF_Object::Registe<MAFA_MusicPlc>("music_plc");
}

maf_void* MAFA_MusicPlc::_malloc = 0;
maf_void* MAFA_MusicPlc::_free = 0;

MAFA_MusicPlc::MAFA_MusicPlc()
{
}
MAFA_MusicPlc::~MAFA_MusicPlc()
{
}

class PlcMemoryLocal_c : public PlcMemory_c {
public:
	PlcMemoryLocal_c() {}
	~PlcMemoryLocal_c() {}
public:
	void Init(MAF_Memory* memory) {
		_memory = memory;
	}
	virtual void* Malloc(int32_t size) final {
#if 1
		static maf_int32 sizeTotal = 0;
		sizeTotal += size;
		maf_void* ptr = _memory->Malloc(size);
		MAF_PRINT("malloc, ptr:%x, size:%d, sizeTotal:%d,", (maf_uint32)ptr, size, sizeTotal);
		return ptr;
#else
		return ((ALGO_Malloc_t)_malloc)(size);
#endif
	};
	virtual void Free(void* block) final {
#if 1
		MAF_PRINT("free, ptr:%x", (maf_uint32)block);
#endif
		return _memory->Free(block);
	};
private:
	MAF_Memory* _memory;
};
static PlcMemoryLocal_c plcMemoryLocal;
maf_int32 MAFA_MusicPlc::Init()
{
	MAF_PRINT();
	PlcParam_t initParam;
	MAF_MEM_SET(&initParam, 0, sizeof(PlcParam_t));
#if 1
	_malloc = _memory.GetMalloc();
	_free = _memory.GetFree();

#endif
#if 0
	typedef struct {
		//common
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
			struct {
			}otherParam;
		};
		PlcMemory_c* memory;
		void* other;
	}PlcParam_t;
#endif

	initParam.fsHz = _rate;
	initParam.channels = _ch;
	initParam.frameSamples = _frameSamples;
	initParam.width = _width;
	initParam.mode = PlcMode_e::PlcModeMusicPlc;
	initParam.MusicPlcParam.overlapSamples = _overlapMs * _rate / 1000;
	initParam.MusicPlcParam.holdSamples = 0 * _rate / 1000;
	initParam.MusicPlcParam.fadeSamples = _decayMs * _rate / 1000;
	initParam.MusicPlcParam.gainSamples = _gainMs * _rate / 1000;
	initParam.MusicPlcParam.seekSamples = 8 * _rate / 1000;
	initParam.MusicPlcParam.matchSamples = 3* _rate / 1000;
	initParam.MusicPlcParam.channelSelect = 0xffff;
	plcMemoryLocal.Init(&_memory);
	initParam.memory = (PlcMemory_c*) & plcMemoryLocal;
	_hdSize = Plc_c::GetStateSize(&initParam);
	_hd = _memory.Malloc(_hdSize);
	MAF_PRINT("_hd=%x,size:%d", (maf_uint32)_hd, _hdSize);
	if (!_hd)
	{
		return -1;
	}

	maf_int32 ret = Plc_c::Init(_hd, &initParam);

	if (ret < 0)
	{
		return -1;
	}
	return 0;
}

maf_int32 MAFA_MusicPlc::Deinit()
{
	MAF_PRINT();
	Plc_c::Deinit(_hd);
	_memory.Free(_hd);

	return 0;
}

maf_int32 MAFA_MusicPlc::Process(MAF_Data* dataIn, MAF_Data* dataOut)
{
	maf_int32 ret;

	maf_int32 outByte = dataOut->GetLeftSize();

	if (dataIn->CheckFlag(MAFA_FRAME_IS_EMPTY))
	{
#if 1
		dataIn->ClearFlag(MAFA_FRAME_IS_EMPTY);
		ret = Plc_c::Run(
			_hd,
			NULL,
			0,
			0,
			dataOut->GetLeftData(),
			&outByte,
			true);
#else
		outByte= _frameSamples* _width *_ch;
		MAF_MEM_SET(dataOut->GetLeftData(), 0, outByte);
#endif
		dataOut->Append(outByte);
	}
	else
	{
#if 0
		ret = MusicPlc_Run(
			_hd,
			dataIn->GetData(),
			dataIn->GetSize(),
			dataOut->GetLeftData(),
			&outByte,
			false);
		dataOut->Append(outByte);
#else
		dataOut->Append(dataIn->GetData(), dataIn->GetSize());
		ret = Plc_c::Run(
			_hd,
			dataOut->GetData(),
			dataOut->GetSize(),
			0,
			dataOut->GetData(),
			&outByte,
			false);
#endif
	}
	if (ret < 0)
	{
		return -1;
	}
	dataIn->Used(dataIn->GetSize());
	dataIn->ClearUsed();
	return 0;
}

maf_int32 MAFA_MusicPlc::Set(const maf_int8* key, maf_void* val)
{
	if (MAF_String::StrCompare(key, "decayMs")) {
		_decayMs = (maf_int16)val; return 0;
	}
	else if (MAF_String::StrCompare(key, "gainMs")) {
		_gainMs = (maf_int16)val; return 0;
	}
	else if (MAF_String::StrCompare(key, "overlapMs")) {
		_overlapMs = (maf_int16)val; return 0;
	}
	return MAF_Audio::Set(key, val);
}

maf_int32 MAFA_MusicPlc::Get(const maf_int8* key, maf_void* val)
{
	return MAF_Audio::Get(key, val);
}

#endif