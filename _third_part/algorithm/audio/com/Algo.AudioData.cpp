#include"Algo.AudioData.h"


b1 AudioData::Init(const AudioInfo* pInfo, u8* buf, i32 max)
{
	AudioData::Data::Init(buf, max);
	_info = pInfo;
	return true;
}
