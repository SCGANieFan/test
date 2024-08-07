#if 1
#include "MAFA.SbcEnc.h"
#include "MAF.Objects.h"
#include "MAF.String.h"
#include "sbc_enc.h"

maf_void maf_sbc_enc_register()
{
	MAF_Object::Registe<MAFA_SbcEnc>("sbc_enc");
}

maf_void* MAFA_SbcEnc::_malloc = 0;
maf_void* MAFA_SbcEnc::_free = 0;

MAFA_SbcEnc::MAFA_SbcEnc()
{
}
MAFA_SbcEnc::~MAFA_SbcEnc()
{
}

maf_int32 MAFA_SbcEnc::Init()
{
	MAF_PRINT();
#if 0
	MusicPlcInitParam initParam;
	MAF_MEM_SET(&initParam, 0, sizeof(MusicPlcInitParam));
#if 1
	_malloc = _memory.GetMalloc();
	_free = _memory.GetFree();

	_basePorting = _memory.Malloc(sizeof(AlgoBasePorting));
	AlgoBasePorting* basePorting = (AlgoBasePorting*)_basePorting;

	basePorting->Malloc = (ALGO_Malloc_t)MallocLocal;
	basePorting->Free = (ALGO_Free_t)FreeLocal;
	initParam.basePorting = basePorting;
#endif
	initParam.fsHz = _rate;
	initParam.channels = _ch;
	initParam.width = _width;
	initParam.frameSamples = _frameSamples;
	initParam.overlapSamples = _overlapMs * _rate / 1000;
	initParam.attenuateSamplesAfterLost = _decayMs * _rate / 1000;
	initParam.gainSamplesAfterNoLost= _gainMs * _rate / 1000;
	initParam.seekSamples = 8 *_rate / 1000;
	initParam.matchSamples = 3 *_rate / 1000;
	_hdSize = MusicPlc_GetStateSize();
	_hd = _memory.Malloc(_hdSize);
	MAF_PRINT("_hd=%x,size:%d", (maf_uint32)_hd, _hdSize);
	if (!_hd)
	{
		return -1;
	}

	maf_int32 ret = MusicPlc_StateInit(_hd, &initParam);

	if (ret < 0)
	{
		return -1;
	}
#else
	_hdSize  = sbc_encoder_get_size();
	_hd = _memory.Malloc(_hdSize);
	_pInfo = _memory.Malloc(_hdSize);
	sbc_info_t *pInfo = (sbc_info_t *)_pInfo;
	MAF_MEM_SET(_pInfo,0,sizeof(sbc_info_t));
	pInfo->bitPool=_bitPool;
	switch (_rate)
	{
	case 16000:pInfo->sampleFreq=SbcSampleRate_e::SBC_SAMPLE_FREQ_16K;break;
	case 32000:pInfo->sampleFreq=SbcSampleRate_e::SBC_SAMPLE_FREQ_32K;break;
	case 44100:pInfo->sampleFreq=SbcSampleRate_e::SBC_SAMPLE_FREQ_44K1;break;
	case 48000:pInfo->sampleFreq=SbcSampleRate_e::SBC_SAMPLE_FREQ_48K;break;
	default:
		break;
	}
	pInfo->channelMode=(SbcChannelMode_e)_channelMode;
	pInfo->allocMethod=(SbcAllocMethod_e)_allocMethod;
	pInfo->numBlocks=_blocks;
	pInfo->numSubBands=_subBands;
	pInfo->numChannels=_ch;
	pInfo->sbcMode=(Sbc_Mode_E)_sbcMode;
	pInfo->sampleRate=_rate;
	// info.vlc_size=;
	// info.pcm_size=;
	// info.pcm_samples=;

	bool ret = sbc_encoder_init(_hd, pInfo);
	if(!ret){
		MAF_PRINT("sbc_encoder_init error");
		return -1;
	}
		

	return 0;
#endif
}

maf_int32 MAFA_SbcEnc::Deinit()
{
	MAF_PRINT();
#if 0
	MusicPlc_StateDeInit(_hd);
	_memory.Free(_hd);
	_memory.Free(_basePorting);
#endif
	_memory.Free(_hd);
	_memory.Free(_pInfo);
	return 0;
}

maf_int32 MAFA_SbcEnc::Process(MAF_Data* dataIn, MAF_Data* dataOut)
{
#if 0
	maf_int32 ret;

	maf_int32 outByte = dataOut->GetLeftSize();

	if (dataIn->CheckFlag(MAFA_FRAME_IS_EMPTY))
	{
#if 1
		dataIn->ClearFlag(MAFA_FRAME_IS_EMPTY);
		ret = MusicPlc_Run(
			_hd,
			NULL,
			0,
			dataOut->GetLeftData(),
			&outByte,
			true);
#else
		outByte= _frameSamples* _width *_ch;
		MAF_MEM_SET(dataOut->GetLeftData(), 0, outByte);
#endif
		dataOut->Append(outByte);
	}
	else
	{
#if 0
		ret = MusicPlc_Run(
			_hd,
			dataIn->GetData(),
			dataIn->GetSize(),
			dataOut->GetLeftData(),
			&outByte,
			false);
		dataOut->Append(outByte);
#else
		dataOut->Append(dataIn->GetData(), dataIn->GetSize());
		ret = MusicPlc_Run(
			_hd,
			dataOut->GetData(),
			dataOut->GetSize(),
			dataOut->GetData(),
			&outByte,
			false);
#endif
	}
	if (ret < 0)
	{
		return -1;
	}
	dataIn->Used(dataIn->GetSize());
	dataIn->ClearUsed();
#else
#if 1
	while (1) {
		if (dataIn->GetSize() < ((sbc_info_t*)_pInfo)->pcm_size) {
			break;
		}
		pcm_s16_t pcm;
		MAF_MEM_SET(&pcm, 0, sizeof(pcm_s16_t));


#if 0
		if (_ch == 1) {
			maf_uint16* pIn = (maf_uint16*)dataIn->GetData();
			for (maf_int32 s = 0; s < ((sbc_info_t*)_pInfo)->numBlocks * ((sbc_info_t*)_pInfo)->numSubBands; s++) {
				_ch0[s] = pIn[0];
				pIn++;
			}
		}
		else if (_ch == 2) {
			maf_uint16* pIn0 = (maf_uint16*)dataIn->GetData();
			maf_uint16* pIn1 = (maf_uint16*)dataIn->GetData() + 1;
			for (maf_int32 s = 0; s < ((sbc_info_t*)_pInfo)->numBlocks * ((sbc_info_t*)_pInfo)->numSubBands; s++) {
				_ch0[s] = pIn0[0];
				_ch1[s] = pIn1[0];
				pIn0+=2;
				pIn1+=2;
			}
		}
		pcm.ch0 = (maf_int16*)&_ch0[0];
		pcm.ch1 = (maf_int16*)&_ch1[0];
#else
		pcm.ch0 = (maf_int16*)dataIn->GetData();
		//pcm.ch1 = (maf_int16*)dataIn->GetData() + 1;
		pcm.flags = PCM_S16_FLAGS_INTERLACE;
#endif
		pcm.samples = dataIn->GetSize() / _ch/_width;
		int ret = sbc_encode(_hd, &pcm, dataOut->GetLeftData(), dataOut->GetLeftSize());
		dataOut->Append(((sbc_info_t*)_pInfo)->vlc_size);
		dataIn->Used(((sbc_info_t*)_pInfo)->pcm_size);
		dataIn->ClearUsed();
	}
	return 0;
#else
	
	dataOut->Append(dataIn->GetData(), dataIn->GetSize());
	dataIn->Used(dataIn->GetSize());
	dataIn->ClearUsed();
	return 0;
#endif
#endif
}

maf_int32 MAFA_SbcEnc::Set(const maf_int8* key, maf_void* val)
{
	if (MAF_String::StrCompare(key, "bitPool")) {
		_bitPool = (maf_uint8)val; return 0;
	}
	else if (MAF_String::StrCompare(key, "blocks")) {
		_blocks = (maf_uint8)val; return 0;
	}
	else if (MAF_String::StrCompare(key, "subBands")) {
		_subBands = (maf_uint8)val; return 0;
	}
	else if (MAF_String::StrCompare(key, "channelMode")) {
		_channelMode = (maf_uint8)val; return 0;
	}
	else if (MAF_String::StrCompare(key, "allocMethod")) {
		_allocMethod = (maf_uint8)val; return 0;
	}
	else if (MAF_String::StrCompare(key, "sbcMode")) {
		_sbcMode = (maf_uint8)val; return 0;
	}
	return MAF_Audio::Set(key, val);
}

maf_int32 MAFA_SbcEnc::Get(const maf_int8* key, maf_void* val)
{
	return MAF_Audio::Get(key, val);
}

typedef void* (*Malloc_cb)(maf_int32);
typedef void (*Free_cb)(maf_void*);
maf_void* MAFA_SbcEnc::MallocLocal(int32_t size)
{
#if 1
	static maf_int32 sizeTotal = 0;
	sizeTotal += size;
	maf_void* ptr = ((Malloc_cb)_malloc)(size);
	MAF_PRINT("malloc, ptr:%x, size:%d, sizeTotal:%d,", (maf_uint32)ptr, size, sizeTotal);
	return ptr;
#else
	return ((Malloc_cb)_malloc)(size);
#endif	
}

maf_void MAFA_SbcEnc::FreeLocal(maf_void* block)
{
#if 1
	MAF_PRINT("free, ptr:%x", (maf_uint32)block);
#endif
	return ((Free_cb)_free)(block);
}

#endif