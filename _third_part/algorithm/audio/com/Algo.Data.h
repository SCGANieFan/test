#pragma once

#include"Algo.Type.h"
#include"Algo.Memory.h"
class Data
{
public:
	Data() {};
	~Data() {};
public:
	//set
	void SetFlags(u32 flags) {
		_flags |= flags;
	};
	void ClearFlags(u32 flags) {
		_flags &= ~flags;
	};

	//get
	u8* GetData() {
		return _buff + _off;
	};

	u8* GetLeftData() {
		return GetData() + _size;
	};

	i32 GetLeftSize() {
		return _max - _off - _size;
	};

	u32 GetFlags() {
		return _flags;
	};

	u8* GetBuf() {
		return _buff;
	};

	//
	b1 Init(u8* buf, i32 max) {
		_buff = buf;
		_off = 0;
		_size = 0;
		_max = max;
		_flags = 0;
		return true;
	};

	b1 Append(u8* buf, i32 size) {		
		ALGO_MEM_CPY(GetLeftData(), buf, size);
		_size += size;
	};

	void Used(i32 usedSize)
	{
		_off += usedSize;
		_size -= usedSize;
	}

	void ClearUsed()
	{
		if (_off)
		{
			ALGO_MEM_MOVE(_buff, GetData(), _size);
			_off = 0;
		}
	}

	b1 CheckFlag(u32 flag)
	{
		return (b1)(_flags & flag);
	}
protected:
	u8* _buff = 0;
	i32 _off = 0;
	i32 _size = 0;
	i32 _max = 0;
	u32 _flags = 0;
};


