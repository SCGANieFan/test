#pragma once
#include"MTF.Process.h"
class MTF_MusicPlc :public MTF_Process
{
public:
	MTF_MusicPlc();
	~MTF_MusicPlc();
public:
	virtual mtf_int32 receive(MTF_Data& iData) final;
	virtual mtf_int32 generate(MTF_Data*& oData) final;

	virtual mtf_int32 Set(const mtf_int8* key, mtf_void* val) final;
	virtual mtf_int32 Get(const mtf_int8* key, mtf_void* val) final;
private:
#if 1
	MTF_Data _iData;
	MTF_Data _oData;
#endif
private:


};

