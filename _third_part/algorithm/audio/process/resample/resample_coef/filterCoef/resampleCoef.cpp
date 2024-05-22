#include "resamples.inner.h"

#define TABLE_NAME(src,dst) resample_##src##_to_##dst##_filter_for_1600

#if 0
static const int16_t TABLE_NAME(8000,44100)[]{
#include "resample_8k_to_44p1k_filter_for_1600.txt"
};
DefineResampleRegister(8000, 44100)

static const int16_t TABLE_NAME(16000,44100)[]{
#include "resample_16k_to_44p1k_filter_for_1600.txt"
};
DefineResampleRegister(16000, 44100)


static const int16_t TABLE_NAME(24000,44100)[]{
#include "resample_24k_to_44p1k_filter_for_1600.txt"
};
DefineResampleRegister(24000, 44100)


static const int16_t TABLE_NAME(32000,44100)[]{
#include "resample_32k_to_44p1k_filter_for_1600.txt"
};
DefineResampleRegister(32000, 44100)


static const int16_t TABLE_NAME(32000,48000)[]{
#include "resample_32k_to_48k_filter_for_1600.txt"
};
DefineResampleRegister(32000, 48000)

static const int16_t TABLE_NAME(44100,8000)[]{
#include "resample_44p1k_to_8k_filter_for_1600.txt"
};
DefineResampleRegister(44100, 8000)


static const int16_t TABLE_NAME(44100,16000)[]{
#include "resample_44p1k_to_16k_filter_for_1600.txt"
};
DefineResampleRegister(44100, 16000)


static const int16_t TABLE_NAME(44100,24000)[]{
#include "resample_44p1k_to_24k_filter_for_1600.txt"
};
DefineResampleRegister(44100, 24000)

static const int16_t TABLE_NAME(44100,32000)[]{
#include "resample_44p1k_to_32k_filter_for_1600.txt"
};
DefineResampleRegister(44100, 32000)


static const int16_t TABLE_NAME(44100,48000)[]{
#include "resample_44p1k_to_48k_filter_for_1600.txt"
};
DefineResampleRegister(44100, 48000)


static const int16_t TABLE_NAME(44100,96000)[]{
#include "resample_44p1k_to_96k_filter_for_1600.txt"
};
DefineResampleRegister(44100, 96000)


static const int16_t TABLE_NAME(48000, 32000)[]{
#include "resample_48k_to_32k_filter_for_1600.txt"
};
DefineResampleRegister(48000, 32000)

#endif
static const int16_t TABLE_NAME(48000,44100)[]{
#include "resample_48k_to_44p1k_filter_for_1600.txt"
};

DefineResampleRegister(48000, 44100)


