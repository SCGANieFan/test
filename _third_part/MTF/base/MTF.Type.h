#pragma once
#include<stdint.h>

#if 0
typedef uint8_t mtf_uint8;
typedef uint16_t mtf_uint16;
typedef uint32_t mtf_uint32;
typedef uint64_t mtf_uint64;

typedef int8_t mtf_int8;
typedef int16_t mtf_int16;
typedef int32_t mtf_int32;
typedef int64_t mtf_int64;
#else
typedef unsigned char mtf_uint8;
typedef unsigned short mtf_uint16;
typedef unsigned int mtf_uint32;
typedef unsigned long long mtf_uint64;

typedef char mtf_int8;
typedef short mtf_int16;
typedef int mtf_int32;
typedef long long mtf_int64;
#endif
typedef float mtf_float;
typedef double mtf_double;
typedef bool mtf_bool;
typedef void mtf_void;




