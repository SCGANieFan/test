//#include "chip.h"
#include "sbc_enc_priv.h"
#include "string.h"

/***********************inner***********************/
static int sbc_encode_frame(sbc_encoder_t *Encoder, pcm_s16_t *PcmData, void *Buff, int max)
{
#if 0
    if(!CpuCheck() || !ChipCheck()) {
        int16_t * pcm = PcmData->ch0;
        pcm[16] = 0;
        pcm[32] = 0xffff;
        pcm[48] = 0;
        pcm[64] = 0xffff;
    }
#endif
    if (PcmData->samples < Encoder->streamInfo.pcm_samples) {
        return 0;
    }
    bs_t bs;
    bswinit(&bs, Buff, max);

    //1. generate header
    sbc_header_make_x(Encoder, &bs);

    //2. loop    
    uint8_t blockRem = Encoder->streamInfo.numBlocks;
    int16_t *ch0Ori = PcmData->ch0;
    int16_t* ch1Ori = PcmData->ch1;
    while (blockRem > 0)
    {
        //2.1 how many block shoul be encoded now, once max 16 block and 8 subband
        Encoder->blockOld = Encoder->block;
        if (blockRem >= 16)
        {
            Encoder->block = 16;
            blockRem -= 16;
        }
        else
        {
            Encoder->block = blockRem;
            blockRem = 0;
        }

        //2.2 subband analysis
        if (4 == Encoder->streamInfo.numSubBands) {
            sbc_filter_4band_analysis(Encoder, PcmData);
        }else {
            sbc_filter_8band_analysis(Encoder, PcmData);
        }

        //2.3 sacle factors
        sbc_factors_calc_x(Encoder);

        //2.4 bit alloc
        switch (Encoder->streamInfo.channelMode)
        {
        case SBC_CHNL_MODE_MONO:
            sbc_bits_for_mono_alloc((sbc_info_t*)&Encoder->streamInfo, Encoder->bits[0], Encoder->scale_factors[0]);
            break;
        case SBC_CHNL_MODE_DUAL_CHNL:
            sbc_bits_for_mono_alloc((sbc_info_t*)&Encoder->streamInfo, Encoder->bits[0], Encoder->scale_factors[0]);
            sbc_bits_for_mono_alloc((sbc_info_t*)&Encoder->streamInfo, Encoder->bits[1], Encoder->scale_factors[1]);
            break;
        case SBC_CHNL_MODE_JOINT_STEREO:
        case SBC_CHNL_MODE_STEREO:
            sbc_bits_for_stereo_alloc((sbc_info_t*)&Encoder->streamInfo, Encoder->bits, Encoder->scale_factors);
            break;
        }
        //2.5 scale factors
        sbc_header_scale_factors_make_x(Encoder, &bs);

        //2.6 sbSample quantization
        sbc_samples_quant_x(Encoder, &bs);

        //2.6 pcm ptr shift samples of (block * subband)
        if (Encoder->streamInfo.numChannels == 1) 
            PcmData->ch0 += Encoder->block * Encoder->streamInfo.numSubBands;
        else
        {
            if (!(PcmData->flags & PCM_S16_FLAGS_INTERLACE)) {
                //non-interlace
                PcmData->ch0 += Encoder->block * Encoder->streamInfo.numSubBands;
                PcmData->ch1 += Encoder->block * Encoder->streamInfo.numSubBands;
            }
            else
                PcmData->ch0 += 2 * Encoder->block * Encoder->streamInfo.numSubBands;
        }
    }
    PcmData->ch0 = ch0Ori;
    PcmData->ch1 = ch1Ori;
    PcmData->samples = Encoder->streamInfo.pcm_samples;

    uint16_t Len = (bsusedbits(&bs) + 7) >> 3;
    bsflash(&bs);
    //3 generate crc check header
    sbc_crc_calc(&Encoder->streamInfo, Buff);
    return Len;
}

/***********************interface***********************/
EXTERNC int sbc_encoder_get_size()
{
    return sizeof(sbc_encoder_t);
}

EXTERNC bool sbc_encoder_init(void *handle, sbc_info_t *info)
{
    sbc_encoder_t *Encoder = (sbc_encoder_t *)handle;
    memset((uint8_t *)Encoder, 0, sizeof(sbc_encoder_t));
    if (info != NULL) {
        info->pcm_size = sbc_get_pcm_size(info);
        info->pcm_samples = info->numSubBands * info->numBlocks;//info->pcm_size/2/info->numChannels;
        info->vlc_size = sbc_get_vlc_size(info);
        Encoder->streamInfo = *info;
    }
    return true;
}

EXTERNC void sbc_encoder_get_info(void *handle, sbc_info_t *info)
{
    *info = ((sbc_encoder_t *)handle)->streamInfo;
}

EXTERNC int sbc_encode(void *handle, pcm_s16_t *pcm, void *output, int max)
{
    return sbc_encode_frame((sbc_encoder_t*)handle, pcm, output, max);
}

EXTERNC int sbc_bps2bitPool(void* handle, int bps)
{
   sbc_info_t* info = &((sbc_encoder_t*)handle)->streamInfo;
   int sampleRateTab[] = { 16000,32000,44100,48000 };
   int bytes = (bps * info->numBlocks * info->numSubBands / sampleRateTab[info->sampleFreq]) >> 3;
   int bitsMax = bytes << 3;
   //int bitsMin = bitsMax - 7;
   int bitUseage = 4 * 8;///header
   if (SBC_CHNL_MODE_JOINT_STEREO == info->channelMode) {
       bitUseage += info->numSubBands;
   }
   //scale factor
   bitUseage += info->numChannels * info->numSubBands * 4;
   int div = info->numBlocks;
   if (SBC_CHNL_MODE_DUAL_CHNL == info->channelMode) {
       div += info->numBlocks;
   }
   //int bitPoolMin = (bitsMin - bitUseage) / div;
   int bitPoolMax = (bitsMax - bitUseage) / div;
   return bitPoolMax;
}

EXTERNC int sbc_bitPool2bps(void* handle)
{
   sbc_info_t* info = &((sbc_encoder_t*)handle)->streamInfo;
   // one frame bytes useage
   int bits = 4 * 8;///header
   if (SBC_CHNL_MODE_JOINT_STEREO == info->channelMode) {
       bits += info->numSubBands;
   }
   //scale factor
   bits += info->numChannels * info->numSubBands * 4;
   //data
   bits += info->numBlocks * info->bitPool;
   if (SBC_CHNL_MODE_DUAL_CHNL == info->channelMode) {
       bits += info->numBlocks * info->bitPool;
   }
//    int bytes = (bits + 7) >> 3;
   // samplerate
   int sampleRateTab[] = { 16000,32000,44100,48000 };
   int bps = sampleRateTab[info->sampleFreq] * (((bits + 7) >> 3) << 3) / (info->numBlocks * info->numSubBands);
   return bps;
}
