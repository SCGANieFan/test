
#include "MAF.Objects.h"
#include "MAF.String.h"
#include "MAF.Algorithm.h"
#include "MAF.h"
#include<stdlib.h>

static maf_bool prase(maf_void* hd, const maf_int8* script, maf_void** param)
{
	const maf_int8* pStr = script;
	maf_int32 strLen = MAF_String::StrLen(pStr);
	maf_int32 strLen0 = strLen;

	if (strLen0 < 1)
		return false;

	maf_int32 posStart;
	maf_int32 posEnd;
	//type	
	posStart = MAF_String::Search(pStr, "type=", 0, strLen0 - 1);
	if (posStart < 0)
		return false;
	if (*(pStr + posStart + 5) == '$')
		MAF_Object::Create((const maf_int8*)param[*(pStr + posStart + 6) - 48], hd);
	pStr += posStart + 7;
	strLen0 -= posStart + 7;

	//param
	while (1)
	{
		posStart = MAF_String::Search(pStr, ",", 0, strLen0 - 1);
		if (posStart < 0)
		{
			((MAF_Algorithm*)hd)->Init();
			return true;
		}

		pStr += posStart + 1;
		strLen0 -= posStart + 1;
		posStart = MAF_String::Search(pStr, "=", 0, strLen0 - 1);

		if (*(pStr + posStart + 1) == '$')
		{
			((MAF_Algorithm*)hd)->Set(pStr, param[*(pStr + posStart + 2) - 48]);
		}
		pStr += posStart + 3;
		strLen0 -= posStart + 3;
	}
}

EXTERNC
{
	MA_Ret MAF_GetHandleSize(const char* type, int32_t * size)
	{
	//check
	if (!type
		|| !size)
		return MA_RET_INPUT_ERROR;
	maf_int32 ssize = MAF_Object::GetSize(type);
	if (ssize <= 0)
		return MA_RET_FAIL;
	*size = ssize;
	return MA_RET_SUCCESS;
}



MA_Ret MAF_Init(void* hd, const char* script, void** param)
{
	//check
	if (!hd
		|| !script
		|| !param)
		return MA_RET_INPUT_ERROR;

	prase(hd, script, param);
	return MA_RET_SUCCESS;
}
MA_Ret MAF_Deinit(void* hd)
{
	((MAF_Algorithm*)hd)->Deinit();
	((MAF_Algorithm*)hd)->~MAF_Algorithm();
	return MA_RET_SUCCESS;
}


MA_Ret MAF_Set(void* hd, const char* param)
{
	return MA_RET_SUCCESS;
}

MA_Ret MAF_Run(void* hd, AA_Data * dataIn, AA_Data * dataOut)
{
	MAF_Data iData;
	iData.Init(dataIn->buff + dataIn->off, dataIn->size);	
	iData.SetFlag(dataIn->flags);
		
	MAF_Data oData;
	oData.Init(dataOut->buff + dataOut->off + dataOut->size, 0, dataOut->max - dataOut->off - dataOut->size);

	((MAF_Algorithm*)hd)->Process(&iData, &oData);
	
	int32_t used = dataIn->size - iData.GetSize();
	dataIn->off += used;
	dataIn->size -= used;
	dataIn->flags = iData.GetFlags();

	dataOut->size += oData.GetSize();
	dataOut->flags = oData.GetFlags();

	return MA_RET_SUCCESS;
}
}
