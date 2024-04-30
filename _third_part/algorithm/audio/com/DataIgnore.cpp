#include<stdlib.h>
#include<string.h>
#include"Data.h"

#define DEBUG
#define STATIC_INLINE static inline
typedef struct
{
	uint8_t* _buff;
	int32_t _off;
	int32_t _size;
	int32_t _max;
	int32_t _flags;
}Data;

STATIC_INLINE DataRet SetFlag(Data *data, void *flags) {
	data->_flags |= (uint32_t)flags;
	return DataRet_Sucess;
}

STATIC_INLINE DataRet GetBuf(Data *data, void *buff) {
	*(uint32_t*)buff = (uint32_t)data->_buff;
	return DataRet_Sucess;
}

STATIC_INLINE DataRet GetMax(Data *data, void *max) {
	*(uint32_t*)max = (uint32_t)data->_max;
	return DataRet_Sucess;
}

STATIC_INLINE DataRet GetData(Data *data, void *dataBuf) {
	*(uint32_t*)dataBuf = (uint32_t)(data->_buff + data->_off);
	return DataRet_Sucess;
}

STATIC_INLINE DataRet GetSize(Data *data, void *size) {
	*(uint32_t*)size = (uint32_t)data->_size;
	return DataRet_Sucess;
}

STATIC_INLINE DataRet GetLeftData(Data *data, void *leftData) {
	*(uint32_t*)leftData = (uint32_t)(data->_buff);
	return DataRet_Sucess;
}

STATIC_INLINE DataRet GetLeftSize(Data* data, void* leftSize) {
	*(uint32_t*)leftSize = (uint32_t)(data->_max - data->_off - data->_size);
	return DataRet_Sucess;
}

STATIC_INLINE DataRet GetFlags(Data* data, void* flags) {
	*(uint32_t*)flags = (uint32_t)(data->_flags);
	return DataRet_Sucess;
}

//
STATIC_INLINE DataRet Init(Data *data, void *buff, void* max) {
	memset(data, 0, sizeof(Data));
	data->_buff = (uint8_t*)buff;
	data->_max = (uint32_t)max;
	return DataRet_Sucess;
}

STATIC_INLINE DataRet Append(Data* data, void* buff, void* size0) {
	uint8_t* iBuf = (uint8_t*)buff;
	int32_t size = (int32_t)size0;
	int32_t remSize = data->_max - data->_off - data->_size;
#ifdef  DEBUG
	if (!iBuf
		|| size < 0
		|| remSize < size)
		return DataRet_InputErr;
#endif
	memcpy(data->_buff + data->_off + data->_size, iBuf, size);
	data->_size += size;

	return DataRet_Sucess;
}

STATIC_INLINE DataRet AppendSize(Data* data, void* size0) {
	int32_t size = (int32_t)size0;
	int32_t remSize = data->_max - data->_off - data->_size;
#ifdef  DEBUG
	if (size < 0
		|| remSize < size)
		return DataRet_InputErr;
#endif
	data->_size += size;
	return DataRet_Sucess;
}

STATIC_INLINE DataRet Used(Data* data, void* size0) {
	int32_t size = (int32_t)size0;
#ifdef  DEBUG
	if (size < 0
		|| data->_size < size)
		return DataRet_InputErr;
#endif
	data->_off += size;
	data->_size -= size;
	return DataRet_Sucess;
}

STATIC_INLINE DataRet ClearUsed(Data* data) {
	if (data->_size > 0
		&& data->_off > 0)
	{
		memmove(data->_buff, data->_buff + data->_off, data->_size);
	}
	data->_off = 0;
	return DataRet_Sucess;
}

STATIC_INLINE DataRet CheckFlag(Data* data, void* flag0, void* ret) {
	uint32_t flag = (uint32_t)flag0;
	*(bool*)ret = false;
	if ((bool)(data->_flags &= flag))
		*(bool*)ret = true;
	return DataRet_Sucess;
}

STATIC_INLINE DataRet ClearFlag(Data* data, void* flag0) {
	uint32_t flag = (uint32_t)flag0;
	data->_flags &= ~flag;
	return DataRet_Sucess;
}

DataRet DataGetSize(int32_t* hdSize) {
	if(!hdSize)
		return DataRet_InputErr;
	*hdSize = sizeof(Data);
	return DataRet_Sucess;
}

DataRet DataRun(void* data, DataRunChoose_e choose, void **val)
{
	//check
#ifdef  DEBUG
	if(!data
	||choose > DataRunChoose_MAX)
		return DataRet_InputErr;

	if(!val)
		if(choose!= DataRunChoose_ClearUsed)
			return DataRet_InputErr;
#endif
	//
	Data *pData = (Data *)data;
	switch (choose)
	{
	//set
	case DataRunChoose_SetFlag:
		return SetFlag(pData, val[0]);
	//get
	case DataRunChoose_GetBuf:
		return GetBuf(pData, val[0]);
	case DataRunChoose_GetMax:
		return GetMax(pData, val[0]);
	case DataRunChoose_GetData:
		return GetData(pData, val[0]);
	case DataRunChoose_GetSize:
		return GetSize(pData, val[0]);
	case DataRunChoose_GetLeftData:
		return GetLeftData(pData, val[0]);
	case DataRunChoose_GetLeftSize:
		return GetLeftSize(pData, val[0]);
	case DataRunChoose_GetFlags:
		return GetFlags(pData, val[0]);
	//
	case DataRunChoose_Init:
		return Init(pData, val[0], val[1]);
	case DataRunChoose_Append:
		return Append(pData, val[0], val[1]);
	case DataRunChoose_AppendSize:
		return AppendSize(pData, val[0]);
	case DataRunChoose_Used:
		return Used(pData, val[0]);
	case DataRunChoose_ClearUsed:
		return ClearUsed(pData);
	case DataRunChoose_CheckFlag:
		return CheckFlag(pData, val[0], val[0]);
	case DataRunChoose_ClearFlag:
		return ClearFlag(pData, val[0]);
	}
	return DataRet_Sucess;
}



