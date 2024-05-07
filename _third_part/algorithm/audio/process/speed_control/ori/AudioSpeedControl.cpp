#if 1

#include "AudioSpeedControlInner.h"

static inline int32_t TDStretchGetBufferTemplateSize(int32_t sampleRate, int32_t sampleWidth, int32_t channels, int32_t voerlapMs)
{
    return (sampleRate * voerlapMs / 1000) * sampleWidth * channels;
}

static inline int32_t TDStretchGetTDStretchSize()
{
    return ALIGEN8(sizeof(TDStretch));
}

EXTERNC{

bool TDStretchSetChannels(void* hd, int32_t channels)
{
    if (channels < 1)
        return false;
    ((TDStretch*)hd)->channels = channels;
    return true;
}
bool TDStretchSetSampleRate(void* hd, int32_t samplerate)
{
    if (samplerate < 1)
        return false;
    ((TDStretch*)hd)->sampleRate = samplerate;
    return true;
}

bool TDStretchSetSampleWidth(void* hd, int32_t sampleWidth)
{
    if (sampleWidth < 1
        || sampleWidth>4)
        return false;
    ((TDStretch*)hd)->sampleWidth = sampleWidth;
    return true;
}

bool TDStretchSetSeekMs(void* hd, int32_t seekMs)
{
    if (seekMs < 1)
        return false;
    ((TDStretch*)hd)->seekMs = seekMs;
    return true;
}
bool TDStretchSetOverlapMs(void* hd, int32_t overlapMs)
{
    if (overlapMs < 1)
        return false;
    ((TDStretch*)hd)->overlapMs = overlapMs;
    return true;
}

bool TDStretchSetConstMs(void* hd, int32_t constMs)
{
    if (constMs < 1)
        return false;
    ((TDStretch*)hd)->constMs = constMs;
    return true;
}

bool TDStretchSetSpeed(void* hd, double speed)
{
    if (speed <= 0
        || speed>3)
        return false;
    ((TDStretch*)hd)->speed = speed;
    return true;
}

bool TDStretchSetEnd(void* hd, bool isEnd)
{
    ((TDStretch*)hd)->isEnd = isEnd;
    return true;
}

int32_t TDStretchGetDealInMinByte(void* hd)
{
    TDStretch* pTDStretch = (TDStretch*)hd;

    int32_t size1 = (pTDStretch->seekSamples + pTDStretch->overlapSamples + pTDStretch->constSamples + pTDStretch->overlapSamples) * pTDStretch->sampleWidth * pTDStretch->channels;
    int32_t size2 = (int32_t)(pTDStretch->speed * (pTDStretch->overlapSamples + pTDStretch->constSamples) * pTDStretch->sampleWidth * pTDStretch->channels);

    return MAX(size1, size2);
}

bool TDStretchSetFinished(void* hd)
{
    TDStretch* pTDStretch = (TDStretch*)hd;
    //check
    if (pTDStretch->channels < 1)
        return false;
    if (pTDStretch->sampleRate != 8000
        && pTDStretch->sampleRate != 16000
        && pTDStretch->sampleRate != 32000
        && pTDStretch->sampleRate != 48000
        && pTDStretch->sampleRate != 96000
        && pTDStretch->sampleRate != 192000)
        return false;
    if (pTDStretch->sampleWidth < 1
        || pTDStretch->sampleWidth > 4)
        return false;
#if 0
    if (pTDStretch->seekMs < 1)
        return false;
#endif

    if (pTDStretch->overlapMs < 1)
        return false;
#if 0
    if (pTDStretch->constMs < 1)
        return false;
#endif
    if (pTDStretch->speed < 0)
        return false;
    //set param

    //channels
    //pTDStretch->channels

    //sampleRate
    //pTDStretch->sampleRate

    //sampleWidth
    //pTDStretch->sampleWidth


    // Adjust tempo param according to tempo, so that variating processing sequence length is used
    // at various tempo settings, between the given low...top limits

    //seekSamples
    if (pTDStretch->seekMs < 1)
    {
        double seek;
        seek = AUTOSEEK_C + AUTOSEEK_K * pTDStretch->speed;
        seek = CHECK_LIMITS(seek, AUTOSEEK_AT_MAX, AUTOSEEK_AT_MIN);
        pTDStretch->seekMs = (int32_t)(seek + 0.5);
    }
    pTDStretch->seekSamples = pTDStretch->sampleRate * pTDStretch->seekMs / 1000;

    //overlapSamples
    pTDStretch->overlapSamples = pTDStretch->sampleRate * pTDStretch->overlapMs / 1000;

    //constSamples
    if (pTDStretch->constMs < 1)
    {
        int32_t dealN;
        double seq;
        seq = AUTOSEQ_C + AUTOSEQ_K * pTDStretch->speed;
        seq = CHECK_LIMITS(seq, AUTOSEQ_AT_MAX, AUTOSEQ_AT_MIN);
        dealN = (int32_t)(seq + 0.5);
        dealN = MAX(dealN, 2 * pTDStretch->overlapMs);
        pTDStretch->constMs = dealN - 2 * pTDStretch->overlapMs;
    }
    pTDStretch->constSamples = pTDStretch->sampleRate * pTDStretch->constMs / 1000;

    //pTDStretch->isEnd

    return true;
}

int32_t TDStretchGetSize(int32_t sampleRate, int32_t sampleWidth, int32_t channels, int32_t overlapMs)
{
    return TDStretchGetTDStretchSize() + TDStretchGetBufferTemplateSize(sampleRate, sampleWidth, channels, overlapMs);
}

void TDStretchInit(void* hd)
{
    TDStretch* pTDStretch = (TDStretch*)hd;
    memset(pTDStretch, 0, TDStretchGetTDStretchSize());
    pTDStretch->channels = 0;
    pTDStretch->sampleRate = 0;
    pTDStretch->sampleWidth = 0;

    pTDStretch->seekMs = 0;
    pTDStretch->overlapMs = 0;
    pTDStretch->constMs = 0;

    pTDStretch->seekSamples = 0;
    pTDStretch->overlapSamples = 0;
    pTDStretch->constSamples = 0;

    pTDStretch->speed = 1;
    pTDStretch->isBeginning = true;

    pTDStretch->bufferTemplate = (int8_t*)hd + TDStretchGetTDStretchSize();

    pTDStretch->isEnd = false;
}

int32_t TDStretchRun(void* hd, int8_t * in, int32_t inSize, int32_t * inUsed, int8_t * out, int32_t * outSize)
{
    TDStretch* pTDStretch = (TDStretch*)hd;
    int32_t outSizeMax = *outSize;
    //int32_t inSamples = inSize / (pTDStretch->sampleWidth * pTDStretch->channels);

    int32_t inOffset = 0;
    int32_t offset = 0;
    int32_t outOffset = 0;

    int32_t seekSize = pTDStretch->seekSamples * pTDStretch->sampleWidth * pTDStretch->channels;
    int32_t overlapSize = pTDStretch->overlapSamples * pTDStretch->sampleWidth * pTDStretch->channels;
    int32_t constSize = pTDStretch->constSamples * pTDStretch->sampleWidth * pTDStretch->channels;
    int32_t skipSize = (int32_t)(pTDStretch->speed * (pTDStretch->overlapSamples + pTDStretch->constSamples) * pTDStretch->sampleWidth * pTDStretch->channels);

    while (1)
    {
        int32_t size1 = seekSize + overlapSize + constSize + overlapSize;
        if ((inSize - inOffset) < MAX(size1, skipSize))
            break;
        if ((outSizeMax - outOffset) < overlapSize + constSize)
            break;

        if (pTDStretch->isBeginning == true)
        {
            //init bufferTemplate
            pTDStretch->isBeginning = false;
            memcpy(pTDStretch->bufferTemplate, in, overlapSize);
        }
        int32_t bestLag;
        bestLag = SeekBestLag(in + inOffset, pTDStretch->bufferTemplate, pTDStretch->sampleWidth, pTDStretch->seekSamples, pTDStretch->overlapSamples, pTDStretch->channels);
        offset = bestLag * pTDStretch->sampleWidth * pTDStretch->channels;

        overlap(out + outOffset, in + inOffset + offset, pTDStretch->bufferTemplate, pTDStretch->overlapSamples, pTDStretch->channels, pTDStretch->sampleWidth);
        offset += overlapSize;
        outOffset += overlapSize;

        //copy constSamples from in to out
        memcpy(out + outOffset, in + inOffset + offset, constSize);
        offset += constSize;
        outOffset += constSize;

        //updata bufferTemplate
        memcpy(pTDStretch->bufferTemplate, in + inOffset + offset, overlapSize);

        inOffset += skipSize;
    }
    if(inUsed)
        *inUsed = inOffset;
    *outSize = outOffset;

    return 0;
}
}





#endif





