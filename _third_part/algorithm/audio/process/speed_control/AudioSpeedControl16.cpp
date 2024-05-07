#if 1
#include"AudioSpeedControl16.h"
#include"AudioSpeedControlInner16.h"
#include"Algo.AS_Calculator16.h"
static AS_Calculator16 asCalculator;
int32_t TDStretchGetDealInMinSamlpes(TDStretch* pTDStretch)
{
    int32_t samples1 = (pTDStretch->seekSamples + pTDStretch->overlapSamples + pTDStretch->constSamples + pTDStretch->overlapSamples);
    int32_t samples2 = (int32_t)(pTDStretch->speed * (pTDStretch->overlapSamples + pTDStretch->constSamples));
    return MAX(samples1, samples2);
}

AudioSpeedControlRet AudioSpeedControl_RunInner16b(TDStretch* pTDStretch, AudioSamples *pIn, AudioSamples* pOut)
{
#if 1
    int32_t offset = 0;
    if (pOut->GetLeftSamples() < (pTDStretch->overlapSamples + pTDStretch->constSamples))
        return AUDIO_SPEED_CONTROL_Ret_INPUT_ERROR;
    if (pTDStretch->isBeginning == true)
    {
        pTDStretch->isBeginning = false;
        pTDStretch->bufferTemplate.Append(*pIn, pIn->GetUsedSamples(), pTDStretch->overlapSamples);
    }
    int32_t bestLag;
    bestLag = asCalculator.WaveFormMatch(
        AS_Calculator::WaveformMatchChoose_e::WAVEFORM_MATCH_SUM,
        *pIn,
        pIn->GetUsedSamples(),
        pTDStretch->bufferTemplate,
        0,
        pTDStretch->seekSamples,
        pTDStretch->overlapSamples);

    offset = bestLag;
    asCalculator.OverlapAdd(
        pTDStretch->bufferTemplate,
        0,
        *pIn,
        pIn->GetUsedSamples() + offset,
        pTDStretch->overlapSamples);
    pOut->Append(
        pTDStretch->bufferTemplate,
        0,
        pTDStretch->overlapSamples);
    offset += pTDStretch->overlapSamples;

    //copy constSamples from in to out
    pOut->Append(
        *pIn,
        pIn->GetUsedSamples() + offset,
        pTDStretch->constSamples);
    offset += pTDStretch->constSamples;

    //updata bufferTemplate
    pTDStretch->bufferTemplate.Clear(pTDStretch->bufferTemplate.GetValidSamples());
    pTDStretch->bufferTemplate.Append(*pIn, pIn->GetUsedSamples() + offset, pTDStretch->overlapSamples);

    pIn->Clear(pTDStretch->speed*(pTDStretch->overlapSamples+ pTDStretch->constSamples));
    
#else
#if 0
    pOut->Append(*pIn, 0, pIn->GetValidSamples()>>1);
    pIn->Used(pIn->GetValidSamples());
#endif
#endif
    return AUDIO_SPEED_CONTROL_Ret_SUCCESS;
}

EXTERNC{

    int32_t AudioSpeedControl_GetStateSize16b()
    {
        return sizeof(TDStretch);
    }
    
    AudioSpeedControlRet AudioSpeedControl_Init16b(void* pMusicPlcStateIn, AudioSpeedInitParam *param)
    {
        TDStretch* pTDStretch = (TDStretch*)pMusicPlcStateIn;
        memset(pTDStretch, 0, sizeof(TDStretch));
        pTDStretch->channels = param->channels;
        pTDStretch->sampleRate = param->fsHz;
        pTDStretch->sampleWidth = param->width;
        pTDStretch->seekMs = param->seekMs;
        pTDStretch->overlapMs = param->overlapMs;
        pTDStretch->constMs = param->constMs;
        pTDStretch->seekSamples = pTDStretch->seekMs * pTDStretch->sampleRate/ 1000;
        pTDStretch->overlapSamples = pTDStretch->overlapMs * pTDStretch->sampleRate / 1000;
        pTDStretch->constSamples = pTDStretch->constMs * pTDStretch->sampleRate / 1000;
        pTDStretch->speed = param->speed;
        pTDStretch->isBeginning = true;
        
        i32 samples;
        u8* buf;
        pTDStretch->info.Init(pTDStretch->sampleRate, pTDStretch->sampleWidth, pTDStretch->channels);

        samples = pTDStretch->overlapMs * pTDStretch->sampleRate / 1000;
        buf = (u8*)AudioSpeedControl_Malloc(samples * pTDStretch->info._bytesPerSample);
        pTDStretch->bufferTemplate.Init(&pTDStretch->info, buf, samples);

        samples = TDStretchGetDealInMinSamlpes(pTDStretch);
        buf= (u8*)AudioSpeedControl_Malloc(samples* pTDStretch->info._bytesPerSample);
        pTDStretch->iCache.Init(&pTDStretch->info, buf, samples);

        pTDStretch->isEnd = false;
        return AUDIO_SPEED_CONTROL_Ret_SUCCESS;
    }

    AudioSpeedControlRet AudioSpeedControl_Set(void* pMusicPlcStateIn, AudioSpeedControl_SetChhoose_e choose, void* val)
    {
        TDStretch* pTDStretch = (TDStretch*)pMusicPlcStateIn;
        switch (choose)
        {
        case AUDIO_SPEED_CONTROL_SET_CHOOSE_SPEEDQ8:
            pTDStretch->speed = ((float)(uint32_t)val) / (1 >> 8);
        default:
            break;
        }
        return AUDIO_SPEED_CONTROL_Ret_SUCCESS;
    }
    AudioSpeedControlRet AudioSpeedControl_Run16b(void* hd, int8_t* in, int32_t inSize, int8_t* out, int32_t* outSize)
    {
        TDStretch* pTDStretch = (TDStretch*)hd;

        AudioSamples pIn;
        pIn.Init(&pTDStretch->info, (u8*)in, inSize / pTDStretch->info._bytesPerSample);
        pIn.Append(pIn.GetSamplesMax());
        AudioSamples pOut;
        pOut.Init(&pTDStretch->info, (u8*)out, *outSize / pTDStretch->info._bytesPerSample);
        i32 outOffMax = *outSize;
        *outSize = 0;
        i32 outOff = 0;
        int32_t outSizee = 0;
        i32 inUsed = 0;
        while (1)
        {
            i32 AppendSamples;
            AppendSamples = pTDStretch->iCache.GetSamplesMax() - pTDStretch->iCache.GetValidSamples();
            AppendSamples = AppendSamples < pIn.GetValidSamples() ? AppendSamples : pIn.GetValidSamples();
            pTDStretch->iCache.Append(pIn, pIn.GetUsedSamples(), AppendSamples);
            pIn.Used(AppendSamples);
            if (!pTDStretch->iCache.IsFull()) {
                break;
            }
            outSizee = outOffMax - outOff;
            AudioSpeedControl_RunInner16b(pTDStretch, &pTDStretch->iCache, &pOut);
            //pTDStretch->iCache.Clear(pTDStretch->iCache.GetValidSamples());
            //pTDStretch->iCache.ClearUsed();

        }
        *outSize = pOut.GetValidSamples() * pTDStretch->info._bytesPerSample;
        
        return AUDIO_SPEED_CONTROL_Ret_SUCCESS;
    }

    AudioSpeedControlRet AudioSpeedControl_DeInit16b(void* pMusicPlcStateIn)
    {
        TDStretch* pTDStretch = (TDStretch*)pMusicPlcStateIn;
        AudioSpeedControl_Free(&pTDStretch->bufferTemplate[0]);

        AudioSpeedControl_Free(&pTDStretch->iCache[0]);
        return AUDIO_SPEED_CONTROL_Ret_SUCCESS;
    }
}

#endif

