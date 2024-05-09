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
    int32_t offset = 0;
    if (pOut->GetLeftSamples() < (pTDStretch->overlapSamples + pTDStretch->constSamples))
        return AUDIO_SPEED_CONTROL_RET_OUT_BUFF_NOT_ENOUGH;
    if (pTDStretch->isBeginning == true) {
        pTDStretch->isBeginning = false;
        pTDStretch->tmpBuf.Append(*pIn, pIn->GetUsedSamples(), pTDStretch->overlapSamples);
    }
    int32_t bestLag;
    bestLag = asCalculator.WaveFormMatch(
        AS_Calculator::WaveformMatchChoose_e::WAVEFORM_MATCH_SUM,
        *pIn,
        pIn->GetUsedSamples(),
        pTDStretch->tmpBuf,
        0,
        pTDStretch->seekSamples,
        pTDStretch->overlapSamples);

    asCalculator.OverlapAdd(
        pTDStretch->tmpBuf,
        0,
        *pIn,
        pIn->GetUsedSamples() + bestLag,
        pTDStretch->overlapSamples);

    pOut->Append(
        pTDStretch->tmpBuf,
        0,
        pTDStretch->overlapSamples);

    offset += bestLag + pTDStretch->overlapSamples;

    //copy constSamples from in to out
    pOut->Append(
        *pIn,
        pIn->GetUsedSamples() + offset,
        pTDStretch->constSamples);
    offset += pTDStretch->constSamples;

    //updata tmpBuf
    pTDStretch->tmpBuf.Clear(pTDStretch->tmpBuf.GetValidSamples());
    pTDStretch->tmpBuf.Append(*pIn, pIn->GetUsedSamples() + offset, pTDStretch->overlapSamples);

    pIn->Clear(pTDStretch->speed * (pTDStretch->overlapSamples + pTDStretch->constSamples));

    return AUDIO_SPEED_CONTROL_RET_SUCCESS;
}

EXTERNC{

    int32_t AudioSpeedControl_GetStateSize16b()
    {
        return sizeof(TDStretch);
    }
    
    AudioSpeedControlRet AudioSpeedControl_Init16b(void* pMusicPlcStateIn, AudioSpeedInitParam *param)
    {
        //check
        if (!pMusicPlcStateIn
            || !param)
            return AUDIO_SPEED_CONTROL_RET_INPUT_ERROR;


        TDStretch* pTDStretch = (TDStretch*)pMusicPlcStateIn;
        
        AudioSpeedControl_MemSet((u8*)pTDStretch, 0, sizeof(TDStretch));
        pTDStretch->info.Init(param->fsHz, param->width, param->channels);
        pTDStretch->seekSamples = param->seekMs * pTDStretch->info._rate / 1000;
        pTDStretch->overlapSamples = param->overlapMs * pTDStretch->info._rate / 1000;
        pTDStretch->constSamples = param->constMs * pTDStretch->info._rate / 1000;

        BufferSamples bufferSamples;

        bufferSamples._samples = pTDStretch->overlapSamples ;
        bufferSamples._buf = (u8*)AudioSpeedControl_Malloc(bufferSamples._samples * pTDStretch->info._bytesPerSample);
        pTDStretch->tmpBuf.Init(&pTDStretch->info, &bufferSamples);
      
        bufferSamples._samples = TDStretchGetDealInMinSamlpes(pTDStretch) * pTDStretch->info._bytesPerSample;
        bufferSamples._buf = (u8*)AudioSpeedControl_Malloc(bufferSamples._samples * pTDStretch->info._bytesPerSample);
        pTDStretch->iCache.Init(&pTDStretch->info, &bufferSamples);

        pTDStretch->speed = param->speed;
        pTDStretch->isBeginning = true;
        pTDStretch->isInited = true;
        return AUDIO_SPEED_CONTROL_RET_SUCCESS;
    }

    AudioSpeedControlRet AudioSpeedControl_Set(void* pMusicPlcStateIn, AudioSpeedControl_SetChhoose_e choose, void* val)
    {
        //check
        if (!pMusicPlcStateIn
            || choose >= AUDIO_SPEED_CONTROL_SET_CHOOSE_MAX
            || !val)
            return AUDIO_SPEED_CONTROL_RET_INPUT_ERROR;

        TDStretch* pTDStretch = (TDStretch*)pMusicPlcStateIn;
        if (!pTDStretch->isInited == false)
            return AUDIO_SPEED_CONTROL_RET_FAIL;
        
        switch (choose)
        {
        case AUDIO_SPEED_CONTROL_SET_CHOOSE_SPEEDQ8:
            pTDStretch->speed = ((float)(uint32_t)val) / (1 >> 8);
        default:
            break;
        }
        return AUDIO_SPEED_CONTROL_RET_SUCCESS;
    }

    AudioSpeedControlRet AudioSpeedControl_Run16b(void* hd, uint8_t* in, int32_t inSize, uint8_t* out, int32_t* outSize)
    {
        //check
        if (!hd
            || !in
            || inSize < 0
            || !out)
            return AUDIO_SPEED_CONTROL_RET_INPUT_ERROR;

        TDStretch* pTDStretch = (TDStretch*)hd;

        Buffer buffer;

        AudioSamples pIn;
        buffer._buf = in;
        buffer._max = inSize;
        pIn.Init(&pTDStretch->info, &buffer);
        pIn.Append(pIn.GetSamplesMax());

        AudioSamples pOut;
        buffer._buf = out;
        buffer._max = *outSize;
        pOut.Init(&pTDStretch->info, &buffer);

        i32 usedSamples;
        AudioSpeedControlRet ret;
        while (1) {
            pTDStretch->iCache.AppendFully(pIn,&usedSamples);
            pIn.Used(usedSamples);
            if (!pTDStretch->iCache.IsFull()) {
                break;
            }
            ret = AudioSpeedControl_RunInner16b(pTDStretch, &pTDStretch->iCache, &pOut);
            if(ret!= AUDIO_SPEED_CONTROL_RET_SUCCESS)
                return ret;
        }
        *outSize = pOut.GetValidSamples() * pTDStretch->info._bytesPerSample;        
        return AUDIO_SPEED_CONTROL_RET_SUCCESS;
    }

    AudioSpeedControlRet AudioSpeedControl_DeInit16b(void* pMusicPlcStateIn)
    {
        TDStretch* pTDStretch = (TDStretch*)pMusicPlcStateIn;
        AudioSpeedControl_Free(&pTDStretch->tmpBuf[0]);

        AudioSpeedControl_Free(&pTDStretch->iCache[0]);
        return AUDIO_SPEED_CONTROL_RET_SUCCESS;
    }
}

#endif

