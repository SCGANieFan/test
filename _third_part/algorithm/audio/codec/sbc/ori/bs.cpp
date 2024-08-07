#include <stdio.h>
#include "bs.h"

unsigned bsrinit(bs_t* bs, void* buff, int bytes) {
	bs->buffer = (uint8_t*)buff;
	bs->index = 0;
	bs->bitsInCache = 0;
	bs->cacheWord = 0;
	//bs->bitsUsed = 0;
	return 0;
}

unsigned bswinit(bs_t* bs, void* buff, int bytes) {
	bs->buffer = (uint8_t*)buff;
	bs->index = 0;
	bs->bitsInCache = 0;
	bs->cacheWord = 0;
	//bs->bitsUsed = 0;
	return 0;
}

/*static const uint32_t sbcBitMask[16 + 1] = {
    0x0,        0x1,        0x3,       0x7,       0xf,       0x1f,
    0x3f,       0x7f,       0xff,      0x1ff,     0x3ff,     0x7ff,
    0xfff,      0x1fff,     0x3fff,    0x7fff,    0xffff};*/

unsigned bsusedbits(bs_t*bs){
	uint32_t used = bs->index*8 + bs->bitsInCache;
	return used;
}

uint16_t bsread(bs_t*bs, uint8_t num){
	return bsread_inline(bs, num);
}

void bsflash(bs_t*bs){
	int byteInCache = (bs->bitsInCache + 7)/8;
	int shf = (byteInCache - 1) * 8;
	bs->cacheWord <<= (byteInCache * 8 - bs->bitsInCache);
	for (int i = 0; i < byteInCache; i++) {
		bs->buffer[bs->index++] = (uint8_t)(bs->cacheWord >> shf);
		shf-=8;
	}
	bs->cacheWord = 0;
	bs->bitsInCache = 0;
}

void bswrite(bs_t*bs,unsigned val, uint8_t bits){
	bswrite_inline(bs, val, bits);
}

