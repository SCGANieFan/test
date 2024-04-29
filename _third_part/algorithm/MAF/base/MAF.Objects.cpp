#include "MAF.Objects.h"
#include "MAF.String.h"

typedef maf_void* (*Create_t)(maf_void* buff);
typedef maf_int32 (*Getsize_t)(maf_void);

MAF_Object MAF_Object::object;


MAF_Object::MAF_Object()
{
    MAF_MEM_SET(_items, 0, sizeof(_items));
}
MAF_Object::~MAF_Object()
{
}

maf_int32 MAF_Object::GetSize(const maf_int8* type)
{
	MAF_Object::Item* item = Search(type);
	if (!item)
		return 0;
	return ((Getsize_t)(item->getSize))();
}


maf_void* MAF_Object::Create(const maf_int8* type, maf_void* buff)
{
	MAF_Object::Item* item = Search(type);
	if(!item)
		return NULL;
	return ((Create_t)(item->create))(buff);
}

MAF_Object::Item* MAF_Object::Search(const maf_int8* type)
{
	maf_uint64 inType = *((maf_uint64*)type);
	for (auto& item : object._items)
	{
		if (IsTypeEqual(item.type, type))
		{
			return &item;
		}
	}
	return NULL;
}

maf_bool MAF_Object::IsTypeEqual(const maf_int8* type0, const maf_int8* type1)
{
	const maf_int8* ptr;
	maf_int32 len0 = 0;
	ptr = type0;
	if(!ptr)
		len0 = 0;
	else
	{
		while (*ptr++ != '\0')
		{
			len0++;
			if (len0 > 100)
				break;
		}
	}
	

	maf_int32 len1 = 0;
	ptr = type1;
	if (!ptr)
		len1 = 0;
	else
	{
		while (*ptr++ != '\0')
		{
			len1++;
			if (len1 > 100)
				break;
		}
	}

	if (len0 != len1)
		return false;

	for (maf_int32 i = 0; i < len0; i++)
	{
		if(type0[i] != type1[i])
			return false;
	}
	
	return true;
}

