#if 1
#include "MAFA.SbcDec.h"
#include "MAF.Objects.h"
#include "MAF.String.h"
#include "sbc_dec.h"

maf_void maf_sbc_dec_register()
{
	MAF_Object::Registe<MAFA_SbcDec>("sbc_dec");
}

maf_void* MAFA_SbcDec::_malloc = 0;
maf_void* MAFA_SbcDec::_free = 0;

MAFA_SbcDec::MAFA_SbcDec()
{
}
MAFA_SbcDec::~MAFA_SbcDec()
{
}

maf_int32 MAFA_SbcDec::Init()
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
	return 0;
#else
	_pInfo = _memory.Malloc(sizeof(sbc_info_t));
	maf_int32 _hdSize = sbc_decoder_get_size();
	_hd = _memory.Malloc(_hdSize);
	MAF_PRINT("_hd=%x,size:%d", (maf_uint32)_hd, _hdSize);
	if (!_hd){
		return -1;
	}
	bool ret = sbc_decoder_init(_hd);
	if(!ret){
		return -1;
	}
	return 0;
#endif
}

maf_int32 MAFA_SbcDec::Deinit()
{
	MAF_PRINT();
#if 0
	MusicPlc_StateDeInit(_hd);
	_memory.Free(_hd);
	_memory.Free(_basePorting);
#else
	_memory.Free(_hd);
	_memory.Free(_pInfo);
#endif

	return 0;
}

maf_int32 MAFA_SbcDec::Process(MAF_Data* dataIn, MAF_Data* dataOut)
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
	return 0;
#else

#if 0
	typedef struct pcm_s16_t {
		int16_t *ch0,*ch1;
		uint16_t samples;
		uint16_t flags;//pcm_s16_flags_e
	} pcm_s16_t;
#endif
#if 1
	while (1)
	{
		pcm_s16_t pcm;
		MAF_MEM_SET(&pcm, 0, sizeof(pcm_s16_t));
		
		pcm.ch0 = (int16_t*)&_ch0[0];
		pcm.ch1 = (int16_t*)&_ch1[0];
		
		maf_int32 ret = sbc_decode(_hd, dataIn->GetData(), dataIn->GetSize(), &pcm);

		if (ret <= 0)
			break;
		if (_isFirst) {
			_isFirst = false;
			sbc_decoder_get_info(_hd, (sbc_info_t*)_pInfo);
		}

		//deinterlace
		if (_ch == 1) {
			maf_uint16* pOut = (maf_uint16*)dataOut->GetLeftData();
			for (maf_int32 s = 0; s < pcm.samples; s++) {
				pOut[0] = _ch0[s];
				pOut += 1;
			}
		}
		else if (_ch == 2) {
			maf_uint16* pOut0 = (maf_uint16*)dataOut->GetLeftData();
			maf_uint16* pOut1 = (maf_uint16*)dataOut->GetLeftData() + 1;
			for (maf_int32 s = 0; s < pcm.samples; s++) {
				pOut0[0] = _ch0[s];
				pOut1[0] = _ch1[s];
				pOut0 += 2;
				pOut1 += 2;
			}
		}
		dataIn->Used(((sbc_info_t*)_pInfo)->vlc_size);
		dataIn->ClearUsed();
		dataOut->Append(pcm.samples* _ch* _width);
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

maf_int32 MAFA_SbcDec::Set(const maf_int8* key, maf_void* val)
{
#if 0
	if (MAF_String::StrCompare(key, "decayMs")) {
		_decayMs = (maf_int16)val; return 0;
	}
	else if (MAF_String::StrCompare(key, "gainMs")) {
		_gainMs = (maf_int16)val; return 0;
	}
	else if (MAF_String::StrCompare(key, "overlapMs")) {
		_overlapMs = (maf_int16)val; return 0;
	}
#endif
	return MAF_Audio::Set(key, val);
}

maf_int32 MAFA_SbcDec::Get(const maf_int8* key, maf_void* val)
{
	return MAF_Audio::Get(key, val);
}

typedef void* (*Malloc_cb)(maf_int32);
typedef void (*Free_cb)(maf_void*);
maf_void* MAFA_SbcDec::MallocLocal(int32_t size)
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

maf_void MAFA_SbcDec::FreeLocal(maf_void* block)
{
#if 1
	MAF_PRINT("free, ptr:%x", (maf_uint32)block);
#endif
	return ((Free_cb)_free)(block);
}

#endif