
#include "MAF.Algorithm.h"
MAF_Algorithm::MAF_Algorithm()
{
}

MAF_Algorithm::~MAF_Algorithm()
{
}


maf_int32 MAF_Algorithm::Set(const maf_int8* key, maf_void* val)
{
	if (MAF_String::StrCompare(key, "Malloc")) {
		_memory._malloc = (MAF_Memory::Malloc_t)val;
	}
	else if (MAF_String::StrCompare(key, "Realloc")) {
		_memory._realloc = (MAF_Memory::Realloc_t)val;
	}
	else if (MAF_String::StrCompare(key, "Calloc")) {
		_memory._calloc = (MAF_Memory::Calloc_t)val;
	}
	else if (MAF_String::StrCompare(key, "Free")) {
		_memory._free = (MAF_Memory::Free_t)val;
	}
	return -1;
}
maf_int32 MAF_Algorithm::Get(const maf_int8* key, maf_void* val)
{
	return -1;
}


