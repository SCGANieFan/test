#include "MTF.AudioProcess.h"
#include "MTF.String.h"


MTF_AudioProcess::MTF_AudioProcess()
{

}
MTF_AudioProcess::~MTF_AudioProcess()
{

}


mtf_int32 MTF_AudioProcess::Set(const mtf_int8* key, mtf_void* val)
{
	return MTF_Process::Set(key, val) & MTF_AudioInfo::Set(key, val);
}

mtf_int32 MTF_AudioProcess::Get(const mtf_int8* key, mtf_void* val)
{
	return MTF_Process::Get(key, val);
}
