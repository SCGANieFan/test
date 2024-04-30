
#include<stdlib.h>

#include"MTF.Objects.h"
#include"MTF.Element.h"
#include"MTF.String.h"

#include"MultimeidaTest.h"
#include"MTF.Memory.h"





bool PraseElement(const char* str, int32_t strLen, MTF_Element**ele, void** param)
{

	const char* pStr = str;
	int32_t strLen0 = strLen;

	if (strLen0 < 3)
		return false;

	int32_t posStart;
	int32_t posEnd;

	posStart = MTF_String::Search(pStr, "|", 0, strLen0 - 1);
	if (posStart < 0)
		return false;
	*ele = (MTF_Element*)MTF_Objects::Create(pStr + posStart + 1);
	pStr += posStart + 1;
	strLen0 -= posStart + 1;

	while (1)
	{
		posStart = MTF_String::Search(pStr, ",", 0, strLen0 - 1);
		if (posStart < 0)
		{
			(*ele)->Init();
			return true;
		}

		pStr += posStart + 1;
		strLen0 -= posStart + 1;
		posStart = MTF_String::Search(pStr, "=", 0, strLen0 - 1);
		if (*(pStr + posStart + 1) == '$')
		{
			(*ele)->Set(pStr, param[*(pStr + posStart + 2)-48]);
		}
		pStr += posStart + 3;
		strLen0 -= posStart + 3;
	}
}



int32_t MultiemdiaTestInit()
{
	MTF_Memory::Init(malloc, realloc, calloc, free);
#if 0
	MTF_REGISTER(pcm_demuxer);
	MTF_REGISTER(music_plc);
	MTF_REGISTER(pcm_muxer);
#endif
	return 0;
}

MTF_Element* eles[10];

int32_t MultiemdiaApi(const char* str, void** param)
{
	const char* pStr = str;
	int32_t strLen = MTF_String::StrLen(pStr);
	int32_t posStart;
	int32_t posEnd;

	int32_t i = 0;
	while (1)
	{
		posStart = MTF_String::Search(pStr, "|", 0, strLen - 1);
		if (posStart < 0)
			break;
		posEnd = MTF_String::Search(pStr, "|", posStart + 1, strLen - 1);
		PraseElement(pStr + posStart, posEnd - posStart + 1, &eles[i], param);
		if (i > 0)
		{
			eles[i - 1]->Set("to", eles[i]);
			eles[i]->Set("from", eles[i-1]);
		}
		pStr += posEnd + 1;
		strLen -= posEnd + 1;
		i++;
	}
	while(eles[0]->Run()==0);
	return 0;
}


