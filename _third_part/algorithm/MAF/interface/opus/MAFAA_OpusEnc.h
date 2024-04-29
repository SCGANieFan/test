#pragma once
#include "MAF.Audio.h"
class MAFAA_OpusEnc:public MAF_Audio
{
	typedef struct
	{
		maf_int32 fsHz;
		maf_int16 channels;
		maf_int32 width;
		maf_int32 frameSamples;
	}AudioInfo;

	typedef struct
	{
		maf_int32 sampling_rate_hz;
		maf_uint8 channels;
		maf_uint32 application;
		maf_uint32 bitrate_bps;
		maf_int32 bandwidth;
		maf_uint8 use_vbr;
		maf_uint8 cvbr;
		maf_uint8 complexity;
		maf_uint8 use_inbandfec;
		maf_int32 forcechannels;
		maf_uint8 use_dtx;
		//maf_uint8 packet_loss_perc;
		maf_uint8 lsb_depth;
		maf_uint32 variable_duration;
		maf_uint32 frame_duration_0p1ms;
	}EncInfo;



public:
	MAFAA_OpusEnc();
	~MAFAA_OpusEnc();
public:
	virtual maf_int32 Init() override;
	virtual maf_int32 Deinit() override;
	virtual maf_int32 Process(MAF_Data* dataIn, MAF_Data* dataOut) override;
private:
	maf_void *_hd;
	maf_int32 _hdSize;
	AudioInfo _audioInfo;
	EncInfo _encInfo;
};
