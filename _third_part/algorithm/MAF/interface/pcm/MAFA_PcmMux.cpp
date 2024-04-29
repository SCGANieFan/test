#if 1
#include<string.h>

// MAF
#include "MAFA_PcmMux.h"
#include "MAF.Objects.h"
#include "MAF.Interface.Opus.h"


maf_void maf_algorithm_audio_mux_pcm_register()
{
	MAF_Object::Registe<MAFA_PcmMux>("mux-pcm");
}

MAFA_PcmMux::MAFA_PcmMux()
{
}
MAFA_PcmMux::~MAFA_PcmMux()
{
}


maf_int32 MAFA_PcmMux::Init()
{
	MAF_PRINT("");
	return 0;
}

maf_int32 MAFA_PcmMux::Deinit()
{
	MAF_PRINT("");
	return 0;
}

maf_int32 MAFA_PcmMux::Process(MAF_Data* dataIn, MAF_Data* dataOut)
{
	MAF_PRINT("");
	return 0;
}


#endif