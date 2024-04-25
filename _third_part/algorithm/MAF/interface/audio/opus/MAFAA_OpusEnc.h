#pragma once
#include "MAFA_Audio.h"
class MAFAA_OpusEnc:public MAFA_Audio
{
	typedef struct
	{
		int32_t fsHz;
		int16_t channels;
		int32_t width;
		int32_t frameSamples;
	}AudioInfo;

	typedef struct
	{
		int32_t sampling_rate_hz;
		uint8_t channels;
		uint32_t application;
		uint32_t bitrate_bps;
		int32_t bandwidth;
		uint8_t use_vbr;
		uint8_t cvbr;
		uint8_t complexity;
		uint8_t use_inbandfec;
		int32_t forcechannels;
		uint8_t use_dtx;
		//uint8_t packet_loss_perc;
		uint8_t lsb_depth;
		uint32_t variable_duration;
		uint32_t frame_duration_0p1ms;
	}EncInfo;



public:
	MAFAA_OpusEnc();
	~MAFAA_OpusEnc();
public:
	virtual int32_t Init(void* param) override;
	virtual int32_t Deinit() override;
	virtual int32_t Process(MAFA_Frame* frameIn, MAFA_Frame* frameOut) override;
private:
	void *_hd;
	int32_t _hdSize;
	AudioInfo _audioInfo;
	EncInfo _encInfo;
};
