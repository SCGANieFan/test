#pragma once

#include"Algo.Type.h"
#include"Algo.Macro.h"
#include"Algo.Memory.h"
#include"Algo.Buffer.h"

namespace Algo {
	class Data
	{
	public:
		Data() {};
		~Data() {};
	public:
		//set
		INLINE void SetFlags(u32 flags) {
			_flags |= flags;
		};
		INLINE void ClearFlags(u32 flags) {
			_flags &= ~flags;
		};

		//get
		INLINE u8* GetData() {
			return _buff + _off;
		};

		INLINE i32 GetSize() {
			return _size;
		};

		INLINE u8* GetLeftData() {
			return GetData() + _size;
		};

		INLINE i32 GetLeftSize() {
			return _max - _off - _size;
		};

		INLINE u32 GetFlags() {
			return _flags;
		};

		INLINE u8* GetBuf() {
			return _buff;
		};


		INLINE b1 Init(Buffer* buffer) {
			_buff = buffer->_buf;
			_off = 0;
			_size = 0;
			_max = buffer->_max;
			_flags = 0;
			return true;
		};

		INLINE b1 Append(u8* buf, i32 size) {
			ALGO_MEM_CPY(GetLeftData(), buf, size);
			_size += size;
			return true;
		};
		
		INLINE b1 Append(i32 size) {
			_size += size;
			return true;
		};

		INLINE b1 AppendFully(u8* buf, i32 size, i32 *usedSize) {
			i32 appendSize = GetLeftSize();
			appendSize = appendSize > size ? size : appendSize;
			Append(buf, appendSize);
			if(usedSize)
				*usedSize = appendSize;
			return true;
		};

		INLINE void Used(i32 usedSize)
		{
			_off += usedSize;
			_size -= usedSize;
		}

		INLINE void ClearUsed()
		{
			if (_off)
			{
				ALGO_ASSERT(_off > 0);
				ALGO_ASSERT(_size > 0);
				if (_size < 0)
					int a1 = 1;
				ALGO_MEM_MOVE(_buff, GetData(), _size);
				_off = 0;
			}
		}

		INLINE b1 CheckFlag(u32 flag)
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
}

