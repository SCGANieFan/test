#if 1
#include<string.h>

// MAF
#include "MAFA.OpusEnc.h"
#include "MAF.Objects.h"
#include "OpusMemory.h"

// opus
#include "OpusApi.h"
using namespace OpusApi_ns;

#include<stdlib.h>
maf_void maf_opus_enc_register()
{
	MAF_Object::Registe<MAFA_OpusEnc>("opus_enc");

	OpusApiMemory_t opusApiMemory;
	opusApiMemory.malloc_cb = opus_malloc;
	opusApiMemory.realloc_cb = opus_realloc;
	opusApiMemory.free_cb = opus_free;
	OpusApi::memory_register(&opusApiMemory);
}

MAFA_OpusEnc::MAFA_OpusEnc()
{
}
MAFA_OpusEnc::~MAFA_OpusEnc()
{
}


maf_int32 MAFA_OpusEnc::Init()
{
	MAF_PRINT("");
	
	MAF_PRINT("channels:%d",_ch);
	MAF_PRINT("frameSamples:%d",_frameSamples);
	MAF_PRINT("fsHz:%d",_rate);

	bool haveHead = false;
	OpusApiRet_t ret = OpusApi::create_encoder(&_hd, _rate, _ch, haveHead);
	if (ret != OPUS_API_RET_SUCCESS)
	{
		MAF_PRINT("opus create fail, %d,(%p,%d,%d,%d)", _hd, _rate, _ch, haveHead);
		return -1;
	}
	_frame0p1Ms = _frameSamples*1000*10/_rate;
	ret = OpusApi::encoder_set(_hd, OpusApi_EncSetChhoose_e::OPUS_API_ENC_SET_BIT_RATE, (void*)_bitrate);
	if(ret != OPUS_API_RET_SUCCESS) return -1;
	ret = OpusApi::encoder_set(_hd, OpusApi_EncSetChhoose_e::OPUS_API_ENC_SET_FRAME_DURATION_0P1MS, (void*)_frame0p1Ms);
	if(ret != OPUS_API_RET_SUCCESS) return -1;
	ret = OpusApi::encoder_set(_hd, OpusApi_EncSetChhoose_e::OPUS_API_ENC_SET_USE_VBR, (void*)false);
	if(ret != OPUS_API_RET_SUCCESS) return -1;
	ret = OpusApi::encoder_set(_hd, OpusApi_EncSetChhoose_e::OPUS_API_ENC_SET_COMPLEXITY, (void*)_complexity);
	if(ret != OPUS_API_RET_SUCCESS) return -1;

	MAF_PRINT("create encoder success");
    return 0;
}

maf_int32 MAFA_OpusEnc::Deinit()
{
	MAF_PRINT("");
	OpusApi::destory_encoder(_hd);
	_hd=0;
	// _memory.Free(_hd);
	return 0;
}

maf_int32 MAFA_OpusEnc::Process(MAF_Data* dataIn, MAF_Data* dataOut)
{
	if (dataIn->GetSize() < _frameBytes) {
		dataOut->SetFlag(MAFA_FRAME_IS_EOS);
		return -1;
	}
	maf_int32 outLen;
	maf_int16 *iBuff = (maf_int16 *)dataIn->GetData();
	maf_uint8 *oBuff = (maf_uint8 *)dataOut->GetData();
	maf_int32 oSize = dataOut->GetLeftSize();
	OpusApiRet_t ret = OpusApi::encoder_run(_hd, iBuff, _frameSamples, oBuff, &oSize);
	if(ret!=OPUS_API_RET_SUCCESS){
		return -1;
	}
	if(oSize<=0){
		return -1;
	}
	
	dataIn->Used(_frameBytes);
	dataIn->ClearUsed();
	dataOut->Append(oBuff, oSize);
	return 0;
}

maf_int32 MAFA_OpusEnc::Set(const maf_int8* key, maf_void* val){
	if (MAF_String::StrCompare(key, "bitrate")) {
		_bitrate = (maf_int32)val; return 0;
	}
	else if (MAF_String::StrCompare(key, "cpmplexity")) {
		_complexity = (maf_int32)val; return 0;
	}
	else if (MAF_String::StrCompare(key, "vbr")) {
		_vbr = !!((maf_int32)val); return 0;
	}
	return MAF_Audio::Set(key, val);
}

maf_int32 MAFA_OpusEnc::Get(const maf_int8* key, maf_void* val) {
	return MAF_Audio::Get(key, val);
}

#endif