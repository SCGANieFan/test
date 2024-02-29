#include <stdlib.h>
#include <string.h>
#include "Ogg.h"
#include "OggDemux2Opus.h"
#if 1

enum OggStage_e {
    OGG_STAGE_FIRST_PAGE = 0,
    OGG_STAGE_SECOND_PAGE,
    OGG_STAGE_OTHER_PAGE_HEAD,
    OGG_STAGE_OTHER_PAGE_BODY,
    OGG_STAGE_MAX,
};
typedef struct {
    unsigned char* date;
    int max;
    int size;
    int used;
}Buffer;


typedef struct {
    ogg_stream_state oggStreamS; /* take physical pages, weld into a logical stream of packets */
    ogg_sync_state   oggSyncS; /* sync and verify incoming physical bitstream */
    ogg_page         oggPage; /* one Ogg bitstream page. Vorbis packets are inside */
    ogg_packet       oggPacket; /* one raw packet of data for decode */
    OggStage_e stage;
    Buffer bodyData;
    Buffer headData;
    unsigned char* OggSyncBuff;
}OggDemux2OpusState;

int GetBodyDataSize()
{
    return 1024;
}
int GetHeadDataSize()
{
    return 1024;
}
int GetOggSyncBuffSize()
{
    return 1024;
}
int GetOggDemux2OpusStateSize()
{
    return sizeof(OggDemux2OpusState) + GetBodyDataSize() + GetHeadDataSize()+ GetOggSyncBuffSize();
}

void OggDemux2OpusStateInit(void* hd)
{
    OggDemux2OpusState* state = (OggDemux2OpusState*)hd;
    memset(state, 0, sizeof(OggDemux2OpusState));
    ogg_sync_init(&state->oggSyncS); /* Now we can read pages */

    state->stage = OggStage_e::OGG_STAGE_FIRST_PAGE;
    state->bodyData.date = (unsigned char*)state + sizeof(OggDemux2OpusState);
    state->bodyData.max = GetBodyDataSize();

    state->headData.date = state->bodyData.date + state->bodyData.max;
    state->headData.max = GetHeadDataSize();

    state->OggSyncBuff = state->headData.date + state->headData.max;
    state->oggSyncS.storage = GetOggSyncBuffSize();
    state->oggSyncS.data = state->OggSyncBuff;
}

int frames = 0;
OggDmux2OpusRet OggDemux2OpusRun(void* hd, char* in, int inByte, char* out, int* outByte)
{
    if (!outByte) {
        return OGG_DMUX_TO_OPUS_RET_SUCCESS;
    }
    int outByteMax = *outByte;
    *outByte = 0;
    if (outByteMax < 1) {
        return OGG_DMUX_TO_OPUS_RET_SUCCESS;
    }
    if (!hd
        || !in
        || inByte < 1
        || !out)
    {
        return OGG_DMUX_TO_OPUS_RET_SUCCESS;
    }

    int outByteOffset = 0;
    OggDemux2OpusState* state = (OggDemux2OpusState*)hd;
    int result;
    char* buffer;
    int inOffset = 0;
    OggDmux2OpusRet ret = OGG_DMUX_TO_OPUS_RET_SUCCESS;
    unsigned char* page;
    long bytes;
    while (1)
    {
        //input
        if (state->stage != OGG_STAGE_OTHER_PAGE_BODY)
        {
            int inRem = inByte - inOffset;
            if (inRem < 1)
            {
                break;
            }
            if (state->oggSyncS.returned) {
                state->oggSyncS.fill -= state->oggSyncS.returned;
                if (state->oggSyncS.fill > 0)
                    memmove(state->oggSyncS.data, state->oggSyncS.data + state->oggSyncS.returned, state->oggSyncS.fill);
                state->oggSyncS.returned = 0;
            }
            int rem = state->oggSyncS.storage - state->oggSyncS.fill;
            int copySize = inRem < rem ? inRem : rem;
            memcpy(state->oggSyncS.data+ state->oggSyncS.fill, in + inOffset, copySize);
            state->oggSyncS.fill += copySize;
            inOffset += copySize;
        }
        else
        {
            int rem = state->bodyData.size - state->bodyData.used;
            if (rem)
            {
                memmove(state->bodyData.date, state->bodyData.date + state->bodyData.used, rem);
            }
            state->bodyData.used = 0;
            state->bodyData.size = rem;

            int inRem = inByte - inOffset;
            if (inRem < 1)
            {
                break;
            }
            rem = state->bodyData.max - state->bodyData.size;
            int copySize = inRem < rem ? inRem : rem;
            memcpy(state->bodyData.date + state->bodyData.size, in + inOffset, copySize);
            state->bodyData.size += copySize;
            inOffset += copySize;
        }

        bool loop = true;
        while (loop)
        {
            switch (state->stage)
            {
            case OGG_STAGE_FIRST_PAGE:
                //Get the first page
                result = ogg_sync_pageout(&state->oggSyncS, &state->oggPage);
                if (result == 0)
                {
                    loop = false;
                    break;
                }
                if (result < 0)
                {
                    ret = OGG_DMUX_TO_OPUS_RET_HEAD_ERROR;
                    loop = false;
                    break;
                }
                ogg_stream_init(&state->oggStreamS, ogg_page_serialno(&state->oggPage));
                if (ogg_stream_pagein(&state->oggStreamS, &state->oggPage) < 0) {
                    //Error reading first page of Ogg bitstream data
                    loop = false;
                    break;
                }
                state->oggStreamS.body_data = state->oggPage.body;
                state->oggStreamS.body_data_valid_len = state->oggPage.body_len_valid;
                result = ogg_stream_packetout(&state->oggStreamS, &state->oggPacket);
                if (result == 0)
                {
                    loop = false;
                    break;
                }
                if (result < 0)
                {
                    ret = OGG_DMUX_TO_OPUS_RET_HEAD_ERROR;
                    loop = false;
                    break;
                }
                state->stage = OggStage_e::OGG_STAGE_SECOND_PAGE;
                break;
            case OGG_STAGE_SECOND_PAGE:
                result = ogg_sync_pageout(&state->oggSyncS, &state->oggPage);
                if (result == 0)
                {
                    loop = false;
                    break;
                }
                if (result == 1) {
                    /* we can ignore any errors here as they'll also become apparent at packetout */
                    ogg_stream_pagein(&state->oggStreamS, &state->oggPage);
                    memcpy(state->bodyData.date, state->oggPage.body, state->oggPage.body_len_valid);
                    state->bodyData.size = state->oggPage.body_len_valid;
                    state->bodyData.used = 0;
                    while (1)
                    {
                        state->oggStreamS.body_data = state->bodyData.date + state->bodyData.used;
                        state->oggStreamS.body_data_valid_len = state->bodyData.size - state->bodyData.used;
                        result = ogg_stream_packetout(&state->oggStreamS, &state->oggPacket);
                        if (result == 0)
                        {
                            //loop = false;
                            break;
                        }
                        if (result < 0) {
                            /* Uh oh; data at some point was corrupted or missing! We can't tolerate that in a header.  Die. */
                            //Corrupt secondary header.  Exiting
                            ret = OGG_DMUX_TO_OPUS_RET_HEAD_ERROR;
                            loop = false;
                            break;
                        }
                        state->bodyData.used += state->oggPacket.bytes;
                    }
                    state->stage = OggStage_e::OGG_STAGE_OTHER_PAGE_HEAD;
                }
                break;
            case OGG_STAGE_OTHER_PAGE_HEAD:
                result = 1;
                page = state->oggSyncS.data + state->oggSyncS.returned;
                bytes = state->oggSyncS.fill - state->oggSyncS.returned;
                if (state->oggSyncS.headerbytes == 0) {
                    int headerbytes, i;
                    if (bytes < 27)
                    {
                        loop = false;
                        result = 0;
                        break;
                    }
                    else
                    {
                        /* verify capture pattern */
                        if (memcmp(page, "OggS", 4))
                        {
                            ret = OGG_DMUX_TO_OPUS_RET_HEAD_ERROR;
                            loop = false;
                            break;
                        }

                        headerbytes = page[26] + 27;
                        if (bytes < headerbytes)
                        {
                            loop = false;
                            result = 0;
                            break;
                        }

                        /* count up body length in the segment table */

                        for (i = 0; i < page[26]; i++)
                            state->oggSyncS.bodybytes += page[27 + i];
                        state->oggSyncS.headerbytes = headerbytes;
                    }

                }
                /* yes, have a whole page all ready to go */
                {
                    memcpy(state->headData.date, page, state->oggSyncS.headerbytes);
                    state->headData.size = state->oggSyncS.headerbytes;
                    state->oggPage.header = state->headData.date;
                    state->oggPage.header_len = state->headData.size;

                    state->oggPage.body_len_valid = bytes - state->oggPage.header_len;
                    memcpy(state->bodyData.date, page + state->oggPage.header_len, state->oggPage.body_len_valid);
                    state->bodyData.size = state->oggPage.body_len_valid;
                    state->bodyData.used = 0;

                    state->oggPage.body = state->bodyData.date;
                    state->oggPage.body_len = state->oggSyncS.bodybytes;
                    
                    state->oggSyncS.unsynced = 0;
                    state->oggSyncS.returned += (state->oggSyncS.headerbytes + state->oggSyncS.bodybytes) < bytes ? (state->oggSyncS.headerbytes + state->oggSyncS.bodybytes) : bytes;
                    state->oggSyncS.headerbytes = 0;
                    state->oggSyncS.bodybytes = 0;
                }
                ogg_stream_pagein(&state->oggStreamS, &state->oggPage); /* can safely ignore errors at this point */


                state->stage = OggStage_e::OGG_STAGE_OTHER_PAGE_BODY;
                break;

            case OGG_STAGE_OTHER_PAGE_BODY:

                while (1)
                {
#if 0
                    result = ogg_stream_packetout(&state->oggStreamS, &state->oggPacket);
#else
                    state->oggStreamS.body_data = state->bodyData.date + state->bodyData.used;
                    state->oggStreamS.body_data_valid_len = state->bodyData.size - state->bodyData.used;
                    result = ogg_stream_packetout(&state->oggStreamS, &state->oggPacket);
#endif
                    if (result == 2
                        || result == 0)
                    {
                        loop = false;
                        break;
                    }
                    char B4[4];
                    //pack size, 4byte
                    B4[3] = state->oggPacket.bytes & 0xff;
                    B4[2] = (state->oggPacket.bytes >> 8) & 0xff;
                    B4[1] = (state->oggPacket.bytes >> 16) & 0xff;
                    B4[0] = (state->oggPacket.bytes >> 24) & 0xff;
                    memcpy(out + outByteOffset, B4, 4);
                    outByteOffset += 4;
#if 1
                    //range code, 4byte
                    B4[0] = 0;
                    B4[1] = 0;
                    B4[2] = 0;
                    B4[3] = 0;
                    memcpy(out + outByteOffset, B4, 4);
                    outByteOffset += 4;
#endif
                    //encoded data
                    memcpy(out + outByteOffset, state->oggPacket.packet, state->oggPacket.bytes);
                    outByteOffset += state->oggPacket.bytes;
                    state->bodyData.used += state->oggPacket.bytes;
                    if (state->bodyData.used > state->bodyData.max)
                        int a1 = 1;

                    frames++;
#if 0
                    printf("frames:%d\n", frames);
#endif
                }
                if (result != 2)
                {
                    
                    int rem = state->bodyData.size - state->bodyData.used;
                    buffer = ogg_sync_buffer(&state->oggSyncS, rem);
                    memcpy(buffer, state->bodyData.date + state->bodyData.used, rem);
                    ogg_sync_wrote(&state->oggSyncS, rem);                    

                    if (ogg_page_eos(&state->oggPage))
                    {
                        state->stage = OggStage_e::OGG_STAGE_FIRST_PAGE;
                        /* clean up this logical bitstream; before exit we see if we're followed by another [chained] */
                        ogg_stream_clear(&state->oggStreamS);
                        break;
                    }
                    else
                    {
                        state->stage = OggStage_e::OGG_STAGE_OTHER_PAGE_HEAD;
                    }
                }
                break;
            case OGG_STAGE_MAX:
            default:
                state->stage = OggStage_e::OGG_STAGE_FIRST_PAGE;
                break;
            }
        }

    }
    if (outByteOffset > outByteMax)
        ret = OGG_DMUX_TO_OPUS_RET_FAIL;
    *outByte = outByteOffset;
    return ret;

}

void OggDemux2OpusEnd(void* hd)
{
    OggDemux2OpusState* state = (OggDemux2OpusState*)hd;
    ogg_sync_clear(&state->oggSyncS);
}


#endif
