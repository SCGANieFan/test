#include <stdio.h>
#include "MTF.WavMuxer.h"
#include "MTF.String.h"
#include "MTF.Objects.h"

void mtf_wav_muxer_register()
{
	MTF_Objects::Registe<MTF_WavMuxer>("wav_muxer");
}

MTF_WavMuxer::MTF_WavMuxer()
{

}

MTF_WavMuxer::~MTF_WavMuxer()
{
	if (_pFile)
		fclose((FILE*)_pFile);
	if (_iData.Data())
	{
		_iData.Used(_iData._size);
		MTF_FREE(_iData.Data());
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

	mtf_int32 size = _frameBytes;
	_iData.Init((mtf_uint8*)MTF_MALLOC(size), size);
	return 0;
}

mtf_int32 MTF_WavMuxer::receive(MTF_Data& iData)
{
	fwrite(iData.Data(), 1, iData._size, (FILE*)_pFile);
	iData.Used(iData._size);
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



