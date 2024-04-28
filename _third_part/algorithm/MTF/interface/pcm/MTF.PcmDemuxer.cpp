#include <stdio.h>
#include "MTF.PcmDemuxer.h"
#include "MTF.String.h"
#include "MTF.Objects.h"

MTF_PcmDemuxer ::MTF_PcmDemuxer ()
{
	_pFile = 0;
	_url = 0;

	mtf_int32 size = 1024;
	_oData.Init((mtf_uint8*)MTF_MALLOC(size), size);
}
MTF_PcmDemuxer ::~MTF_PcmDemuxer ()
{
	if (_pFile)
		fclose((FILE*)_pFile);

	_oData.Used(_oData._size);
	MTF_FREE(_oData.Data());
}

void mtf_pcm_demuxer_register()
{
	MTF_Objects::Registe<MTF_PcmDemuxer>("pcm_demuxer");
}

mtf_int32 MTF_PcmDemuxer::generate(MTF_Data*& oData)
{
	mtf_int32 readedSize = fread(_oData.LeftData(), 1, _oData.LeftSize(), (FILE*)_pFile);
	if (readedSize <= 0)
	{
		if (_oData._size <= 0)
			return -1;
		_oData._flags &= MTF_DataFlag_ESO;
	}
	_oData._size += readedSize;
	oData = &_oData;
	return 0;
}


mtf_int32 MTF_PcmDemuxer ::Set(const mtf_int8* key, mtf_void* val)
{
	if (MTF_Srting::StrCompare(key, "url"))
	{
		MTF_PRINT("url,%s", (const mtf_int8*)val);
		_url = (const mtf_int8*)val;
		if (_pFile)
			fclose((FILE*)_pFile);
		_pFile = fopen(_url, "rb+");
		return 0;
	}
	return MTF_AudioDemuxer::Set(key, val);
}
mtf_int32 MTF_PcmDemuxer ::Get(const mtf_int8* key, mtf_void* val)
{
	return MTF_AudioDemuxer::Get(key, val);
}




