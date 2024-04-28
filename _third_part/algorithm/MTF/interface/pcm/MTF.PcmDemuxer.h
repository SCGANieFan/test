#pragma once
#include"MTF.AudioDemuxer.h"
class MTF_PcmDemuxer :public MTF_AudioDemuxer
{
public:
	MTF_PcmDemuxer();
	~MTF_PcmDemuxer();
public:
	virtual mtf_int32 generate(MTF_Data*& oData) final;

	virtual mtf_int32 Set(const mtf_int8* key, mtf_void* val) final;
	virtual mtf_int32 Get(const mtf_int8* key, mtf_void* val) final;
private:

private:
	void* _pFile;
	const mtf_int8* _url;
private:
	MTF_Data _oData;
};

