#include "MTF.Element.h"
#include "MTF.String.h"

MTF_Element::MTF_Element()
{
}
MTF_Element::~MTF_Element()
{
}

mtf_int32 MTF_Element::Set(const mtf_int8* key, mtf_void* val)
{
	return MTF_Object::Set(key,val);
}
mtf_int32 MTF_Element::Get(const mtf_int8* key, mtf_void* val)
{
	return MTF_Object::Get(key, val);
}
