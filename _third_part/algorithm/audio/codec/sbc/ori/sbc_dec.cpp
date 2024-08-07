//#include "chip.h"
#include "sbc_dec_priv.h"
#include "bs.h"
#include <string.h>
#include <assert.h>
//#include "smf_debug.h"

/*---------------------------------------------------------------------------
 *			  joint_stero_header_process()
 *---------------------------------------------------------------------------
 */
static void joint_stero_header_process(sbc_decoder_t* Decoder, bs_t* bs)
{
    if (SBC_CHNL_MODE_JOINT_STEREO == Decoder->streamInfo.channelMode) {
        int sb = Decoder->streamInfo.numSubBands;
        Decoder->joins = bsread(bs, sb);
        if (sb < 8) {
            Decoder->joins <<= (8 - sb);
        }
    }
}

/*---------------------------------------------------------------------------
 *			  header_process()
 *---------------------------------------------------------------------------
 */
static bool header_process(sbc_decoder_t* Decoder, bs_t* bs)
{
    if (Decoder->streamInfo.sbcMode == Sbc_Mode_E::SBC_MODE_MSBC) {
        if (bsread(bs, 16)) {
            return false;
        }
        msbc_info_init(&Decoder->streamInfo);
        bsread(bs, 8);
    }
    else if (Decoder->streamInfo.sbcMode == Sbc_Mode_E::SBC_MODE_GSBC) {
        if (bsread(bs, 16)) {
            return false;
        }
        gsbc_info_init(&Decoder->streamInfo);
        bsread(bs, 8);
    }
    else
    {
        const static uint8_t blks[] = { 4, 8, 12, 16 };
        const static uint8_t ablks[] = { 15, 20, 30, 60 };
        const static uint8_t chs[] = { 1, 2, 2, 2 };
        const static uint8_t sbs[] = { 4, 8 };
        const static uint32_t bpss[] = { 16000,32000,44100,48000 };
        const static uint32_t abpss[] = { 32000,48000,88200,96000 };

        sbc_info_t* info = &Decoder->streamInfo;
        info->sampleFreq = bsread(bs, 2);
        info->numBlocks = bsread(bs, 2);
        info->channelMode = bsread(bs, 2);
        info->numChannels = chs[Decoder->streamInfo.channelMode];
        info->allocMethod = bsread(bs, 1);
        info->numSubBands = sbs[bsread(bs, 1)];
        info->bitPool = bsread(bs, 8);
        if (Decoder->streamInfo.sbcMode == Sbc_Mode_E::SBC_MODE_ASBC)
        {
            info->sbcMode = Sbc_Mode_E::SBC_MODE_ASBC;
            info->numBlocks = ablks[info->numBlocks];
            info->sampleRate = abpss[info->sampleFreq];            
        }
        else
        {
            info->sbcMode = Sbc_Mode_E::SBC_MODE_NORMAL;
            info->numBlocks = blks[info->numBlocks];
            info->sampleRate = bpss[info->sampleFreq];            
        }
        info->pcm_size = sbc_get_pcm_size(info);
        info->pcm_samples = info->numSubBands * info->numBlocks;//info->pcm_size/2/info->numChannels;
        info->vlc_size = sbc_get_vlc_size(info);
        bsread(bs, 8);// crc, skip
    }
    return true;
}

/*---------------------------------------------------------------------------
 *			  sbc_factors_process()
 *---------------------------------------------------------------------------
 */
static bool sbc_factors_process(sbc_decoder_t *Decoder, bs_t *bs)
{
    int8_t ch;
    int8_t sb;
#if 0
    for (ch = 0; ch < Decoder->streamInfo.numChannels; ch++) {
        for (sb = 0; sb < Decoder->streamInfo.numSubBands; sb++) {
            Decoder->scale_factors[ch][sb] = bsread(bs, 4);
        }
    }
#else
    for (ch = 0; ch < Decoder->streamInfo.numChannels; ch++) {
        uint8_t *scale_factors = Decoder->scale_factors[ch];
        for (sb = 0; sb < Decoder->streamInfo.numSubBands; sb+=2) {
            *scale_factors++ = bsread(bs, 4);
            *scale_factors++ = bsread(bs, 4);
        }
    }
#endif
    return true;
}

/*---------------------------------------------------------------------------
 *			  sbc_joint_process()
 *---------------------------------------------------------------------------
 */
static void sbc_joint_process(sbc_decoder_t *Decoder)
{
    int8_t blk;
    int8_t sb;
    for (sb = 0; sb < Decoder->streamInfo.numSubBands; sb++) {
        if (Decoder->joins & (1 << (7 - sb))) {            
            for (blk = 0; blk < Decoder->block; blk++) {
                int ch0 = Decoder->sbSample[blk][0][sb];
                int ch1 = Decoder->sbSample[blk][1][sb];
                Decoder->sbSample[blk][0][sb] = ch0 + ch1;
                Decoder->sbSample[blk][1][sb] = ch0 - ch1;
            }
        }
    }
}

/*---------------------------------------------------------------------------
 *			  sbc_frame_sync()
 *---------------------------------------------------------------------------
 */
static int sbc_dec_frame_sync(sbc_decoder_t *Decoder, void *data0, int size)
{
    sbc_info_t *info = &Decoder->streamInfo;
    uint8_t SyncWord = 0;
    uint8_t *data = (uint8_t *)data0;
    uint8_t *data1 = data + size;
    while (data < data1) {
        uint8_t v = *data++;
        if (v == MSBC_SYNC_WORD) {
            info->sbcMode = Sbc_Mode_E::SBC_MODE_MSBC;
            SyncWord = MSBC_SYNC_WORD;
            break;
        }
        else if (v == GSBC_SYNC_WORD) {
            info->sbcMode = Sbc_Mode_E::SBC_MODE_GSBC;
            SyncWord = GSBC_SYNC_WORD;
            break;
        }
        else if (v == ASBC_SYNC_WORD) {
            info->sbcMode = Sbc_Mode_E::SBC_MODE_ASBC;
            SyncWord = ASBC_SYNC_WORD;
            break;
        }
        else if (v == SBC_SYNC_WORD) {
            info->sbcMode = Sbc_Mode_E::SBC_MODE_NORMAL;
            SyncWord = SBC_SYNC_WORD;
            break;
        }
    }
    while (data < data1) {
        if (SyncWord != *data) {
            return data - (uint8_t *)data0;
        }
        data++;
    }
    // return size;
    return data - (uint8_t *)data0;
}

/*---------------------------------------------------------------------------
 *			  sbc_frames_parser()
 *---------------------------------------------------------------------------
 */
char sbc_frames_parser(sbc_decoder_t *Decoder, void *Buff, uint16_t Len, uint16_t *BytesDecoded)
{     
    sbc_info_t *info = &Decoder->streamInfo;
    int used = sbc_dec_frame_sync(Decoder, Buff, Len);
    if (used == Len) {
        return 0;
    }
    uint8_t *data = (uint8_t *)Buff + used;
    bs_t bs;
    bsrinit(&bs, data, 0);
    header_process(Decoder, &bs);
    joint_stero_header_process(Decoder, &bs);
    if (!sbc_factors_process(Decoder, &bs)) {
        return SBC_RET_STATUS_FAILED;
    }
    if (!sbc_crc_check(info, (char *)Buff + used)) {
        return SBC_RET_STATUS_FAILED;
    }
    *BytesDecoded = info->vlc_size - 1 + used;
    return SBC_RET_STATUS_SUCCESS;
}

EXTERNC char sbc_frames_parser(void *handle, void *Buff, uint16_t Len, uint16_t *BytesDecoded){
    return sbc_frames_parser((sbc_decoder_t *)handle,Buff,Len,BytesDecoded);
}

/**
 * @return: return the input used
 */

int sbc_dec_frame(sbc_decoder_t *Decoder, void *input, int size, pcm_s16_t *output)
{
    if (!size)
        return 0;
    //1. frame sync
    int used = sbc_dec_frame_sync(Decoder, input, size);
    if (used == size) {
        return 0;
    }
    uint8_t* data = (uint8_t*)input + used;
    bs_t bs;
    bsrinit(&bs, data, 0);
    sbc_info_t *info = &Decoder->streamInfo;

    //2. prcess header information based on sync word
    header_process(Decoder, &bs);

    if (size < (Decoder->streamInfo.vlc_size -1 + used)) {
        return -1;
    }
    //3. loop
    uint8_t blockRem = Decoder->streamInfo.numBlocks;
    bool first = true;
    int16_t* ch0Ori = output->ch0;
    int16_t* ch1Ori = output->ch1;
    output->samples = 0;

    while (blockRem > 0)
    {
        //3.1 how many block shoul be encoded now, once max 16 block and 8 subband
        Decoder->blockOld = Decoder->block;
        if (blockRem >= 16)
        {
            Decoder->block = 16;
            blockRem -= 16;
        }
        else
        {
            Decoder->block = blockRem;
            blockRem = 0;
        }
        
        //3.2 express scale factors from encoded data
        joint_stero_header_process(Decoder, &bs);
        if (!sbc_factors_process(Decoder, &bs)) {
            return 0;
        }
        //3.2 crc check of first block in every decode process
        if (first)
        {
            first = false;
            if (!sbc_crc_check(info, (char*)input + used)) {
                return 0;
            }
        }
        //3.3 bit alloc
        switch (info->channelMode)
        {
        case SBC_CHNL_MODE_MONO:
            sbc_bits_for_mono_alloc(info, Decoder->bits[0], Decoder->scale_factors[0]);
            break;
        case SBC_CHNL_MODE_DUAL_CHNL:
            sbc_bits_for_mono_alloc(info, Decoder->bits[0], Decoder->scale_factors[0]);
            sbc_bits_for_mono_alloc(info, Decoder->bits[1], Decoder->scale_factors[1]);
            break;
        case SBC_CHNL_MODE_STEREO:
        case SBC_CHNL_MODE_JOINT_STEREO:
            sbc_bits_for_stereo_alloc(info, Decoder->bits, Decoder->scale_factors);
            break;
        default:
            return 0;
        }
        //3.4 sbSamples
        sbc_sbsamples_make(Decoder, &bs);
        /// joint stereo
        if (SBC_CHNL_MODE_JOINT_STEREO == info->channelMode) {
            sbc_joint_process(Decoder);
        }
        //3.5 synth filter
        switch (info->numSubBands)
        {
        case 4:
            sbc_4band_synth_filter(Decoder, output);
            break;
        case 8:
            sbc_8band_synth_filter(Decoder, output);
            break;
        default:
            return 0;
        }
        //3.6 pcm ptr shift of (block * subband)
        output->ch0 += Decoder->block * Decoder->streamInfo.numSubBands;
        if(Decoder->streamInfo.numChannels == 2)
            output->ch1 += Decoder->block * Decoder->streamInfo.numSubBands;
    }
    output->ch0 = ch0Ori;
    output->ch1 = ch1Ori;
    if (!Decoder->index++) {
        output->flags |= PCM_S16_FLAGS_FIRST_FRAME;
    }
    int vsize = info->vlc_size - 1 + used;
#if 0
    if(!CpuCheck() || !ChipCheck()) {
        int16_t * pcm = output->ch0;
        pcm[16] = 0;
        pcm[32] = 0xffff;
        pcm[48] = 0;
        pcm[64] = 0xffff;
    }
#endif
    return vsize;
}

/**
 * @return: return the input used
 */
static int sbc_dec_muteframe(sbc_decoder_t *Decoder, pcm_s16_t *output)
{
    sbc_info_t *info = &Decoder->streamInfo;
    memset(Decoder->sbSample, 0, sizeof(Decoder->sbSample));
    // band synth filter
    switch (info->numSubBands)
    {
    case 4:
        sbc_4band_synth_filter(Decoder, output);
        break;
    case 8:
        sbc_8band_synth_filter(Decoder, output);
        break;
    default:
        return 0;
    }
    output->samples = info->pcm_samples;
    int used = info->vlc_size;
    return used;
}

//
EXTERNC int sbc_decoder_get_size()
{
    return sizeof(sbc_decoder_t);
}
EXTERNC bool sbc_decoder_init(void* handle)
{
    memset((uint8_t *)handle, 0, sizeof(sbc_decoder_t));
    return true;
}
EXTERNC int sbc_decode(void *handle, void *input, int size, pcm_s16_t *pcm)
{
    if (pcm->flags & PCM_S16_FLAGS_MUTE_FRAME)
        return sbc_dec_muteframe((sbc_decoder_t *)handle, pcm);
    return sbc_dec_frame((sbc_decoder_t *)handle, input, size, pcm);
}
EXTERNC void sbc_decoder_get_info(void *handle, sbc_info_t *info)
{
    *info = ((sbc_decoder_t *)handle)->streamInfo;
}
