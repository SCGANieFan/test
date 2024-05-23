#if 1

#include"Algo.Printer.h"
#include"ApeOri.h"
#include"Ape.Inner.func.h"

static inline uint8_t* ApeRollBufferGetBuffer(ApeRollBuffer* rollBuffer)
{
	return rollBuffer->buffer + rollBuffer->offset;
}

static inline int16_t ApeClipInt16(int32_t a)
{
	if ((a + 0x8000) & ~0xFFFF)
		return (a >> 31) ^ 0x7FFF;
	else
		return a;
}


void ApeDoNNFilter(ApeNNFilter_t* filter, int32_t* inOut, uint16_t order, uint8_t fracbits, uint16_t version)
{
#if 0
	int32_t in = *inOut;
	int32_t out;
	//filter
	out = 0;

	for (uint16_t i = 0; i < order; i++)
		out += filter->coeffs[i] * filter->delay[i];
	out = in + ((out + (1 << (fracbits - 1))) >> fracbits);

	//updata coeffs
	int32_t inSign = APE_SIGN(in);
	for (uint16_t i = 0; i < order; i++)
		filter->coeffs[i] += inSign * filter->adaptCoeffs[i];

	//updata adaptCoeffs
	for (uint16_t i = 1; i < order; i++)
		filter->adaptCoeffs[i - 1] = filter->adaptCoeffs[i];
	if (version < 3980)
	{
		filter->adaptCoeffs[order - 1] = (out == 0) ? 0 : ((out >> 28) & 8) - 4;
		filter->adaptCoeffs[order - 5] >>= 1;
		filter->adaptCoeffs[order - 9] >>= 1;
	}
	else
	{
		int32_t outAbs;
		int32_t outSign;
		outAbs = APE_ABS(out);
		outSign = APE_SIGN(out);
		if (outAbs > filter->avg * 3)
			filter->adaptCoeffs[order - 1] = outSign * 32;
		else if (outAbs > filter->avg * 4 / 3)
			filter->adaptCoeffs[order - 1] = outSign * 16;
		else if (outAbs > 0)
			filter->adaptCoeffs[order - 1] = outSign * 8;
		else
			filter->adaptCoeffs[order - 1] = 0;

		filter->adaptCoeffs[order - 2] >>= 1;
		filter->adaptCoeffs[order - 3] >>= 1;
		filter->adaptCoeffs[order - 9] >>= 1;

		filter->avg += (outAbs - filter->avg) / 16;
	}

	// delay
	for (uint16_t i = 1; i < order; i++)
		filter->delay[i - 1] = filter->delay[i];
	filter->delay[order - 1] = ApeClipInt16(out);

	//out
	*inOut = out;

#else
	int32_t in = *inOut;
	int32_t out;

	int16_t* pCoffs = (int16_t*)ApeRollBufferGetBuffer(&filter->coeffs);
	int16_t* pDelay = (int16_t*)ApeRollBufferGetBuffer(&filter->delay);
	int16_t* pAdaptCoeffs = (int16_t*)ApeRollBufferGetBuffer(&filter->adaptCoeffs);

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
	ApeRollBufferIncrement(&filter->adaptCoeffs, sizeof(int16_t));
	pAdaptCoeffs = (int16_t*)ApeRollBufferGetBuffer(&filter->adaptCoeffs);

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
		if (outAbs > filter->avg * 3)
			pAdaptCoeffs[order - 1] = outSign * 32;
		else if (outAbs > filter->avg * 4 / 3)
			pAdaptCoeffs[order - 1] = outSign * 16;
		else if (outAbs > 0)
			pAdaptCoeffs[order - 1] = outSign * 8;
		else
			pAdaptCoeffs[order - 1] = 0;

		pAdaptCoeffs[order - 2] >>= 1;
		pAdaptCoeffs[order - 3] >>= 1;
		pAdaptCoeffs[order - 9] >>= 1;

		filter->avg += (outAbs - filter->avg) / 16;
	}

	// delay
	ApeRollBufferIncrement(&filter->delay, sizeof(int16_t));
	pDelay = (int16_t*)ApeRollBufferGetBuffer(&filter->delay);

	pDelay[order - 1] = ApeClipInt16(out);
	//out
	*inOut = out;
#endif
}

void ApeNNFilter(ApeNNFilter_t filter[APE_FILTER_LEVELS], int32_t* inOut, uint16_t version, uint16_t fSet)
{
	for (uint8_t i = 0; i < APE_FILTER_LEVELS; i++) {
		if (!ape_filter_orders[fSet][i])
			break;
		ApeDoNNFilter(&filter[i], inOut, ape_filter_orders[fSet][i], ape_filter_fracbits[fSet][i], version);
	}
}








#endif