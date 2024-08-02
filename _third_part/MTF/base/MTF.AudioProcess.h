#pragma once
#include"MTF.Process.h"
#include"MTF.AudioInfo.h"

class MTF_AudioProcess :public MTF_Process, public MTF_AudioInfo
{
public:
	MTF_AudioProcess();
	virtual ~MTF_AudioProcess();
protected:
	virtual mtf_int32 Init() = 0;
	virtual mtf_int32 receive(MTF_Data& iData) = 0;
	virtual mtf_int32 generate(MTF_Data*& oData) = 0;
public:
	virtual mtf_int32 Set(const mtf_int8* key, mtf_void* val) override;
	virtual mtf_int32 Get(const mtf_int8* key, mtf_void* val) override;
protected:
};

