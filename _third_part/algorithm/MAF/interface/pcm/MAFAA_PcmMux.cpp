#if 1
#include<string.h>

// MAF
#include "MAFAA_PcmMux.h"
#include "MAF.Objects.h"
#include "MAF.Interface.Opus.h"


void maf_algorithm_audio_mux_pcm_register()
{
	MAF_Object::Registe<MAFAA_PcmMux>("mux-pcm");
}

MAFAA_PcmMux::MAFAA_PcmMux()
{
}
MAFAA_PcmMux::~MAFAA_PcmMux()
{
}


int32_t MAFAA_PcmMux::Init(void* param)
{
	MAF_PRINT("");
	return 0;
}

int32_t MAFAA_PcmMux::Deinit()
{
	MAF_PRINT("");
	return 0;
}

int32_t MAFAA_PcmMux::Process(MAFA_Frame* frameIn, MAFA_Frame* frameOut)
{
	MAF_PRINT("");
	return 0;
}


#endif