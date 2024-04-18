
#if 1
#include "MAFA_Audio.h"
#include "AudioAlgorithm.h"
#include "MAF.Objects.h"

#include<stdlib.h>

AudioAlgorithmRet AudioAlgorithmGetHandleSize(const char* type, int32_t* size)
{
	//check
	if (!type
		|| !size)
		return AUDIO_ALGORITHM_RET_INPUT_ERROR;
	int32_t ssize = MAF_Object::GetSize(type);
	if(ssize<=0)
		return AUDIO_ALGORITHM_RET_FAIL;
	*size = ssize;
	return AUDIO_ALGORITHM_RET_SUCCESS;
}

AudioAlgorithmRet AudioAlgorithmCreate(const char* type, void* hd)
{
	//check
	if (!type
		|| !hd)
		return AUDIO_ALGORITHM_RET_INPUT_ERROR;
	void* ret = MAF_Object::Create(type, hd);
	if (ret)
		return AUDIO_ALGORITHM_RET_SUCCESS;
	return AUDIO_ALGORITHM_RET_FAIL;
}

AudioAlgorithmRet AudioAlgorithmDestory(void* hd)
{
#if 1
	((MAFA_Audio*)hd)->~MAFA_Audio();
#else
	delete ((MAFA_Audio*)hd);
#endif
	return AUDIO_ALGORITHM_RET_SUCCESS;
}

AudioAlgorithmRet AlgorithmSetMemory(void* hd, Malloc_t* malloc, Realloc_t* realloc, Calloc_t* calloc, Free_t* free)
{
	((MAFA_Audio*)hd)->_memory.Init(malloc, realloc, calloc, free);
	return AUDIO_ALGORITHM_RET_SUCCESS;
}

AudioAlgorithmRet AlgorithmInit(void* hd, void* param)
{
	((MAFA_Audio*)hd)->Init(param);
	return AUDIO_ALGORITHM_RET_SUCCESS;
}
AudioAlgorithmRet AlgorithmDeinit(void* hd)
{
	((MAFA_Audio*)hd)->Deinit();
	return AUDIO_ALGORITHM_RET_SUCCESS;
}

AudioAlgorithmRet AudioAlgorithmRun(void* hd, MAFA_Frame* frameIn, MAFA_Frame* frameOut)
{
	((MAFA_Audio*)hd)->Process(frameIn, frameOut);
	return AUDIO_ALGORITHM_RET_SUCCESS;
}
#endif