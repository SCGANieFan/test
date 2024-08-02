#pragma once
#include"Algo.Type.h"
#include"Algo.Macro.h"

namespace Algo {
	class Buffer
	{
	public:
		Buffer() {};
		Buffer(u8* buf, i32 max) { _buf = buf; _max = max; };
		//INLINE Buffer(u8* buf, i32 max) { _buf = buf; _max = max; };
		~Buffer() {};
	public:
		INLINE void Init(u8* buf, i32 max) { _buf = buf; _max = max; };
	public:
		u8* _buf = 0;
		i32 _max = 0;
	};
}