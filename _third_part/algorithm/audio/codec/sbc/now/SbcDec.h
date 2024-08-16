#pragma once
#include "SbcCom.h"
#include"BitStream.h"
class SbcDec_c
{
public:
	SbcDec_c() {}
	~SbcDec_c() {}

private:
	int FrameSync(void* data0, int size);
	bool HeaderProcess(BitStream_c* bs);
	void JointSteroHeaderProcess(BitStream_c* bs);
	bool FactorsProcess(BitStream_c* bs);
	void SbsamplesMake(BitStream_c* bs);
	void JointProcess();
	void BandSynthFilter4(i16* ch0, i16* ch1);
	void BandSynthFilter8(i16 *ch0, i16* ch1);
public:

	int Run(void* input, int size, void* out, int outSize);
public:
	SbcInfo_t _streamInfo;
private:
	int32_t _VV[2][160 + 16 * 15] __ALIGN8__;
	uint8_t _scaleFactors[SBC_MAX_NUM_CHNL][SBC_MAX_NUM_SB];
	uint8_t _bits[SBC_MAX_NUM_CHNL][SBC_MAX_NUM_SB];
	int32_t _sbSample[SBC_MAX_NUM_BLK][SBC_MAX_NUM_CHNL][SBC_MAX_NUM_SB];
	uint32_t _joins;
	uint8_t _block;
};










