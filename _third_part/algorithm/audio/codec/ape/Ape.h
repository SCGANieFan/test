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
	APE_INIT_MODE_DMUX,
	APE_INIT_MODE_DEC_USE_CONTEXT,
	APE_INIT_MODE_DEC_USE_BUFFER,
	APE_INIT_MODE_DEC,
	APE_INIT_MODE_MAX,
}ApeMode;

typedef struct
{
	AlgoBasePorting* basePorting;
	ApeMode mode;
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
	APESET_DEC_HAS_IN_CACHE=0,
	APESET_DEC_START_NEW_FRAME,
	APESET_MAX,
}ApeSetChhoose_e;

typedef enum {
	APEGET_CONTEXT=0,
	APEGET_FRAME_SEEK_TABLE,
	APEGET_MAX,
}ApeGetChhoose_e;

typedef struct{
    uint32_t* seektable;
	uint32_t totalframes;
}FrameSeekTable;


EXTERNC int32_t Ape_GetSize(ApeMode mode);
EXTERNC int32_t Ape_Init(void* pMusicPlcStateIn, ApeInitParam* sampleParam);
#if 0
void* paramSet[] = { (void*)10 };
Ape_Set(_hd, ApeSetChhoose_e::APESET_DEC_START_NEW_FRAME, paramSet);
#endif
EXTERNC int32_t Ape_Set(void* pMusicPlcStateIn, ApeSetChhoose_e choose, void** val);

#if 0
FrameSeekTable table;
void* paramGet[] = { (void*)&table };
Ape_Get(_hd, ApeGetChhoose_e::APEGET_FRAME_SEEK_TABLE, paramGet);
#endif
EXTERNC int32_t Ape_Get(void* pMusicPlcStateIn, ApeGetChhoose_e choose, void** val);
EXTERNC int32_t Ape_Run(void* pMusicPlcStateIn, uint8_t* in, int32_t inLen, uint8_t* out, int32_t* outLen);
EXTERNC int32_t Ape_DeInit(void* pMusicPlcStateIn);

#endif