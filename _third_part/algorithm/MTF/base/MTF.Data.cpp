#include "MTF.Data.h"
#include "MTF.String.h"

MTF_Data::MTF_Data()
{
}
MTF_Data::~MTF_Data()
{
}

mtf_int32 MTF_Data::Init(mtf_uint8* buff, mtf_int32 len)
{
	_size = 0;
	_flags = 0;
	_buff = buff;
	_off = 0;
	_max = len;
	return 0;
}
mtf_int32 MTF_Data::DeInit()
{
	_size = 0;
	_flags = 0;
	_buff = 0;
	_off = 0;
	_max = 0;
	return 0;
}
mtf_int32 MTF_Data::Append(mtf_uint8* buff, mtf_int32 len)
{
	MTF_MEM_CPY((mtf_int8*)Data(), (mtf_int8*)buff, len);
	_size += len;
	return 0;
}

mtf_void MTF_Data::Clear()
{
}
mtf_uint8* MTF_Data::Data()
{
	return _buff + _off;
}
mtf_uint8* MTF_Data::LeftData()
{
	return _buff + _off + _size;
}
mtf_int32 MTF_Data::LeftSize()
{
	return _max - _off - _size;
}
mtf_int32 MTF_Data::Used(mtf_int32 size)
{
	size = size < _size ? size : _size;
	_off += size;
	_size -= size;
	if (_size > 0)
	{
		MTF_MEM_MOVE((mtf_int8*)_buff, (mtf_int8*)_buff + _off, _size);
		_size = 0;
	}
	_off = 0;
	return 0;
}