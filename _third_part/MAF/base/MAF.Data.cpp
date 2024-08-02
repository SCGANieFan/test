
#include"MAF.Data.h"
//#include"MAF.Memory.h"
#include"MAF.Printf.h"

#if 1
#define MAF_DATA_PRINT(fmt,...) MAF_PRINT(fmt,##__VA_ARGS__)
#else
#define MAF_DATA_PRINT(s) 
#endif

MAF_Data::MAF_Data()
{
}
MAF_Data::~MAF_Data()
{
}

//set
maf_int32 MAF_Data::SetFlag(maf_uint32 flag)
{
	_flags |= flag;
	return 0;
}

//get
maf_uint8* MAF_Data::GetBuf()
{
	return _buff;
}

maf_int32 MAF_Data::GetMax()
{
	return _max;
}

maf_uint8* MAF_Data::GetData()
{
	return _buff + _off;
}

maf_int32 MAF_Data::GetSize()
{
	return _size;
}

maf_uint8* MAF_Data::GetLeftData()
{
	return GetData() + GetSize();
}

maf_int32 MAF_Data::GetLeftSize()
{
	return GetMax() - _off - GetSize();
}

maf_int32 MAF_Data::GetFlags()
{
	return _flags;
}

//
maf_int32 MAF_Data::Init(maf_uint8* buf, maf_int32 size)
{
	return Init(buf, size, size);
}

maf_int32 MAF_Data::Init(maf_uint8* buf, maf_int32 size, maf_int32 max)
{
	if (!buf
		|| size < 0
		|| max < 0)
	{
		MAF_DATA_PRINT("err,buf:%x,size:%d,max:%d", (maf_uint32)buf, size, max);
		return -1;
	}
	_buff = buf;
	_off = 0;
	_size = size;
	_max = max;
	_flags = 0;
	return 0;
}

//
maf_int32 MAF_Data::Append(maf_uint8* buf, maf_int32 size)
{
	if (!buf
		|| GetLeftSize() < size)
	{
		MAF_DATA_PRINT("err,buf:%x,size:%d,leftSize", (maf_uint32)buf, size, GetLeftSize());
		return -1;
	}
	MAF_MEM_CPY(GetLeftData(), buf, size);
	_size += size;	
	return 0;
}

maf_int32 MAF_Data::Append(maf_int32 size)
{
	if (GetLeftSize() < size)
	{
		MAF_DATA_PRINT("err,size:%d,leftSize:%d", size, GetLeftSize());
		return -1;
	}
	_size += size;
	return 0;
}

maf_int32 MAF_Data::Used(maf_int32 size)
{
	if (GetSize() < size)
	{
		MAF_DATA_PRINT("err,size:%d,usedSize£º%d", _size, size);
		return -1;
	}
	_off += size;
	_size -= size;
	return 0;
}

maf_bool MAF_Data::CheckFlag(maf_uint32 flag)
{
	return (maf_bool)(_flags & flag);
}

maf_int32 MAF_Data::ClearFlag(maf_uint32 flag)
{
	_flags &= ~flag;
	return 0;
}

maf_int32 MAF_Data::ClearUsed()
{
	if (_size > 0
		&& _off > 0)
	{
		MAF_MEM_MOVE(_buff, GetData(), _size);
	}
	_off = 0;
	return 0;
}

