#pragma once
#include"MTF.AudioProcess.h"



class MTF_SbcEnc :public MTF_AudioProcess
{
public:
	MTF_SbcEnc();
	~MTF_SbcEnc();
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



	mtf_int32 _frames = 0;

	mtf_uint8 _bitPool = 0;
	mtf_uint8 _blocks = 0;
	mtf_uint8 _subBands = 0;
	mtf_uint8 _channelMode = 0;
	mtf_uint8 _allocMethod = 0;
	mtf_uint8 _sbcMode = 0;


};

