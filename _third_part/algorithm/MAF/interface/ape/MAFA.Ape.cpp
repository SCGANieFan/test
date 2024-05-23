#if 1
#include "MAFA.Ape.h"
#include "MAF.Objects.h"
#include "MAF.String.h"
//#include "Ape.h"
#include "ApeOri.h"

maf_void maf_ape_register()
{
	MAF_Object::Registe<MAFA_Ape>("ape");
}

maf_void* MAFA_Ape::_malloc = 0;
maf_void* MAFA_Ape::_free = 0;

MAFA_Ape::MAFA_Ape()
{
}
MAFA_Ape::~MAFA_Ape()
{
}

maf_int32 MAFA_Ape::Init()
{
	MAF_PRINT();
	ApeInitParam initParam;
	MAF_MEM_SET(&initParam, 0, sizeof(ApeInitParam));
#if 1
	_malloc = _memory.GetMalloc();
	_free = _memory.GetFree();

	_basePorting = _memory.Malloc(sizeof(AlgoBasePorting));
	AlgoBasePorting* basePorting = (AlgoBasePorting*)_basePorting;

	basePorting->Malloc = (ALGO_Malloc_t)MallocLocal;
	basePorting->Free = (ALGO_Free_t)FreeLocal;
	initParam.basePorting = basePorting;
#endif
	initParam.mode = ApeInitMode::APE_INIT_MODE_DEC_USE_NO_PARAM;

	_hdSize = Ape_GetStateSize();
	_hd = _memory.Malloc(_hdSize);
	MAF_PRINT("_hd=%x,size:%d", (maf_uint32)_hd, _hdSize);
	if (!_hd)
	{
		return -1;
	}

	maf_int32 ret = Ape_StateInit(_hd, &initParam);
	if (ret < 0)
	{
		return -1;
	}
	return 0;
}

maf_int32 MAFA_Ape::Deinit()
{
	MAF_PRINT();
	Ape_StateDeInit(_hd);
	_memory.Free(_hd);
	_memory.Free(_basePorting);

	return 0;
}

maf_int32 MAFA_Ape::Process(MAF_Data* dataIn, MAF_Data* dataOut)
{
	maf_int32 ret;
	maf_int32 outByte = dataOut->GetLeftSize();
	ret = Ape_Run(_hd, dataIn->GetData(), dataIn->GetSize(), dataOut->GetLeftData(), &outByte);
	if (ret < 0) {
		return -1;
	}
	dataIn->Used(dataIn->GetSize());
	dataIn->ClearUsed();
	dataOut->Append(outByte);
	return 0;
}

maf_int32 MAFA_Ape::Set(const maf_int8* key, maf_void* val)
{
#if 0
	if (MAF_String::StrCompare(key, "decayMs")) {
		_decayMs = (maf_int16)val; return 0;
	}
	else if (MAF_String::StrCompare(key, "overlapMs")) {
		_overlapMs = (maf_int16)val; return 0;
	}
#endif
	return MAF_Audio::Set(key, val);
}

maf_int32 MAFA_Ape::Get(const maf_int8* key, maf_void* val)
{
	return MAF_Audio::Get(key, val);
}

maf_void* MAFA_Ape::MallocLocal(int32_t size)
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

maf_void MAFA_Ape::FreeLocal(maf_void* block)
{
#if 1
	MAF_PRINT("free, ptr:%x", (maf_uint32)block);
#endif
	return ((ALGO_Free_t)_free)(block);
}

#endif