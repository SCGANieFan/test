#pragma once
#include"MAF.Type.h"
class MAF_String
{
public:
	MAF_String();
	~MAF_String();

public:
	static maf_void* MemCpy(maf_int8* dst, maf_int8* src, maf_int32 size);
	static maf_void* MemSet(maf_int8* dst, maf_int32 val, maf_int32 size);
	static maf_void* MemMove(maf_int8* dst, maf_int8* src, maf_int32 size);

	static maf_int32 StrLen(const maf_int8* str);
	static maf_int32 Search(const maf_int8* str, const maf_int8* temp, maf_int32 posStart, maf_int32 posEnd, maf_bool dir = true);
	static maf_int32 Search(const maf_int8* str, const maf_int8* temp, maf_bool dir = true);
	static maf_int32 Cut(const maf_int8* str, const maf_int8* characteristic, maf_int8* out, maf_int32 outByteMax);
	static maf_int32 Cut(const maf_int8* str, const maf_int8* characteristic, maf_int8* out);
	static maf_int32 StrAppend(maf_int8* src, maf_int8* dst);

	static maf_bool StrCompare(const maf_int8* cmp0, const maf_int8* cmp1);
	static const maf_int8* BaseName(const maf_int8* str);
private:

};


#if 1
#define MAF_MEM_CPY(dst,src,size) MAF_String::MemCpy((maf_int8*)dst,(maf_int8*)src,(maf_int32)size)
#define MAF_MEM_SET(dst,val,size) MAF_String::MemSet((maf_int8*)dst,(maf_int32)val,(maf_int32)size)
#define MAF_MEM_MOVE(dst,src,size) MAF_String::MemMove((maf_int8*)dst,(maf_int8*)src,(maf_int32)size)
#else
#include<string.h>
#define MAF_MCM_CPY(dst,src,size) memcpy(dst,src,size)
#define MAF_MCM_SET(dst,val,size) memset(dst,val,size)
#define MAF_MCM_MOVE(dsr,src,size) memmove(dsr,src,size)
#endif



