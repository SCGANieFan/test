#include"Algo.Printer.h"
#include"Algo.Memory.h"
#include"ApeOri.h"
#include"Ape.Inner.type.h"

#if 0



class BufferRead
{
public:
	BufferRead() {};
	~BufferRead() {};

public:
	u32 inline GetBufferUsed(void* bufferRead) {
		return sizeUsedByte;
	};
	u32 inline GetByteUsed(void* bufferRead) {
		return sizeUsedByteAlign1;
	}
	u32 inline GetBufferRemByte(void* bufferRead) {
		return sizeRemByte;
	}
	b1 inline Init(void* bufferRead)
	{
		offset = -1;
		return true;
	}
	b1 inline SetBufferIn(void* bufferRead, u8* in, u32 inSize) {
		//check
		if ((!bufferRead)
			|| (!in)
			|| (inSize <= 0))
			return false;
		buffer = in;
		sizeRemByte = inSize;
		sizeUsedByte = 0;
		sizeUsedByteAlign1 = 0;

		return true;
	}

	inline b1 BufferCacheReadByte(u8* readedByte)
	{
		if (offset < 0)
			return false;
		*readedByte = data8[offset];
		offset--;
		return true;
	}

	inline b1 BufferCacheUpdata(u8* in, u32 inSize)
	{
#if 0
		if (inSize > 4)
		{
			dw->data32 = *((u32*)in);
			dw->offset = 3;
		}
		else
		{
			uint16_t shift = 0;
			dw->data32 = 0;
			for (u8 i = 0; i < inSize; i++)
			{
				dw->data32 += *(in++) << shift;
				shift += 8;
			}
			dw->offset = inSize;
		}
#else
		ALGO_MEM_CPY(data8, in, 4);
		offset = 3;
#endif
		return true;
	}

	//not saftey
	inline u8 BufferReadByte()
	{
		u8 readedByte;
		if (!BufferCacheReadByte(&readedByte))
		{
			BufferCacheUpdata(buffer + sizeUsedByte, sizeRemByte);
			BufferCacheReadByte(&readedByte);
#if 1
			sizeUsedByte += 4;
			sizeRemByte -= 4;
#else
			if (sizeRemByte > 4)
			{
				sizeUsedByte += 4;
				sizeRemByte -= 4;
			}
			else
			{
				sizeUsedByte += sizeRemByte;
				sizeRemByte = 0;
			}
#endif
		}
		sizeUsedByteAlign1++;
		return readedByte;
	}


	u32 inline Read(void* bufferRead, u8 readByte)
	{
		//read
		u32 readedDoubleWord = 0;
		for (u8 i = 0; i < readByte; i++)
		{
			readedDoubleWord <<= 8;
			readedDoubleWord += (u32)BufferReadByte();
		}
		return readedDoubleWord;
	}
private:
	union {
		u32 data32;
		u8 data8[4];
	};
	int8_t offset=-1;
	u8* buffer=0;
	u32 sizeUsedByte=0;
	u32 sizeUsedByteAlign1=0;
	u32 sizeRemByte=0;
};


#endif



#if 0
#include"ApePrive.h"
#include<string.h>
#include<stdlib.h>
typedef struct
{
	union {
		u32 data32;
		u8 data8[4];
	};
	int8_t offset;
} BufferCacheDW_t;



typedef struct {
	u8* buffer;
	u32 sizeUsedByte;
	u32 sizeUsedByteAlign1;
	u32 sizeRemByte;
	BufferCacheDW_t dw;
}BufferRead_t;

#if 0
static inline u8 GetBufferCacheRemByte(BufferRead_t* read)
{
	return dw.offset + 1;
}
#endif

static inline void BufferCacheInit(BufferCacheDW_t* dw)
{
	dw->data32 = 0;
	dw->offset = -1;
}
static inline b1 BufferCacheReadByte(BufferCacheDW_t* dw, u8* readedByte)
{
	if (dw->offset < 0)
		return false;
	*readedByte = dw->data8[dw->offset];
	dw->offset--;
	return true;
}
static inline b1 BufferCacheUpdata(BufferCacheDW_t* dw, u8* in, u32 inSize)
{
#if 0
	if (inSize > 4)
	{
		dw->data32 = *((u32*)in);
		dw->offset = 3;
	}
	else
	{
		uint16_t shift = 0;
		dw->data32 = 0;
		for (u8 i = 0; i < inSize; i++)
		{
			dw->data32 += *(in++) << shift;
			shift += 8;
		}
		dw->offset = inSize;
	}
#else
	memcpy(dw->data8, in, 4);
	dw->offset = 3;
#endif
	return true;
}


//BufferRead_t
u32 GetBufferReadSize()
{
	return sizeof(BufferRead_t);
}

u32 GetBufferUsed(void* bufferRead)
{
	return ((BufferRead_t*)bufferRead)->sizeUsedByte;
}
u32 GetByteUsed(void* bufferRead)
{
	return ((BufferRead_t*)bufferRead)->sizeUsedByteAlign1;
}

u32 GetBufferRemByte(void* bufferRead)
{
	return ((BufferRead_t*)bufferRead)->sizeRemByte;
}
//interface
b1 BufferReadInit(void* bufferRead)
{
	if (!bufferRead)
		return false;
	memset(bufferRead, 0, sizeof(BufferRead_t));
	BufferCacheInit(&((BufferRead_t*)bufferRead)->dw);
	return true;
}

b1 SetBufferIn(void* bufferRead, u8* in, u32 inSize)
{
	//check
	if ((!bufferRead)
		|| (!in)
		|| (inSize <= 0))
		return false;
	BufferRead_t* read = (BufferRead_t*)bufferRead;
	buffer = in;
	sizeRemByte = inSize;
	sizeUsedByte = 0;
	sizeUsedByteAlign1 = 0;

	return true;
}

//not saftey
//defaule buff have enough size
static inline u8 BufferReadByte(BufferRead_t* read)
{
	u8 readedByte;
	BufferCacheDW_t* dw = &dw;
	if (!BufferCacheReadByte(dw, &readedByte))
	{
		BufferCacheUpdata(dw, buffer + sizeUsedByte, sizeRemByte);
		BufferCacheReadByte(dw, &readedByte);
#if 1
		sizeUsedByte += 4;
		sizeRemByte -= 4;
#else
		if (sizeRemByte > 4)
		{
			sizeUsedByte += 4;
			sizeRemByte -= 4;
		}
		else
		{
			sizeUsedByte += sizeRemByte;
			sizeRemByte = 0;
		}
#endif
	}
	sizeUsedByteAlign1++;
	return readedByte;
}

u32 BufferRead(void* bufferRead, u8 readByte)
{
	//check
#if 0
	if (readByte > 4)
		return false;
	if (sizeRemByte + GetBufferCacheRemByte(read) < readByte)
	{
		return false;
	}
#endif
	//read
	u32 readedDoubleWord = 0;
	for (u8 i = 0; i < readByte; i++)
	{
		readedDoubleWord <<= 8;
		readedDoubleWord += (u32)BufferReadByte((BufferRead_t*)bufferRead);
	}
	return readedDoubleWord;
}

#endif
