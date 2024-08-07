#include "sbc_enc_priv.h"
#include "string.h"
#include <stdio.h>
#include <stdint.h>

void sbc_header_make_x(sbc_encoder_t* Encoder, bs_t* bs)
{	
	if (Encoder->streamInfo.sbcMode == Sbc_Mode_E::SBC_MODE_MSBC)
	{
		bswrite(bs, MSBC_SYNC_WORD, 8);
		bswrite(bs, 0, 8);
		bswrite(bs, 0, 8);
	}
	else  if (Encoder->streamInfo.sbcMode == Sbc_Mode_E::SBC_MODE_GSBC)
	{
		bswrite(bs, GSBC_SYNC_WORD, 8);
		bswrite(bs, 0, 8);
		bswrite(bs, 0, 8);
	}
	else
	{
		uint8_t sync_word = SBC_SYNC_WORD;
		uint8_t numblocks = Encoder->streamInfo.numBlocks / 4 - 1;
		if (Encoder->streamInfo.sbcMode == Sbc_Mode_E::SBC_MODE_ASBC)
		{
			sync_word = ASBC_SYNC_WORD;
			switch (Encoder->streamInfo.numBlocks) {
			case ASBC_BLOCKS_15:
				numblocks = 0;
				break;
			case ASBC_BLOCKS_20:
				numblocks = 1;
				break;
			case ASBC_BLOCKS_30:
				numblocks = 2;
				break;
			case ASBC_BLOCKS_60:
				numblocks = 3;
				break;
			default:
				break;
			}
		}
		uint8_t t = (Encoder->streamInfo.sampleFreq << 6)
			| (numblocks << 4)
			| (Encoder->streamInfo.channelMode << 2)
			| (Encoder->streamInfo.allocMethod << 1)
			| (Encoder->streamInfo.numSubBands == 8 ? 1 : 0)
			;
		bswrite(bs, sync_word, 8);
		bswrite(bs, t, 8);
		bswrite(bs, Encoder->streamInfo.bitPool, 8);
	}
	//crc
	bswrite(bs, 0, 8);	
}

void sbc_header_scale_factors_make_x(sbc_encoder_t* Encoder, bs_t* bs)
{
	int8_t	 ch;
	int8_t	 sb;
	if (SBC_CHNL_MODE_JOINT_STEREO == Encoder->streamInfo.channelMode) {
		uint8_t t = Encoder->joins >> 24;
		if (8 == Encoder->streamInfo.numSubBands) {
			bswrite(bs, t, 8);
		}
		else {
			bswrite(bs, t >> 4, 4);
		}
	}	
	for (ch = 0; ch < Encoder->streamInfo.numChannels; ch++) {
		for (sb = 0; sb < Encoder->streamInfo.numSubBands; sb++) {
			bswrite(bs, Encoder->scale_factors[ch][sb], 4);
		}
	}
}

