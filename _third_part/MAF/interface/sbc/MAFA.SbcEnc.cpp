#if 1
#include "MAFA.SbcEnc.h"
#include "MAF.Objects.h"
#include "MAF.String.h"
#include "SbcEnc.Interface.h"

maf_void maf_sbc_enc_register()
{
	MAF_Object::Registe<MAFA_SbcEnc>("sbc_enc");
}

maf_void* MAFA_SbcEnc::_malloc = 0;
maf_void* MAFA_SbcEnc::_free = 0;

MAFA_SbcEnc::MAFA_SbcEnc()
{
}
MAFA_SbcEnc::~MAFA_SbcEnc()
{
}

maf_int32 MAFA_SbcEnc::Init()
{
	MAF_PRINT();
	_hdSize  = SbcEnc_GetSize();
	_hd = _memory.Malloc(_hdSize);

	SbcEncInitParam_t initParam;
	MAF_MEM_SET(&initParam, 0, sizeof(SbcEncInitParam_t));
	initParam.sampleRate = _rate;
	initParam.channels = _ch;
	initParam.bitPool = _bitPool;
	initParam.channelMode = (SbcInterfaceChannelMode_e)_channelMode;
	initParam.allocMethod = (SbcInterfaceAllocMethod_e)_allocMethod;
	initParam.blocks = (SbcInterfaceBlocks_e)_blocks;
	initParam.subBands = (SbcInterfaceSubBandS_e)_subBands;
	
	bool ret = SbcEnc_Init(_hd, &initParam);
	if(!ret){
		MAF_PRINT("sbc_encoder_init error");
		return -1;
	}
	maf_uint32 val;
	ret = SbcEnc_Get(_hd, SBC_ENC_GET_PCM_SIZE, &val);
	if(ret !=SBC_RET_SUCCESS)return -1;
	_pcmSize = val;

	SbcEnc_Get(_hd, SBC_ENC_GET_VLC_SIZE, &val);
	if (ret != SBC_RET_SUCCESS)return -1;
	_vlcSize = val;
	return 0;
}

maf_int32 MAFA_SbcEnc::Deinit()
{
	MAF_PRINT();
	_memory.Free(_hd);
	return 0;
}

maf_int32 MAFA_SbcEnc::Process(MAF_Data* dataIn, MAF_Data* dataOut)
{
	while (1) {
		if (dataIn->GetSize() < _pcmSize) {
			break;
		}

		int ret = SbcEnc_Run(_hd, dataIn->GetData(), dataIn->GetSize(), dataOut->GetLeftData(), dataOut->GetLeftSize());
		dataOut->Append(_vlcSize);
		dataIn->Used(_pcmSize);
		dataIn->ClearUsed();
	}
	return 0;
}

maf_int32 MAFA_SbcEnc::Set(const maf_int8* key, maf_void* val)
{
	if (MAF_String::StrCompare(key, "bitPool")) {
		_bitPool = (maf_uint8)val; return 0;
	}
	else if (MAF_String::StrCompare(key, "blocks")) {
		_blocks = (maf_uint8)val; return 0;
	}
	else if (MAF_String::StrCompare(key, "subBands")) {
		_subBands = (maf_uint8)val; return 0;
	}
	else if (MAF_String::StrCompare(key, "channelMode")) {
		_channelMode = (maf_uint8)val; return 0;
	}
	else if (MAF_String::StrCompare(key, "allocMethod")) {
		_allocMethod = (maf_uint8)val; return 0;
	}
	else if (MAF_String::StrCompare(key, "sbcMode")) {
		_sbcMode = (maf_uint8)val; return 0;
	}
	return MAF_Audio::Set(key, val);
}

maf_int32 MAFA_SbcEnc::Get(const maf_int8* key, maf_void* val)
{
	return MAF_Audio::Get(key, val);
}

typedef void* (*Malloc_cb)(maf_int32);
typedef void (*Free_cb)(maf_void*);
maf_void* MAFA_SbcEnc::MallocLocal(int32_t size)
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

maf_void MAFA_SbcEnc::FreeLocal(maf_void* block)
{
#if 1
	MAF_PRINT("free, ptr:%x", (maf_uint32)block);
#endif
	return ((Free_cb)_free)(block);
}

#endif