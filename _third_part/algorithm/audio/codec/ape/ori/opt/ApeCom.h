#pragma once


#include <stdint.h>


#ifndef __cplusplus
#define EXTERNC
#else
#define EXTERNC extern "C"
#endif




#define APE_BLOCKS_MAX   512

#define APE_RET_SUCCESS					 0
#define APE_RET_NOT_ENOUGH_INPUT		-1
//#define APE_RET_GET_SYMBOL_OVERFLOW		-2
#define APE_RET_ENTROPY_DECODE_ERROR		-2
#define APE_RET_PRASE_ERROR				-3
#define APE_RET_NOT_ENOUGH_OUTPUT       -4
#define APE_RET_INPUT_ERROR		        -5
#define APE_RET_FINISH		            -6
#define APE_RET_CONTINUE        		-8

typedef int16_t APE_RET_t;

EXTERNC bool ApeInit();
EXTERNC bool ApeDeinit();