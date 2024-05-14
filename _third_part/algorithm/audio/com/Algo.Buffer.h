#pragma once
#include"Algo.Type.h"

namespace Algo {
	class Buffer
	{
	public:
		Buffer() {};
		~Buffer() {};
	public:
		u8* _buf = 0;
		i32 _max = 0;
	};
}