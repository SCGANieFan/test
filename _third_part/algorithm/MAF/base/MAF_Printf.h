#pragma once
#include<stdint.h>

class MAF_Printf
{
protected:

public:
	MAF_Printf();
	~MAF_Printf();
public:
	static void Printf(const char* _Format, ...);
private:

};

#define MAF_PRINT(fmt,...)		MAF_Printf::Printf("<%s>[%d]" fmt "\n", __func__, __LINE__, ##__VA_ARGS__)

