#include "MTF.MusicPlc.h"
#include "MTF.String.h"
#include "MTF.Objects.h"

MTF_MusicPlc::MTF_MusicPlc()
{
	mtf_int32 size = 1024;
	_iData.Init((mtf_uint8*)MTF_MALLOC(size), size);
	_oData.Init((mtf_uint8*)MTF_MALLOC(size), size);
}
MTF_MusicPlc::~MTF_MusicPlc()
{
	_iData.Used(_iData._size);
	MTF_FREE(_iData.Data());

	_oData.Used(_oData._size);
	MTF_FREE(_oData.Data());
}

void mtf_music_plc_register()
{
	MTF_Objects::Registe<MTF_MusicPlc>("music_plc");
}

mtf_int32 MTF_MusicPlc::receive(MTF_Data& iData)
{
	_iData.Append(iData.Data(), iData._size);
	iData.Used(iData._size);

	return 0;
}

mtf_int32 MTF_MusicPlc::generate(MTF_Data*& oData)
{
	_oData.Append(_iData.Data(), _iData._size);
	_iData.Used(_iData._size);
	oData = &_oData;
	return 0;
}

mtf_int32 MTF_MusicPlc::Set(const mtf_int8* key, mtf_void* val)
{
#if 0
	if (MTF_Srting::StrCompare(key, "from"))
	{
		_from = (MTF_Element*)val; return 0;
	}
	else if(MTF_Srting::StrCompare(key, "to"))
	{
		_to = (MTF_Element*)val; return 0;
	}
#endif
	return MTF_Process::Set(key, val);
}
mtf_int32 MTF_MusicPlc::Get(const mtf_int8* key, mtf_void* val)
{
	return MTF_Process::Get(key, val);
}
