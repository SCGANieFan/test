#include <stdio.h>
#include "MTF.ApeDemux.h"
#include "MTF.String.h"
#include "MTF.Objects.h"
#include "MAF.h"

#define MIN(a,b) (a)<(b)?(a):(b)

mtf_void mtf_ape_demux_register()
{
	MTF_Objects::Registe<MTF_ApeDemux>("ape_demux");
	MAF_REGISTER(ape_demux);
}
MTF_ApeDemux::MTF_ApeDemux()
{

}

MTF_ApeDemux::~MTF_ApeDemux()
{
	if (_oData.Data())
	{
		_oData.Used(_oData._size);
		MTF_FREE(_oData.Data());
	}
	if (_hd)
	{
		MAF_Deinit(_hd);
		MTF_FREE(_hd);
	}
	if (_pFile)
		fclose((FILE*)_pFile);
	if(_seekTableManger._seektableRead)
		MTF_FREE(_seekTableManger._seektableRead);
}

mtf_int32 MTF_ApeDemux::Init()
{	
	MTF_PRINT();
	if (!_url) {
		MTF_PRINT("error, _url = 0");
		return -1;
	}
	_pFile = fopen(_url, "rb+");
	if (!_pFile) {
		MTF_PRINT("error, no such file:%s", _url);
		return -1;
	}

	//lib init
	const mtf_int8* type = "ape_demux";
	MA_Ret ret;
	ret = MAF_GetHandleSize(type, &_hdSize);
	if (ret != MA_RET_SUCCESS)
		MTF_PRINT("err");
	if (_hdSize < 1)
		MTF_PRINT("err");
	_hd = MTF_MALLOC(_hdSize);
	if (!_hd)
		MTF_PRINT("err");

	mtf_void* param[] = {
	(mtf_void*)type,
	(mtf_void*)MTF_Memory::Malloc,
	(mtf_void*)MTF_Memory::Realloc,
	(mtf_void*)MTF_Memory::Calloc,
	(mtf_void*)MTF_Memory::Free,
	};

	const mtf_int8* script = "type=$0,Malloc=$1,Realloc=$2,Calloc=$3,Free=$4;";
	ret = MAF_Init(_hd, script, param);
	if (ret != MA_RET_SUCCESS)
		MTF_PRINT("err");

#if 1
	MTF_MEM_SET(&_extraInfo, 0, sizeof(ExtraInfo_t));
	//_extraInfo.startFrame = 1;
	mtf_int32 readDataMaxLen = 2*1024;
	mtf_int32 readDataLen = 0;
	mtf_uint8* readData = (mtf_uint8*)MTF_MALLOC(readDataMaxLen);
#if 1
	while (1){
		//MAF_Get(_hd, "apeHeader", (mtf_void**)&_extraInfo.apeHeader)
		mtf_int32 readedSize = fread(readData + readDataLen, 1, 512, (FILE*)_pFile);
		readDataLen += readedSize;
		AA_Data AA_iData;
		MTF_MEM_SET(&AA_iData, 0, sizeof(AA_Data));
		AA_iData.buff = readData;
		AA_iData.size = readDataLen;
		MAF_Run(_hd, &AA_iData, 0);
		MAF_Get(_hd, "apeHeader", (mtf_void**)&_extraInfo.apeHeader);
		if (_extraInfo.apeHeader) {
			break;
		}
	}
#if 0
	MAF_Get(_hd, "seekTablePos", (mtf_void**)&_seekTableManger._seekTablePos);
	MAF_Get(_hd, "seekTableBytes", (mtf_void**)&_seekTableManger._seekTableSizeByte);
#else
	mtf_void *param0[]={&_seekTableManger._seekTablePos,&_seekTableManger._seekTableSizeByte};
	MAF_Get(_hd, "seekTable", (mtf_void**)param0);
#endif	
	_seekTableManger._seekTableNum = _seekTableManger._seekTableSizeByte >> 2;
	const uint32_t tableNumTmp = 2;
	_seekTableManger._seektableReadSizeByte = tableNumTmp * 4;
	_seekTableManger._seektableRead = (mtf_uint32*)MTF_MALLOC(readDataMaxLen);
	//fseek((FILE*)_pFile, _seekTableManger._seekTablePos + _seekTableManger._seekTableUsedSizeByte, SEEK_SET);
	fseek((FILE*)_pFile, _seekTableManger._seekTablePos, SEEK_SET);
	mtf_uint32 _startPosTmp;
	while (1) {
		_seekTableManger._seektableReadValidSizeByte = _seekTableManger._seekTableSizeByte - _seekTableManger._seekTableUsedSizeByte;
		_seekTableManger._seektableReadValidSizeByte = MIN(_seekTableManger._seektableReadValidSizeByte, _seekTableManger._seektableReadSizeByte);
		fread(_seekTableManger._seektableRead, 1, _seekTableManger._seektableReadValidSizeByte, (FILE*)_pFile);
#if 0
		MAF_Set(_hd, "seekTable", (mtf_void**)&_seekTableManger._seektableRead);
		MAF_Set(_hd, "seekTableBytes", (mtf_void**)&_seekTableManger._seektableReadValidSizeByte);
#else
		mtf_void *param1[]={&_seekTableManger._seektableRead,&_seekTableManger._seektableReadValidSizeByte};
		MAF_Set(_hd, "seekTable", (mtf_void**)param1);
#endif
		_startPosTmp = _startPos;
		mtf_uint32 seekTableNumOffset;
		mtf_uint32 skip;
		mtf_void* param2[3] = { &_startPosTmp, &seekTableNumOffset ,&skip };
		if (MAF_Get(_hd, "startInfoFromPos", (mtf_void**)param2)== MA_RET_SUCCESS){
			_startPos = _startPosTmp;
			_extraInfo.startFrame += seekTableNumOffset;
			_extraInfo.skip = skip;
			MTF_PRINT("startFrame:%d, startPos:%d,_startSkip:%d", _extraInfo.startFrame, _startPos, _extraInfo.skip);
			break;
		}
		_extraInfo.startFrame += tableNumTmp;
	}

#else
	while (1)
	{
		
		mtf_int32 readedSize = fread(readData + readDataLen, 1, 512, (FILE*)_pFile);
		readDataLen += readedSize;
		AA_Data AA_iData;
		MTF_MEM_SET(&AA_iData, 0, sizeof(AA_Data));
		AA_iData.buff = readData;
		AA_iData.size = readDataLen;
		MAF_Run(_hd, &AA_iData, 0);
		MAF_Get(_hd, "apeHeader", (mtf_void**) &_extraInfo.apeHeader);
		if (_extraInfo.apeHeader){
			_startPos = _extraInfo.startFrame;
			MAF_Get(_hd, "startPosFromFrame", (mtf_void**)&_startPos); 
			_extraInfo.skip = _extraInfo.startFrame;
			MAF_Get(_hd, "startSkip", (mtf_void**)&_extraInfo.skip);
			MTF_PRINT("startFrame:%d, startPos:%d,_startSkip:%d", _extraInfo.startFrame, _startPos, _extraInfo.skip);
			fseek((FILE*)_pFile, _startPos, SEEK_SET);
			break;
		}

	}	
#endif
	_extraDataLen = sizeof(sizeof(ExtraInfo_t));
	_isFirstFrame = true;
#endif
#if 0
	mtf_uint32 rate;
	mtf_uint32 ch;
	mtf_uint32 width;
	MAF_Get(_hd, "rate", (mtf_void**)&rate);
	MAF_Get(_hd, "ch", (mtf_void**)&ch);
	MAF_Get(_hd, "width", (mtf_void**)&width);
#else
	mtf_uint32 rate;
	mtf_uint32 ch;
	mtf_uint32 width;
	mtf_void* param3[3] = { &rate,&ch, &width };
	MAF_Get(_hd, "audioInfo", (mtf_void**)param3);
#endif
	MTF_AudioDemuxer::Set("rate", (mtf_void*)rate);
	MTF_AudioDemuxer::Set("ch", (mtf_void*)ch);
	MTF_AudioDemuxer::Set("width", (mtf_void*)width);
	MTF_AudioDemuxer::Set("fMs", (mtf_void*)20);

	//io data
	mtf_int32 size = _frameBytes;
	_oData.Init((mtf_uint8*)MTF_MALLOC(size), size);

	return 0;
}

mtf_int32 MTF_ApeDemux::generate(MTF_Data*& oData)
{
	if (!(_oData._flags & MTF_DataFlag_ESO))
	{

		if (_isFirstFrame)
		{
			_isFirstFrame = false;
			_oData._flags |= MTF_DataFlag_EXTRA_INFO;
			MTF_MEM_CPY(_oData.LeftData(), &_extraInfo, sizeof(ExtraInfo_t));
			_oData._size += sizeof(ExtraInfo_t);
			oData = &_oData;
			return 0;
		}

		mtf_int32 readedSize = fread(_oData.LeftData(), 1, _oData.LeftSize(), (FILE*)_pFile);
		if (readedSize < _oData.LeftSize()) {
			_oData._flags |= MTF_DataFlag_ESO;
		}
		_oData._size += readedSize;
	}
	oData = &_oData;
	return 0;
}

mtf_int32 MTF_ApeDemux::Set(const mtf_int8* key, mtf_void* val)
{
	if (MTF_String::StrCompare(key, "url"))
	{
		MTF_PRINT("url,%s", (const mtf_int8*)val);
		_url = (const mtf_int8*)val;

		return 0;
	}
	return MTF_AudioDemuxer::Set(key, val);
}
mtf_int32 MTF_ApeDemux::Get(const mtf_int8* key, mtf_void* val)
{
	return MTF_AudioDemuxer::Get(key, val);
}
