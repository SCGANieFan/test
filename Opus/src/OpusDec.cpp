#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "opus.h"
#include "opus_multistream.h"
#include "opus_private.h"
#include "opus_types.h"
#include "OpusDec.h"


int OpusDecoderSize(int channels)
{
    return opus_decoder_get_size(channels);
}

int OpusDecoderInit(void*hd, OpusDecInfo_t *pOpusDecInfo)
{
    int err;
    OpusDecoder* dec = (OpusDecoder*)hd;
    //dec = opus_decoder_create(pOpusDecInfo->sampling_rate_hz, pOpusDecInfo->channels, &err);
    err = opus_decoder_init(dec, pOpusDecInfo->sampling_rate_hz, pOpusDecInfo->channels);
    if (err != OPUS_OK)
    {
        printf("Cannot create decoder\n");
        goto failure;
    }
    //*hd = (void*)dec;
    return 0;
failure:
    //*hd = NULL;
    return -1;
}



int OpusDec(void* hd,OpusDecIn_t *pOpusDecIn)
{
    int ret = EXIT_FAILURE;
    *pOpusDecIn->pcmOutLen = opus_decode((OpusDecoder*)hd, pOpusDecIn->bitIn,
        pOpusDecIn->bitInLen, pOpusDecIn->pcmOut, *pOpusDecIn->pcmOutLen, 0);
    if (*pOpusDecIn->pcmOutLen <= 0)
    {
        printf("error decoding frame\n");
        goto failure;
    }


    ret = EXIT_SUCCESS;
failure:
    return ret;
}

