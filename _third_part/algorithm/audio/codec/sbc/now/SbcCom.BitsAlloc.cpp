#include"SbcCom.h"
const char loudness_offs_4band[2 * 4][4] = {
	{(char)-1,  0,  0,  0},
	{(char)-2,  0,  0,  1},
	{(char)-2,  0,  0,  1},
	{(char)-2,  0,  0,  1},
#if 0
	/*asbc*/
	{(char)-2,  (char)-1,  3,  5},	//32 khz
	{(char)-2,  2,  4,  6},		//48 khz
	{(char)-2,  6,  8,  10},	//88.2 khz
	{(char)-2,  6,  8,  10}		//96 khz
#endif
};

const char loudness_offs_8band[2 * 4][8] = {
	{(char)-2,  0,  0,  0,  0,  0,  0,  1},
	{(char)-3,  0,  0,  0,  0,  0,  1,  2},
	{(char)-4,  0,  0,  0,  0,  0,  1,  2},
	{(char)-4,  0,  0,  0,  0,  0,  1,  2},
#if 0
	/*asbc*/
	{(char)0,  (char)-2,  (char)-1,  2,  4,  3,  4,  6},	//32 khz
	{(char)-2,  (char)-1,  3,  5,  7,  9,  9,  11},	//48 khz
	{(char)-2,  2,  4,  6,  8,  10,  12,  14},	//88.2 khz
	{(char)-2,  2,  4,  6,  8,  10,  12,  14}	//96 khzs
#endif
};

void SbcCom_c::BitAllocMono(SbcInfo_t* _streamInfo, uint8_t* bits, uint8_t* scale_factors) {
	int8_t	sb;
	int8_t	bitSlice;
	int8_t	loudness;
	int8_t	maxBitNeed = 0;
	uint8_t	bitCount = 0;
	uint8_t	sliceCount = 0;
	uint8_t sampleFreq = _streamInfo->sampleFreq;
	int8_t	bitNeed0[SBC_MAX_NUM_SB];

	if (SBC_ALLOC_METHOD_SNR == _streamInfo->allocMethod) {
		for (sb = 0; sb < _streamInfo->numSubBands; sb++) {
			bitNeed0[sb] = scale_factors[sb];
			if (bitNeed0[sb] > maxBitNeed) {
				maxBitNeed = bitNeed0[sb];
			}
		}
	}
	else {
		for (sb = 0; sb < _streamInfo->numSubBands; sb++) {
			if (0 == scale_factors[sb]) {
				bitNeed0[sb] = (int8_t)-5;
			}
			else {
				if (4 == _streamInfo->numSubBands) {
					loudness
						= (int8_t)(scale_factors[sb]
							- loudness_offs_4band[sampleFreq][sb]);
				}
				else {
					loudness
						= (int8_t)(scale_factors[sb]
							- loudness_offs_8band[sampleFreq][sb]);
				}
				if (0 < loudness) {
					bitNeed0[sb] = (int8_t)(loudness >> 1);
				}
				else {
					bitNeed0[sb] = loudness;
				}
			}
			if (bitNeed0[sb] > maxBitNeed) {
				maxBitNeed = bitNeed0[sb];
			}
		}
	}
	bitSlice = (int8_t)(maxBitNeed + 1);
	do {
		bitSlice--;
		bitCount = (uint8_t)(bitCount + sliceCount);
		sliceCount = 0;
		for (sb = 0; sb < _streamInfo->numSubBands; sb++) {
			if ((bitNeed0[sb] > (bitSlice + 1)) &&
				(bitNeed0[sb] < (bitSlice + 16))) {
				sliceCount++;
			}
			else if (bitNeed0[sb] == (bitSlice + 1)) {
				sliceCount += 2;
			}
		}
	} while ((bitCount + sliceCount) < _streamInfo->bitPool);

	if ((bitCount + sliceCount) == _streamInfo->bitPool) {
		bitCount = (uint8_t)(bitCount + sliceCount);
		bitSlice--;
	}
	for (sb = 0; sb < _streamInfo->numSubBands; sb++) {
		if (bitNeed0[sb] < (bitSlice + 2)) {
			bits[sb] = 0;
		}
		else {
			bits[sb] = (uint8_t)(ALGO_MIN(bitNeed0[sb] - bitSlice, 16));
		}
	}

	sb = 0;
	while ((bitCount < _streamInfo->bitPool) &&
		(sb < _streamInfo->numSubBands)) {
		if ((bits[sb] >= 2) && (bits[sb] < 16)) {
			bits[sb]++;
			bitCount++;
		}
		else {
			if (bitNeed0[sb] == (bitSlice + 1)) {
				if (_streamInfo->bitPool > (bitCount + 1)) {
					bits[sb] = 2;
					bitCount += 2;
				}
			}
		}
		sb++;
	}

	sb = 0;
	while ((bitCount < _streamInfo->bitPool) &&
		(sb < _streamInfo->numSubBands)) {
		if (16 > bits[sb]) {
			bits[sb]++;
			bitCount++;
		}
		sb++;
	}
}
void SbcCom_c::BitAllocStereo(SbcInfo_t* _streamInfo, uint8_t(*bits)[8], uint8_t(*scale_factors)[8]) {
	int8_t	 sb;
	int8_t bitSlice;
	int8_t loudness;
	int8_t maxBitNeed = 0;
	uint8_t	 bitCount = 0;
	uint8_t	 sliceCount = 0;
	int8_t	 bitNeed0[SBC_MAX_NUM_SB];
	int8_t	 bitNeed1[SBC_MAX_NUM_SB];
	uint8_t sampleFreq = _streamInfo->sampleFreq;
	if (SBC_ALLOC_METHOD_SNR == _streamInfo->allocMethod) {
		for (sb = 0; sb < _streamInfo->numSubBands; sb++) {
			bitNeed0[sb] = scale_factors[0][sb];

			if ((int8_t)bitNeed0[sb] > maxBitNeed) {
				maxBitNeed = (int8_t)bitNeed0[sb];
			}
		}

		for (sb = 0; sb < _streamInfo->numSubBands; sb++) {
			bitNeed1[sb] = scale_factors[1][sb];

			if ((int8_t)bitNeed1[sb] > maxBitNeed) {
				maxBitNeed = (int8_t)bitNeed1[sb];
			}
		}
	}
	else {
		for (sb = 0; sb < _streamInfo->numSubBands; sb++) {
			if (0 == scale_factors[0][sb]) {
				bitNeed0[sb] = (int8_t)-5;
			}
			else {
				if (4 == _streamInfo->numSubBands) {
					loudness =
						(int8_t)(scale_factors[0][sb] -
							loudness_offs_4band[sampleFreq][sb]);
				}
				else {
					loudness =
						(int8_t)(scale_factors[0][sb] -
							loudness_offs_8band[sampleFreq][sb]);
				}
				if (0 < loudness) {

					bitNeed0[sb] = (int8_t)(loudness >> 1);

				}
				else {
					bitNeed0[sb] = loudness;
				}
			}
			if ((int8_t)bitNeed0[sb] > maxBitNeed) {
				maxBitNeed = (int8_t)bitNeed0[sb];
			}

		}

		for (sb = 0; sb < _streamInfo->numSubBands; sb++) {
			if (0 == scale_factors[1][sb]) {
				bitNeed1[sb] = (int8_t)-5;
			}
			else {
				if (4 == _streamInfo->numSubBands) {
					loudness =
						(int8_t)(scale_factors[1][sb] -
							loudness_offs_4band[sampleFreq][sb]);
				}
				else {
					loudness =
						(int8_t)(scale_factors[1][sb] -
							loudness_offs_8band[sampleFreq][sb]);
				}
				if (0 < loudness) {

					bitNeed1[sb] = (int8_t)(loudness >> 1);

				}
				else {
					bitNeed1[sb] = loudness;
				}
			}
			if ((int8_t)bitNeed1[sb] > maxBitNeed) {
				maxBitNeed = (int8_t)bitNeed1[sb];
			}

		}
	}

	bitSlice = (int8_t)(maxBitNeed + 1);
	do {
		bitSlice--;
		bitCount = (uint8_t)(bitCount + sliceCount);
		sliceCount = 0;
		for (sb = 0; sb < _streamInfo->numSubBands; sb++) {
			if (((int8_t)bitNeed0[sb] > (bitSlice + 1)) &&
				((int8_t)bitNeed0[sb] < (bitSlice + 16))) {
				sliceCount++;
			}
			else if ((int8_t)bitNeed0[sb] == bitSlice + 1) {
				sliceCount += 2;
			}
		}

		for (sb = 0; sb < _streamInfo->numSubBands; sb++) {
			if (((int8_t)bitNeed1[sb] > (bitSlice + 1)) &&
				((int8_t)bitNeed1[sb] < (bitSlice + 16))) {
				sliceCount++;
			}
			else if ((int8_t)bitNeed1[sb] == bitSlice + 1) {
				sliceCount += 2;
			}
		}
	} while ((bitCount + sliceCount) < _streamInfo->bitPool);

	if ((bitCount + sliceCount) == _streamInfo->bitPool) {
		bitCount = (uint8_t)(bitCount + sliceCount);
		bitSlice--;
	}

	for (sb = 0; sb < _streamInfo->numSubBands; sb++) {
		if ((int8_t)bitNeed0[sb] < ((int8_t)bitSlice + 2)) {
			bits[0][sb] = 0;
		}
		else {
			bits[0][sb] =
				(uint8_t)ALGO_MIN((int8_t)bitNeed0[sb] - (int8_t)bitSlice,
					16);
		}
	}

	for (sb = 0; sb < _streamInfo->numSubBands; sb++) {
		if ((int8_t)bitNeed1[sb] < ((int8_t)bitSlice + 2)) {
			bits[1][sb] = 0;
		}
		else {
			bits[1][sb] =
				(uint8_t)ALGO_MIN((int8_t)bitNeed1[sb] - (int8_t)bitSlice,
					16);
		}
	}

	sb = 0;
	while ((bitCount < _streamInfo->bitPool) &&
		(sb < _streamInfo->numSubBands)) {

		if ((bits[0][sb] >= 2) &&
			(bits[0][sb] < 16)) {
			bits[0][sb]++;
			bitCount++;
		}
		else {
			if ((int8_t)bitNeed0[sb] == ((int8_t)bitSlice + 1)) {
				if (_streamInfo->bitPool > (bitCount + 1)) {
					bits[0][sb] = 2;
					bitCount += 2;
				}
			}
		}
		if (!((bitCount < _streamInfo->bitPool) &&
			(sb < _streamInfo->numSubBands))) {
			break;
		}

		if ((2 <= bits[1][sb]) &&
			(16 > bits[1][sb])) {
			bits[1][sb]++;
			bitCount++;
		}
		else {
			if ((int8_t)bitNeed1[sb] == ((int8_t)bitSlice + 1)) {
				if (_streamInfo->bitPool > (bitCount + 1)) {
					bits[1][sb] = 2;
					bitCount += 2;
				}
			}
		}
		sb++;
	}
	sb = 0;
	while ((bitCount < _streamInfo->bitPool) &&
		(sb < _streamInfo->numSubBands)) {

		if (16 > bits[0][sb]) {
			bits[0][sb]++;
			bitCount++;
		}

		if (!((bitCount < _streamInfo->bitPool) &&
			(sb < _streamInfo->numSubBands))) {
			break;
		}

		if (16 > bits[1][sb]) {
			bits[1][sb]++;
			bitCount++;
		}

		sb++;
	}
}

