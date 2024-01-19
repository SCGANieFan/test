#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include"../inc/SilkDec.h"
#include "SKP_Silk_SDK_API.h"

//#define MAX_BYTES_PER_FRAME     1024
#define MAX_INPUT_FRAMES        5
//#define MAX_FRAME_LENGTH        480
//#define FRAME_LENGTH_MS         20
//#define MAX_API_FS_KHZ          48
//#define MAX_LBRR_DELAY          2

int SilkDecoderSize()
{
    int decSizeBytes;
    SKP_Silk_SDK_Get_Decoder_Size(&decSizeBytes);
    return decSizeBytes;
}

int SilkDecoderControlSize()
{
    return sizeof(SKP_SILK_SDK_DecControlStruct);
}

int SilkDecoderCreate(void* hd, void* decControl, SilkDecInfo_t* pInfo)
{
    //int decSizeBytes;
    //SKP_Silk_SDK_Get_Decoder_Size(&decSizeBytes);
    //*hd = (void*)malloc(sizeof(char)*decSizeBytes);
    //memset(hd, 0, sizeof(char) * decSizeBytes);

    //SKP_SILK_SDK_DecControlStruct* pDecControl = (SKP_SILK_SDK_DecControlStruct*)malloc(sizeof(SKP_SILK_SDK_DecControlStruct));
    SKP_SILK_SDK_DecControlStruct* pDecControl = (SKP_SILK_SDK_DecControlStruct*)decControl;
    //memset(pDecControl, 0, sizeof(SKP_SILK_SDK_DecControlStruct));
    
    pDecControl->API_sampleRate = pInfo->sample_rate;

    /* Set the samplingrate that is requested for the output */
    if (pDecControl->API_sampleRate == 0) {
        pDecControl->API_sampleRate = 24000;
    }
    /* Initialize to one frame per packet, for proper concealment before first packet arrives */
    pDecControl->framesPerPacket = 1;

    /* Reset decoder */
    SKP_Silk_SDK_InitDecoder(hd);
    //*decControl = (void*)pDecControl;
    return 0;
}
int SilkDec(void* hd, void* decControl, SilkDecIn_t* pIn)
{
    if (hd == NULL
        || decControl == NULL
        || pIn == NULL)
    {
        printf("input have null\n");
    }
    SKP_SILK_SDK_DecControlStruct* pDecControl = (SKP_SILK_SDK_DecControlStruct*)decControl;
    //unsigned char* bitIn;
    //unsigned int* bitInLen;
    //short* pcmOut;
    //unsigned int pcmOutLen;
    /* Read payload size */
    short nBytes = (short)pIn->bitInLen;
    /* Read payload */
    unsigned char* payloadToDec = pIn->bitIn;
    int i;
    short ret, len, tot_len;
    short* outPtr;
    int frames, lost;

    /* Silk decoder */
    outPtr = (short*)pIn->pcmOut;
    tot_len = 0;
    if (nBytes > 0)
        lost = 0;
    else
        lost = 1;

    if (lost == 0) {
        /* No Loss: Decode all frames in the packet */
        frames = 0;
        do {
            /* Decode 20 ms */
            ret = SKP_Silk_SDK_Decode(hd, pDecControl, 0, payloadToDec, nBytes, outPtr, &len);
            if (ret) {
                printf("\nSKP_Silk_SDK_Decode returned %d", ret);
            }

            frames++;
            outPtr += len;
            tot_len += len;
            if (frames > MAX_INPUT_FRAMES) {
                /* Hack for corrupt stream that could generate too many frames */
                outPtr = (short*)pIn->pcmOut;
                tot_len = 0;
                frames = 0;
            }
            /* Until last 20 ms frame of packet has been decoded */
        } while (pDecControl->moreInternalDecoderFrames);
    }
    else {
        /* Loss: Decode enough frames to cover one packet duration */
        for (i = 0; i < pDecControl->framesPerPacket; i++) {
            /* Generate 20 ms */
            ret = SKP_Silk_SDK_Decode(hd, pDecControl, 1, payloadToDec, nBytes, outPtr, &len);
            if (ret) {
                printf("\nSKP_Silk_Decode returned %d", ret);
            }
            outPtr += len;
            tot_len += len;
        }
    }
    *pIn->pcmOutLen = tot_len;
    return 0;
}






