#pragma once

#include"Algo.Type.h"
#include"Algo.Macro.h"

namespace Algo {
#define ALGO_MIN(a,b) ((a)<(b)?(a):(b))
#define ALGO_MAX(a,b) ((a)<(b)?(b):(a))


	class Logical_c
	{
	public:
		Logical_c() {}
		~Logical_c() {}
	public:
		STATIC INLINE i32 SaturateFrom32To16(i32 v) {
			const i32 MIN = -32768;
			const i32 MAX = 32767;
			if (v < MIN) return MIN;
			else if (v > MAX) return MAX;
			return v;
		}

		STATIC INLINE i32 CLZ32(i32 n) {
			i32 cnt = 0;
			if (n == 0)
				return 0;
			for (i32 i = 0; i < 31; i++) {
				if (!(n & 0x40000000)) {
					cnt++;
					n <<= 1;
				}
				else {
					break;
				}
			}
			return cnt;
		}
		STATIC INLINE  i32 MAQ32(i32 a, i32 b, i32 c)
		{
			i32 y = (i32)((i64)a * b >> 32) + c;
			return y;
		}


	private:

	};


}

