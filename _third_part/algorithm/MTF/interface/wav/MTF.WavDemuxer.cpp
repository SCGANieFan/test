#include<stdio.h>
#include"MTF.WavDemuxer.h"
#include"MTF.Objects.h"
#include"MAF.h"

void mtf_wav_demuxer_register()
{
	MTF_Objects::Registe<MTF_WavDemuxer>("wav_demuxer");
	MAF_REGISTER(wav_demux);
}	


MTF_WavDemuxer ::MTF_WavDemuxer ()
{

}

MTF_WavDemuxer ::~MTF_WavDemuxer ()
{
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

mtf_int32 MTF_WavDemuxer::Init()
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
	//mtf_int32 size = _frameBytes;
	mtf_int32 size = 1024;
	_iData.Init((mtf_uint8*)MTF_MALLOC(size), size);
	_oData.Init((mtf_uint8*)MTF_MALLOC(size), size);
	
#if 1
	const mtf_int8* type = "wav_demux";
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

#endif
#endif
	return 0;
}


mtf_int32 MTF_WavDemuxer::generate(MTF_Data*& oData)
{
#if 1
	mtf_int32 readedSize = fread(_iData.LeftData(), 1, _iData.LeftSize(), (FILE*)_pFile);
	if (readedSize <= 0)
	{
		if (_iData._size <= 0)
			return -1;
		_iData._flags &= MTF_DataFlag_ESO;
	}
	_iData._size += readedSize;
#if 0
	oData = &_iData;
	return 0;
#endif
#endif
#if 1
	AA_Data AA_iData;
	MTF_MEM_SET(&AA_iData, 0, sizeof(AA_Data));
	AA_iData.buff = _iData.Data();
	AA_iData.max = AA_iData.size = _iData._size;

	
	AA_Data AA_oData;
	MTF_MEM_SET(&AA_oData, 0, sizeof(AA_Data));
	AA_oData.buff = _oData.LeftData();
	AA_oData.max = _oData.LeftSize();

	MAF_Run(_hd, &AA_iData, &AA_oData);


	
	mtf_uint32 hasHead;
	void* param[1] = { &hasHead };
	MAF_Get(_hd, "hasHead", param);
	if (hasHead>0)
	{
		mtf_uint32 rate;
		param[0] = &rate;
		MAF_Get(_hd, "rate", param);
		Set("rate", (void*)rate);

		mtf_uint32 ch;
		param[0] = &ch;
		MAF_Get(_hd, "ch", param);
		Set("ch", (void*)ch);

		mtf_uint32 width;
		param[0] = &width;
		MAF_Get(_hd, "width", param);
		Set("width", (void*)width);
	}

	_iData.Used(_iData._size);
	_oData._size += AA_oData.size;

	oData = &_oData;
	return 0;
#endif
	return 0;
}


mtf_int32 MTF_WavDemuxer ::Set(const mtf_int8* key, mtf_void* val)
{
	if (MTF_String::StrCompare(key, "url"))
	{
		MTF_PRINT("url,%s", (const mtf_int8*)val);
		_url = (const mtf_int8*)val;

		return 0;
	}
	return MTF_AudioDemuxer::Set(key, val);
}
mtf_int32 MTF_WavDemuxer ::Get(const mtf_int8* key, mtf_void* val)
{
	return MTF_AudioDemuxer::Get(key, val);
}




