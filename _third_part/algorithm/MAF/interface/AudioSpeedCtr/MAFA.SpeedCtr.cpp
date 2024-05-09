#if 1
#include "MAFA.SpeedCtr.h"
#include "MAF.Objects.h"
#include "MAF.String.h"
#include "AudioSpeedControl16.h"

maf_void maf_auio_speedCtr_register()
{
	MAF_Object::Registe<MAFA_SpeedCtr>("auio_speedCtr");
}

MAFA_SpeedCtr::MAFA_SpeedCtr()
{
}
MAFA_SpeedCtr::~MAFA_SpeedCtr()
{
}

maf_int32 MAFA_SpeedCtr::Init()
{
	MAF_PRINT();
	
#if 1
	_hdSize = AudioSpeedControl_GetStateSize16b();
	_hd = _memory.Malloc(_hdSize);
	if (!_hd) {
		return -1;
	}
	MAF_PRINT("_hd=%x,size:%d", (unsigned)_hd, _hdSize);
	AudioSpeedInitParam param;

#if 1
	param.fsHz = _rate;
	param.width = _width;
	param.channels = _ch;
	if (_isSpeech)
	{
		// use settings for speech processing
		param.seekMs = 15;
		param.overlapMs = 8;
		param.constMs = 24;
	}
	else
	{
		// use settings for speech processing
		param.seekMs = 15;
		param.overlapMs = 8;
		param.constMs = 24;
	}

	param.speed = _speed;
#endif
	AudioSpeedControl_Init16b(_hd, &param);

	int32_t size;
	maf_void* buf;
	size = 10 * 1024;
	buf = _memory.Malloc(size);
	_oDataCache.Init((maf_uint8*)buf, 0, size);
	
#endif
	return 0;
}

maf_int32 MAFA_SpeedCtr::Deinit()
{
	MAF_PRINT();
#if 1
	MAF_PRINT("_hd=%x", (unsigned)_hd);
	AudioSpeedControl_DeInit16b(_hd);
	_memory.Free(_hd);
#endif
	return 0;
}

maf_int32 MAFA_SpeedCtr::Process(MAF_Data* dataIn, MAF_Data* dataOut)
{
	static int num = 0;
#if 1
	int32_t inUsed;
	int32_t outLen;
	
	outLen = _oDataCache.GetLeftSize();
	AudioSpeedControl_Run16b(
		_hd,
		dataIn->GetData(),
		dataIn->GetSize(),
		_oDataCache.GetLeftData(),
		&outLen);
	_oDataCache.Append(outLen);
	if (_oDataCache.GetSize() > 0)
	{
		maf_int32 appendSize = _oDataCache.GetSize();
		appendSize = appendSize > dataOut->GetLeftSize() ? dataOut->GetLeftSize() : appendSize;
		dataOut->Append(_oDataCache.GetData(), appendSize);
		_oDataCache.Used(appendSize);
		_oDataCache.ClearUsed();
	}

	//MAF_PRINT("[%d],isize:%d,osize:%d", num++, dataIn->GetSize(), dataOut->GetSize());
	dataIn->Used(dataIn->GetSize());
#endif
	return 0;
}

maf_int32 MAFA_SpeedCtr::Set(const maf_int8* key, maf_void* val)
{
	if (MAF_String::StrCompare(key, "speedQ8")){
		_speed = (maf_float)(maf_uint32)val / (1 << 8); return 0;
	}
	return MAF_Audio::Set(key, val);
}

maf_int32 MAFA_SpeedCtr::Get(const maf_int8* key, maf_void* val)
{
	return MAF_Audio::Get(key, val);
}


#endif