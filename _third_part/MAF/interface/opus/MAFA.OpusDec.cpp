#if 1
#include<string.h>

// MAF
#include "MAFA.OpusDec.h"
#include "MAF.Objects.h"
#include "OpusMemory.h"

// opus
#include "OpusApi.h"
using namespace OpusApi_ns;

maf_void maf_opus_dec_register()
{
	MAF_Object::Registe<MAFA_OpusDec>("opus_dec");
	OpusApiMemory_t opusApiMemory;
	opusApiMemory.malloc_cb = opus_malloc;
	opusApiMemory.realloc_cb = opus_realloc;
	opusApiMemory.free_cb = opus_free;
	OpusApi::memory_register(&opusApiMemory);
}

MAFA_OpusDec::MAFA_OpusDec()
{

}
MAFA_OpusDec::~MAFA_OpusDec()
{
}


maf_int32 MAFA_OpusDec::Init()
{
	MAF_PRINT("");
	MAF_PRINT("channels:%d",_ch);
	MAF_PRINT("frameSamples:%d",_frameSamples);
	MAF_PRINT("fsHz:%d",_rate);

	OpusApiRet_t ret = OpusApi::create_decoder(&_hd, _rate, _ch);
	if (ret != OPUS_API_RET_SUCCESS) {
		MAF_PRINT("Cannot create decoder: %d\n", ret);
		return false;
	}
	MAF_PRINT("create decoder success\n");
	return 0;
}

maf_int32 MAFA_OpusDec::Deinit()
{
	MAF_PRINT("");
	OpusApi::destory_decoder(_hd);
	_hd = 0;
	// _memory.Free(_hd);
	return 0;
}

maf_int32 MAFA_OpusDec::Process(MAF_Data* dataIn, MAF_Data* dataOut)
{
#if 0
	maf_int32 inUsed=dataIn->size;
	dataIn->off += inUsed;
	dataIn->size -= inUsed;
	dataOut->size += outLen*_audioInfo.channels* _audioInfo.width;
#endif
	maf_uint8* iBuff = (maf_uint8*)dataIn->GetData();
	maf_int32 iSize = dataIn->GetSize();
	maf_int16* oBuff = (maf_int16*)dataOut->GetLeftData();
	maf_int32 oSample = dataOut->GetLeftSize()/(2*_ch);
	maf_bool isPlc = false;
	OpusApiRet_t ret = OpusApi::decoder_run(_hd, iBuff, iSize, oBuff, &oSample, isPlc);
	if(ret!=OPUS_API_RET_SUCCESS){
		return -1;
	}
	dataIn->Used(iSize);
	dataIn->ClearUsed();
	dataOut->Append((maf_uint8*)oBuff, oSample*2*_ch);
	return 0;
}

maf_int32 MAFA_OpusDec::Set(const maf_int8* key, maf_void* val)
{
#if 0
	if (MAF_String::StrCompare(key, "decayMs")) {
		_decayMs = (maf_int16)val; return 0;
	}
	else if (MAF_String::StrCompare(key, "gainMs")) {
		_gainMs = (maf_int16)val; return 0;
	}
#endif
	return MAF_Audio::Set(key, val);
}

maf_int32 MAFA_OpusDec::Get(const maf_int8* key, maf_void* val)
{
	return MAF_Audio::Get(key, val);
}
#endif