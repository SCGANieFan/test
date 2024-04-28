#include <stdio.h>
#include "MTF.PcmMuxer.h"
#include "MTF.String.h"
#include "MTF.Objects.h"


MTF_PcmMuxer::MTF_PcmMuxer()
{
	_pFile = 0;
	_url = 0;
	mtf_int32 size = 1024;
	_iData.Init((mtf_uint8*)MTF_MALLOC(size), size);
}
MTF_PcmMuxer::~MTF_PcmMuxer()
{
	if (_pFile)
		fclose((FILE*)_pFile);
	_iData.Used(_iData._size);
	MTF_FREE(_iData.Data());
}

void mtf_pcm_muxer_register()
{
	MTF_Objects::Registe<MTF_PcmMuxer>("pcm_muxer");
}

mtf_int32 MTF_PcmMuxer::receive(MTF_Data& iData)
{
	fwrite(iData.Data(), 1, iData._size, (FILE*)_pFile);
	iData.Used(iData._size);
	return 0;
}

mtf_int32 MTF_PcmMuxer::Set(const mtf_int8* key, mtf_void* val)
{
	if (MTF_Srting::StrCompare(key, "url"))
	{
		MTF_PRINT("url,%s", (const mtf_int8*)val);
		_url = (const mtf_int8*)val;
		if (_pFile)
			fclose((FILE*)_pFile);
		_pFile = fopen(_url, "wb+");
		return 0;
}
	return MTF_Sink::Set(key, val);
}
mtf_int32 MTF_PcmMuxer::Get(const mtf_int8* key, mtf_void* val)
{
	return MTF_Sink::Get(key, val);
}



