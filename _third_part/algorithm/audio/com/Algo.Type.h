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

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef char i8;
typedef short i16;
typedef int i32;
typedef long long i64;


struct i24 {
	u8 _vs[3];
	i24(i32 v) { *this = v; }
	inline i32 operator+(i24& v) { return (i32)*this + (i32)v; }
	inline i32 operator<<(i32 shf) { return (i32)*this << shf; }
	inline i32 operator>>(i32 shf) { return (i32)*this >> shf; }
	inline i32 operator<<=(i32 shf) { return *this = (i32)*this << shf; }
	inline i32 operator>>=(i32 shf) { return *this = (i32)*this >> shf; }
	inline i32 operator=(i32 v) { _vs[0] = ((u8*)&v)[0]; _vs[1] = ((u8*)&v)[1]; _vs[2] = ((u8*)&v)[2]; return v; }
	inline i32 operator=(i24& v) { _vs[0] = v._vs[0]; _vs[1] = v._vs[1]; _vs[2] = v._vs[2]; return v; }
	inline operator i32()const { return (i32)((_vs[2] << 24) | (_vs[1] << 16) | (_vs[0] << 8)) >> 8; }
};


#endif
typedef float f32;
typedef double f64;
typedef bool b1;
