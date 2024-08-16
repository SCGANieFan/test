#include"SbcDec.h"

int SbcDec_c::FrameSync(void* data0, int size) {
    uint8_t SyncWord = 0;
    uint8_t* data = (uint8_t*)data0;
    uint8_t* data1 = data + size;
    while (data < data1) {
        uint8_t v = *data++;
        if (v == SBC_SYNC_WORD) {
            SyncWord = SBC_SYNC_WORD;
            break;
        }
    }
    while (data < data1) {
        if (SyncWord != *data) {
            return data - (uint8_t*)data0;
        }
        data++;
    }
    // return size;
    return data - (uint8_t*)data0;
}

bool SbcDec_c::HeaderProcess(BitStream_c* bs) {
    const static uint8_t blks[] = { 4, 8, 12, 16 };
    const static uint8_t ablks[] = { 15, 20, 30, 60 };
    const static uint8_t chs[] = { 1, 2, 2, 2 };
    const static uint8_t sbs[] = { 4, 8 };
    const static uint32_t bpss[] = { 16000,32000,44100,48000 };
    const static uint32_t abpss[] = { 32000,48000,88200,96000 };

    _streamInfo.sampleFreq = bs->Read(2);
    _streamInfo.numBlocks = bs->Read(2);
    _streamInfo.channelMode = bs->Read(2);
    _streamInfo.numChannels = chs[_streamInfo.channelMode];
    _streamInfo.allocMethod = bs->Read(1);
    _streamInfo.numSubBands = sbs[bs->Read(1)];
    _streamInfo.bitPool = bs->Read(8);

    _streamInfo.numBlocks = blks[_streamInfo.numBlocks];
    _streamInfo.sampleRate = bpss[_streamInfo.sampleFreq];
    _streamInfo.pcm_size = SbcCom_c::SbcGetPcmSize(&_streamInfo);
    _streamInfo.pcm_samples = _streamInfo.numSubBands * _streamInfo.numBlocks;//_streamInfo.pcm_size/2/_streamInfo.numChannels;
    _streamInfo.vlc_size = SbcCom_c::SbcGetVlcSize(_streamInfo.channelMode, _streamInfo.numSubBands, _streamInfo.numBlocks, _streamInfo.numChannels, _streamInfo.bitPool);
    bs->Read(8);// crc, skip

    return true;
}

void SbcDec_c::JointSteroHeaderProcess(BitStream_c* bs) {
    if (SBC_CHNL_MODE_JOINT_STEREO == _streamInfo.channelMode) {
        int sb = _streamInfo.numSubBands;
        _joins = bs->Read(sb);
        if (sb < 8) {
            _joins <<= (8 - sb);
        }
    }
}


bool SbcDec_c::FactorsProcess(BitStream_c* bs) {
    int8_t ch;
    int8_t sb;
    for (ch = 0; ch < _streamInfo.numChannels; ch++) {
        uint8_t* scale_factors0 = _scaleFactors[ch];
        for (sb = 0; sb < _streamInfo.numSubBands; sb += 2) {
            *scale_factors0++ = bs->Read(4);
            *scale_factors0++ = bs->Read(4);
        }
    }
    return true;
}

void SbcDec_c::JointProcess() {
    int8_t blk;
    int8_t sb;
    for (sb = 0; sb < _streamInfo.numSubBands; sb++) {
        if (_joins & (1 << (7 - sb))) {
            for (blk = 0; blk < _block; blk++) {
                int ch0 = _sbSample[blk][0][sb];
                int ch1 = _sbSample[blk][1][sb];
                _sbSample[blk][0][sb] = ch0 + ch1;
                _sbSample[blk][1][sb] = ch0 - ch1;
            }
        }
    }
}

int SbcDec_c::Run(void* input, int size, void* out, int outSize) {
    if (!size)
        return 0;
    //1. frame sync
    int used = FrameSync(input, size);
    if (used == size) {
        return 0;
    }
    uint8_t* data = (uint8_t*)input + used;
    BitStream_c bs;
    bs.Init(data, 0);
    //2. prcess header information based on sync word
    HeaderProcess(&bs);

    if (size < (_streamInfo.vlc_size - 1 + used)) {
        return -1;
    }
    //3. loop
    uint8_t blockRem = _streamInfo.numBlocks;
    i16 tmp[2][16 * 8];
    int16_t* ch0Ori = tmp[0];
    int16_t* ch1Ori = tmp[1];
    //output->samples = 0;


    _block = _streamInfo.numBlocks;
    //3.2 express scale factors from encoded data
    JointSteroHeaderProcess(&bs);
    if (!FactorsProcess(&bs)) {
        return 0;
    }
    //3.2 crc check of first block in every decode process

    if (!SbcCom_c::CRCCheck(&_streamInfo, (char*)input + used)) {
        return 0;
    }
    //3.3 bit alloc
    switch (_streamInfo.channelMode)
    {
    case SBC_CHNL_MODE_MONO:
        SbcCom_c::BitAllocMono(&_streamInfo, _bits[0], _scaleFactors[0]);
        break;
    case SBC_CHNL_MODE_DUAL_CHNL:
        SbcCom_c::BitAllocMono(&_streamInfo, _bits[0], _scaleFactors[0]);
        SbcCom_c::BitAllocMono(&_streamInfo, _bits[1], _scaleFactors[1]);
        break;
    case SBC_CHNL_MODE_STEREO:
    case SBC_CHNL_MODE_JOINT_STEREO:
        SbcCom_c::BitAllocStereo(&_streamInfo, _bits, _scaleFactors);
        break;
    default:
        return 0;
    }
    //3.4 sbSamples
    SbsamplesMake(&bs);
    /// joint stereo
    if (SBC_CHNL_MODE_JOINT_STEREO == _streamInfo.channelMode) {
        JointProcess();
    }
    //3.5 synth filter
    switch (_streamInfo.numSubBands)
    {
    case 4:
        BandSynthFilter4(ch0Ori, ch1Ori);
        break;
    case 8:
        BandSynthFilter8(ch0Ori, ch1Ori);
        break;
    default:
        return 0;
    }
    i16* pOut;
    if (_streamInfo.numChannels == 1) {
        i16* pOut = (i16*)out;
        for (int n = 0; n < _streamInfo.numBlocks * _streamInfo.numSubBands; n++) {
            pOut[0] = tmp[0][n];
            pOut += 1;
        }
    }

    else {
        i16* pOut0 = (i16*)out;
        i16* pOut1 = (i16*)out+1;
        for (int n = 0; n < _streamInfo.numBlocks * _streamInfo.numSubBands; n++) {
            pOut0[0] = tmp[0][n];
            pOut1[0] = tmp[1][n];
            pOut0 += 2;
            pOut1 += 2;
        }
    }
    int vsize = _streamInfo.vlc_size - 1 + used;
    return vsize;
}



























