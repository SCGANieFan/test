#include"M2Fapi.h"

#if 0
M2FApi::M2FApi()
{

}
M2FApi::~M2FApi()
{

}

int M2FApi::Encode(M2F* pM2F, short* pcmIn, unsigned int pcmInLen, unsigned char* bitOut, unsigned int* bitOutLen)
{


	pM2F->pMultimedia->pAudio->pCodec->

}



M2FAudioCodec* M2FCreateAudioCodec(const char* type)
{
	return M2FCreateAudioCodecOri(type);
}

void M2FDeleteAudioCodec(M2FAudioCodec* pM2FAudioCodec)
{
	delete(pM2FAudioCodec);
}
int M2FEncode(M2FAudioCodec* pM2FAudioCodec, M2FAudioCodecEncodeIn* pIn)
{
	return pM2FAudioCodec->Encode(pIn);
}
int M2FHdEncInit(M2FAudioCodec* pM2FAudioCodec, void* param)
{
	return pM2FAudioCodec->HdEncInit(param);
}

int M2FDecode(M2FAudioCodec* pM2FAudioCodec, M2FAudioCodecDecodeIn* pIn)
{
	return pM2FAudioCodec->Decode(pIn);
}
int M2FHdDecInit(M2FAudioCodec* pM2FAudioCodec, void* param)
{
	return pM2FAudioCodec->HdDecInit(param);
}

int M2FEncFrameSize(M2FAudioCodec* pM2FAudioCodec)
{
	return pM2FAudioCodec->frameSize;
}

#endif



