#if 1
#include"Algo.Printer.h"
#include"ApeOri.h"
#include"Ape.Inner.func.h"


static uint32_t range_dec_normalize(ApeRangecoder* rc, BufferRead* bufferRead)
{
	//check
	while (rc->range <= ((1u << (32 - 1)) >> 8))
	{
		rc->buffer <<= 8;
		rc->buffer += bufferRead->Read(1);
		rc->low = (rc->low << 8) | ((rc->buffer >> 1) & 0xFF);
		rc->range <<= 8;
		if (rc->range == 0)
			return 0;
	}
	return 0;
}
i64 frames = 0;
static bool range_get_symbol(ApeRangecoder* rc, const uint16_t counts[], const uint16_t counts_diff[], BufferRead* bufferRead, int32_t* symbol)
{
	int32_t cf;
	int32_t out;
	range_dec_normalize(rc, bufferRead);
	rc->range = rc->range >> 16;
	cf = rc->low / rc->range;
	
#if 1
	frames++;
	if (frames > 16000)
	{
		//ALGO_PRINT("[%lld] buffer:%x,\tlow:%x,\trange:%x,\tnRangeTotal:%x", frames, rc->buffer, rc->low, rc->range, cf);
		if (frames >= 17716)
			int a = 1;
	}
#endif
	if (cf > 65492) {
		out = cf - 65535 + 63;
		rc->low -= rc->range * cf;
		if (cf > 65535)
			return false;
		*symbol = out;
		return true;
	}
	/* figure out the symbol inefficiently; a binary search would be much better */
	for (out = 0; counts[out + 1] <= cf; out++);
	rc->low -= rc->range * counts[out];
	rc->range = rc->range * counts_diff[out];
	*symbol = out;
	return true;
}

static int32_t range_decode_bits(ApeRangecoder* rc, int n, BufferRead* bufferRead)
{
	int32_t sym;
	range_dec_normalize(rc, bufferRead);
	rc->range = rc->range >> n;
	sym = rc->low / rc->range;
	rc->low -= rc->range * sym;
	return sym;
}

bool ApeEntropyDecode(ApeRice* rice, ApeRangecoder* rc, BufferRead* bufferRead, int32_t* out, uint16_t version)
{
	//check

	//init
	int32_t x;
	int32_t overflow;
	//get x
	if (version < 3990)
	{
		int32_t tmpk;
		if (!range_get_symbol(rc, counts_3970, counts_diff_3970, bufferRead, &overflow))
			return false;

		if (overflow == (MODEL_ELEMENTS - 1)) {
			tmpk = range_decode_bits(rc, 5, bufferRead);
			overflow = 0;
		}
		else
			tmpk = (rice->k < 1) ? 0 : rice->k - 1;

		if (tmpk <= 16)
			x = range_decode_bits(rc, tmpk, bufferRead);
		else {
			x = range_decode_bits(rc, 16, bufferRead);
			x |= (range_decode_bits(rc, tmpk - 16, bufferRead) << 16);
		}
		x += overflow << tmpk;
	}
	else
	{
		int32_t base, pivot;

		pivot = rice->ksum >> 5;
		if (pivot == 0)
			pivot = 1;
		if (!range_get_symbol(rc, counts_3980, counts_diff_3980, bufferRead, &overflow))
			return false;
		if (overflow == (MODEL_ELEMENTS - 1))
		{
			overflow = range_decode_bits(rc, 16, bufferRead) << 16;
			overflow |= range_decode_bits(rc, 16, bufferRead);
			//// detect overflow signaling here, adjust the pivot value and try again
			//if (overflow == 1)
			//{
			//	pivot = 32768;
			//	return DecodeOverflow(nPivotValue);
			//}
		}

		if (pivot < 0x10000)
		{
			range_dec_normalize(rc, bufferRead);
			rc->range = rc->range / pivot;
			base = rc->low / rc->range;
			rc->low -= rc->range * base;
		}
		else
		{
			int32_t base_hi = pivot, base_lo;
			int32_t bbits = 0;

			while (base_hi & ~0xFFFF) {
				base_hi >>= 1;
				bbits++;
			}
			range_dec_normalize(rc, bufferRead);
			rc->range = rc->range / (base_hi + 1);

			base_hi = rc->low / rc->range;
			rc->low -= rc->range * base_hi;

			range_dec_normalize(rc, bufferRead);
			rc->range = rc->range / (1 << bbits);

			base_lo = rc->low / rc->range;
			rc->low -= rc->range * base_lo;
			base = (base_hi << bbits) + base_lo;
		}

		x = base + overflow * pivot;
	}
	rice->ksum += ((x + 1) / 2) - ((rice->ksum + 16) >> 5);

	if (rice->ksum < K_SUM_MIN_BOUNDARY[rice->k])
		rice->k--;
	else if (rice->ksum >= K_SUM_MIN_BOUNDARY[rice->k + 1])
		rice->k++;

	//get out
	//Convert to signed
	if (x & 1)
		*out = (x >> 1) + 1;
	else
		*out = -(x >> 1);

	return true;
}


















#endif