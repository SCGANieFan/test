#pragma once
#include<stdint.h>



#if 1
#include<string.h>
#define AS_MCM_CPY(dst,src,size) memcpy(dst,src,size)
#define AS_MCM_SET(dst,val,size) memset(dst,val,size)
#define AS_MCM_MOVE(dsr,src,size) memmove(dsr,src,size)
#else
#define AS_MCM_CPY(dst,src,size)
#define AS_MCM_SET(dst,val,size)
#define AS_MCM_MOVE(dsr,src,size)
#endif

