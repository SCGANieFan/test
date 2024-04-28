#include "MTF.AudioMuxer.h"
#include "MTF.String.h"

MTF_AudioMuxer::MTF_AudioMuxer()
{
}
MTF_AudioMuxer::~MTF_AudioMuxer()
{
}



mtf_int32 MTF_AudioMuxer::receive(MTF_Data& iData)
{
	return 0;
}

mtf_int32 MTF_AudioMuxer::Set(const mtf_int8* key, mtf_void* val)
{
	return MTF_Sink::Set(key,val);
}
mtf_int32 MTF_AudioMuxer::Get(const mtf_int8* key, mtf_void* val)
{
	return MTF_Sink::Get(key, val);
}



