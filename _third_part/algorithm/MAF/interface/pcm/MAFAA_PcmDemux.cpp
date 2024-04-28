#if 1
#include<string.h>

// MAF
#include "MAFAA_PcmDemux.h"
#include "MAF.Objects.h"
#include "MAF.Interface.Opus.h"


void maf_algorithm_audio_demux_pcm_register()
{
	MAF_Object::Registe<MAFAA_PcmDemux>("demux-pcm");
}

MAFAA_PcmDemux::MAFAA_PcmDemux()
{
}
MAFAA_PcmDemux::~MAFAA_PcmDemux()
{
}


int32_t MAFAA_PcmDemux::Init(void* param)
{
	MAF_PRINT("");
	return 0;
}

int32_t MAFAA_PcmDemux::Deinit()
{
	MAF_PRINT("");
	return 0;
}

int32_t MAFAA_PcmDemux::Process(MAFA_Frame* frameIn, MAFA_Frame* frameOut)
{
	MAF_PRINT("");
	return 0;
}


#endif