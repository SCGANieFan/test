#include <stdlib.h>
#include <string.h>
#include "Ogg.h"
#include "OggDemux2Opus.h"
#if 0

enum OggStage_e {
    OGG_STAGE_FIRST_PAGE = 0,
    OGG_STAGE_SECOND_PAGE = 1,
    OGG_STAGE_OTHER_PAGE = 2,
    OGG_STAGE_MAX = 3,
};

typedef struct {
    ogg_stream_state oggStreamS; /* take physical pages, weld into a logical stream of packets */
    ogg_sync_state   oggSyncS; /* sync and verify incoming physical bitstream */
    ogg_page         oggPage; /* one Ogg bitstream page. Vorbis packets are inside */
    ogg_packet       oggPacket; /* one raw packet of data for decode */
    OggStage_e stage;
}OggDemux2OpusState;

int GetOggDemux2OpusStateSize()
{
    return sizeof(OggDemux2OpusState);
}

void OggDemux2OpusStateInit(void* hd)
{
    OggDemux2OpusState* state = (OggDemux2OpusState*)hd;
    state->stage = OggStage_e::OGG_STAGE_FIRST_PAGE;
    ogg_sync_init(&state->oggSyncS); /* Now we can read pages */
}

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
    char* buffer = ogg_sync_buffer(&state->oggSyncS, inByte);
    memcpy(buffer, in, inByte);
    ogg_sync_wrote(&state->oggSyncS, inByte);
    bool loop = true;
    OggDmux2OpusRet ret = OGG_DMUX_TO_OPUS_RET_SUCCESS;
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
                //for (int p = 0; p < state->oggStreamS.lacing_packet; p++)
                while (1)
                {
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
                }
                state->stage = OggStage_e::OGG_STAGE_OTHER_PAGE;
            }
            break;
        case OGG_STAGE_OTHER_PAGE:
            while (1)
            {
                result = ogg_sync_pageout(&state->oggSyncS, &state->oggPage);
                if (result == 0)
                {
                    loop = false;
                    break;
                }
                if (result < 0) { /* missing or corrupt data at this page position */
                    //Corrupt or missing data in bitstream; continuing...                    
                }
                ogg_stream_pagein(&state->oggStreamS, &state->oggPage); /* can safely ignore errors at this point */
                while (1)
                {
                    result = ogg_stream_packetout(&state->oggStreamS, &state->oggPacket);
                    if (result == 0)
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
                }
                if (ogg_page_eos(&state->oggPage))
                {
                    state->stage = OggStage_e::OGG_STAGE_FIRST_PAGE;
                    /* clean up this logical bitstream; before exit we see if we're followed by another [chained] */
                    ogg_stream_clear(&state->oggStreamS);
                    break;
                }
            }
            break;
        case OGG_STAGE_MAX:
        default:
            state->stage = OggStage_e::OGG_STAGE_FIRST_PAGE;
            break;
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
