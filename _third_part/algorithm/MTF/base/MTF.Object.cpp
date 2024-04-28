
#include "MTF.Object.h"
#include "MTF.String.h"


MTF_Object::MTF_Object()
{
}
MTF_Object::~MTF_Object()
{
}


mtf_int32 MTF_Object::Set(const mtf_int8* key, mtf_void* val)
{
#if 0
	if (MTF_Srting::StrCompare(key, "tag"))
	{
		SetTag((const mtf_int8*)val); return 0;
	}
#endif
	return -1;
}

mtf_int32 MTF_Object::Get(const mtf_int8* key, mtf_void* val)
{
	return -1;
}


