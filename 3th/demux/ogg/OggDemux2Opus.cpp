#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "Ogg.h"
#include "Buffer.h"
#include "OggDemux2Opus.h"
#if 1
using namespace OggDemux;

enum OggStage_e {
    OGG_STAGE_FIRST_PAGE = 0,
    OGG_STAGE_SECOND_PAGE,
    OGG_STAGE_OTHER_PAGE_HEAD,
    OGG_STAGE_OTHER_PAGE_BODY,
    OGG_STAGE_MAX,
};


typedef struct {
    ogg_sync_state   oggSyncS; /* sync and verify incoming physical bitstream */
    ogg_page         oggPage; /* one Ogg bitstream page. Vorbis packets are inside */
    ogg_stream_state oggStreamS; /* take physical pages, weld into a logical stream of packets */
    ogg_packet       oggPacket; /* one raw packet of data for decode */
    OggStage_e stage;
    Buffer inBuff;
}OggDemux2OpusState;

int32_t MallocSize;
void* OggDemuxMalloc(int size)
{
    MallocSize += size;
    //printf("OggDemuxMalloc, MallocSize:%d\n", MallocSize);
    return malloc(size);
}

void* OggDemuxRealloc(void* block, int size)
{
    //printf("OggDemuxRealloc, newSize:%d\n", size);
    return realloc(block, size);
}
void* OggDemuxCalloc(int count, int size)
{
    return calloc(count, size);
}
void  OggDemuxFree(void* block)
{   
    //printf("OggDemuxFree\n");
    free(block);
}



static inline int GetInBuffDefaultSize()
{
    return 10*1024;
}

int GetOggDemux2OpusStateSize()
{
    return sizeof(OggDemux2OpusState);
}



OggDmux2OpusRet OggDemux2OpusStateInit(void* hd)
{
    if (!hd)
        return OGG_DMUX_TO_OPUS_RET_INPUT_ERROR;
    memset(hd, 0, GetOggDemux2OpusStateSize());

    OggDemux2OpusState* state = (OggDemux2OpusState*)hd;

    //oggSyncS
    ogg_sync_init(&state->oggSyncS);

    //oggStreamS
    ogg_stream_state* oStreamS = &state->oggStreamS;
    oStreamS->lacing_storage = 512;
    oStreamS->lacing_vals = (int*)OggDemuxMalloc(oStreamS->lacing_storage * sizeof(*oStreamS->lacing_vals));
    oStreamS->granule_vals = (ogg_int64_t*)OggDemuxMalloc(oStreamS->lacing_storage * sizeof(*oStreamS->granule_vals));


    //stage
    state->stage = OGG_STAGE_FIRST_PAGE;

    //inBuff
    int size = GetInBuffDefaultSize();
    uint8_t* buf = (uint8_t*)OggDemuxMalloc(size);
    if (!buf)
        return OGG_DMUX_TO_OPUS_RET_MALLOC_ERROR;
    state->inBuff.Init(buf, size);

    return OGG_DMUX_TO_OPUS_RET_SUCCESS;
}


OggDmux2OpusRet OggDemuxReceive(void* hd, char* in, int inByte)
{
    //check
    if (!hd)
        return OGG_DMUX_TO_OPUS_RET_INPUT_ERROR;
    if (inByte < 1)
        return OGG_DMUX_TO_OPUS_RET_INPUT_ERROR;
    if (!in)
        return OGG_DMUX_TO_OPUS_RET_INPUT_ERROR;

    OggDemux2OpusState* state = (OggDemux2OpusState*)hd;

    while (1)
    {
        bool ret = state->inBuff.Append((uint8_t*)in, inByte);
        if (ret == true)
            break;
        int32_t newSize = state->inBuff.GetMax() + 1024;
        uint8_t* buf = (uint8_t*)OggDemuxRealloc(state->inBuff.GetBuff(), newSize);
        if (!buf)
            return OGG_DMUX_TO_OPUS_RET_MALLOC_ERROR;
        state->inBuff.SetBuf(buf, newSize);
    }
    return OGG_DMUX_TO_OPUS_RET_SUCCESS;
}

OggDmux2OpusRet OggDemuxGenerate(void* hd, char* out, int* outByte)
{
    //hd
    if (!hd)
        return OGG_DMUX_TO_OPUS_RET_INPUT_ERROR;
    if (!out)
        return OGG_DMUX_TO_OPUS_RET_INPUT_ERROR;
    if (outByte)
        *outByte = 0;
    OggDemux2OpusState* state = (OggDemux2OpusState*)hd;


    int32_t result;
    int32_t outByteOffset = 0;
    OggDmux2OpusRet ret = OGG_DMUX_TO_OPUS_RET_SUCCESS;
    while (1)
    {
        if (state->stage != OGG_STAGE_OTHER_PAGE_BODY)
        {
            ogg_sync_wrote(&state->oggSyncS, state->inBuff.GetBuffValid(), state->inBuff.GetBuffValidSize());
            result = ogg_sync_pageout(&state->oggSyncS, &state->oggPage);
            if (result == 0) {
                goto exit;//need more data
            }
            if (result != 1) {
                //error case, Input does not appear to be an Ogg bitstream
                state->stage = OGG_STAGE_FIRST_PAGE;
                state->inBuff.Used(state->oggSyncS.returned);
                if (state->inBuff.GetUsed() > 1024)
                    state->inBuff.MoveToHead();
                goto exit;
            }
        }
        switch (state->stage)
        {
        case OGG_STAGE_FIRST_PAGE:
            /* Get the first page. */
            /* serialno ; use it to set up a logical stream */
            ogg_stream_init(&state->oggStreamS, ogg_page_serialno(&state->oggPage));
            if (ogg_stream_pagein(&state->oggStreamS, &state->oggPage) < 0) {
                /* error; stream version mismatch perhaps */
                //Error reading first page of Ogg bitstream data
                ret = OGG_DMUX_TO_OPUS_RET_INNER_ERROR;
                goto exit;
            }

            if (ogg_stream_packetout(&state->oggStreamS, &state->oggPacket) != 1) {
                //transmit discontinue
                ret = OGG_DMUX_TO_OPUS_RET_FAIL;
                goto exit;
            }
            state->inBuff.Used(state->oggSyncS.returned);
            state->stage = OGG_STAGE_SECOND_PAGE;
            break;
        case OGG_STAGE_SECOND_PAGE:
            /* Get the second page. */
            /* we can ignore any errors here as they'll also become apparent at packetout */
            if (ogg_stream_pagein(&state->oggStreamS, &state->oggPage) < 0) {
                /* error; stream version mismatch perhaps */
                //Error reading first page of Ogg bitstream data
                state->stage = OGG_STAGE_FIRST_PAGE;
                ret = OGG_DMUX_TO_OPUS_RET_INNER_ERROR;
                goto exit;
            }
            while (1)
            {
                result = ogg_stream_packetout(&state->oggStreamS, &state->oggPacket);
                if (result == 0) {
                    break;
                }
                if (result < 0) {
                    /* Uh oh; data at some point was corrupted or missing! We can't tolerate that in a header.  Die. */
                    //Corrupt secondary header.  Exiting
                    state->stage = OGG_STAGE_FIRST_PAGE;
                    ret = OGG_DMUX_TO_OPUS_RET_FAIL;
                    goto exit;
                }
            }
            state->inBuff.Used(state->oggSyncS.returned);
            state->stage = OGG_STAGE_OTHER_PAGE_HEAD;
            break;
        case OGG_STAGE_OTHER_PAGE_HEAD:
            /* Get the other page. */
            if (ogg_stream_pagein(&state->oggStreamS, &state->oggPage) < 0) {
                /* error; stream version mismatch perhaps */
                //Error reading first page of Ogg bitstream data
                state->stage = OGG_STAGE_FIRST_PAGE;
                ret = OGG_DMUX_TO_OPUS_RET_FAIL;
                goto exit;
            }
            state->stage = OGG_STAGE_OTHER_PAGE_BODY;
            state->inBuff.Used(state->oggSyncS.returned);
            break;
        case OGG_STAGE_OTHER_PAGE_BODY:
            result = ogg_stream_packetout(&state->oggStreamS, &state->oggPacket);
            if (result == 0) {
                if (ogg_page_eos(&state->oggPage))
                {
                    state->stage = OggStage_e::OGG_STAGE_FIRST_PAGE;
                    /* clean up this logical bitstream; before exit we see if we're followed by another [chained] */
                    ogg_stream_clear(&state->oggStreamS);
                }
                else
                {
                    state->stage = OggStage_e::OGG_STAGE_OTHER_PAGE_HEAD;
                }
                state->inBuff.Used(state->oggSyncS.returned);
                state->inBuff.MoveToHead();
                break;
            }
            if (result < 0) {
                /* Uh oh; data at some point was corrupted or missing! We can't tolerate that in a header.  Die. */
                //Corrupt secondary header.  Exiting
                state->stage = OGG_STAGE_FIRST_PAGE;
                ret = OGG_DMUX_TO_OPUS_RET_FAIL;
                goto exit;
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
            goto exit;
        default:
            break;
        }
    }
exit:
    if (outByte)
        *outByte = outByteOffset;
    return ret;
}
void OggDemux2OpusEnd(void* hd)
{
    OggDemux2OpusState* state = (OggDemux2OpusState*)hd;
    OggDemuxFree(state->inBuff.GetBuff());
    ogg_sync_clear(&state->oggSyncS);
    OggDemuxFree(state->oggStreamS.lacing_vals);
    OggDemuxFree(state->oggStreamS.granule_vals);
}


#endif
