#pragma once
#if 1
#include"Algo.BasePorting.h"
#ifdef __cplusplus
#define EXTERNC	extern "C"
#else
#define EXTERNC
#endif

#define APERET_SUCCESS 0
#define APERET_FAIL -1


typedef enum {
	APE_INIT_MODE_DMUX_USE_BUFFER = 0,
	APE_INIT_MODE_DMUX_USE_NO_PARAM,
	APE_INIT_MODE_DEC_USE_CONTEXT,
	APE_INIT_MODE_DEC_USE_BUFFER,
	APE_INIT_MODE_DEC_USE_NO_PARAM,
}ApeInitMode;

typedef struct
{
	AlgoBasePorting* basePorting;
	ApeInitMode mode;
	union
	{
		struct
		{
			void* context;
		}contextInit;
		struct
		{
			void* buffer;
			int32_t* size;
		}bufferInit;		
	};
}ApeInitParam;


typedef enum {
	APESET_CHOOSE_MAX,
}ApeSetChhoose_e;

typedef enum {
	APEGET_CHOOSE_CONTEXT=0,
	APEGET_CHOOSE_MAX,
}ApeGetChhoose_e;

EXTERNC int32_t Ape_GetStateSize();
EXTERNC int32_t Ape_StateInit(void* pMusicPlcStateIn, ApeInitParam* sampleParam);
EXTERNC int32_t Ape_Set(void* pMusicPlcStateIn, ApeSetChhoose_e choose, void** val);
EXTERNC int32_t Ape_Get(void* pMusicPlcStateIn, ApeGetChhoose_e choose, void** val);
EXTERNC int32_t Ape_Run(void* pMusicPlcStateIn, uint8_t* in, int32_t inLen, uint8_t* out, int32_t* outLen);
EXTERNC int32_t Ape_StateDeInit(void* pMusicPlcStateIn);
#endif