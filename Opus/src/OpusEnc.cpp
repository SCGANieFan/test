#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "opus.h"
#include "opus_types.h"
#include "opus_private.h"
#include "OpusEnc.h"


int OpusEncoderSize(int channels)
{
    return opus_encoder_get_size(channels);
}


int OpusEncoderInit(void *hd,OpusEncInfo_t*pEncInfo)
{
    int err;
    //OpusEncoder* enc = NULL;
    //enc = opus_encoder_create(pEncInfo->sampling_rate_hz, pEncInfo->channels, pEncInfo->application, &err);
    OpusEncoder* enc = (OpusEncoder*)hd;
    err = opus_encoder_init(enc, pEncInfo->sampling_rate_hz, pEncInfo->channels, pEncInfo->application);
    if (err != OPUS_OK)
    {
        //printf("Cannot create encoder: %d\n", err);
        goto failure;
    }
    err = opus_encoder_ctl(enc, OPUS_SET_BITRATE(pEncInfo->bitrate_bps));
    err = opus_encoder_ctl(enc, OPUS_SET_BANDWIDTH(pEncInfo->bandwidth));
    err = opus_encoder_ctl(enc, OPUS_SET_VBR(pEncInfo->use_vbr));
    err = opus_encoder_ctl(enc, OPUS_SET_VBR_CONSTRAINT(pEncInfo->cvbr));
    err = opus_encoder_ctl(enc, OPUS_SET_COMPLEXITY(pEncInfo->complexity));
    err = opus_encoder_ctl(enc, OPUS_SET_INBAND_FEC(pEncInfo->use_inbandfec));
    err = opus_encoder_ctl(enc, OPUS_SET_FORCE_CHANNELS(pEncInfo->forcechannels));
    err = opus_encoder_ctl(enc, OPUS_SET_DTX(pEncInfo->use_dtx));
    //err = opus_encoder_ctl(enc, OPUS_SET_PACKET_LOSS_PERC(pEncInfo->packet_loss_perc));
    err = opus_encoder_ctl(enc, OPUS_SET_LSB_DEPTH(pEncInfo->lsb_depth));
    err = opus_encoder_ctl(enc, OPUS_SET_EXPERT_FRAME_DURATION(pEncInfo->variable_duration));
    if (err != OPUS_OK)
    {
        //printf("opus_encoder_ctl err: %d\n", err);
        goto failure;
    }
    //printf("OpusEncoderCreat success!\n");
    //*hd = (void*)enc;
    return 0;
failure:
    //printf("OpusEncoderCreat failure!\n");
    //*hd = (void*)NULL;
    return -1;
}




int OpusEnc(void* hd,OpusEncIn_t *pOpusEncIn)
{
    int ret = EXIT_FAILURE;
    *pOpusEncIn->bitOutLen = opus_encode((OpusEncoder*)hd, (const opus_int16*)pOpusEncIn->pcmIn, pOpusEncIn->pcmInLen, pOpusEncIn->bitOut, *pOpusEncIn->bitOutLen);
    if (*pOpusEncIn->bitOutLen < 0)
    {        
        //fprintf(stderr, "opus_encode() returned %d\n", *pOpusEncIn->bitOutLen);
        goto failure;
    }
    ret = EXIT_SUCCESS;
failure:
    return ret;
}
