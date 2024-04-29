#if 1
#include<string.h>

// MAF
#include "MAFA_PcmDemux.h"
#include "MAF.Objects.h"
#include "MAF.Interface.Opus.h"


maf_void maf_algorithm_audio_demux_pcm_register()
{
	MAF_Object::Registe<MAFA_PcmDemux>("demux-pcm");
}

MAFA_PcmDemux::MAFA_PcmDemux()
{
}
MAFA_PcmDemux::~MAFA_PcmDemux()
{
}


maf_int32 MAFA_PcmDemux::Init()
{
	MAF_PRINT("");
	return 0;
}

maf_int32 MAFA_PcmDemux::Deinit()
{
	MAF_PRINT("");
	return 0;
}

maf_int32 MAFA_PcmDemux::Process(MAF_Data* dataIn, MAF_Data* dataOut)
{
	MAF_PRINT("");
	return 0;
}


#endif