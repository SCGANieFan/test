#pragma once
#include"MTF.Type.h"
#include"MTF.Printer.h"
#include"MTF.Memory.h"
class MTF_Object
{
public:
	MTF_Object();
	virtual ~MTF_Object();
public:
	virtual mtf_int32 Set(const mtf_int8*key, mtf_void *val);
	virtual mtf_int32 Get(const mtf_int8*key, mtf_void *val);

private:

};

