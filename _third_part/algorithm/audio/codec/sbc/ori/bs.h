#pragma once
#include <stdint.h>

typedef struct {
	uint8_t *buffer;
	uint32_t index;
	uint32_t bitsInCache;
	uint32_t cacheWord;
	//uint32_t bitsUsed;
} bs_t;

extern unsigned bsusedbits(bs_t*bs);
extern unsigned bsrinit(bs_t* bs,void*buff, int bytes);
extern uint16_t bsread(bs_t*bs, uint8_t num);
static inline uint16_t bsread_inline(bs_t*bs, uint8_t num){
	int tmpnum = num - bs->bitsInCache;
	uint32_t bits = 0;
	if(tmpnum > 0){
		bits = bs->cacheWord << tmpnum;
		bs->cacheWord = (bs->buffer[bs->index]<<24) |
		(bs->buffer[bs->index+1]<<16) |
		(bs->buffer[bs->index+2]<<8) |
		(bs->buffer[bs->index+3]);
		bs->index+=4;
		bs->bitsInCache += 32;
	}
	bs->bitsInCache-=num;
	return (bits | (bs->cacheWord >> bs->bitsInCache)) & ((1u<<num)-1);//sbcBitMask[num];
}

extern unsigned bswinit(bs_t* bs,void*buff, int bytes);
extern void bswrite(bs_t*bs,unsigned val,uint8_t bits);
static inline void bswrite_inline(bs_t*bs,unsigned val,uint8_t bits)
{
	uint32_t validMask = ((1u<<bits)-1);//sbcBitMask[bits];
	if(bs->bitsInCache + bits < 32){
		bs->cacheWord = (bs->cacheWord << bits)| (val & validMask);
		bs->bitsInCache+=bits;
	} else {
		int tmp = 32 - bs->bitsInCache;
		int remaintmp = bits - tmp;
		val = val & validMask;
		uint32_t cacheWord = (bs->cacheWord << tmp) | (val >> remaintmp);
		bs->buffer[bs->index++] = (uint8_t)(cacheWord>>24);
		bs->buffer[bs->index++] = (uint8_t)(cacheWord>>16);
		bs->buffer[bs->index++] = (uint8_t)(cacheWord>>8);
		bs->buffer[bs->index++] = (uint8_t)(cacheWord>>0);
		bs->cacheWord = val&((1<< remaintmp)-1);
		bs->bitsInCache = remaintmp;
	}
}

extern void bsflash(bs_t*bs);
