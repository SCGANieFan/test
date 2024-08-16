#include "SbcDec.h"
/*---------------------------------------------------------------------------
 *			  sbc_sbsamples_make()
 *---------------------------------------------------------------------------
 */

#define ONE_F       (0x00008000)
#define ONE_F_P     (ONE_F << 15)

void SbcDec_c::SbsamplesMake(BitStream_c *bs)
{
    int8_t blk;
    int8_t ch;
    int8_t sb;
    for (blk = 0; blk < _block; blk++) {
        for (ch = 0; ch < _streamInfo.numChannels; ch++) {
            for (sb = 0; sb < _streamInfo.numSubBands; sb++) {
                uint16_t bits0 = _bits[ch][sb];
                if (bits0) {
                    int32_t val1 = bs->Read(bits0);
                    uint32_t shf = _scaleFactors[ch][sb];
                    uint16_t lv = (uint16_t)(((uint16_t)1 << bits0) - 1);
                    int32_t rst1 = ((((((val1 << 1) + 1) << 14) / lv) << 1) - ONE_F) << shf;
                    _sbSample[blk][ch][sb] = rst1;
                } else {
                    _sbSample[blk][ch][sb] = 0;
                }
            }
        }
    }
}

