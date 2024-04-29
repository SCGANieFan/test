#if 1
#include<string.h>

// MAF
#include "MAFAA_OpusEnc.h"
#include "MAF.Objects.h"
#include "MAF.Interface.Opus.h"
// opus
#include "opus.h"
#include "opus_types.h"
#include "opus_private.h"


maf_void maf_algorithm_audio_opus_enc_register()
{
	MAF_Object::Registe<MAFAA_OpusEnc>("opus_enc");
}

MAFAA_OpusEnc::MAFAA_OpusEnc()
{
}
MAFAA_OpusEnc::~MAFAA_OpusEnc()
{
}


maf_int32 MAFAA_OpusEnc::Init()
{
	MAF_PRINT("");
#if 0
	if(!param)
		return -1;
	MAF_InterfaceOpusEnc* encParam = (MAF_InterfaceOpusEnc*)param;
	MAF_PRINT("channels:%d",encParam->channels);
	MAF_PRINT("frameSamples:%d",encParam->frameSamples);
	MAF_PRINT("fsHz:%d",encParam->fsHz);
	_audioInfo.channels = encParam->channels;
	_audioInfo.fsHz = encParam->fsHz;
	_audioInfo.width = 2;
	_audioInfo.frameSamples = encParam->frameSamples;

	_encInfo.bitrate_bps=encParam->bitrate;
	_encInfo.lsb_depth=_audioInfo.width<<3;

	_hdSize = opus_encoder_get_size(encParam->channels);
	_hd = _memory.Malloc(_hdSize);
	if(!_hd)
	{
		MAF_PRINT("Cannot create decoder,hdSize:%d",_hdSize);
        return -1;
	}

    maf_int32 err;
    err = opus_encoder_init((OpusEncoder*)_hd, encParam->fsHz, encParam->channels,OPUS_APPLICATION_VOIP);

    if (err != OPUS_OK)
    {
        MAF_PRINT("Cannot create encoder\n");
        return -1;
    }
	
	_encInfo.use_vbr = 0;
	_encInfo.complexity = 0;
	
	_encInfo.variable_duration = OPUS_FRAMESIZE_20_MS;

	OpusEncoder* enc = (OpusEncoder*)_hd;
	
    err = opus_encoder_ctl(enc, OPUS_SET_BITRATE(_encInfo.bitrate_bps));
    // err = opus_encoder_ctl(enc, OPUS_SET_BANDWIDTH(_encInfo.bandwidth));
    err = opus_encoder_ctl(enc, OPUS_SET_VBR(_encInfo.use_vbr));
    // err = opus_encoder_ctl(enc, OPUS_SET_VBR_CONSTRAINT(_encInfo.cvbr));
    err = opus_encoder_ctl(enc, OPUS_SET_COMPLEXITY(_encInfo.complexity));
    // err = opus_encoder_ctl(enc, OPUS_SET_INBAND_FEC(_encInfo.use_inbandfec));
    // err = opus_encoder_ctl(enc, OPUS_SET_FORCE_CHANNELS(_encInfo.forcechannels));
    // err = opus_encoder_ctl(enc, OPUS_SET_DTX(_encInfo.use_dtx));
    //err = opus_encoder_ctl(enc, OPUS_SET_PACKET_LOSS_PERC(_encInfo.packet_loss_perc));
    err = opus_encoder_ctl(enc, OPUS_SET_LSB_DEPTH(_encInfo.lsb_depth));
	
    err = opus_encoder_ctl(enc, OPUS_SET_EXPERT_FRAME_DURATION(_encInfo.variable_duration));
    if (err != OPUS_OK)
    {
        MAF_PRINT("opus_encoder_ctl err: %d", err);
        return -1;
    }
	MAF_PRINT("create encoder success\n");
    return 0;
#else
    return 0;
#endif
}

maf_int32 MAFAA_OpusEnc::Deinit()
{
	MAF_PRINT("");
	_memory.Free(_hd);
	return 0;
}

maf_int32 MAFAA_OpusEnc::Process(MAF_Data* dataIn, MAF_Data* dataOut)
{
#if 0
	if(!dataIn
	||!dataOut)
	{
		return -1;	
	}

	maf_int32 outLen;
    outLen = opus_encode(
		(OpusEncoder*)_hd, 
		(const opus_int16*)(dataIn->buff+dataIn->off),
		dataIn->size,
		dataOut->buff+dataOut->off+dataOut->size,
		dataOut->max-dataOut->off-dataOut->size);

    if (outLen < 0)
    {        
        MAF_PRINT("opus_encode() returned %d", outLen);
        return -1;
    }
	
	maf_int32 inUsed=_audioInfo.frameSamples*_audioInfo.channels*_audioInfo.width;
	dataIn->off += inUsed;
	dataIn->size -= inUsed;
	dataOut->size += outLen;	
#endif
	return 0;
}


#endif