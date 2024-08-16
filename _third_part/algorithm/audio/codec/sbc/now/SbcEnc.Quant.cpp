#include "SbcEnc.h"

void SbcEncoder_c::SamplesQuantX(BitStream_c* bs) {
	int8_t			   blk;
	int8_t			   ch;
	int8_t			   sb;
	SbcInfo_t* info = &_streamInfo;

	for (blk = 0; blk < _block; blk++) {
		for (ch = 0; ch < info->numChannels; ch++) {
			int* vs = _sbSample[blk][ch];
			for (sb = 0; sb < info->numSubBands; sb++) {
				uint8_t bits0 = _bits[ch][sb];
				int v = *vs++;
				if (bits0) {
					int fac = _scaleFactors[ch][sb];
					int lvl = ((1u << bits0) - 1) >> 1;
					int a = lvl << (16 - fac);
					int b = lvl;
					unsigned tmp = b + (((int64_t)v * a + 0x80000000ll) >> 32);
					bs->Write(tmp, bits0);
				}
			}
		}
	}
}









