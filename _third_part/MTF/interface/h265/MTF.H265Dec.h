#pragma once
#include"MTF.VideoProcess.h"
class MTF_H265Dec :public MTF_VideoProcess
{
public:
	MTF_H265Dec();
	~MTF_H265Dec();
protected:
	virtual mtf_int32 Init() final;
	virtual mtf_int32 receive(MTF_Data& iData) final;
	virtual mtf_int32 generate(MTF_Data*& oData) final;
public:
	virtual mtf_int32 Set(const mtf_int8* key, mtf_void* val) final;
	virtual mtf_int32 Get(const mtf_int8* key, mtf_void* val) final;

private:
#if 0
	MTF_Data _iData;
#endif
	mtf_int32 _iFlags = 0;
	MTF_Data _oData;

	mtf_void* _hd = 0;
	mtf_int32 _hdSize = 0;

	mtf_int16 _decayMs = 0;
	mtf_int16 _gainMs = 0;
	mtf_int16 _overlapMs = 0;

	mtf_int32 _frames = 0;

};

