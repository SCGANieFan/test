#pragma once

#include<stdint.h>

#include"MAF_Memory.h"
#include"MAF_Printf.h"


class MAF_Algorithm
{
public:
	MAF_Algorithm();
	virtual ~MAF_Algorithm();
public:
	MAF_Memory _memory;
	MAF_Printf _printer;
protected:

private:

};

