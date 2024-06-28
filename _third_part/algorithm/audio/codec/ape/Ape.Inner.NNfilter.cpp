#if 1
#include"Ape.Inner.NNfilter.h"

/** Filter orders depending on compression level */
static const u16 ape_filter_orders[5][APE_FILTER_LEVELS] = {
	{  0,   0,    0 },
	{ 16,   0,    0 },
	{ 64,   0,    0 },
	{ 32, 256,    0 },
	{ 16, 256, 1280 }
};

/** Filter fraction bits depending on compression level */
static const u8 ape_filter_fracbits[5][APE_FILTER_LEVELS] = {
	{  0,  0,  0 },
	{ 11,  0,  0 },
	{ 11,  0,  0 },
	{ 10, 13,  0 },
	{ 11, 13, 15 }
};

static inline int16_t ApeClipInt16(int32_t a)
{
	if ((a + 0x8000) & ~0xFFFF)
		return (a >> 31) ^ 0x7FFF;
	else
		return a;
}


void ApeNNFilter::Init(u8* buf, i32 lenBytes, i32 orderLenBytes)
{
	coeffs.Init(buf, lenBytes, orderLenBytes);
	delay.Init(buf + lenBytes, lenBytes, orderLenBytes);
	adaptCoeffs.Init(buf + lenBytes * 2, lenBytes, orderLenBytes);
	ALGO_MEM_SET(buf, 0, lenBytes * 3);
	avg = 0;
}


void ApeNNFilter::DoFilter(int32_t* inOut, uint16_t order, uint8_t fracbits, uint16_t version)
{
	int32_t in = *inOut;
	int32_t out;

	int16_t* pCoffs = (int16_t*)coeffs.GetBuffer();
	int16_t* pDelay = (int16_t*)delay.GetBuffer();
	int16_t* pAdaptCoeffs = (int16_t*)adaptCoeffs.GetBuffer();

	//filter
	out = 0;
	for (uint16_t i = 0; i < order; i++)
		out += pCoffs[i] * pDelay[i];
	out = in + ((out + (1 << (fracbits - 1))) >> fracbits);

	//updata coeffs
	int32_t inSign = APE_SIGN(in);
	// for (uint16_t i = 0; i < order; i++)
	// 	pCoffs[i] += inSign * pAdaptCoeffs[i];
	if (inSign > 0)
	{
		for (uint16_t i = 0; i < order; i++)
			pCoffs[i] += pAdaptCoeffs[i];
	}
	else if (inSign < 0)
	{
		for (uint16_t i = 0; i < order; i++)
			pCoffs[i] -= pAdaptCoeffs[i];
	}
	//updata adaptCoeffs
	adaptCoeffs.Increment(sizeof(int16_t));
	pAdaptCoeffs = (int16_t*)adaptCoeffs.GetBuffer();

	if (version < 3980)
	{
		pAdaptCoeffs[order - 1] = (out == 0) ? 0 : ((out >> 28) & 8) - 4;
		pAdaptCoeffs[order - 5] >>= 1;
		pAdaptCoeffs[order - 9] >>= 1;
	}
	else
	{
		int32_t outAbs;
		int32_t outSign;
		outAbs = APE_ABS(out);
		outSign = APE_SIGN(out);
		if (outAbs > avg * 3)
			pAdaptCoeffs[order - 1] = outSign * 32;
		else if (outAbs > avg * 4 / 3)
			pAdaptCoeffs[order - 1] = outSign * 16;
		else if (outAbs > 0)
			pAdaptCoeffs[order - 1] = outSign * 8;
		else
			pAdaptCoeffs[order - 1] = 0;

		pAdaptCoeffs[order - 2] >>= 1;
		pAdaptCoeffs[order - 3] >>= 1;
		pAdaptCoeffs[order - 9] >>= 1;

		avg += (outAbs - avg) / 16;
	}

	// delay
	delay.Increment(sizeof(int16_t));
	pDelay = (int16_t*)delay.GetBuffer();

	pDelay[order - 1] = ApeClipInt16(out);
	//out
	*inOut = out;
}
void ApeNNFilters::Reset()
{
	for (uint16_t ch = 0; ch < APE_MAX_CHANNELS; ch++) {
		for (int i = 0; i < APE_FILTER_LEVELS; i++) {
			if (!ape_filter_orders[_fset][i])
				break;
			uint32_t lenBytes = (ape_filter_orders[_fset][i] + APE_NN_FILTER_ROLL_BUFFER_EXTRA_LENGTH) * sizeof(int16_t);
			uint32_t orderLenBytes = ape_filter_orders[_fset][i] * sizeof(int16_t);
			filters[ch][i].Init((uint8_t*)filterbuf[ch][i], lenBytes, orderLenBytes);
		}
	}
}
void ApeNNFilters::Init(MemoryManger_c* MM, u16 fSet, u16 fileVersion)
{
	_fset = fSet;
	_fileVersion = fileVersion;
	for (u16 ch = 0; ch < APE_MAX_CHANNELS; ch++)
	{
		for (i32 i = 0; i < APE_FILTER_LEVELS; i++) {
			if (!ape_filter_orders[_fset][i])
				break;
			i32 size = (((ape_filter_orders[_fset][i] + APE_NN_FILTER_ROLL_BUFFER_EXTRA_LENGTH) * sizeof(i16))) * 3;
			filterbuf[ch][i] = (i16*)MM->Malloc(size);
			ALGO_MEM_SET(filterbuf[ch][i], 0, size);
		}
	}
	Reset();
}
void ApeNNFilters::DoFilter(i16 ch, int32_t* inOut)
{

	for (uint8_t i = 0; i < APE_FILTER_LEVELS; i++) {
		if (!ape_filter_orders[_fset][i])
			break;
		filters[ch][i].DoFilter(inOut, ape_filter_orders[_fset][i], ape_filter_fracbits[_fset][i], _fileVersion);
	}
}







#endif