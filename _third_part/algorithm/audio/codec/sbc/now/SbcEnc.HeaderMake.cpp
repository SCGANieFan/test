#include "SbcEnc.h"

void SbcEncoder_c::HeaderMakeX(BitStream_c* bs)
{	
	uint8_t sync_word = SBC_SYNC_WORD;
	uint8_t numblocks = _streamInfo.numBlocks / 4 - 1;
	uint8_t t = (_streamInfo.sampleFreq << 6)
		| (numblocks << 4)
		| (_streamInfo.channelMode << 2)
		| (_streamInfo.allocMethod << 1)
		| (_streamInfo.numSubBands == 8 ? 1 : 0)
		;
	bs->Write(sync_word, 8);
	bs->Write(t, 8);
	bs->Write(_streamInfo.bitPool, 8);
	//crc
	bs->Write(0, 8);	
}

void SbcEncoder_c::HeaderScaleFactorsMakeX(BitStream_c* bs)
{
	int8_t	 ch;
	int8_t	 sb;
	if (SBC_CHNL_MODE_JOINT_STEREO == _streamInfo.channelMode) {
		uint8_t t = _joins >> 24;
		if (8 == _streamInfo.numSubBands) {
			bs->Write(t, 8);
		}
		else {
			bs->Write(t >> 4, 4);
		}
	}	
	for (ch = 0; ch < _streamInfo.numChannels; ch++) {
		for (sb = 0; sb < _streamInfo.numSubBands; sb++) {
			bs->Write(_scaleFactors[ch][sb], 4);
		}
	}
}

