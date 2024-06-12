#include "MTF.Ape.h"
#include "MTF.String.h"
#include "MTF.Objects.h"
#include "MAF.h"

void mtf_ape_register()
{
	MTF_Objects::Registe<MTF_Ape>("ape");
	MAF_REGISTER(ape);
}
MTF_Ape::MTF_Ape()
{

}

MTF_Ape::~MTF_Ape()
{
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
	if (_hd)
	{
		MAF_Deinit(_hd);
		MTF_FREE(_hd);
	}
}

mtf_int32 MTF_Ape::Init()
{	
	//lib init
	const mtf_int8* type = "ape";
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
	(mtf_void*)_rate,
	(mtf_void*)_ch,
	(mtf_void*)_width,
	(mtf_void*)_frameSamples,
	};

	const mtf_int8* script = "type=$0,Malloc=$1,Realloc=$2,Calloc=$3,Free=$4"\
							 ",rate=$5,ch=$6,width=$7,fSamples=$8;";
	ret = MAF_Init(_hd, script, param);
	if (ret != MA_RET_SUCCESS)
		MTF_PRINT("err");

	//io data
	mtf_int32 size = _frameBytes;
	_iData.Init((mtf_uint8*)MTF_MALLOC(size), size);
	size = 10 * size;
	_oData.Init((mtf_uint8*)MTF_MALLOC(size), size);

	return 0;
}

mtf_int32 MTF_Ape::receive(MTF_Data& iData)
{
	_iData.Append(iData.Data(), iData._size);
	_iData._flags = iData._flags;
	iData.Used(iData._size);
	return 0;
}

#define FRAMES_LOST 5
#define FRAMES_TOTAL 50
mtf_int32 MTF_Ape::generate(MTF_Data*& oData)
{
	AA_Data AA_iData;
	MTF_MEM_SET(&AA_iData, 0, sizeof(AA_Data));
	AA_iData.buff = _iData.Data();
	AA_iData.max = AA_iData.size = _iData._size;
	if (_iData._flags & MTF_DataFlag_ESO)
		AA_iData.flags = AA_DataFlag_FRAME_IS_EOS;
	_frames++;

	AA_Data AA_oData;
	MTF_MEM_SET(&AA_oData, 0, sizeof(AA_Data));
	AA_oData.buff = _oData.LeftData();
	AA_oData.max = _oData.LeftSize();

	MAF_Run(_hd, &AA_iData, &AA_oData);
	_iData.Used(_iData._size);
	_oData._size += AA_oData.size;

	//exit check
	if ((_iData._flags & MTF_DataFlag_ESO)
		&& _oData._size <= 0)
	{
		return -1;
	}
	oData = &_oData;
	return 0;
}

mtf_int32 MTF_Ape::Set(const mtf_int8* key, mtf_void* val)
{
#if 0
	if (MTF_String::StrCompare(key, "decayMs")) {
		_decayMs = (mtf_int16)val; return 0;
	}
	else if (MTF_String::StrCompare(key, "overlapMs")) {
		_overlapMs = (mtf_int16)val; return 0;
	}
#endif
	return MTF_AudioProcess::Set(key, val);
}
mtf_int32 MTF_Ape::Get(const mtf_int8* key, mtf_void* val)
{
	return MTF_AudioProcess::Get(key, val);
}
