#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include"../inc/SilkEnc.h"
#include "SKP_Silk_SDK_API.h"

#define MAX_API_FS_KHZ          48

int SilkEncoderSize()
{
    int encSizeBytes;
    SKP_Silk_SDK_Get_Encoder_Size(&encSizeBytes);
    return encSizeBytes;
}


int SilkEncoderControlSize()
{
    return sizeof(SKP_SILK_SDK_EncControlStruct);
}

int SilkEncoderInit(void* hd, void* encControl, SilkEncInfo_t* pInfo)
{
    /* Create Encoder */
    //int encSizeBytes;
    //SKP_Silk_SDK_Get_Encoder_Size(&encSizeBytes);
    //*hd = (void*)malloc(sizeof(char) * encSizeBytes);

    //memset(hd, 0, sizeof(char)* encSizeBytes);
    /* Create Encoder Control*/
    //SKP_SILK_SDK_EncControlStruct* pEncControl = (SKP_SILK_SDK_EncControlStruct*)malloc(sizeof(SKP_SILK_SDK_EncControlStruct));
    SKP_SILK_SDK_EncControlStruct* pEncControl = (SKP_SILK_SDK_EncControlStruct*)encControl;
    //memset(pEncControl, 0, sizeof(SKP_SILK_SDK_EncControlStruct));
    /* Set Encoder parameters */
    pEncControl->API_sampleRate = pInfo->sample_rate;
    pEncControl->maxInternalSampleRate = pInfo->max_internal_fs_Hz;
    pEncControl->packetSize = (pInfo->packetSize_ms * pInfo->sample_rate) / 1000;
    pEncControl->packetLossPercentage = pInfo->packetLoss_perc;
    pEncControl->useInBandFEC = pInfo->INBandFEC_enabled;
    pEncControl->useDTX = pInfo->DTX_enabled;
    pEncControl->complexity = pInfo->complexity;
    pEncControl->bitRate = (pInfo->bit_rate > 0 ? pInfo->bit_rate : 0);
    /* If no max internal is specified, set to minimum of API fs and 24 kHz */
    if (pEncControl->maxInternalSampleRate == 0)
    {
        pEncControl->maxInternalSampleRate = 24000;
        if (pEncControl->API_sampleRate < pEncControl->maxInternalSampleRate)
        {
            pEncControl->maxInternalSampleRate = pEncControl->API_sampleRate;
        }
    }
    if (pEncControl->API_sampleRate > MAX_API_FS_KHZ * 1000 || pEncControl->API_sampleRate < 0)
    {
        return -1;
    }
    /* Reset Encoder     */
    int ret;
    SKP_SILK_SDK_EncControlStruct* encStatus = (SKP_SILK_SDK_EncControlStruct*)malloc(sizeof(SKP_SILK_SDK_EncControlStruct));
    ret = SKP_Silk_SDK_InitEncoder(hd, encStatus);
    if (ret)
    {
        return -1;
    }
    free(encStatus);
    //*encControl = (void*)pEncControl;
    return 0;
}
int SilkEnc(void* hd, void* encControl, SilkEncIn_t* pIn)
{
    if (pIn->pcmInLen % 2 == 1)
    {
        printf("in_length is error, in_length = %d\n", pIn->pcmInLen);
    }
    int ret;
    /* Silk Encoder */
    ret = SKP_Silk_SDK_Encode(hd, (SKP_SILK_SDK_EncControlStruct*)encControl, pIn->pcmIn, pIn->pcmInLen, pIn->bitOut, (SKP_int16*)pIn->bitOutLen);

    if (ret) {
        printf("SKP_Silk_Encode returned %d \n", ret);
    }
    return ret;
}
