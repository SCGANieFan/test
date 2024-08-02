#pragma once

#include"Algo.Type.h"
#include"Algo.Macro.h"
// #include"Algo.Memory.h"
// #include"Algo.Buffer.h"

namespace Algo {
	class Math_c
	{
	public:
		Math_c() {};
		~Math_c() {};
	public:
		STATIC INLINE u64 Sqrt(u64 x) {
			if (x == 0)
				return 0;
			u64 left = 1;
			u64 right = x;
			while (left <= right) {
				u64 mid = left + ((right - left) >> 1);
				if (x >= (mid * mid)) {
					if (x < (mid + 1) * (mid + 1)) {
						return mid;
					}
					else {
						left = mid + 1;
					}
				}
				else {
					right = mid - 1;
				}
			}
			return 0;
		}

		STATIC INLINE i64 DivisionLocal(i64 dividend, i64 divisor) {
			if (divisor == 0) return -1;
			if (dividend == 0) return 0;
			if (dividend < divisor) return 0;

			i64 result = 0;
			i64 temp_divisor = divisor;
			i64 multiple = 1;
			while (dividend >= (temp_divisor << 1)) {
				temp_divisor <<= 1;
				multiple <<= 1;
			}

			while (dividend >= divisor) {
				if (dividend >= temp_divisor) {
					dividend -= temp_divisor;
					result += multiple;
				}
				multiple >>= 1;
				temp_divisor >>= 1;
			}

			return result;
		}

	};
}

