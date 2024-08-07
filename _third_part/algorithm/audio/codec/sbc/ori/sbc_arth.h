#pragma once
#include <stdint.h>

static inline int32_t __smmla(int32_t a, int32_t b, int32_t c)
{
	int32_t y = (int32_t)((int64_t)a * b >> 32) + c;
	return y;
}

static inline int SATURATE32(int ARG1, int ARG2)
{
	if (ARG1 < -32768) {
        return -32768;
    } else if (ARG1 > 32767) {
        return 32767;
    } else {
        return ARG1;
    }
	
}

#define SBC_ABS(x) ((x) > 0)? (x) : (-(x))
#define SBC_MAX(x, y) ((x) > (y))? (x) : (y)

static inline int CLZ(int n)
{
	int cnt = 0;
	if (n == 0)
		return 0;
	for (int i = 0; i < 31; i++) {
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

