#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"OggDemux2Opus.h"

#define FILE_NAME_SRC "./tts_audio.opusx.ogg"
#define FILE_NAME_DST "./tts_audio.opusx"

#define READ_BYTE 1024
#define OUT_BUFF_BYTE 10*1024
int num = 0;
bool OggTest()
{
    char* buffer = (char*)malloc(READ_BYTE);
    char* outBuffer = (char*)malloc(OUT_BUFF_BYTE);
    int  bytes;
    FILE* fIn = fopen(FILE_NAME_SRC, "rb");
    FILE* fOut = fopen(FILE_NAME_DST, "wb");
    int hdSize = GetOggDemux2OpusStateSize();
    //printf("hdSize:%d\n", hdSize);
    void* hd = malloc(hdSize);
    OggDemux2OpusStateInit(hd);
    while (1)
    {
        bytes = fread(buffer, 1, READ_BYTE, fIn);
        if (bytes < 1)
            break;
        int outByte = OUT_BUFF_BYTE;
        OggDmux2OpusRet ret=OggDemux2OpusRun(hd, (char*)buffer, bytes, (char*)outBuffer, &outByte);
        num++;
        //printf("[%d] iByte:%d,oByte:%d\n", num,bytes, outByte);
        if (ret == OGG_DMUX_TO_OPUS_RET_HEAD_ERROR)
        {
            //it is not ogg file
        }
        fwrite(outBuffer, 1, outByte, fOut);
    }
    OggDemux2OpusEnd(hd);
    fclose(fIn);
    fclose(fOut);
    return true;
}


