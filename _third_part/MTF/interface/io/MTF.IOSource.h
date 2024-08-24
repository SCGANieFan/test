#pragma once
// #include"MTF.AudioDemuxer.h"
#include "MTF.Source.h"
class MTF_IOSource :public MTF_Source
{
public:
	MTF_IOSource();
	~MTF_IOSource();

private:
	virtual mtf_int32 Init() final;

public:
	virtual mtf_int32 generate(MTF_Data*& oData) final;
	virtual mtf_int32 Set(const mtf_int8* key, mtf_void* val) final;
	virtual mtf_int32 Get(const mtf_int8* key, mtf_void* val) final;

private:
	void* _pFile = 0;
	const mtf_int8* _url = 0;
	mtf_int32 _oPortMax = 1*1024;
	mtf_int32 _oPortMin = 1*1024;
private:
	MTF_Data _oData;
};

