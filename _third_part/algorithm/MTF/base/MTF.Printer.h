#pragma once
#include"MTF.Type.h"
class MTF_Printer
{
public:
	MTF_Printer();
	~MTF_Printer();
public:
	static mtf_void Printf(const mtf_int8* _Format, ...);
private:
};
#define MTF_PRINT(fmt,...)		MTF_Printer::Printf("<%s>[%d]" fmt "\n", __func__, __LINE__, ##__VA_ARGS__)
