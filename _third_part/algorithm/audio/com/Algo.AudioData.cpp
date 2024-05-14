#include"Algo.AudioData.h"

using namespace Algo;
using namespace Audio;
b1 AudioData::Init(const AudioInfo* pInfo, Buffer *buffer)
{
	AudioData::Data::Init(buffer);
	_info = pInfo;
	return true;
}
