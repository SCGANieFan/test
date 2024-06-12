#if 1
#include"Ape.Inner.rollbuffer.h"

bool ApeRollBuffer::Init(uint8_t* buffer, uint32_t bufferLen, uint32_t bufferLenRemMin)
{
	this->buffer = buffer;
	this->bufferLen = bufferLen;
	this->bufferLenRemMin = bufferLenRemMin;
	this->offset = 0;
	this->bufferLenRem = bufferLen;
	return true;
}

void ApeRollBuffer::Increment(uint32_t bytes)
{
#if 1
	offset += bytes;
	bufferLenRem -= bytes;
	if (bufferLenRem < bufferLenRemMin)
	{
		memcpy(buffer, buffer + offset, bufferLenRem);
		offset = 0;
		bufferLenRem = bufferLen;
	}
#endif
}











#endif