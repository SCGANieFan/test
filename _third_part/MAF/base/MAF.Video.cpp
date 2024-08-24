
#include "MAF.Video.h"
MAF_Video::MAF_Video()
{
}
MAF_Video::~MAF_Video()
{
}

maf_int32 MAF_Video::Set(const maf_int8* key, maf_void* val)
{
	return MAF_Algorithm::Set(key, val) & MAFV_Info::Set(key, val);
}
maf_int32 MAF_Video::Get(const maf_int8* key, maf_void* val)
{
	return MAF_Algorithm::Get(key, val) & MAFV_Info::Get(key, val);
}

