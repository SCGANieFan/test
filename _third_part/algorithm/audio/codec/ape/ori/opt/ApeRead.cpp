#include"ApePrive.h"
#include<string.h>
#include<stdlib.h>
typedef struct
{
	union {
		uint32_t data32;
		uint8_t data8[4];
	};
	int8_t offset;
} BufferCacheDW_t;



typedef struct {
	uint8_t* buffer;
	uint32_t sizeUsedByte;
	uint32_t sizeUsedByteAlign1;
	uint32_t sizeRemByte;
	BufferCacheDW_t dw;
}BufferRead_t;

#if 0
static inline uint8_t GetBufferCacheRemByte(BufferRead_t* read)
{
	return read->dw.offset + 1;
}
#endif

static inline void BufferCacheInit(BufferCacheDW_t* dw)
{
	dw->data32 = 0;
	dw->offset = -1;
}
static inline bool BufferCacheReadByte(BufferCacheDW_t* dw, uint8_t* readedByte)
{
	if (dw->offset < 0)
		return false;
	*readedByte = dw->data8[dw->offset];
	dw->offset--;
	return true;
}
static inline bool BufferCacheUpdata(BufferCacheDW_t* dw, uint8_t *in,uint32_t inSize)
{
#if 0
	if (inSize > 4)
	{
		dw->data32 = *((uint32_t*)in);
		dw->offset = 3;
	}
	else
	{
		uint16_t shift = 0;
		dw->data32 = 0;
		for (uint8_t i = 0; i < inSize; i++)
		{
			dw->data32 += *(in++) << shift;
			shift += 8;
		}
				dw->offset = inSize;
	}
#else
	memcpy(dw->data8,in,4);
	dw->offset = 3;
#endif
	return true;
}


//BufferRead_t
uint32_t GetBufferReadSize()
{
	return sizeof(BufferRead_t);
}

uint32_t GetBufferUsed(void* bufferRead)
{
	return ((BufferRead_t*)bufferRead)->sizeUsedByte;
}
uint32_t GetByteUsed(void* bufferRead)
{
	return ((BufferRead_t*)bufferRead)->sizeUsedByteAlign1;
}

uint32_t GetBufferRemByte(void* bufferRead)
{
	return ((BufferRead_t*)bufferRead)->sizeRemByte;
}
//interface
bool BufferReadInit(void* bufferRead)
{
	if (!bufferRead)
		return false;
	memset(bufferRead, 0, sizeof(BufferRead_t));
	BufferCacheInit(&((BufferRead_t*)bufferRead)->dw);
	return true;
}

bool SetBufferIn(void* bufferRead, uint8_t* in, uint32_t inSize)
{
	//check
	if ((!bufferRead)
		|| (!in)
		|| (inSize <= 0))
		return false;
	BufferRead_t* read = (BufferRead_t*)bufferRead;
	read->buffer = in;
	read->sizeRemByte = inSize;
	read->sizeUsedByte = 0;
	read->sizeUsedByteAlign1 = 0;
	
	return true;
}

//not saftey
//defaule read->buff have enough size
static inline uint8_t BufferReadByte(BufferRead_t* read)
{
	uint8_t readedByte;
	BufferCacheDW_t* dw = &read->dw;
	if (!BufferCacheReadByte(dw, &readedByte))
	{
		BufferCacheUpdata(dw, read->buffer + read->sizeUsedByte, read->sizeRemByte);
		BufferCacheReadByte(dw, &readedByte);
#if 1
		read->sizeUsedByte += 4;
		read->sizeRemByte -= 4;
#else
		if (read->sizeRemByte > 4)
		{
			read->sizeUsedByte += 4;
			read->sizeRemByte -= 4;
		}
		else
		{
			read->sizeUsedByte += read->sizeRemByte;
			read->sizeRemByte = 0;
		}
#endif
	}
	read->sizeUsedByteAlign1++;
	return readedByte;
}

uint32_t BufferRead(void* bufferRead, uint8_t readByte)
{
	//check
#if 0
	if (readByte > 4)
		return false;
	if (read->sizeRemByte + GetBufferCacheRemByte(read) < readByte)
	{
		return false;
	}
#endif
	//read
	uint32_t readedDoubleWord = 0;
	for (uint8_t i = 0; i < readByte; i++)
	{
		readedDoubleWord <<= 8;
		readedDoubleWord += (uint32_t)BufferReadByte((BufferRead_t*)bufferRead);
	}
	return readedDoubleWord;
}
