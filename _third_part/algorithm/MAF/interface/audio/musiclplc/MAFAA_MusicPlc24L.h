#pragma once
#include "MAFA_Audio.h"
class MAFAA_MusicPlc24L:public MAFA_Audio
{
public:
	MAFAA_MusicPlc24L();
	~MAFAA_MusicPlc24L();
public:
	virtual int32_t Init(void* param) override;
	virtual int32_t Deinit() override;
	virtual int32_t Process(MAFA_Frame* frameIn, MAFA_Frame* frameOut) override;
private:
	void *_hd;
	int32_t _hdSize;
};
