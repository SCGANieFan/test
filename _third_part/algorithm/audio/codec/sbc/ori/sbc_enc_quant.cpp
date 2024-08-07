#include "sbc_enc_priv.h"
#include "string.h"
#include <stdio.h>
#include <stdint.h>

void sbc_samples_quant_x(sbc_encoder_t* Encoder, bs_t* bs) {
	int8_t			   blk;
	int8_t			   ch;
	int8_t			   sb;
	sbc_info_t* info = &Encoder->streamInfo;

	for (blk = 0; blk < Encoder->block; blk++) {
		for (ch = 0; ch < info->numChannels; ch++) {
			int* vs = Encoder->sbSample[blk][ch];
			for (sb = 0; sb < info->numSubBands; sb++) {
				uint8_t bits = Encoder->bits[ch][sb];
				int v = *vs++;
				if (bits) {
					int fac = Encoder->scale_factors[ch][sb];
					int lvl = ((1u << bits) - 1) >> 1;
					int a = lvl << (16 - fac);
					int b = lvl;
					unsigned tmp = b + (((int64_t)v * a + 0x80000000ll) >> 32);
					bswrite_inline(bs, tmp, bits);					
				}
			}
		}
	}
}









