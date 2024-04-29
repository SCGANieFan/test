#pragma once
#include"MTF.Type.h"
class MTF_String
{
public:
	MTF_String();
	~MTF_String();

public:
	static void* MemCpy(mtf_int8* dst, mtf_int8* src, mtf_int32 size);
	static void* MemSet(mtf_int8* dst, mtf_int32 val, mtf_int32 size);
	static void* MemMove(mtf_int8* dst, mtf_int8* src, mtf_int32 size);

	static mtf_int32 StrLen(const mtf_int8* str);
	static mtf_int32 Search(const mtf_int8* str, const mtf_int8* temp, mtf_int32 posStart, mtf_int32 posEnd, mtf_bool dir = true);
	static mtf_int32 Search(const mtf_int8* str, const mtf_int8* temp, mtf_bool dir = true);
	static mtf_int32 Cut(const mtf_int8* str, const mtf_int8* characteristic, mtf_int8* out, mtf_int32 outByteMax);
	static mtf_int32 Cut(const mtf_int8* str, const mtf_int8* characteristic, mtf_int8* out);
	static mtf_int32 StrAppend(mtf_int8* src, mtf_int8* dst);

	static bool StrCompare(const mtf_int8* cmp0, const mtf_int8* cmp1);
	static const mtf_int8* BaseName(const mtf_int8* str);
private:

};


#if 1
#define MTF_MEM_CPY(dst,src,size) MTF_String::MemCpy((mtf_int8*)dst,(mtf_int8*)src,(mtf_int32)size)
#define MTF_MEM_SET(dst,val,size) MTF_String::MemSet((mtf_int8*)dst,(mtf_int32)val,(mtf_int32)size)
#define MTF_MEM_MOVE(dst,src,size) MTF_String::MemMove((mtf_int8*)dst,(mtf_int8*)src,(mtf_int32)size)
#else
#include<string.h>
#define MTF_MCM_CPY(dst,src,size) memcpy(dst,src,size)
#define MTF_MCM_SET(dst,val,size) memset(dst,val,size)
#define MTF_MCM_MOVE(dsr,src,size) memmove(dsr,src,size)
#endif



