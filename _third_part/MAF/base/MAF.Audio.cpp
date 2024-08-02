
#include "MAF.Audio.h"
MAF_Audio::MAF_Audio()
{
}
MAF_Audio::~MAF_Audio()
{
}

maf_int32 MAF_Audio::Set(const maf_int8* key, maf_void* val)
{
	return MAF_Algorithm::Set(key, val) & MAFA_Info::Set(key, val);
}
maf_int32 MAF_Audio::Get(const maf_int8* key, maf_void* val)
{
	return MAF_Algorithm::Get(key, val) & MAFA_Info::Get(key, val);
}

