#include"Algo.AudioData.h"

b1 AudioData::Init(const AudioInfo* pInfo, Buffer *buffer)
{
	AudioData::Data::Init(buffer);
	_info = pInfo;
	return true;
}
