#pragma once
#include"MTF.AudioProcess.h"
class MTF_OpusDec :public MTF_AudioProcess
{
public:
	MTF_OpusDec();
	~MTF_OpusDec();
protected:
	virtual mtf_int32 Init() final;
	virtual mtf_int32 receive(MTF_Data& iData) final;
	virtual mtf_int32 generate(MTF_Data*& oData) final;
public:
	virtual mtf_int32 Set(const mtf_int8* key, mtf_void* val) final;
	virtual mtf_int32 Get(const mtf_int8* key, mtf_void* val) final;

private:
	MTF_Data _iData;
	MTF_Data _oData;

	mtf_void* _hd = 0;
	mtf_int32 _hdSize = 0;
};

