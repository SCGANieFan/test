#include "MTF.Source.h"
#include "MTF.String.h"


MTF_Source::MTF_Source()
{
	_to = 0;
}
MTF_Source::~MTF_Source()
{
}

mtf_int32 MTF_Source::Push(MTF_Data& oData)
{
	if (!_to)
	{
		MTF_PRINT("err");
		return -1;
	}
	_to->Receive(oData);
}

mtf_int32 MTF_Source::Run()
{
	MTF_Data* oData;
	if (generate(oData) < 0)
		return -1;
	if (Push(*oData) < 0)
		return -1;
	return 0;
}

mtf_int32 MTF_Source::Generate(MTF_Data*& oData)
{
	generate(oData);
	return 0;
}

mtf_int32 MTF_Source::Set(const mtf_int8* key, mtf_void* val)
{
	if (MTF_Srting::StrCompare(key, "to"))
	{
		_to = (MTF_Element*)val; return 0;
	}
	return MTF_Element::Set(key, val);
}

mtf_int32 MTF_Source::Get(const mtf_int8* key, mtf_void* val)
{
	return MTF_Element::Get(key, val);
}






