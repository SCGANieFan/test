#pragma once

#define OGG_DMUX_TO_OPUS_RET_SUCCESS 0
#define OGG_DMUX_TO_OPUS_RET_FAIL -1
#define OGG_DMUX_TO_OPUS_RET_INPUT_ERROR -2
#define OGG_DMUX_TO_OPUS_RET_MALLOC_ERROR -3
#define OGG_DMUX_TO_OPUS_RET_INNER_ERROR -4
#define OGG_DMUX_TO_OPUS_RET_HEAD_ERROR -5





typedef int OggDmux2OpusRet;

int GetOggDemux2OpusStateSize();

OggDmux2OpusRet OggDemux2OpusStateInit(void* hd);

OggDmux2OpusRet OggDemuxReceive(void* hd, char* in, int inByte);
OggDmux2OpusRet OggDemuxGenerate(void* hd, char* out, int* outByte);


void OggDemux2OpusEnd(void* hd);


