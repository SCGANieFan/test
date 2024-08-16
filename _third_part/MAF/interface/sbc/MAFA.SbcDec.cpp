#if 1
#include "MAFA.SbcDec.h"
#include "MAF.Objects.h"
#include "MAF.String.h"
#include "SbcDec.Interface.h"
maf_void maf_sbc_dec_register()
{
	MAF_Object::Registe<MAFA_SbcDec>("sbc_dec");
}

maf_void* MAFA_SbcDec::_malloc = 0;
maf_void* MAFA_SbcDec::_free = 0;

MAFA_SbcDec::MAFA_SbcDec()
{
}
MAFA_SbcDec::~MAFA_SbcDec()
{
}

maf_int32 MAFA_SbcDec::Init()
{
	MAF_PRINT();
	maf_int32 _hdSize = SbcDec_GetSize();
	_hd = _memory.Malloc(_hdSize);
	MAF_PRINT("_hd=%x,size:%d", (maf_uint32)_hd, _hdSize);
	if (!_hd){
		return -1;
	}
	bool ret = SbcDec_Init(_hd);
	if(!ret){
		return -1;
	}
	

	return 0;
}

maf_int32 MAFA_SbcDec::Deinit()
{
	MAF_PRINT();
	_memory.Free(_hd);
	return 0;
}

maf_int32 MAFA_SbcDec::Process(MAF_Data* dataIn, MAF_Data* dataOut)
{
	while (1)
	{
		maf_int32 ret = SbcDec_Run(_hd, dataIn->GetData(), dataIn->GetSize(), dataOut->GetLeftData(), dataOut->GetLeftSize());
		if (ret <= 0)
			break;
		if (_isFirst) {
			_isFirst = false;
			SbcDec_Get(_hd, SBC_DEC_GET_VLC_SIZE, &_vlcSize);
			SbcDec_Get(_hd, SBC_DEC_GET_PCM_SIZE, &_pcmSize);
		}
		dataIn->Used(_vlcSize);
		dataIn->ClearUsed();
		dataOut->Append(_pcmSize);
	}
	return 0;
}

maf_int32 MAFA_SbcDec::Set(const maf_int8* key, maf_void* val)
{
#if 0
	if (MAF_String::StrCompare(key, "decayMs")) {
		_decayMs = (maf_int16)val; return 0;
	}
	else if (MAF_String::StrCompare(key, "gainMs")) {
		_gainMs = (maf_int16)val; return 0;
	}
	else if (MAF_String::StrCompare(key, "overlapMs")) {
		_overlapMs = (maf_int16)val; return 0;
	}
#endif
	return MAF_Audio::Set(key, val);
}

maf_int32 MAFA_SbcDec::Get(const maf_int8* key, maf_void* val)
{
	return MAF_Audio::Get(key, val);
}

typedef void* (*Malloc_cb)(maf_int32);
typedef void (*Free_cb)(maf_void*);
maf_void* MAFA_SbcDec::MallocLocal(int32_t size)
{
#if 1
	static maf_int32 sizeTotal = 0;
	sizeTotal += size;
	maf_void* ptr = ((Malloc_cb)_malloc)(size);
	MAF_PRINT("malloc, ptr:%x, size:%d, sizeTotal:%d,", (maf_uint32)ptr, size, sizeTotal);
	return ptr;
#else
	return ((Malloc_cb)_malloc)(size);
#endif
}

maf_void MAFA_SbcDec::FreeLocal(maf_void* block)
{
#if 1
	MAF_PRINT("free, ptr:%x", (maf_uint32)block);
#endif
	return ((Free_cb)_free)(block);
}

#endif