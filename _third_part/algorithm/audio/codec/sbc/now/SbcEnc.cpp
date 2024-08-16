#include "SbcEnc.h"
#include "BitStream.h"
#include "SbcEnc.Interface.h"


b1 SbcEncoder_c::Init(u32 sampleRate, u8 channels, u8 bitPool, SbcChannelMode_e channelMode, SbcAllocMethod_e allocMethod, SbcBlocks_e blocks, SbcSubBandS_e subBands) {
    // alogmem
    ALGO_MEM_SET(this, 0, sizeof(SbcEncoder_c));
    SbcInfo_t info;
    ALGO_MEM_SET(&info, 0, sizeof(SbcInfo_t));
#if 1
    info.bitPool = bitPool;
    info.sampleRate = sampleRate;
    info.channelMode = channelMode;
    info.allocMethod = allocMethod;
    info.numBlocks = blocks;
    info.numSubBands = subBands;
    info.numChannels = channels;
    switch (info.sampleRate)
    {
    case 16000:info.sampleFreq = SbcSampleRate_e::SBC_SAMPLE_FREQ_16K; break;
    case 32000:info.sampleFreq = SbcSampleRate_e::SBC_SAMPLE_FREQ_32K; break;
    case 44100:info.sampleFreq = SbcSampleRate_e::SBC_SAMPLE_FREQ_44K1; break;
    case 48000:info.sampleFreq = SbcSampleRate_e::SBC_SAMPLE_FREQ_48K; break;
    default:
        break;
    }
#endif
    info.pcm_samples = subBands * blocks;
    info.pcm_size = info.pcm_samples * channels * 2;
    info.vlc_size = SbcCom_c::SbcGetVlcSize(channelMode, subBands, blocks, channels, bitPool);
    _streamInfo = info;
    return true;
}

i32 SbcEncoder_c::EncodeFrame(u8* in, i32 inLen, u8* out, i32 outLen)
{
    //check
    if (!in
        || inLen < _streamInfo.pcm_size
        || !out
        || outLen < _streamInfo.vlc_size)
        return SBC_RET_FAIL;
    
    BitStream_c bs;
    bs.Init(out, outLen);

    //1. generate header
    HeaderMakeX(&bs);
    i16* pcm0 = (i16*)in;
    i16* pcm1 = (i16*)in+1;
    //2. loop    
    uint8_t blockRem = _streamInfo.numBlocks;

    //2.1 how many _block shoul be encoded now, once max 16 _block and 8 subband
    _block = _streamInfo.numBlocks;

    //2.2 subband analysis
    if (4 == _streamInfo.numSubBands) {
        Filter4bandAnalysis(pcm0, pcm1);
    }
    else {
        Filter8bandAnalysis(pcm0, pcm1);
    }

    //2.3 sacle factors
    FactorsCalcX();

    //2.4 bit alloc
    switch (_streamInfo.channelMode)
    {
    case SBC_CHNL_MODE_MONO:
        SbcCom_c::BitAllocMono((SbcInfo_t*)&_streamInfo, _bits[0], _scaleFactors[0]);
        break;
    case SBC_CHNL_MODE_DUAL_CHNL:
        SbcCom_c::BitAllocMono((SbcInfo_t*)&_streamInfo, _bits[0], _scaleFactors[0]);
        SbcCom_c::BitAllocMono((SbcInfo_t*)&_streamInfo, _bits[1], _scaleFactors[1]);
        break;
    case SBC_CHNL_MODE_JOINT_STEREO:
    case SBC_CHNL_MODE_STEREO:
        SbcCom_c::BitAllocStereo((SbcInfo_t*)&_streamInfo, _bits, _scaleFactors);
        break;
    }
    //2.5 scale factors
    HeaderScaleFactorsMakeX(&bs);

    //2.6 sbSample quantization
    SamplesQuantX(&bs);

    //2.6 pcm ptr shift samples of (_block * subband)
    if (_streamInfo.numChannels == 1)
        pcm0 += _block * _streamInfo.numSubBands;
    else
    {
        pcm0 += 2 * _block * _streamInfo.numSubBands;
    }

    
    uint16_t Len = (bs.GetUsedBits() + 7) >> 3;
    bs.Flash();
    //3 generate crc check header
    SbcCom_c::CRCCalc(&_streamInfo, out);
    return Len;
}
