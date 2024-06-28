#include"Ape.Inner.context.h"

#if 1

/**************************************************************************************************
WAV header structure
**************************************************************************************************/
typedef struct
{
    // RIFF header
    i8 cRIFFHeader[4];
    u32 nRIFFBytes;

    // data type
    i8 cDataTypeID[4];

    // wave format
    i8 cFormatHeader[4];
    u32 nFormatBytes;

    u16 nFormatTag;
    u16 nChannels;
    u32 nSamplesPerSec;
    u32 nAvgBytesPerSec;
    u16 nBlockAlign;
    u16 nBitsPerSample;

    // data chunk header
    i8 cDataHeader[4];
    u32 nDataBytes;
}WaveHeader;


typedef struct {
    u8 cID[4];                         // should equal 'MAC '
    u16 nVersion;                        // version number * 1000 (3.81 = 3810)
    u16 nCompressionLevel;               // the compression level
    u16 nFormatFlags;                    // any format flags (for future use)
    u16 nChannels;                       // the number of channels (1 or 2)
    u32 nSampleRate;                     // the sample rate (typically 44100)
    u32 nHeaderBytes;                    // the bytes after the MAC header that compose the WAV header
    u32 nTerminatingBytes;               // the bytes after that raw data (for extended info)
    u32 nTotalFrames;                    // the number of frames in the file
    u32 nFinalFrameBlocks;               // the number of samples in the final frame
}ApeHeaderOld;

//inner
STATIC INLINE i32 GetHeaderSize()
{
    i32 size1 = sizeof(ApeDescriptor);
    i32 size2 = sizeof(ApeHeaderOld);
    return size1 > size2 ? size1 : size2;
}

STATIC INLINE i32 GetPraseInBufferByte(u8* in, i32 inLen)
{
    u8* pIn = in;
    i32 inByteUsed = 0;
    i32 inByteMax = inLen;
    while ((*(u32*)(&pIn[inByteUsed]) != *(u32*)"MAC "))
    {
        //search Head
        if (inByteUsed > (inByteMax - 4)) {
            return 1024 * 1024;
        }
        inByteUsed++;
    }
    inByteUsed += 4;
    u16 fileversion = (*(u16*)(&pIn[inByteUsed]));
    inByteUsed -= 4;
    if (fileversion >= 3980)
    {
        ApeDescriptor* descriptor = (ApeDescriptor*)&pIn[inByteUsed];
        inByteUsed += sizeof(ApeDescriptor) + sizeof(ApeHeader) + descriptor->seektablelength + descriptor->wavheaderlength;
    }
    else
    {
        ApeHeaderOld headerOld;
        ALGO_MEM_CPY(&headerOld, &pIn[inByteUsed], sizeof(ApeHeaderOld));
        inByteUsed += sizeof(ApeHeaderOld);

        if (headerOld.nTotalFrames == 0)
            return 1024 * 1024;

        if (headerOld.nFormatFlags & APE_FORMAT_FLAG_HAS_PEAK_LEVEL)
        {
            inByteUsed += 4;
        }

        //nSeekTableElements
        i32 seektablelength;
        if (headerOld.nFormatFlags & APE_FORMAT_FLAG_HAS_SEEK_ELEMENTS)
        {
            seektablelength = (*((i32*)&pIn[inByteUsed])) * 4;
            inByteUsed += 4;
        }
        else
            seektablelength = (i32)(headerOld.nTotalFrames * 4);
        inByteUsed += seektablelength;

        //wavheaderlength
        uint32_t wavheaderlength;
        wavheaderlength = (headerOld.nFormatFlags & APE_FORMAT_FLAG_CREATE_WAV_HEADER) ? (int64_t)(sizeof(WaveHeader)) : headerOld.nHeaderBytes;
        inByteUsed += wavheaderlength;

    }
    return inByteUsed;
}


#if 0
u32 ApeContext::GetStartPosFromFrame(u32 frame) {
#if 0
    ApeContext* pContext = ((ApeDemuxer*)demuxer)->context;

    if (Pos < pContext->seektable[0])
    {
        info->pos = pContext->seektable[0];
        info->FrameNum = 1;
    }
    else if (Pos > pContext->seektable[pContext->header.totalframes - 1])
    {
        info->pos = pContext->seektable[pContext->header.totalframes - 1];
        info->FrameNum = pContext->header.totalframes;
        return;
    }
    else
    {
        for (int f = 1; f < pContext->header.totalframes; f++)
        {
            if (Pos < pContext->seektable[f])
            {
                info->pos = pContext->seektable[f - 1];
                info->FrameNum = f;
                break;
            }
        }
    }
    int skip = (info->pos - pContext->seektable[0]) & 3;
#if 0
    if (skip)
        info->pos += 4 - skip;
#else
    info->pos -= skip;
#endif
#endif

    u32 pos = _context.seektable[frame - 1];
    int skip = (pos - _context.seektable[0]) & 3;
    pos -= skip;
    return pos;
}

u32 ApeContext::GetStartSkip(u32 frame) {
    return (_context.seektable[frame - 1] - _context.seektable[0]) & 3;
}
#endif
i32 ApeContext::Parser(MemoryManger_c* mm, u8* in, i32 inByte)
{
    // ALGO_PRINT();
    u8* pIn = in;
    i32 inByteMax = inByte;
    i32 inByteUsed = 0;
    ApeContext_t* context = &_context;
    memset(context, 0, sizeof(ApeContext_t));
    //search Head
    while ((*(u32*)(&pIn[inByteUsed]) != *(u32*)"MAC "))
    {
        if (inByteUsed > (inByteMax - 4)) {
            return APERET_FAIL;
        }
        inByteUsed++;
    }

    //inbyte 
    //if ((inByteMax - inByteUsed) < GetPraseInBufferByte(&pIn[inByteUsed], inByteMax - inByteUsed))
    if ((inByteMax - inByteUsed) < GetContextSize())
        return APERET_FAIL;

    ApeDescriptor* descriptor = &context->descriptor;
    ApeHeader* header = &context->header;

    inByteUsed += 4;
    descriptor->fileversion = *((u16*)(&pIn[inByteUsed]));
    inByteUsed -= 4;

    if (descriptor->fileversion >= 3980) {
        ALGO_MEM_CPY(descriptor, &pIn[inByteUsed], sizeof(ApeDescriptor));
        if (descriptor->descriptorlength > sizeof(ApeDescriptor))
            inByteUsed += descriptor->descriptorlength;
        else
            inByteUsed += sizeof(ApeDescriptor);

        ALGO_MEM_CPY(header, &pIn[inByteUsed], sizeof(ApeHeader));
        if (descriptor->headerlength > sizeof(ApeHeader))
            inByteUsed += descriptor->headerlength;
        else
            inByteUsed += sizeof(ApeHeader);

        //seektable
#if 0
        context->seektable = (uint32_t*)mm->Malloc(descriptor->seektablelength);
        ALGO_MEM_CPY(context->seektable, &pIn[inByteUsed], descriptor->seektablelength);
        inByteUsed += descriptor->seektablelength;

        //wav header        
        context->waveHeader = mm->Malloc(descriptor->wavheaderlength);
        ALGO_MEM_CPY(context->waveHeader, &pIn[inByteUsed], descriptor->wavheaderlength);
        inByteUsed += descriptor->wavheaderlength;
#else
        seektablePos = inByteUsed;
        inByteUsed += descriptor->seektablelength;
        waveHeaderPos = inByteUsed;
        inByteUsed += descriptor->wavheaderlength;
#endif
    }
    else {
        //headerOld
        ApeHeaderOld headerOld;
        ALGO_MEM_CPY(&headerOld, pIn, sizeof(ApeHeaderOld));
        pIn += sizeof(ApeHeaderOld);

        // fail on 0 length APE files (catches non-finalized APE files)
        if (headerOld.nTotalFrames == 0)
            return APE_RET_INVALID_INPUT_FILE;

        //nPeakLevel
#if 0
        i32 nPeakLevel = -1;
        if (headerOld.nFormatFlags & APE_FORMAT_FLAG_HAS_PEAK_LEVEL)
        {
            nPeakLevel = *((i32*)pIn);
            inByteUsed += 4;
        }
#else
        if (headerOld.nFormatFlags & APE_FORMAT_FLAG_HAS_PEAK_LEVEL)
        {
            inByteUsed += 4;
        }
#endif
        //descriptor
        ALGO_MEM_CPY(descriptor->magic, headerOld.cID, 4);
        descriptor->fileversion = (i32)headerOld.nVersion;
        descriptor->padding = 0;
        descriptor->descriptorlength = 0;
        descriptor->headerlength = 0;
        if (headerOld.nFormatFlags & APE_FORMAT_FLAG_HAS_SEEK_ELEMENTS)
        {
            descriptor->seektablelength = (*((i32*)&pIn[inByteUsed])) * 4;
            inByteUsed += 4;
        }
        else
            descriptor->seektablelength = (i32)(headerOld.nTotalFrames * 4);
        descriptor->wavheaderlength = (headerOld.nFormatFlags & APE_FORMAT_FLAG_CREATE_WAV_HEADER) ? (int64_t)(sizeof(WaveHeader)) : headerOld.nHeaderBytes;
        descriptor->audiodatalength = (uint32_t)0;
        descriptor->audiodatalength_high = (uint32_t)0;
        descriptor->wavtaillength = (uint32_t)0;
        ALGO_MEM_SET(descriptor->md5, 0, sizeof(descriptor->md5));
        //header
        header->compressiontype = (i32)headerOld.nCompressionLevel;
        header->formatflags = (i32)headerOld.nFormatFlags;
        header->blocksperframe = (uint32_t)(((headerOld.nVersion >= 3900) || ((headerOld.nVersion >= 3800) && (headerOld.nCompressionLevel == APE_COMPRESSION_LEVEL_EXTRA_HIGH))) ? 73728 : 9216);
        if (headerOld.nVersion >= 3950)
            header->blocksperframe = 73728 * 4;
        header->finalframeblocks = (uint32_t)headerOld.nFinalFrameBlocks;
        header->totalframes = (uint32_t)headerOld.nTotalFrames;
        header->bps = (headerOld.nFormatFlags & APE_FORMAT_FLAG_8_BIT) ? 8 : ((headerOld.nFormatFlags & APE_FORMAT_FLAG_24_BIT) ? 24 : 16);
        header->channels = (i32)headerOld.nChannels;
        header->samplerate = (i32)headerOld.nSampleRate;

        // check for an invalid blocks per frame
        if (header->blocksperframe > (10 * ONE_MILLION) || header->blocksperframe <= 0)
            return APE_RET_INVALID_INPUT_FILE;

        // check the final frame size being nonsense
        if (header->finalframeblocks > header->blocksperframe)
            return APE_RET_INVALID_INPUT_FILE;

        // get the wave header
        if (!(headerOld.nFormatFlags & APE_FORMAT_FLAG_CREATE_WAV_HEADER) && (headerOld.nHeaderBytes > 0))
        {
            if (headerOld.nHeaderBytes > 1024 * 1024)
                return APE_RET_INVALID_INPUT_FILE;
#if 0
            context->waveHeader = mm->Malloc(headerOld.nHeaderBytes);
            ALGO_MEM_CPY(context->waveHeader, &pIn[inByteUsed], headerOld.nHeaderBytes);
#else
            waveHeaderPos = inByteUsed;
#endif
            inByteUsed += headerOld.nHeaderBytes;
        }

        // get the seek tables (really no reason to get the whole thing if there's extra)
#if 0
        context->seektable = (uint32_t*)mm->Malloc(descriptor->seektablelength);
        ALGO_MEM_CPY(context->seektable, &pIn[inByteUsed], descriptor->seektablelength);
#else
        seektablePos = inByteUsed;
#endif
        inByteUsed += descriptor->seektablelength;
    }

#if 0
    context->junklength = 0;
    context->firstframe = context->junklength + descriptor->descriptorlength
        + descriptor->headerlength + descriptor->seektablelength + descriptor->wavheaderlength;
    context->totalsamples = header->blocksperframe * (header->totalframes - 1) + header->finalframeblocks;
#endif
    return APERET_SUCCESS;
}
#endif

