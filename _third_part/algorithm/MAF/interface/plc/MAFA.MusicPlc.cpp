#if 1
#include "MAFA.MusicPlc.h"
#include "MAF.Objects.h"
#include "MAF.String.h"
#include "MusicPlc.h"

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

maf_int32 MAFA_MusicPlc::Init()
{
	MAF_PRINT();
	MusicPlcInitParam initParam;
	MAF_MEM_SET(&initParam, 0, sizeof(MusicPlcInitParam));
#if 1
	_malloc = _memory.GetMalloc();
	_free = _memory.GetFree();

	_basePorting = _memory.Malloc(sizeof(AlgoBasePorting));
	AlgoBasePorting* basePorting = (AlgoBasePorting*)_basePorting;

	basePorting->Malloc = (ALGO_Malloc_t)MallocLocal;
	basePorting->Free = (ALGO_Free_t)FreeLocal;
	initParam.basePorting = basePorting;
#endif
	initParam.fsHz = _rate;
	initParam.channels = _ch;
	initParam.width = _width;
	initParam.frameSamples = _frameSamples;
	initParam.overlapUs = _overlapMs * 1000;
	initParam.decayTimeUs = _decayMs * 1000;

	_hdSize = MusicPlc_GetStateSize();
	_hd = _memory.Malloc(_hdSize);
	MAF_PRINT("_hd=%x,size:%d", (maf_uint32)_hd, _hdSize);
	if (!_hd)
	{
		return -1;
	}

	maf_int32 ret = MusicPlc_StateInit(_hd, &initParam);

	if (ret < 0)
	{
		return -1;
	}
	return 0;
}

maf_int32 MAFA_MusicPlc::Deinit()
{
	MAF_PRINT();
	MusicPlc_StateDeInit(_hd);
	_memory.Free(_hd);
	_memory.Free(_basePorting);

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
		ret = MusicPlc_Run(
			_hd,
			NULL,
			0,
			dataOut->GetLeftData(),
			&outByte,
			true);
#else
		outByte= _frameSamples* _width *_ch;
		MAF_MEM_SET(dataOut->GetLeftData(), 0, outByte);
#endif
	}
	else
	{
		ret = MusicPlc_Run(
			_hd,
			dataIn->GetData(),
			dataIn->GetSize(),
			dataOut->GetLeftData(),
			&outByte,
			false);
	}
	if (ret < 0)
	{
		return -1;
	}
	dataIn->Used(dataIn->GetSize());
	dataIn->ClearUsed();
	dataOut->Append(outByte);
	return 0;
}

maf_int32 MAFA_MusicPlc::Set(const maf_int8* key, maf_void* val)
{
	if (MAF_String::StrCompare(key, "decayMs")) {
		_decayMs = (maf_int16)val; return 0;
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

maf_void* MAFA_MusicPlc::MallocLocal(int32_t size)
{
#if 1
	static maf_int32 sizeTotal = 0;
	sizeTotal += size;
	maf_void* ptr = ((ALGO_Malloc_t)_malloc)(size);
	MAF_PRINT("malloc, ptr:%x, size:%d, sizeTotal:%d,", (maf_uint32)ptr, size, sizeTotal);
	return ptr;
#else
	return ((ALGO_Malloc_t)_malloc)(size);
#endif	
}

maf_void MAFA_MusicPlc::FreeLocal(maf_void* block)
{
#if 1
	MAF_PRINT("free, ptr:%x", (maf_uint32)block);
#endif
	return ((ALGO_Free_t)_free)(block);
}

#endif