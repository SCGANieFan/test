#pragma once
#include"MTF.Object.h"
#include"MTF.Element.h"
#include"MTF.Data.h"
class MTF_Port :public MTF_Element
{
public:
	MTF_Port();
	~MTF_Port();
private:
#if 0
	MTF_Element* _from;
	MTF_Element* _to;
#endif
};

