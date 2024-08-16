#pragma once
#include"Algo.Type.h"
#include"Algo.Macro.h"
using namespace Algo;

class BitStream_c
{
public:
	BitStream_c() {};
	~BitStream_c() {};

public:
	INLINE u32 Init(u8* buff, i32 bytes) {
		_buffer = buff;
		_index = 0;
		_bitsInCache = 0;
		_cacheWord = 0;
		return 0;
	}

	INLINE u32 GetUsedBits() { return _index * 8 + _bitsInCache; }


	INLINE u16 Read(u8 num) {
		i32 tmpnum = num - _bitsInCache;
		u32 bits = 0;
		if (tmpnum > 0) {
			bits = _cacheWord << tmpnum;
			_cacheWord = (_buffer[_index] << 24) |
				(_buffer[_index + 1] << 16) |
				(_buffer[_index + 2] << 8) |
				(_buffer[_index + 3]);
			_index += 4;
			_bitsInCache += 32;
		}
		_bitsInCache -= num;
		return (bits | (_cacheWord >> _bitsInCache)) & ((1u << num) - 1);
	}
	INLINE void Write(u32 val, u8 bits)
	{
		u32 validMask = ((1u << bits) - 1);
		if (_bitsInCache + bits < 32) {
			_cacheWord = (_cacheWord << bits) | (val & validMask);
			_bitsInCache += bits;
		}
		else {
			i32 tmp = 32 - _bitsInCache;
			i32 remaintmp = bits - tmp;
			val = val & validMask;
			u32 cacheWord = (_cacheWord << tmp) | (val >> remaintmp);
			_buffer[_index++] = (u8)(cacheWord >> 24);
			_buffer[_index++] = (u8)(cacheWord >> 16);
			_buffer[_index++] = (u8)(cacheWord >> 8);
			_buffer[_index++] = (u8)(cacheWord >> 0);
			_cacheWord = val & ((1 << remaintmp) - 1);
			_bitsInCache = remaintmp;
		}
	}
	INLINE void Flash() {
		i32 byteInCache = (_bitsInCache + 7) / 8;
		i32 shf = (byteInCache - 1) * 8;
		_cacheWord <<= (byteInCache * 8 - _bitsInCache);
		for (i32 i = 0; i < byteInCache; i++) {
			_buffer[_index++] = (u8)(_cacheWord >> shf);
			shf -= 8;
		}
		_cacheWord = 0;
		_bitsInCache = 0;
	}
private:
	u8* _buffer;
	u32 _index;
	u32 _bitsInCache;
	u32 _cacheWord;
private:

};



