#include <stdio.h>
#include "MTF.ApeDemux.h"
#include "MTF.String.h"
#include "MTF.Objects.h"
#include "MAF.h"

void mtf_ape_demux_register()
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
	_extraInfo.startFrame = 2;
	mtf_int32 readDataMaxLen = 2*1024;
	mtf_int32 readDataLen = 0;
	mtf_uint8* readData = (mtf_uint8*)MTF_MALLOC(readDataMaxLen);
	while (1)
	{
		
		mtf_int32 readedSize = fread(readData + readDataLen, 1, 512, (FILE*)_pFile);
		readDataLen += readedSize;
		AA_Data AA_iData;
		MTF_MEM_SET(&AA_iData, 0, sizeof(AA_Data));
		AA_iData.buff = readData;
		AA_iData.size = readDataLen;
		MAF_Run(_hd, &AA_iData, 0);
		MAF_Get(_hd, "apeHeader", (void**) &_extraInfo.apeHeader);
		if (_extraInfo.apeHeader){
			_startPos = _extraInfo.startFrame;
			MAF_Get(_hd, "startPosFromFrame", (void**)&_startPos);
			fseek((FILE*)_pFile, _startPos, SEEK_SET);
			MTF_PRINT("startFrame:%d, startPos:%d", _extraInfo.startFrame, _startPos);
			break;
		}

	}	
	_extraDataLen = sizeof(sizeof(ExtraInfo_t));
	_isFirstFrame = true;
#endif
	mtf_uint32 rate;
	mtf_uint32 ch;
	mtf_uint32 width;
	MAF_Get(_hd, "rate", (void**)&rate);
	MAF_Get(_hd, "ch", (void**)&ch);
	MAF_Get(_hd, "width", (void**)&width);

	MTF_AudioDemuxer::Set("rate", (void*)rate);
	MTF_AudioDemuxer::Set("ch", (void*)ch);
	MTF_AudioDemuxer::Set("width", (void*)width);
	MTF_AudioDemuxer::Set("fMs", (void*)20);

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
