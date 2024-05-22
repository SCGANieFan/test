#include<string.h>
#include"resamples.inner.h"
#include"resamples.h"
//#include"smf_debug.h"
static const ResampleCoef* _resample_coefs[24];

void resample_coef_register(const void* coef) {
	for (auto& item : _resample_coefs) {
		if (item == coef)
			break;
		if (!item) {
			item = (const ResampleCoef*)coef;
			break;
		}
	}
}

const ResampleCoef* resample_get_coef(int rate, int target) {
	for (auto item : _resample_coefs) {
		if (item && (item->upsampleFactor * rate == item->downsampleFactor * target)) {
			return item;
		}
	}
	//dbgErrPXL("no resample coef:%d->%d\n", rate, target);
	return 0;
}

void resample_coef_supported() {
	//dbgTestPXL("resample coefs supported:");
	for (auto item : _resample_coefs) {
		if (item) {
			//dbgTestPXL("%d->%d", item->downsampleFactor, item->upsampleFactor);
		}
	}
}


void audio_resample_ex_flush(void* hd)
{
	ResampleState* ctrl;
	ctrl = (ResampleState*)hd;
	if (ctrl) {
		ctrl->inStartSamples = 0;
		ctrl->coefChoose = 0;
		memset(ctrl->inSamplesCache, 0, sizeof(ctrl->inSamplesCache));
	}
}

//interface 
int resample_get_size(ResamplesInfo* info) {
	const ResampleCoef* coef = resample_get_coef(info->iRate, info->oRate);
	//returnIfErrC(false, !coef);
	auto width = info->iBits > 16 ? 4 : 2;
	return sizeof(ResampleState) + (coef->coefNumPerGroup + SAMPLE_NUM) * width * info->iCh;
}

bool audio_resample_open(void* handle, ResamplesInfo* info) {
	const ResampleCoef* coef = resample_get_coef(info->iRate, info->oRate);
	//returnIfErrC(false, !coef);
	//check
	if (coef == NULL) {
		return false;
	}
	if (coef->coefGroups == NULL) {
		return false;
	}

	if (coef->upsampleFactor == 0 || coef->downsampleFactor == 0) {
		return false;
	}

	if (handle == NULL) {
		return false;
	}
	if ((uintptr_t)handle & 0x3) {
		return false;
	}
	//
	ResampleState* hd;
	hd = (ResampleState*)handle;
	memset(hd, 0, sizeof(ResampleState));
	hd->coef = (ResampleCoef*)coef;

	
	hd->iInfo.ch = info->iCh;
	hd->iInfo.bits = info->iBits;
	hd->iInfo.rate = info->iRate;
	hd->oInfo.ch = info->oCh ? info->oCh : hd->iInfo.ch;
	hd->oInfo.bits = info->oBits ? info->oBits : hd->iInfo.bits;
	hd->oInfo.rate = info->oRate;
	hd->isChCopy = info->isChCopy;
	
	audio_resample_ex_flush(hd);
	return true;
}

void audio_resample_close(void* hd)
{
#if 0
	ResampleState* ctrl;
	ctrl = (ResampleState*)hd;
	if (ctrl && ctrl->magic == RESAMPLE_MAGIC) {
		ctrl->magic = 0;
	}
#endif
}
