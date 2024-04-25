#if 1
#include<string.h>

#include "MAFAA_MusicPlc24L.h"
#include "MAF.Objects.h"
#include "AA_MusicPlc.h"


#include"musicPlc24bL.h"

void maf_algorithm_audio_music_plc24L_register()
{
	MAF_Object::Registe<MAFAA_MusicPlc24L>("music_plc24L");
}

MAFAA_MusicPlc24L::MAFAA_MusicPlc24L()
{
}
MAFAA_MusicPlc24L::~MAFAA_MusicPlc24L()
{
}


int32_t MAFAA_MusicPlc24L::Init(void* param)
{
	MAF_PRINT();
	AA_MusicPlcParam* musicPlcParam = (AA_MusicPlcParam*)param;

	MusicPlcSampleParam sampleParam;
	memset(&sampleParam, 0, sizeof(MusicPlcSampleParam));
	sampleParam.fsHz = musicPlcParam->fsHz;
	sampleParam.channels = musicPlcParam->channels;
	sampleParam.frameSamples = musicPlcParam->frameSamples;

	_fsHz = musicPlcParam->fsHz;
	_channels = musicPlcParam->channels;
	_frameSamples = musicPlcParam->frameSamples;


	_hdSize = MusicPlc24bLGetStateSize(
		musicPlcParam->overlapMs,
		&sampleParam);

	_hd = _memory.Malloc(_hdSize);
	MAF_PRINT("_hd=%x,size:%d", (unsigned)_hd, _hdSize);
	if (!_hd)
	{
		return -1;
	}

	int32_t ret = MusicPlc24bLStateInit(
		_hd,
		musicPlcParam->overlapMs,
		musicPlcParam->decayTimeMs,
		&sampleParam);

	if (ret < 0)
	{
		return -1;
	}

	return 0;
}

int32_t MAFAA_MusicPlc24L::Deinit()
{
	MAF_PRINT();
	_memory.Free(_hd);
	return 0;
}

int32_t MAFAA_MusicPlc24L::Process(MAFA_Frame* frameIn, MAFA_Frame* frameOut)
{
	int32_t inUsed = 0;
	int32_t ret;

	int32_t outByte = frameOut->max - frameOut->off - frameOut->size;

	if (frameIn->flags & MAFA_FRAME_IS_EMPTY)
	{
		ret = MusicPlc24bL(
			_hd,
			NULL,
			0,
			NULL,
			frameOut->buff + frameOut->off + frameOut->size,
			&outByte,
			true);
	}
	else
	{
		ret = MusicPlc24bL(
			_hd,
			frameIn->buff + frameIn->off,
			frameIn->size,
			&inUsed,
			frameOut->buff + frameOut->off + frameOut->size,
			&outByte,
			false);
	}
	if (ret < 0)
	{
		return -1;
	}


#if 1	
	uint32_t* ptr = (uint32_t*)frameOut->buff + frameOut->off + frameOut->size;
	for (int32_t i = 0; i < outByte/sizeof(uint32_t); i++)
	{
		ptr[i] = ptr[i] << 8;
	}
#endif


	frameIn->size -= inUsed;
	frameIn->off += inUsed;
	frameOut->size += outByte;
	return 0;
}


#endif