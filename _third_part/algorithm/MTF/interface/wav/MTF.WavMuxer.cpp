#include<stdio.h>
#include"MTF.WavMuxer.h"
#include"MTF.Objects.h"
#include"MAF.h"

void mtf_wav_muxer_register()
{
	MTF_Objects::Registe<MTF_WavMuxer>("wav_muxer");
	MAF_REGISTER(wav_mux);
}


MTF_WavMuxer::MTF_WavMuxer()
{

}

MTF_WavMuxer::~MTF_WavMuxer()
{

	void* param[1];
	param[0] = (void*)_rate;
	MAF_Set(_hd, "rate", param);
	param[0] = (void*)_ch;
	MAF_Set(_hd, "ch", param);
	param[0] = (void*)_width;
	MAF_Set(_hd, "width", param);
	param[0] = 0;
	MAF_Set(_hd, "basicInfo", param);

	param[0] = _head.Data();
	MAF_Get(_hd, "headInfo", param);

	fseek((FILE*)_pFile, 0, SEEK_SET);
	fwrite(_head.Data(), _head._size, 1, (FILE*)_pFile);
	if(_hd)
		MTF_FREE(_hd);
	if (_pFile)
		fclose((FILE*)_pFile);
	if (_iData.Data())
	{
		_iData.Used(_iData._size);
		MTF_FREE(_iData.Data());
	}
	if (_oData.Data())
	{
		_oData.Used(_oData._size);
		MTF_FREE(_oData.Data());
	}
}


mtf_int32 MTF_WavMuxer::Init()
{
	MTF_PRINT();
	if (!_url) {
		MTF_PRINT("error, _url = 0");
		return -1;
	}
	_pFile = fopen(_url, "wb+");
	if (!_pFile) {
		MTF_PRINT("error, no such file:%s", _url);
		return -1;
	}

	mtf_int32 size = 1024;
	_iData.Init((mtf_uint8*)MTF_MALLOC(size), size);
	_oData.Init((mtf_uint8*)MTF_MALLOC(size), size);
	
#if 1
	const mtf_int8* type = "wav_mux";
	MA_Ret ret;
	ret = MAF_GetHandleSize(type, &_hdSize);
	if (ret != MA_RET_SUCCESS)
		MTF_PRINT("err");
	if (_hdSize < 1)
		MTF_PRINT("err");
	_hd = MTF_MALLOC(_hdSize);
	if (!_hd)
		MTF_PRINT("err");
#if 1
	mtf_void* param[] = {
	(mtf_void*)type,
	(mtf_void*)MTF_Memory::Malloc,
	(mtf_void*)MTF_Memory::Realloc,
	(mtf_void*)MTF_Memory::Calloc,
	(mtf_void*)MTF_Memory::Free,
	};

	const mtf_int8* script = "type=$0,Malloc=$1,Realloc=$2,Calloc=$3,Free=$4"\
		";";
	ret = MAF_Init(_hd, script, param);
	if (ret != MA_RET_SUCCESS)
		MTF_PRINT("err");
	//void* param[1];
	mtf_int32 headSize;
	param[0] = &headSize;
	MAF_Get(_hd, "headSize", param);
	mtf_uint8* buf = (mtf_uint8*)MTF_MALLOC(headSize);
	_head.Init(buf, headSize);
	_head._size = headSize;
	fwrite(_head.Data(), _head._size, 1, (FILE*)_pFile);
#endif
#endif
	return 0;
}

mtf_int32 MTF_WavMuxer::receive(MTF_Data& iData)
{
	AA_Data AA_iData;
	MTF_MEM_SET(&AA_iData, 0, sizeof(AA_Data));

	AA_iData.buff = iData.Data();
	AA_iData.max = AA_iData.size = iData._size;

	AA_Data AA_oData;
	MTF_MEM_SET(&AA_oData, 0, sizeof(AA_Data));
	AA_oData.buff = _oData.LeftData();
	AA_oData.max = _oData.LeftSize();

	MAF_Run(_hd, &AA_iData, &AA_oData);

	iData.Used(iData._size);
	_oData._size += AA_oData.size;

#if 1
	fwrite(_oData.Data(), 1, _oData._size, (FILE*)_pFile);
	_oData.Used(_oData._size);
#endif
	return 0;
}

mtf_int32 MTF_WavMuxer::Set(const mtf_int8* key, mtf_void* val)
{
	if (MTF_String::StrCompare(key, "url"))
	{
		MTF_PRINT("url,%s", (const mtf_int8*)val);
		_url = (const mtf_int8*)val;
		return 0;
	}
	return MTF_Sink::Set(key, val);
}
mtf_int32 MTF_WavMuxer::Get(const mtf_int8* key, mtf_void* val)
{
	return MTF_Sink::Get(key, val);
}



