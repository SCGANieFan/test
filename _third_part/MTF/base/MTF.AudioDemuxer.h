#pragma once
#include"MTF.Source.h"
#include"MTF.AudioInfo.h"
class MTF_AudioDemuxer :public MTF_Source, public MTF_AudioInfo
{
public:
	MTF_AudioDemuxer();
	~MTF_AudioDemuxer();
public:
	virtual mtf_int32 generate(MTF_Data*& oData) override;
	virtual mtf_int32 Set(const mtf_int8* key, mtf_void* val) override;
	virtual mtf_int32 Get(const mtf_int8* key, mtf_void* val) override;
private:
};

