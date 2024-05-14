#pragma once

#if 0

#include<stdint.h>
	typedef uint8_t u8;
	typedef uint16_t u16;
	typedef uint32_t u32;
	typedef uint64_t u64;

	typedef int8_t i8;
	typedef int16_t i16;
	typedef int32_t i32;
	typedef int64_t i64;

#else
namespace Algo {

	typedef unsigned char u8;
	typedef unsigned short u16;
	typedef unsigned int u32;
	typedef unsigned long long u64;

	typedef char i8;
	typedef short i16;
	typedef int i32;
	typedef long long i64;


	struct i24 {
		u8 x[3];
		i24(i32 in) { *this = in; }
		inline i32 operator+(i24& in) { return (i32)*this + (i32)in; }
		inline i32 operator<<(i32 shf) { return (i32)*this << shf; }
		inline i32 operator>>(i32 shf) { return (i32)*this >> shf; }
		inline i32 operator<<=(i32 shf) { return *this = (i32)*this << shf; }
		inline i32 operator>>=(i32 shf) { return *this = (i32)*this >> shf; }
		inline i32 operator=(i32 in) { x[0] = ((u8*)&in)[0]; x[1] = ((u8*)&in)[1]; x[2] = ((u8*)&in)[2]; return in; }
		inline i32 operator=(i24& in) { x[0] = in.x[0]; x[1] = in.x[1]; x[2] = in.x[2]; return in; }
		inline operator i32()const { return (i32)((x[2] << 24) | (x[1] << 16) | (x[0] << 8)) >> 8; }
	};
}

#endif
namespace Algo {
	typedef float f32;
	typedef double f64;
	typedef bool b1;
};
