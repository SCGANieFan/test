#include"SbcCom.h"
#include "BitStream.h"


enum SbcChannelMode_e {
	SBC_CHNL_MODE_MONO_E = 0,
	SBC_CHNL_MODE_DUAL_CHNL_E = 1,
	SBC_CHNL_MODE_STEREO_E = 2,
	SBC_CHNL_MODE_JOINT_STEREO_E = 3,
};

enum SbcAllocMethod_e {
	SBC_ALLOC_METHOD_LOUDNESS_E = 0,
	SBC_ALLOC_METHOD_SNR_E = 1,
};

enum SbcSubBandS_e {
	SBC_SUB_BANDS_4_E = 4,
	SBC_SUB_BANDS_8_E = 8,
};

enum SbcBlocks_e {
	SBC_BLOCKS_4_E = 4,
	SBC_BLOCKS_8_E = 8,
	SBC_BLOCKS_12_E = 12,
	SBC_BLOCKS_16_E = 16,
};

class SbcEncoder_c {
public:
	SbcEncoder_c() {}
	~SbcEncoder_c(){}
public:
	b1 Init(u32 sampleRate,u8 channels,u8 bitPool, SbcChannelMode_e channelMode, SbcAllocMethod_e allocMethod, SbcBlocks_e blocks, SbcSubBandS_e subBands);
	i32 EncodeFrame(u8* in, i32 inLen, u8* out, i32 outLen);
public:
	u16 GetPcmSize(){return _streamInfo.pcm_size;}
	u16 GetVlcSize(){return _streamInfo.vlc_size;}
private:
	void Filter4bandAnalysis(i16* pcm0, i16* pcm1);
	void Filter8bandAnalysis(i16* pcm0, i16* pcm1);
	void HeaderMakeX(BitStream_c* bs);
	void HeaderScaleFactorsMakeX(BitStream_c* bs);
	void SamplesQuantX(BitStream_c* bs);
	void FactorsCalcX();
private:
	SbcInfo_t _streamInfo;
	i16 _XX[2][8 * 16 + 72] __ALIGN8__;
	u8 _bits[SBC_MAX_NUM_CHNL][SBC_MAX_NUM_SB];
	u8 _scaleFactors[SBC_MAX_NUM_CHNL][SBC_MAX_NUM_SB];
	i32 _sbSample[SBC_MAX_NUM_BLK][SBC_MAX_NUM_CHNL][SBC_MAX_NUM_SB];
	i32 _as[SBC_MAX_NUM_CHNL][SBC_MAX_NUM_SB];
	i32 _bs[SBC_MAX_NUM_CHNL][SBC_MAX_NUM_SB];
	i32 _Y[16];
	u32  _joins;
	u8 _block;
	u8 _blockOld;
};


