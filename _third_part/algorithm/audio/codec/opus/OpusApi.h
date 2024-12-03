#pragma once

namespace OpusApi_ns{

#define OPUS_API_RET_SUCCESS 0
#define OPUS_API_RET_FAIL -1

typedef int OpusApiRet_t;

typedef struct{
   void*(*malloc_cb)(int size);
   void*(*realloc_cb)(void *rmem, int newsize);
   void(*free_cb)(void *buf);
}OpusApiMemory_t;

typedef enum {
	OPUS_API_ENC_SET_BIT_RATE = 0,
	OPUS_API_ENC_SET_FRAME_DURATION_0P1MS,
	OPUS_API_ENC_SET_USE_VBR,
	OPUS_API_ENC_SET_COMPLEXITY,
	OPUS_API_ENC_SET_MAX,
}OpusApi_EncSetChhoose_e;

typedef enum {
	OPUS_API_DEC_SET_SEEK_SAMPLES = 0,	
	OPUS_API_DEC_SET_MAX,
}OpusApi_DecSetChhoose_e;

typedef enum {
	//OPUS_API_ENC_GET_FINAL_RANGE = 0,
	OPUS_API_ENC_GET_MAX,
}OpusApi_EncGetChhoose_e;


typedef struct {
	int usr;
}OpusApi_DecReserve_t;


class OpusApi
{
public:
	OpusApi(){}
	~OpusApi(){}
public:
//base
	static OpusApiRet_t memory_register(OpusApiMemory_t *memory);

//enc
public:
	static OpusApiRet_t create_encoder(void** pHd, int fs, int channels, bool isWithHead);
	static OpusApiRet_t destory_encoder(void *hd);
	static OpusApiRet_t encoder_set(void *hd, OpusApi_EncSetChhoose_e choose, void* val);
	static OpusApiRet_t encoder_get(void *hd, OpusApi_EncGetChhoose_e choose, void* val);
	static OpusApiRet_t encoder_run(void *hd, short* in, int inSample, unsigned char* out, int *outByte);

//dec
public:
	static OpusApiRet_t create_decoder(void **pHd, int fs, int channels);
	static OpusApiRet_t destory_decoder(void *hd);
	static OpusApiRet_t decoder_set(void *hd, OpusApi_DecSetChhoose_e choose, void* val);
	static OpusApiRet_t decoder_run(void *hd, unsigned char* in, int inByte, short *out, int *outSample, bool isPlc);
private:
};














}