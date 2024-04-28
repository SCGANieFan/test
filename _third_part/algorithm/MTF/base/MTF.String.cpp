#include "MTF.String.h"
#include<string.h>
MTF_Srting::MTF_Srting()
{
}
MTF_Srting::~MTF_Srting()
{
}

void* MTF_Srting::MemCpy(mtf_int8* dst, mtf_int8* src, mtf_int32 size)
{
	return memcpy(dst, src, size);
}
void* MTF_Srting::MemSet(mtf_int8* dst, mtf_int32 val, mtf_int32 size)
{
	return memset(dst, val, size);
}
void* MTF_Srting::MemMove(mtf_int8* dst, mtf_int8* src, mtf_int32 size)
{
	return memmove(dst, src, size);
}


mtf_int32 MTF_Srting::StrLen(const mtf_int8* str)
{
	mtf_int32 strLen = 0;
	while (*str++ != '\0')
		strLen++;
	return strLen;
}

mtf_int32 MTF_Srting::Search(const mtf_int8* str, const mtf_int8* temp, mtf_int32 posStart, mtf_int32 posEnd, mtf_bool dir)
{
	str += posStart;
	mtf_int32 strLen = posEnd - posStart + 1;
	mtf_int32 tempLen = StrLen(temp);
	mtf_int32 pos = -1;
	if (dir == true)//left -> right
	{
		for (mtf_int32 sl = 0; sl <= strLen; sl++)
		{
			if (str[sl] == temp[0])
			{
				if ((strLen - sl + 1) < tempLen)
				{
					pos = -1;
					break;
				}
				mtf_int32 off = 1;
				for (off = 1; off < tempLen; off++)
				{
					if (str[sl + off] != temp[off])
					{
						break;
					}
				}
				if (off == tempLen)
				{
					pos = sl;
					break;
				}
			}
		}
	}
	else
	{
		for (mtf_int32 sl = strLen-1; sl >= 0; sl--)
		{
			if (str[sl] == temp[tempLen-1])
			{
				if ((sl + 1) < tempLen)
				{
					pos = -1;
					break;
				}
				mtf_int32 off = tempLen - 2;
				for (off = tempLen-2; off >= 0; off--)
				{
					if (str[sl + off - (tempLen - 1)] != temp[off])
					{
						break;
					}
				}
				if (off == -1)
				{
					pos = sl - (tempLen - 1);
					break;
				}
			}
		}
	}
	return pos + posStart;
}

mtf_int32 MTF_Srting::Search(const mtf_int8* str, const mtf_int8* temp, mtf_bool dir)
{

	mtf_int32 strLen = StrLen(str);
	return Search(str, temp, 0, strLen, dir);
}


mtf_int32 MTF_Srting::Cut(const mtf_int8* str, const mtf_int8* characteristic, mtf_int8* out, mtf_int32 outByteMax)
{
	mtf_int32	pos = Search(str, characteristic);
	if (pos < 0)
		return -1;
	mtf_int32 outByte = pos;
	outByte = outByte > outByteMax ? outByteMax : outByte;
	MTF_MCM_CPY(out, (mtf_int8*)str, outByte);
	return pos;
}
mtf_int32 MTF_Srting::Cut(const mtf_int8* str, const mtf_int8* characteristic, mtf_int8* out)
{
	return Cut(str, characteristic, out, 1024);
}

mtf_int32 MTF_Srting::StrAppend(mtf_int8* src, mtf_int8* dst)
{
	//for
	return 0;
}

bool MTF_Srting::StrCompare(const mtf_int8* cmp0, const mtf_int8* cmp1)
{
	
	mtf_int32 len0 = StrLen(cmp0);
	mtf_int32 len1 = StrLen(cmp1);
	mtf_int32 lenMin = len0 > len1 ? len1 : len0;
	for (mtf_int32 i = 0; i < lenMin; i++)
	{
		if (*cmp0++ != *cmp1++)
			return false;
	}
	return true;
}



