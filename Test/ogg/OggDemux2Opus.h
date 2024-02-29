#pragma once

#define OGG_DMUX_TO_OPUS_RET_SUCCESS 0
#define OGG_DMUX_TO_OPUS_RET_FAIL -1
#define OGG_DMUX_TO_OPUS_RET_HEAD_ERROR 2

typedef int OggDmux2OpusRet;
int GetOggDemux2OpusStateSize();
void OggDemux2OpusStateInit(void* hd);
OggDmux2OpusRet OggDemux2OpusRun(void* hd, char* in, int inByte, char* out, int* outByte);
void OggDemux2OpusEnd(void* hd);


