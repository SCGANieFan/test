#if 1
#include"Algo.Printer.h"
#include"Algo.Memory.h"
#include"ApeOri.h"
#include"Ape.Inner.func.h"




STATIC INLINE void* Ape_Malloc(ApeDecoder* pDec, i32 size)
{
    for (i16 i = 0; i < sizeof(pDec->mallocPtr) / sizeof(pDec->mallocPtr[0]); i++)
    {
        if (pDec->mallocPtr[i] == 0)
        {
            pDec->mallocPtr[i] = pDec->basePorting->Malloc(size);
            return pDec->mallocPtr[i];
        }
    }
    return 0;
}

STATIC INLINE void Ape_Free(ApeDecoder* pDec, void* block)
{
    for (i16 i = 0; i < sizeof(pDec->mallocPtr) / sizeof(pDec->mallocPtr[0]); i++)
    {
        if (pDec->mallocPtr[i] == block)
        {
            pDec->basePorting->Free(pDec->mallocPtr[i]);
            pDec->mallocPtr[i] = 0;
            return;
        }
    }
    return;
}

//inner
STATIC INLINE i32 GetHeaderSize()
{
	i32 size1 = sizeof(ApeDescriptor);
	i32 size2 = sizeof(ApeHeaderOld);
	return size1 > size2 ? size1 : size2;
}

STATIC INLINE i32 GetPraseInBufferByte(u8* in, i32 inLen)
{
#if 0
    //check
    if (in == NULL
        || inLen < GetHeaderSize())
        return -1;

    if (in[0] != 'M'
        || in[1] != 'A'
        || in[2] != 'C'
        || in[3] != ' ')
        return -1;

    //init
    u16 fileversion = (*(u16*)(in + 4));
    u8* pIn = in;

    i32 inBuffSize = 0;

    if (fileversion >= 3980)
    {
        ApeDescriptor* descriptor = (ApeDescriptor*)in;
        inBuffSize = sizeof(ApeDescriptor) + sizeof(ApeHeader) + descriptor->seektablelength + descriptor->wavheaderlength;
    }
    else
    {
        ApeHeaderOld headerOld;
        ALGO_MEM_CPY(&headerOld, pIn, sizeof(ApeHeaderOld));
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
        i32 seektablelength;
        if (headerOld.nFormatFlags & APE_FORMAT_FLAG_HAS_SEEK_ELEMENTS)
        {
            seektablelength = (*((i32*)pIn)) * 4; pIn += 4;
            inBuffSize += 4;
        }
        else
            seektablelength = (i32)(headerOld.nTotalFrames * 4);
        inBuffSize += seektablelength;

        //wavheaderlength
        uint32_t wavheaderlength;
        wavheaderlength = (headerOld.nFormatFlags & APE_FORMAT_FLAG_CREATE_WAV_HEADER) ? (int64_t)(sizeof(WaveHeader)) : headerOld.nHeaderBytes;
        inBuffSize += wavheaderlength;

    }
    return inBuffSize;

#else
    u8* pIn = in;
    i32 inByteUsed = 0;
    i32 inByteMax = inLen;
    while ((*(u32*)(&pIn[inByteUsed]) != *(u32*)"MAC "))
    {
        //search Head
        if (inByteUsed > (inByteMax - 4)) {
            return 1024*1024;
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
#endif
}


//interface 
i32 Ape_StateInitWithContext(ApeDecoder* pDecIn, AlgoBasePorting* basePorting, ApeContext* context)
{
	ALGO_PRINT();

    ApeDecoder* pDec = pDecIn;
    ApeMode mode = pDec->mode;
    ALGO_MEM_SET(pDec, 0, sizeof(ApeDecoder));
    pDec->mode= mode;
    pDec->basePorting = basePorting;
    pDec->context = *context;

    //clear NN Filter	
    pDec->fSet = context->header.compressiontype / 1000 - 1;
    for (u16 ch = 0; ch < APE_MAX_CHANNELS; ch++)
    {
        for (i32 i = 0; i < APE_FILTER_LEVELS; i++) {
            if (!ape_filter_orders[pDec->fSet][i])
                break;
            i32 size = (((ape_filter_orders[pDec->fSet][i] + APE_NN_FILTER_ROLL_BUFFER_EXTRA_LENGTH) * sizeof(i16))) * 3;
            pDec->filterbuf[ch][i] = (i16*)Ape_Malloc(pDec,size);
            ALGO_MEM_SET(pDec->filterbuf[ch][i], 0, size);
        }
    }
    //buffer read
    pDec->bufferRead.Init();
    pDec->blocksMax = APE_BLOCKS_MAX;
    pDec->isStartFrame = true;
    pDec->currentframe = 0;
    pDec->isInited = true;


    Buffer buffer;
    i32 size = 10 * 1024;
    buffer.Init((u8*)Ape_Malloc(pDec, size), size);
    pDec->inCache.Init(&buffer);
	return APERET_SUCCESS;
}

i32 Ape_StateInitWithBuffer(ApeDecoder* pDecIn, AlgoBasePorting* basePorting, u8* in, i32* inByte)
{
	ALGO_PRINT();
	u8* pIn = in;
	i32 inByteMax = *inByte;
	i32 inByteUsed = 0;
	ApeDecoder* pDec = pDecIn;
    *inByte = 0;
    ApeMode mdoe = pDecIn->mode;
	ALGO_MEM_SET(pDec, 0, sizeof(ApeDecoder));
    pDecIn->mode = mdoe;

	pDec->basePorting = basePorting;
	ApeContext* context = &pDec->context;

    //search Head
	while ((*(u32*)(&pIn[inByteUsed]) != *(u32*)"MAC "))
	{
		if (inByteUsed > (inByteMax - 4)) {
			return APERET_FAIL;
		}
		inByteUsed++;
	}

    //inbyte 
    if ((inByteMax - inByteUsed)< GetPraseInBufferByte(&pIn[inByteUsed], inByteMax - inByteUsed))
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
        context->seektable = (uint32_t*)Ape_Malloc(pDec,descriptor->seektablelength);
        ALGO_MEM_CPY(context->seektable, &pIn[inByteUsed], descriptor->seektablelength);
        inByteUsed += descriptor->seektablelength;

        //wav header        
        context->waveHeader = Ape_Malloc(pDec,descriptor->wavheaderlength);
        ALGO_MEM_CPY(context->waveHeader, &pIn[inByteUsed], descriptor->wavheaderlength);
        inByteUsed += descriptor->wavheaderlength;
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
            context->waveHeader = Ape_Malloc(pDec,headerOld.nHeaderBytes);
            ALGO_MEM_CPY(context->waveHeader, &pIn[inByteUsed], headerOld.nHeaderBytes);
            inByteUsed += headerOld.nHeaderBytes;
        }

        // get the seek tables (really no reason to get the whole thing if there's extra)
        context->seektable = (uint32_t*)Ape_Malloc(pDec,descriptor->seektablelength);
        ALGO_MEM_CPY(context->seektable, &pIn[inByteUsed], descriptor->seektablelength);
        inByteUsed += descriptor->seektablelength;
    }

    context->junklength = 0;
    context->firstframe = context->junklength + descriptor->descriptorlength
        + descriptor->headerlength + descriptor->seektablelength + descriptor->wavheaderlength;
    context->totalsamples = header->blocksperframe * (header->totalframes - 1) + header->finalframeblocks;


    *inByte = inByteUsed;

    if (pDecIn->mode == ApeMode::APE_MODE_DMUX)
        return APERET_SUCCESS;
    else
    {
        ApeContext contxtTemp = pDec->context;
        return Ape_StateInitWithContext(pDec, basePorting, &contxtTemp);
    }
}




i32 Ape_StateDeInitInner(ApeDecoder* pDecIn)
{
	ALGO_PRINT();

    ApeDecoder* pDec = pDecIn;
    for (i16 i = 0; i < sizeof(pDec->mallocPtr) / sizeof(pDec->mallocPtr[0]); i++)
    {
        if (pDec->mallocPtr[i] != 0)
        {
            Ape_Free(pDec, pDec->mallocPtr[i]);
        }
    }
    pDecIn->isInited = false;
	return APERET_SUCCESS;
}












#endif