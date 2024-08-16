#if 1
//#include<string.h>
//#include<stdint.h>
#include<stdio.h>
//#include<stdarg.h>
#include<iostream>
#include<string>
//#include<vector>
#include<assert.h>

#define MAX_UVLC_LEADING_ZEROS 20
#define UVLC_ERROR -99999
typedef struct DataCache_t {
	union {
		uint8_t cache8[8];
		uint64_t cache;//left aligned
	};
	int validBits;
};


typedef struct bitreader {
	uint8_t* data;
	int dataBytes;
	DataCache_t dataCache[2];
	DataCache_t *pDataCache;
	DataCache_t *pDataCacheNext;
};

static inline void BitReaderUpdateCurrentDataCache(bitreader* br) {
#if 1
	if (br->dataBytes >= 8) {
		for (int i = 0; i < 8; i++) {
			br->pDataCache->cache8[7 - i] = br->data[i];
		}
		br->pDataCache->validBits = 64;
		br->data += 8;
		br->dataBytes -= 8;
	}
	else {
		for (int i = 0; i < br->dataBytes; i++) {
			br->pDataCache->cache8[br->dataBytes - 1 - i] = br->data[i];
		}
		br->pDataCache->validBits = br->dataBytes << 3;
		br->data += br->dataBytes;
		br->dataBytes = 0;
	}
#endif
}
static inline void BitReaderChangePtrForDataCache(bitreader* br) {
	DataCache_t* tmp = br->pDataCache;
	br->pDataCache = br->pDataCacheNext;
	br->pDataCacheNext = tmp;
}

static inline void bitreader_init(bitreader* br, unsigned char* buffer, int len) {
	br->data = buffer;
	br->dataBytes = len;
	br->pDataCache = &br->dataCache[0];
	br->pDataCacheNext = &br->dataCache[1];
	BitReaderUpdateCurrentDataCache(br);
	BitReaderChangePtrForDataCache(br);
	BitReaderUpdateCurrentDataCache(br);
	BitReaderChangePtrForDataCache(br);
}

static inline int get_bits(bitreader* br, int n) {
	int remBits = br->pDataCache->validBits;
	int shift = 64 - n;
	int val = br->pDataCache->cache >> shift;
	if (remBits > n) {
		br->pDataCache->cache <<= n;
		br->pDataCache->validBits-= n;
	}
	else {
		BitReaderUpdateCurrentDataCache(br);
		BitReaderChangePtrForDataCache(br);
#if 0
		if (br->pDataCache->validBits < (n - remBits)) {
			return -1;
		}
#endif
		const int diffBits = n - remBits;
		shift = 64 - diffBits;
		val |= br->pDataCache->cache >> shift;
		br->pDataCache->cache <<= diffBits;
		br->pDataCache->validBits -= diffBits;
	}
	return val;
}

static inline int get_bits_no_updata(bitreader* br, int n) {
	int remBits = br->pDataCache->validBits;
	int shift = 64 - n;
	int val = br->pDataCache->cache >> shift;
	if (remBits < n) {		
		const int diffBits = n - remBits;
		shift = 64 - diffBits;
		val |= br->pDataCacheNext->cache >> shift;
	}
}

static inline void skip_bits(bitreader* br, int n) {
#if 0
	br->nextbits <<= n;
	br->nextbits_cnt -= n;
#else
	int remBits = br->pDataCache->validBits;
	if (remBits > n) {
		br->pDataCache->cache <<= n;
		br->pDataCache->validBits -= n;
	}
	else{
		BitReaderUpdateCurrentDataCache(br);
		BitReaderChangePtrForDataCache(br);
		int diffBits = (n - remBits);
		br->pDataCache->cache <<= diffBits;
		br->pDataCache->validBits -= diffBits;
	}
#endif
}

static inline void prepare_for_CABAC(bitreader* br) {
#if 0
	int nskip = (br->nextbits_cnt & 7);
	br->nextbits <<= nskip;
	br->nextbits_cnt -= nskip;

	int rewind = br->nextbits_cnt / 8;
	br->data -= rewind;
	br->bytes_remaining += rewind;
	br->nextbits = 0;
	br->nextbits_cnt = 0;
#else
	int nskip = (br->pDataCache->validBits & 7);
	br->pDataCache ->cache<<= nskip;
	br->pDataCache ->validBits-= nskip;
	int rewind = br->pDataCache->validBits >> 3;
	br->data -= rewind;
	br->dataBytes += rewind;

	br->pDataCache = &br->dataCache[0];
	br->pDataCacheNext = &br->dataCache[1];
	BitReaderUpdateCurrentDataCache(br);
	BitReaderChangePtrForDataCache(br);
	BitReaderUpdateCurrentDataCache(br);
	BitReaderChangePtrForDataCache(br);
#endif
}

static inline int LeadZero(uint32_t data) {
	int num_zeros = 0;
	while (1) {
		if (data & 0x80000000) { break; }
		num_zeros++;
		data <<= 1;
	}
	return num_zeros;
}

static inline int  get_uvlc(bitreader* br) {
#if 0
	// may return UVLC_ERROR
	int num_zeros = 0;

	while (get_bits(br, 1) == 0) {
		num_zeros++;

		if (num_zeros > MAX_UVLC_LEADING_ZEROS) { return UVLC_ERROR; }
	}

	int offset = 0;
	if (num_zeros != 0) {
		offset = get_bits(br, num_zeros);
		int value = offset + (1 << num_zeros) - 1;
		assert(value > 0);
		return value;
	}
	else {
		return 0;
	}
#else
#if 0
	int data = get_bits_no_updata(br, MAX_UVLC_LEADING_ZEROS + 1);
	int num_zeros = LeadZero(((uint32_t)data)<< (MAX_UVLC_LEADING_ZEROS + 1));
	int offset = 0;
	if(num_zeros> MAX_UVLC_LEADING_ZEROS) { return UVLC_ERROR; }
	if (num_zeros != 0) {
		skip_bits(br, num_zeros + 1);
		offset = get_bits(br, num_zeros);
		int value = offset + (1 << num_zeros) - 1;
		assert(value > 0);
		return value;
	}
	else {
		return 0;
	}
#endif
	int num_zeros = 0;
	while (get_bits(br, 1) == 0) {
		num_zeros++;
		if (num_zeros > MAX_UVLC_LEADING_ZEROS) { return UVLC_ERROR; }
	}

	int offset = 0;
	if (num_zeros != 0) {
		offset = get_bits(br, num_zeros);
		int value = offset + (1 << num_zeros) - 1;
		assert(value > 0);
		return value;
	}
	else {
		return 0;
	}
#endif
}

static inline int  get_svlc(bitreader* br) {
#if 0
	// may return UVLC_ERROR
	int v = get_uvlc(br);
	if (v == 0) return v;
	if (v == UVLC_ERROR) return UVLC_ERROR;

	bool negative = ((v & 1) == 0);
	return negative ? -v / 2 : (v + 1) / 2;
#else
	int v = get_uvlc(br);
	if (v == 0) return v;
	if (v == UVLC_ERROR) return UVLC_ERROR;
	bool negative = ((v & 1) == 0);
	return negative ? (-v)>>1 : (v + 1)>>1;
#endif
}

static inline bool check_rbsp_trailing_bits(bitreader* br) {
#if 0
	// return true if remaining filler bits are all zero
	int stop_bit = get_bits(br, 1);
	assert(stop_bit == 1);

	while (br->nextbits_cnt > 0 || br->bytes_remaining > 0) {
		int filler = get_bits(br, 1);
		if (filler != 0) {
			return false;
		}
	}

	return true;
#else

	int stop_bit = get_bits(br, 1);
	assert(stop_bit == 1);
	while (br->pDataCache->validBits > 0) {
		int bits = get_bits(br, br->pDataCache->validBits);
		if (bits) {
			return false;
		}
	}
	return true;
#endif
}


bool testDemo()
{
	const int N = 100;
	uint8_t bits[N];
	//for (auto b : bits) {
	for (int n = 0; n < N; n++) {
		bits[n] = n;
	}
	bitreader reader;
	bitreader_init(&reader, bits, N);

	static int line = 0;
	int readBit = 8;

	for (int k = 0; k < 10; k++) {
		printf("[%d] \t", line++);
		for (int c = 0; c < 8; c++) {
			int data = get_bits(&reader, readBit);
			printf("%x\t", data);
		}
		printf("\n", line++);
	}


	return 0;
}

#endif