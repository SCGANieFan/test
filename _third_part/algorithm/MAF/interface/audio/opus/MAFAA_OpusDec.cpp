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


void maf_algorithm_audio_opus_dec_register()
{
	MAF_Object::Registe<MAFAA_OpusDec>("opus_dec");
}

MAFAA_OpusDec::MAFAA_OpusDec()
{
}
MAFAA_OpusDec::~MAFAA_OpusDec()
{
}


int32_t MAFAA_OpusDec::Init(void* param)
{
	MAF_PRINT("");
	if(!param)
		return -1;
	MAF_InterfaceOpusDec* decParam = (MAF_InterfaceOpusDec*)param;
	MAF_PRINT("channels:%d",decParam->channels);
	MAF_PRINT("frameSamples:%d",decParam->frameSamples);
	MAF_PRINT("fsHz:%d",decParam->fsHz);
#if 1
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

    int32_t err;
    err = opus_decoder_init((OpusDecoder*)_hd, decParam->fsHz, decParam->channels);
    if (err != OPUS_OK)
    {
        MAF_PRINT("Cannot create decoder\n");
        return -1;
    }
	MAF_PRINT("create decoder success\n");
    return 0;
#endif
}

int32_t MAFAA_OpusDec::Deinit()
{
	MAF_PRINT("");
	_memory.Free(_hd);
	return 0;
}

int32_t MAFAA_OpusDec::Process(MAFA_Frame* frameIn, MAFA_Frame* frameOut)
{

	if(!frameIn
	||!frameOut)
	{
		return -1;	
	}

#if 1
	int32_t outLen;
	
    outLen = opus_decode(
		(OpusDecoder*)_hd,
		frameIn->buff+frameIn->off,
        frameIn->size,
		(opus_int16*)(frameOut->buff+frameOut->off+frameOut->size),
		_audioInfo.frameSamples,
		0);

    if (outLen <= 0)
    {
		
        MAF_PRINT("error decoding frame");
        return -1;
    }
	int32_t inUsed=frameIn->size;
	frameIn->off += inUsed;
	frameIn->size -= inUsed;
	frameOut->size += outLen*_audioInfo.channels* _audioInfo.width;
#endif
	return 0;
}


#endif