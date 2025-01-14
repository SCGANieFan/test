#pragma once

#include"Algo.Type.h"
#include"Algo.Macro.h"
// #include"Algo.Memory.h"
// #include"Algo.Buffer.h"

namespace Algo {
#define ALGO_ABS(x) ((x)>0?(x):(-x))


#if 1
	template<class Ti, class To>
	struct Sqrt_t {
		static inline To Run(Ti x) {
			if (x == 0)
				return 0;
			Ti left = 1;
			Ti right = x;
			while (left <= right) {
				Ti mid = left + ((right - left) >> 1);
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
	};

	template<>
	struct Sqrt_t<f32, f32> {
		static inline f32 Run(f32 a) {
			if (a < 0) {
				return 0;
			}
			static const f32 EPSILON = 0.00001f;
			f32 x = a / 2.0f;
			f32 y = x;
			f32 diff;
			while (1) {
				y = x;
				x = 0.5f * (x + a / x);
				diff = x - y;
				if (ALGO_ABS(diff) < EPSILON) {
					break;
				}
			}
			return x;
		}
	};
#endif

#if 1
	template<class Ti, class To>
	struct Division_t {
		STATIC INLINE To Run(Ti dividend, Ti divisor) {
#if 0
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
#else
			return (To)(dividend / divisor);
#endif
		}
	};

	template<>
	struct Division_t<f32, f32> {
		STATIC INLINE f32 Run(f32 dividend, f32 divisor) {
			return dividend / divisor;
		}
	};
#endif


#if 1
	template<class T>
	class Math_c
	{
	public:
		Math_c() {}
		~Math_c() {}
	public:
		STATIC INLINE T Sqrt(T x) {
			return Sqrt_t<T, T>::Run(x);
		}
		STATIC INLINE T Division(T dividend, T divisor) {
			return Division_t<T, T>::Run(dividend, divisor);
		}
	};
#endif

}

