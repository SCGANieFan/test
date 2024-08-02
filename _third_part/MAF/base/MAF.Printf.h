#pragma once
#include"MAF.Type.h"
#include"MAF.String.h"

class MAF_Printf
{
protected:

public:
	MAF_Printf();
	~MAF_Printf();
public:
	static maf_void Printf(const maf_int8* _Format, ...);
private:

};

#define MAF_PRINT(fmt,...)		MAF_Printf::Printf("<%s>[%s](%d)" fmt "\n", MAF_String::BaseName(__FILE__), __func__, __LINE__, ##__VA_ARGS__)

