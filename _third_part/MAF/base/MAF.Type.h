#pragma once
#include<stdint.h>

#if 0
typedef uint8_t maf_uint8;
typedef uint16_t maf_uint16;
typedef uint32_t maf_uint32;
typedef uint64_t maf_uint64;

typedef int8_t maf_int8;
typedef int16_t maf_int16;
typedef int32_t maf_int32;
typedef int64_t maf_int64;
#else
typedef unsigned char maf_uint8;
typedef unsigned short maf_uint16;
typedef unsigned int maf_uint32;
typedef unsigned long long maf_uint64;

typedef char maf_int8;
typedef short maf_int16;
typedef int maf_int32;
typedef long long maf_int64;
#endif
typedef float maf_float;
typedef double maf_double;
typedef bool maf_bool;
typedef void maf_void;




