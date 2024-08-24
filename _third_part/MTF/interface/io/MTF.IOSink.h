#pragma once
#include"MTF.Sink.h"
class MTF_IOSink :public MTF_Sink
{
public:
	MTF_IOSink();
	~MTF_IOSink();
private:
	virtual mtf_int32 Init() final;
public:
	virtual mtf_int32 receive(MTF_Data& iData) final;
	virtual mtf_int32 Set(const mtf_int8* key, mtf_void* val) final;
	virtual mtf_int32 Get(const mtf_int8* key, mtf_void* val) final;
private:
	MTF_Data _iData;
private:
	void* _pFile = 0;
	const mtf_int8* _url = 0;
	mtf_int32 _iPortMax = 1*1024;
	mtf_int32 _iPortMin = 1*1024;
	
};

