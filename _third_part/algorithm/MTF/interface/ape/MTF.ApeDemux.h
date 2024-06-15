#pragma once
#include"MTF.AudioDemuxer.h"
class MTF_ApeDemux :public MTF_AudioDemuxer
{
public:
	MTF_ApeDemux();
	~MTF_ApeDemux();
protected:
	virtual mtf_int32 Init() final;
	//virtual mtf_int32 receive(MTF_Data& iData) final;
	virtual mtf_int32 generate(MTF_Data*& oData) final;
public:
	virtual mtf_int32 Set(const mtf_int8* key, mtf_void* val) final;
	virtual mtf_int32 Get(const mtf_int8* key, mtf_void* val) final;

private:
	typedef struct {
		mtf_void* apeHeader;
		mtf_uint32 startFrame;
	}ExtraInfo_t;
private:
	MTF_Data _oData;
	mtf_void* _hd = 0;
	mtf_int32 _hdSize = 0;
	mtf_int32 _frames = 0;
	mtf_uint32 _startPos = 0;	
	ExtraInfo_t _extraInfo;
	mtf_uint32 _extraDataLen = 0;
	mtf_bool _isFirstFrame = false;
private:
	void* _pFile = 0;
	const mtf_int8* _url = 0;
};

