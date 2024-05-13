#if 0
#include"Algo.AS_Calculator.h"
#include"AudioResample.h"

typedef struct {
    BasePorting* basePorting;
#if 0
    AudioInfo info;
    int32_t iFs;
    int32_t oFs;
#else
    AudioInfo iInfo;
    AudioInfo oInfo;
#endif
#if 0
    int32_t iSampleIndex;
    int32_t oSampleIndex;
#endif
    AS_Calculator asCalculator;
    b1 isInited;
}AudioResampleState;

#if 0
STATIC INLINE int32_t AudioResample_InMinSamlpes(AudioResampleState* pState)
{
    int32_t samples1 = (pState->seekSamples + pState->overlapSamples + pState->constSamples + pState->overlapSamples);
    int32_t samples2 = (int32_t)(pState->speed * (pState->overlapSamples + pState->constSamples));
    return MAX(samples1, samples2);
}
AudioResampleRet AudioResample_RunInner(AudioResampleState* pState, AudioSamples* pIn, AudioSamples* pOut)
{
    int32_t offset = 0;
    if (pOut->GetLeftSamples() < (pState->overlapSamples + pState->constSamples))
        return AUDIO_RESAMPLE_RET_OUT_BUFF_NOT_ENOUGH;
    if (pState->isBeginning == true) {
        pState->isBeginning = false;
        pState->tmpBuf.Append(*pIn, pIn->GetUsedSamples(), pState->overlapSamples);
    }
    int32_t bestLag;
    bestLag = pState->asCalculator.WaveFormMatch(
        AS_Calculator::WaveformMatchChoose_e::WAVEFORM_MATCH_SUM,
        *pIn,
        pIn->GetUsedSamples(),
        pState->tmpBuf,
        0,
        pState->seekSamples,
        pState->overlapSamples);

    pState->asCalculator.OverlapAdd(
        pState->tmpBuf,
        0,
        *pIn,
        pIn->GetUsedSamples() + bestLag,
        pState->overlapSamples);

    pOut->Append(
        pState->tmpBuf,
        0,
        pState->overlapSamples);

    offset += bestLag + pState->overlapSamples;

    //copy constSamples from in to out
    pOut->Append(
        *pIn,
        pIn->GetUsedSamples() + offset,
        pState->constSamples);
    offset += pState->constSamples;

    //updata tmpBuf
    pState->tmpBuf.Clear(pState->tmpBuf.GetValidSamples());
    pState->tmpBuf.Append(*pIn, pIn->GetUsedSamples() + offset, pState->overlapSamples);

    pIn->Clear(pState->speed * (pState->overlapSamples + pState->constSamples));
    return AUDIO_RESAMPLE_RET_SUCCESS;
}
#endif

EXTERNC{

    int32_t AudioResample_GetStateSize()
    {
        return sizeof(AudioResampleState);
    }

    AudioResampleRet AudioResample_Init(void* pStateIn, AudioResampleInitParam* param)
    {
        //check
        if (!pStateIn
            || !param)
            return AUDIO_RESAMPLE_RET_INPUT_ERROR;

        if(!param->basePorting)
            return AUDIO_RESAMPLE_RET_INPUT_ERROR;

        AudioResampleState* pState = (AudioResampleState*)pStateIn;
        param->basePorting->MemSet((u8*)pState, 0, sizeof(AudioResampleState));
        
        pState->basePorting = param->basePorting;

        pState->iInfo.Init(param->iFs, param->width, param->channels);
        pState->oInfo.Init(param->oFs, param->width, param->channels);

        pState->asCalculator.Init(param->width);

        pState->isInited = true;
        return AUDIO_RESAMPLE_RET_SUCCESS;
    }

    AudioResampleRet AudioResample_Set(void* pStateIn, AudioResample_SetChhoose_e choose, void** val)
    {
        //check
        if (!pStateIn
            ||choose >= AUDIO_RESAMPLE_SET_CHOOSE_MAX
            || !val)
            return AUDIO_RESAMPLE_RET_INPUT_ERROR;
        AudioResampleState* pState = (AudioResampleState*)pStateIn;
        if (pState->isInited == false)
            return AUDIO_RESAMPLE_RET_FAIL;

        switch (choose)
        {
#if 0
        case AUDIO_RESAMPLE_SET_CHOOSE_SPEEDQ8:
            pState->speed = (f32)(u32)val[0] / (1 << 8);
#endif
        default:
            break;
        }
        return AUDIO_RESAMPLE_RET_SUCCESS;
    }

    AudioResampleRet AudioResample_Get(void* pStateIn, AudioResample_GetChhoose_e choose, void** val)
    {
        //check
        if (!pStateIn
            || choose >= AUDIO_RESAMPLE_GET_CHOOSE_MAX
            || !val)
            return AUDIO_RESAMPLE_RET_INPUT_ERROR;
        AudioResampleState* pState = (AudioResampleState*)pStateIn;
        if (pState->isInited == false)
            return AUDIO_RESAMPLE_RET_FAIL;

        switch (choose)
        {
#if 0
        case AUDIO_RESAMPLE_GET_CHOOSE_SPEEDQ8:
            *((u32*)(val[0])) = (u32)(pState->speed * (1 << 8));
#endif
        default:
            break;
        }
        return AUDIO_RESAMPLE_RET_SUCCESS;
    }

#define ONE_Q15	((i32)1 << 15)
#define MASK_Q15 (ONE_Q15 - 1)
    AudioResampleRet AudioResample_Run(void* hd, uint8_t * in, int32_t *inSize, uint8_t * out, int32_t * outSize)
    {
        //check
        if (!hd
            || !in
            || !inSize
            || !out
            || !outSize)
            return AUDIO_RESAMPLE_RET_INPUT_ERROR;
        if (*inSize < 1
            || *outSize < 1)
            return AUDIO_RESAMPLE_RET_INPUT_ERROR;

        AudioResampleState* pState = (AudioResampleState*)hd;
        Buffer buffer;
        AudioSamples pIn;
        buffer._buf = in;
        buffer._max = *inSize;
        pIn.Init(&pState->iInfo, &buffer);
        pIn.Append(pIn.GetSamplesMax());

        AudioSamples pOut;
        buffer._buf = out;
        buffer._max = *outSize;
        pOut.Init(&pState->oInfo, &buffer);

        //pState->asCalculator.

        i32 KQ15 = (i32)(((i64)pIn._info->_rate << 15) / pOut._info->_rate);
        i32 oSamplesMax = pOut.GetLeftSamples();
        i32 oSamples;
        for (oSamples = 0; oSamples < oSamplesMax; oSamples++)
        {
            i32 iSamplesQ15 = oSamples * KQ15;
            i32 iSamplesFloor = iSamplesQ15 >> 15;
            i32 iSamplesCeil = (iSamplesQ15 + MASK_Q15) >> 15;
            if (iSamplesCeil >= pIn.GetValidSamples())
                break;

            i32 iSamplesFracFloorQ15 = iSamplesQ15 & MASK_Q15;
            i32 iSamplesFracCeilQ15 = ONE_Q15 - iSamplesQ15 & MASK_Q15;


            //produce add
            auto pIn0 = (T*)&pIn[iSamplesFloor];
            auto pIn1 = (T*)&pIn[iSamplesCeil];
            auto pOut = (T*)&pOut[pOut.GetUsedSamples() + pOut._validSamples()];

            for (i16 ch = 0; ch < in._info->_channels; ch++)
            {
                *pOut = (T)(((i64)(*pIn0) * iSamplesFracCeilQ15 + (i64)(*pIn1) * iSamplesFracFloorQ15) >> 15);
            }

            out.Used(1);
        }
        oSamples + 1
            in.Used()



        return AUDIO_RESAMPLE_RET_SUCCESS;
    }

    AudioResampleRet AudioResample_DeInit(void* pStateIn)
    {
#if 0
        AudioResampleState* pState = (AudioResampleState*)pStateIn;
#endif
        return AUDIO_RESAMPLE_RET_SUCCESS;
    }
}
#endif

