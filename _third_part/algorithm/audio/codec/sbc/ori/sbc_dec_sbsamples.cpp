#include "sbc_dec_priv.h"
/*---------------------------------------------------------------------------
 *			  sbc_sbsamples_make()
 *---------------------------------------------------------------------------
 */
void sbc_sbsamples_make(sbc_decoder_t *Decoder, bs_t *bs /*, uint8_t* Buffer*/)
{
    int8_t blk;
    int8_t ch;
    int8_t sb;
    bs_t bss = *bs;
    for (blk = 0; blk < Decoder->block; blk++) {
        for (ch = 0; ch < Decoder->streamInfo.numChannels; ch++) {
            for (sb = 0; sb < Decoder->streamInfo.numSubBands; sb++) {
                uint16_t bits = Decoder->bits[ch][sb];
                if (bits) {                    
                    int32_t val1 = bsread_inline(&bss, bits);
                    uint32_t shf = Decoder->scale_factors[ch][sb];
                    uint16_t lv = (uint16_t)(((uint16_t)1 << bits) - 1);
                    int32_t rst1 = ((((((val1 << 1) + 1) << 14) / lv) << 1) - ONE_F) << shf;
                    Decoder->sbSample[blk][ch][sb] = rst1;
                } else {
                    Decoder->sbSample[blk][ch][sb] = 0;
                }
            }
        }
    }
    *bs = bss;
}

