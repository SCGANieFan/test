#if 1
#include<string.h>

// MAF
#include "MAFAA_OpusDec.h"
#include "MAF.Objects.h"
#include "MAF.Interface.Opus.h"
// opus
#include "opus.h"
#include "opus_multistream.h"
#include "opus_private.h"
#include "opus_types.h"


maf_void maf_algorithm_audio_opus_dec_register()
{
	MAF_Object::Registe<MAFAA_OpusDec>("opus_dec");
}

MAFAA_OpusDec::MAFAA_OpusDec()
{
}
MAFAA_OpusDec::~MAFAA_OpusDec()
{
}


maf_int32 MAFAA_OpusDec::Init()
{
	MAF_PRINT("");
#if 0
	if(!param)
		return -1;
	MAF_InterfaceOpusDec* decParam = (MAF_InterfaceOpusDec*)param;
	MAF_PRINT("channels:%d",decParam->channels);
	MAF_PRINT("frameSamples:%d",decParam->frameSamples);
	MAF_PRINT("fsHz:%d",decParam->fsHz);
	_audioInfo.channels = decParam->channels;
	_audioInfo.fsHz = decParam->fsHz;
	_audioInfo.width = 2;
	_audioInfo.frameSamples = decParam->frameSamples;


	_hdSize = opus_decoder_get_size(decParam->channels);
	_hd = _memory.Malloc(_hdSize);
	if(!_hd)
	{
		MAF_PRINT("Cannot create decoder,hdSize:%d",_hdSize);
        return -1;
	}

    maf_int32 err;
    err = opus_decoder_init((OpusDecoder*)_hd, decParam->fsHz, decParam->channels);
    if (err != OPUS_OK)
    {
        MAF_PRINT("Cannot create decoder\n");
        return -1;
    }
	MAF_PRINT("create decoder success\n");
#endif
	return 0;
}

maf_int32 MAFAA_OpusDec::Deinit()
{
	MAF_PRINT("");
	_memory.Free(_hd);
	return 0;
}

maf_int32 MAFAA_OpusDec::Process(MAF_Data* dataIn, MAF_Data* dataOut)
{
#if 0

	if(!dataIn
	||!dataOut)
	{
		return -1;	
	}

	maf_int32 outLen;
	
    outLen = opus_decode(
		(OpusDecoder*)_hd,
		dataIn->buff+dataIn->off,
        dataIn->size,
		(opus_int16*)(dataOut->buff+dataOut->off+dataOut->size),
		_audioInfo.frameSamples,
		0);

    if (outLen <= 0)
    {
		
        MAF_PRINT("error decoding frame");
        return -1;
    }
	maf_int32 inUsed=dataIn->size;
	dataIn->off += inUsed;
	dataIn->size -= inUsed;
	dataOut->size += outLen*_audioInfo.channels* _audioInfo.width;
#endif
	return 0;
}


#endif