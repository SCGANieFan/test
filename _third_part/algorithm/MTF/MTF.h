#pragma once
#include<stdint.h>


#define MTF_REGISTER(name) \
extern void mtf_##name##_register();\
			mtf_##name##_register();

int32_t MultiemdiaTestInit();
int32_t MultiemdiaApi(const char* str, void** param);


