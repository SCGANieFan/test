#if 1
#include "MAFA.Resample.h"
#include "MAF.Objects.h"
#include "MAF.String.h"
#include "AudioResample.h"

maf_void maf_auio_resample_register()
{
	MAF_Object::Registe<MAFA_Resample>("auio_resample");
}

maf_void* MAFA_Resample::_malloc = 0;
maf_void* MAFA_Resample::_free = 0;

MAFA_Resample::MAFA_Resample()
{
}
MAFA_Resample::~MAFA_Resample()
{
}

maf_int32 MAFA_Resample::Init()
{
	MAF_PRINT();
#if 1
	_malloc = _memory.GetMalloc();
	_free= _memory.GetFree();
#endif

#if 1
	_hdSize = AudioResample_GetStateSize();
	_hd = _memory.Malloc(_hdSize);
	if (!_hd) {
		return -1;
	}
	MAF_PRINT("_hd=%x,size:%d", (unsigned)_hd, _hdSize);
#if 1
	AudioResampleInitParam param;
	MAF_MEM_SET(&param, 0, sizeof(AudioResampleInitParam));
	_basePorting = _memory.Malloc(sizeof(AlgoBasePorting));
	AlgoBasePorting* basePorting = (AlgoBasePorting*)_basePorting;

	basePorting->Malloc = (ALGO_Malloc_t)MallocLocal;
	basePorting->Free = (ALGO_Free_t)FreeLocal;

	param.basePorting = basePorting;
	param.iFs = _rate;
	param.oFs = _oFs;
	param.width = _width;
	param.channels = _ch;
#endif
	AudioResample_Init(_hd, &param);

#if 1
	//set
	maf_void* ascSetParam[] = {
		(maf_void*)(maf_uint32)0,
	};
	AudioResample_Set(_hd, AudioResample_SetChhoose_e::AUDIO_RESAMPLE_SET_CHOOSE_MAX, ascSetParam);

	//get
	maf_int32 speedQ8;
	maf_void* ascGetParam[] = {
		(maf_void*)(maf_uint32)0,
	};
	AudioResample_Get(_hd, AudioResample_GetChhoose_e::AUDIO_RESAMPLE_GET_CHOOSE_MAX, ascGetParam);
#endif

	int32_t size;
	maf_void* buf;
	size = 10 * 1024;
	buf = _memory.Malloc(size);
	_oDataCache.Init((maf_uint8*)buf, 0, size);
	
#endif
	return 0;
}

maf_int32 MAFA_Resample::Deinit()
{
	MAF_PRINT();
#if 1
	MAF_PRINT("_hd=%x", (maf_uint32)_hd);
	AudioResample_DeInit(_hd);
	_memory.Free(_hd);
	if(_basePorting)
		_memory.Free(_basePorting);

#endif
	return 0;
}

maf_int32 MAFA_Resample::Process(MAF_Data* dataIn, MAF_Data* dataOut)
{
	static int num = 0;
#if 1
	int32_t inLen;
	int32_t outLen;
	
	inLen = dataIn->GetSize();
	outLen = dataOut->GetLeftSize();
	AudioResample_Run(
		_hd,
		dataIn->GetData(),
		&inLen,
		dataOut->GetLeftData(),
		&outLen);
	if (num == 15)
		int a = 1;
	//MAF_PRINT("[%d],isize:%d,iUsed:%d,osize:%d", num++, dataIn->GetSize(), inLen, outLen);

	dataIn->Used(inLen);
	dataOut->Append(outLen);
#if 0
	_oDataCache.Append(outLen);
	if (_oDataCache.GetSize() > 0)
	{
		maf_int32 appendSize = _oDataCache.GetSize();
		appendSize = appendSize > dataOut->GetLeftSize() ? dataOut->GetLeftSize() : appendSize;
		dataOut->Append(_oDataCache.GetData(), appendSize);
		_oDataCache.Used(appendSize);
		_oDataCache.ClearUsed();
	}
#endif
#endif
	return 0;
}

maf_int32 MAFA_Resample::Set(const maf_int8* key, maf_void* val)
{
	if (MAF_String::StrCompare(key, "oFs")){
		_oFs = (maf_uint32)val; return 0;
	}
	return MAF_Audio::Set(key, val);
}

maf_int32 MAFA_Resample::Get(const maf_int8* key, maf_void* val)
{
	return MAF_Audio::Get(key, val);
}

#if 1
maf_void* MAFA_Resample::MallocLocal(int32_t size)
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

maf_void MAFA_Resample::FreeLocal(maf_void* block)
{
#if 1
	MAF_PRINT("free, ptr:%x", (maf_uint32)block);
#endif
	return ((ALGO_Free_t)_free)(block);
}
#endif
#endif