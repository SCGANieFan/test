#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include"Buffer.h"

using namespace OggDemux;
Buffer::Buffer()
{
}

Buffer::~Buffer()
{

}

int32_t Buffer::GetMax()
{
	return _max;
}
uint8_t* Buffer::GetBuff()
{
	return _data;
}
int32_t Buffer::GetUsed()
{
	return _used;
}
uint8_t* Buffer::GetBuffValid()
{
	return _data + _used;
}
int32_t Buffer::GetBuffValidSize()
{
	return _size;
}
//
bool Buffer::SetBuf(uint8_t* data, int32_t size)
{
	if (!data
		|| size < 1)
		return false;
	_data = data;
	_max = size;
	return true;
}

bool Buffer::Init(uint8_t* data, int32_t size)
{
	if (!data
		|| size < 1)
		return false;
	_data = data;
	_max = size;
	_size = 0;
	_used = 0;
	return true;
}
bool Buffer::Append(uint8_t* data, int32_t size)
{
    int32_t remByte = _max - _used - _size;
    if (remByte < size){
        return false;
    }
    memcpy(_data + _used + _size, data, size);
	_size += size;
	return true;
}
bool Buffer::Used(int32_t size)
{
	if (size < 0)
		return false;
	if (size > _size)
		return false;

	_used += size;
	_size -= size;
}

void Buffer::MoveToHead()
{
	memmove(_data, _data + _used, _max - _used);
	_used = 0;
}
