#pragma once
#include"MTF.Sink.h"
class MTF_AudioMuxer :public MTF_Sink
{
public:
	MTF_AudioMuxer();
	~MTF_AudioMuxer();
public:
	virtual mtf_int32 receive(MTF_Data& iData) override;
	virtual mtf_int32 Set(const mtf_int8* key, mtf_void* val) override;
	virtual mtf_int32 Get(const mtf_int8* key, mtf_void* val) override;
private:
	
};

