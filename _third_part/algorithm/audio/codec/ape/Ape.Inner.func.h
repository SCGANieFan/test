#include"Ape.Inner.type.h"







i32 Ape_StateInitWithContext(ApeDecoder* pDec, AlgoBasePorting* basePorting, ApeContext* context);

i32 Ape_StateInitWithBuffer(ApeDecoder* pDec,  AlgoBasePorting* basePorting, u8* buffer, i32* size);


i32 Ape_RunInner(ApeDecoder* pMusicPlcStateIn, uint8_t * in, int32_t inLen, uint8_t * out, int32_t * outLen);

i32 Ape_StateDeInitInner(ApeDecoder* pMusicPlcStateIn);


bool ApeEntropyDecode(ApeRice* rice, ApeRangecoder* rc, BufferRead* bufferRead, int32_t* out, uint16_t version);


void ApePredictionFilterMono(ApePrediction* prediction, int32_t* Y);
void ApePredictionFilterStero(ApePrediction* prediction, int32_t* Y, int32_t* X);

void ApeNNFilter(ApeNNFilter_t filter[APE_FILTER_LEVELS], int32_t* inOut, uint16_t version, uint16_t fSet);


bool ApeRollBufferInit(ApeRollBuffer* rollBuffer, uint8_t* buffer, uint32_t bufferLen, uint32_t bufferLenRemMin);

void ApeRollBufferIncrement(ApeRollBuffer* rollBuffer, uint32_t bytes);
