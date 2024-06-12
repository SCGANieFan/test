#include<string.h>
#include"ApePrive.h"


//inner


EXTERNC{
//interface

int32_t ApeDemuxerGetHeaderSize()
{
    int32_t size1 = sizeof(ApeDescriptor);
    int32_t size2 = sizeof(ApeHeaderOld);
    return size1 > size2 ? size1 : size2;
}

int32_t ApeDemuxerGetPraseInBufferSize(uint8_t* in, int32_t inLen)
{
    //check
    if (in == NULL
        || inLen < ApeDemuxerGetHeaderSize())
        return -1;

    if(in[0]!='M'
        || in[1] != 'A'
        || in[2] != 'C'
        || in[3] != ' ')
        return -1;

    //init
    uint16_t fileversion = (*(uint16_t*)(in + 4));
    uint8_t* pIn = in;

    int32_t inBuffSize = 0;

    if (fileversion >= 3980)
    {
        ApeDescriptor* descriptor = (ApeDescriptor*)in;
        inBuffSize= sizeof(ApeDescriptor) + sizeof(ApeHeader) + descriptor->seektablelength + descriptor->wavheaderlength;
    }
    else
    {
        ApeHeaderOld headerOld;
        memcpy(&headerOld, pIn, sizeof(ApeHeaderOld));
        pIn += sizeof(ApeHeaderOld);
        inBuffSize += sizeof(ApeHeaderOld);

        if (headerOld.nTotalFrames == 0)
            return -1;

        if (headerOld.nFormatFlags & APE_FORMAT_FLAG_HAS_PEAK_LEVEL)
        {
            pIn += 4;
            inBuffSize += 4;
        }

        //nSeekTableElements
        int32_t seektablelength;
        if (headerOld.nFormatFlags & APE_FORMAT_FLAG_HAS_SEEK_ELEMENTS)
        {
            seektablelength = (*((int32_t*)pIn))*4; pIn += 4;
            inBuffSize += 4;
        }
        else
            seektablelength = (int32_t)(headerOld.nTotalFrames * 4);
        inBuffSize += seektablelength;

        //wavheaderlength
        uint32_t wavheaderlength;
        wavheaderlength = (headerOld.nFormatFlags & APE_FORMAT_FLAG_CREATE_WAV_HEADER) ? (int64_t)(sizeof(WaveHeader)) : headerOld.nHeaderBytes;
        inBuffSize += wavheaderlength;

    }
	    return inBuffSize;
}

int32_t ApeGetDemuxerSize(uint8_t * in,int32_t inLen)
{
    //check
    if (in == NULL
        || inLen < ApeDemuxerGetHeaderSize())
        return -1;

    if (in[0] != 'M'
        || in[1] != 'A'
        || in[2] != 'C'
        || in[3] != ' ')
        return -1;
    uint16_t fileversion = (*(uint16_t*)(in + 4));
    uint32_t seektablelength = 0;
    uint32_t wavheaderlength = 0;
    if (fileversion >= 3980)
    {
        seektablelength = ((ApeDescriptor*)in)->seektablelength;
        wavheaderlength = ((ApeDescriptor*)in)->wavheaderlength;
    }
    else
    {
        uint8_t* pIn = in;
        ApeHeaderOld headerOld;
        memcpy(&headerOld, pIn, sizeof(ApeHeaderOld));
        pIn += sizeof(ApeHeaderOld);
        if (headerOld.nTotalFrames == 0)
            return -1;
        if (headerOld.nFormatFlags & APE_FORMAT_FLAG_HAS_PEAK_LEVEL)
            pIn += 4;
        if (headerOld.nFormatFlags & APE_FORMAT_FLAG_HAS_SEEK_ELEMENTS)
            seektablelength = (*((int32_t*)pIn))*4;
        else
            seektablelength = (int32_t)(headerOld.nTotalFrames*4);
        wavheaderlength = (headerOld.nFormatFlags & APE_FORMAT_FLAG_CREATE_WAV_HEADER) ? (int64_t)(sizeof(WaveHeader)) : headerOld.nHeaderBytes;
    }
    int32_t contextSize = ApeGetContextSize(seektablelength, wavheaderlength);

    if (contextSize > 0)
        return sizeof(ApeDemuxer) + contextSize;
    else
        return contextSize;
}

uint16_t ApeGetDemuxerCompressLevel(void* demuxer)
{
    return ((ApeDemuxer*)demuxer)->context->header.compressiontype;
}
uint32_t ApeGetDemuxerSeektableLength(void* demuxer)
{
    return ((ApeDemuxer*)demuxer)->context->descriptor.seektablelength;
}
uint32_t ApeGetDemuxerWavheaderLength(void* demuxer)
{
    return ((ApeDemuxer*)demuxer)->context->descriptor.wavheaderlength;
}
uint32_t ApeGetDemuxerChannels(void* demuxer)
{
    return ((ApeDemuxer*)demuxer)->context->header.channels;
}
uint32_t ApeGetDemuxerSampleRate(void* demuxer)
{
    return ((ApeDemuxer*)demuxer)->context->header.samplerate;
}
uint32_t ApeGetDemuxerSampleBits(void* demuxer)
{
    return ((ApeDemuxer*)demuxer)->context->header.bps;
}
void* ApeGetDemuxerContext(void* demuxer)
{
    return (void*)((ApeDemuxer*)demuxer)->context;
}

APE_RET_t ApeDemuxerInit(void* demuxer,int32_t demuxerSize)
{
    //check
    if (demuxer == NULL
        || demuxerSize < 1)
        return APE_RET_INPUT_ERROR;
#if 1
    memset(demuxer, 0, demuxerSize);
    ApeDemuxer* demux = (ApeDemuxer*)demuxer;
    demux->context = (ApeContext*)((uint8_t*)demuxer + sizeof(ApeDemuxer));
    //demux->context->seektable = (uint32_t*)((uint8_t*)demux->context + sizeof(ApeContext));
    //demux->context->waveHeader = (uint32_t*)((uint8_t*)demux->context + sizeof(ApeContext)+ApeGetSeekTableSize());
#endif
    return APE_RET_SUCCESS;
}

APE_RET_t ApeDemuxerPrase(void* demuxer, uint8_t * in, uint32_t inSize, uint32_t * inUsed)
{
    //check
    if (demuxer == NULL
        || in == NULL
        || inSize < 1)
        return APE_RET_INPUT_ERROR;
    if (!(in[0] == 'M'
        && in[1] == 'A'
        && in[2] == 'C'
        && in[3] == ' '))
        return APE_RET_PRASE_ERROR;
    int ssize = ApeDemuxerGetPraseInBufferSize(in, inSize);
    if (inSize < ApeDemuxerGetPraseInBufferSize(in,inSize))
        return APE_RET_NOT_ENOUGH_INPUT;
    ApeDemuxer* dem = (ApeDemuxer*)demuxer;
    ApeContext* context = dem->context;
    ApeDescriptor* descriptor = &context->descriptor;
    ApeHeader* header = &context->header;

    uint8_t* pIn = in + 4;
    descriptor->fileversion = *((uint16_t*)(pIn)); pIn += 2;
    //Descriptor+header
    if (descriptor->fileversion >= 3980) {
        pIn -= 6;
        memcpy(descriptor, pIn, sizeof(ApeDescriptor));
        if (descriptor->descriptorlength > sizeof(ApeDescriptor))
            pIn += descriptor->descriptorlength;
        else
            pIn += sizeof(ApeDescriptor);

        memcpy(header, pIn, sizeof(ApeHeader));
        if (descriptor->headerlength > sizeof(ApeHeader))
            pIn += descriptor->headerlength;
        else
            pIn += sizeof(ApeHeader);
        //seektable
        context->seektable = (uint32_t*)((uint8_t*)context + APE_ALIGN_8(sizeof(ApeContext)));
        memcpy(context->seektable, pIn, descriptor->seektablelength);
        pIn += descriptor->seektablelength;

        //wav header
        context->waveHeader = ((uint8_t*)context + APE_ALIGN_8(sizeof(ApeContext)) + APE_ALIGN_8(descriptor->seektablelength));
        memcpy(context->waveHeader, pIn, descriptor->wavheaderlength);
        pIn += descriptor->wavheaderlength;
    }
    else {
        pIn -= 6;

        //headerOld
        ApeHeaderOld headerOld;
        memcpy(&headerOld, pIn, sizeof(ApeHeaderOld));
        pIn += sizeof(ApeHeaderOld);

        // fail on 0 length APE files (catches non-finalized APE files)
        if (headerOld.nTotalFrames == 0)
            return APE_RET_INVALID_INPUT_FILE;

        //nPeakLevel
#if 0
        int32_t nPeakLevel = -1;
        if (headerOld.nFormatFlags & APE_FORMAT_FLAG_HAS_PEAK_LEVEL)
        {
            nPeakLevel = *((int32_t*)pIn); pIn += 4;
        }
#else
        if (headerOld.nFormatFlags & APE_FORMAT_FLAG_HAS_PEAK_LEVEL)
        {
            pIn += 4;
        }
#endif
        //descriptor
        memcpy(descriptor->magic, headerOld.cID, 4);
        descriptor->fileversion = (int32_t)headerOld.nVersion;
        descriptor->padding = 0;
        descriptor->descriptorlength = 0;
        descriptor->headerlength = 0;
        if (headerOld.nFormatFlags & APE_FORMAT_FLAG_HAS_SEEK_ELEMENTS)
        {
            descriptor->seektablelength = (*((int32_t*)pIn)) * 4; pIn += 4;
        }
        else
            descriptor->seektablelength = (int32_t)(headerOld.nTotalFrames * 4);
        descriptor->wavheaderlength = (headerOld.nFormatFlags & APE_FORMAT_FLAG_CREATE_WAV_HEADER) ? (int64_t)(sizeof(WaveHeader)) : headerOld.nHeaderBytes;
        //descriptor->audiodatalength = (uint32_t)(block*channel*bytepersample);
        descriptor->audiodatalength = (uint32_t)0;
        descriptor->audiodatalength_high = (uint32_t)0;
        descriptor->wavtaillength = (uint32_t)0;
        memset(descriptor->md5, 0, sizeof(descriptor->md5));
        //header
        header->compressiontype= (int32_t)headerOld.nCompressionLevel;
        header->formatflags= (int32_t)headerOld.nFormatFlags;
        header->blocksperframe = (uint32_t)(((headerOld.nVersion >= 3900) || ((headerOld.nVersion >= 3800) && (headerOld.nCompressionLevel == APE_COMPRESSION_LEVEL_EXTRA_HIGH))) ? 73728 : 9216);
        if (headerOld.nVersion >= 3950)
            header->blocksperframe = 73728 * 4;
        header->finalframeblocks = (uint32_t)headerOld.nFinalFrameBlocks;
        header->totalframes= (uint32_t)headerOld.nTotalFrames;
        header->bps = (headerOld.nFormatFlags & APE_FORMAT_FLAG_8_BIT) ? 8 : ((headerOld.nFormatFlags & APE_FORMAT_FLAG_24_BIT) ? 24 : 16);
        header->channels= (int32_t)headerOld.nChannels;
        header->samplerate= (int32_t)headerOld.nSampleRate;

        // check for an invalid blocks per frame
        if (header->blocksperframe > (10 * ONE_MILLION) || header->blocksperframe <= 0)
            return APE_RET_INVALID_INPUT_FILE;

        // check the final frame size being nonsense
        if (header->finalframeblocks > header->blocksperframe)
            return APE_RET_INVALID_INPUT_FILE;

        // get the wave header
        context->waveHeader = ((uint8_t*)context + APE_ALIGN_8(sizeof(ApeContext)) + APE_ALIGN_8(descriptor->seektablelength));
        if (!(headerOld.nFormatFlags & APE_FORMAT_FLAG_CREATE_WAV_HEADER) && (headerOld.nHeaderBytes > 0))
        {
            if (headerOld.nHeaderBytes > 1024 * 1024)
                return APE_RET_INVALID_INPUT_FILE;
            memcpy(context->waveHeader, pIn, headerOld.nHeaderBytes);
            pIn += headerOld.nHeaderBytes;
        }

        // get the seek tables (really no reason to get the whole thing if there's extra)
        context->seektable = (uint32_t*)((uint8_t*)context + APE_ALIGN_8(sizeof(ApeContext)));
        memcpy(context->seektable, pIn, descriptor->seektablelength);
        pIn += descriptor->seektablelength;
    }

    context->junklength = 0;
    context->firstframe = context->junklength + descriptor->descriptorlength
        + descriptor->headerlength + descriptor->seektablelength + descriptor->wavheaderlength;
    //dem->currentframe = 0;
    context->totalsamples = header->blocksperframe * (header->totalframes - 1) + header->finalframeblocks;

    if (inUsed)
        *inUsed = pIn - in;
    context->bitStreamReadedIndex = pIn - in;

    return APE_RET_SUCCESS;
}
}

