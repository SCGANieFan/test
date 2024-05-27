#pragma once

namespace Algo {

#ifndef EXTERNC
#ifndef __cplusplus
#define EXTERNC
#else
#define EXTERNC extern "C"
#endif
#endif

#define STATIC static
#define INLINE inline

#define ALIGEN8(x) (((x+7)>>3)<<3)


#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)>(b)?(b):(a))



}
