#pragma once

#include<stdint.h>

#define DataRet_Sucess 0
#define DataRet_Fail -1
#define DataRet_InputErr -2

typedef int32_t DataRet;

typedef enum{
	//set
	DataRunChoose_SetFlag = 0,
	//get	
	DataRunChoose_GetBuf,
	DataRunChoose_GetMax,
	DataRunChoose_GetData,
	DataRunChoose_GetSize,
	DataRunChoose_GetLeftData,
	DataRunChoose_GetLeftSize,
	DataRunChoose_GetFlags,

	//
	DataRunChoose_Init,
	DataRunChoose_Append,
	DataRunChoose_AppendSize,
	DataRunChoose_Used,
	DataRunChoose_ClearUsed,
	DataRunChoose_CheckFlag,
	DataRunChoose_ClearFlag,

	//end
	DataRunChoose_MAX,
}DataRunChoose_e;

typedef enum{
	//set
	DataFlags_NONE = (uint32_t)1<<0,
}DataFlags_e;

DataRet DataGetSize(int32_t* hdSize);
DataRet DataRun(void* data, DataRunChoose_e choose, void **val);



