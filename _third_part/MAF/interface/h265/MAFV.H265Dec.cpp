#if 1
#include "MAFV.H265Dec.h"
#include "MAF.Objects.h"
#include "MAF.String.h"
#include "H265Dec.Interface.h"
maf_void maf_h265_dec_register()
{
	MAF_Object::Registe<MAFV_H265Dec>("h265_dec");
}

maf_void* MAFV_H265Dec::_malloc = 0;
maf_void* MAFV_H265Dec::_free = 0;

MAFV_H265Dec::MAFV_H265Dec()
{
}
MAFV_H265Dec::~MAFV_H265Dec()
{
}

maf_int32 MAFV_H265Dec::Init()
{
	MAF_PRINT();
	maf_int32 _hdSize = H265Dec_GetSize();
	_hd = _memory.Malloc(_hdSize);
	MAF_PRINT("_hd=%x,size:%d", (maf_uint32)_hd, _hdSize);
	if (!_hd){
		return -1;
	}
	H265DecInitParam_t initParam;	
	maf_bool ret = H265Dec_Init(_hd,&initParam);
	if(!ret){
		return -1;
	}
	return 0;
}

maf_int32 MAFV_H265Dec::Deinit()
{
	MAF_PRINT();
	maf_bool ret = H265Dec_DeInit(_hd);
	if(!ret){
		return -1;
	}
	_memory.Free(_hd);
	return 0;
}
#if 0
maf_int32 MAFV_H265Dec::Process(MAF_Data* dataIn, MAF_Data* dataOut)
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
#endif
maf_int32 MAFV_H265Dec::Rceive(MAF_Data* dataIn){
	maf_uint8 *iBuff = dataIn->GetData();
	maf_int32 iBuffSize = dataIn->GetSize();
	int32_t ret = H265Dec_Receive(_hd, iBuff, iBuffSize);
	if(ret!=H265_DEC_RET_SUCCESS)
		return -1;
	dataIn->Used(iBuffSize);
	dataIn->ClearUsed();
	return 0;
}
maf_int32 MAFV_H265Dec::Generate(MAF_Data* dataOut){
	maf_uint8 *oBuff = dataOut->GetLeftData();
	maf_int32 oBuffSize = dataOut->GetLeftSize();
	int32_t ret = H265Dec_Generate(_hd, oBuff, &oBuffSize);
	if(ret!=H265_DEC_RET_SUCCESS)
		return -1;
	dataOut->Append(oBuffSize);
	return 0;
}
maf_int32 MAFV_H265Dec::Set(const maf_int8* key, maf_void* val)
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
	return MAF_Video::Set(key, val);
}

maf_int32 MAFV_H265Dec::Get(const maf_int8* key, maf_void* val)
{
	return MAF_Video::Get(key, val);
}

typedef void* (*Malloc_cb)(maf_int32);
typedef void (*Free_cb)(maf_void*);
maf_void* MAFV_H265Dec::MallocLocal(int32_t size)
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

maf_void MAFV_H265Dec::FreeLocal(maf_void* block)
{
#if 1
	MAF_PRINT("free, ptr:%x", (maf_uint32)block);
#endif
	return ((Free_cb)_free)(block);
}

#endif