#pragma once
#include<stdint.h>

typedef enum {
	MAFA_FRAME_IS_EMPTY = 1 << 0,
}MAFA_FrameFlag;

typedef struct {
	uint8_t* buff;
	int32_t off;
	int32_t size;
	int32_t max;
	uint32_t flags;
}MAFA_Frame;

