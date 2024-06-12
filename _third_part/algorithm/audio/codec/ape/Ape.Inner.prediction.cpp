#if 1
#include"Ape.Inner.prediction.h"

int32_t ApePredictor::DoFilter(int32_t A, int32_t B)
{
	int32_t predictionA, predictionB, sign;
	int32_t out;
	cacheA[0] = lastA;
	cacheA[1] = cacheA[0] - cacheA[1];

	predictionA = cacheA[0] * coeffsA[0]
		+ cacheA[1] * coeffsA[1]
		+ cacheA[2] * coeffsA[2]
		+ cacheA[3] * coeffsA[3];

	cacheB[0] = B - ((BOld * 31) >> 5);
	BOld = B;
	cacheB[1] = cacheB[0] - cacheB[1];

	predictionB = cacheB[0] * coeffsB[0]
		+ cacheB[1] * coeffsB[1]
		+ cacheB[2] * coeffsB[2]
		+ cacheB[3] * coeffsB[3]
		+ cacheB[4] * coeffsB[4];

	lastA = A + ((predictionA + (predictionB >> 1)) >> 10);

	out = lastA + ((outOld * 31) >> 5);
	outOld = out;

	//adaptive
	sign = APE_SIGN(A);
	/*****************************/
	adatpA[0] = APE_SIGN(cacheA[0]);
	adatpA[1] = APE_SIGN(cacheA[1]);

	coeffsA[0] += adatpA[0] * sign;
	coeffsA[1] += adatpA[1] * sign;
	coeffsA[2] += adatpA[2] * sign;
	coeffsA[3] += adatpA[3] * sign;


	adatpB[0] = APE_SIGN(cacheB[0]);
	adatpB[1] = APE_SIGN(cacheB[1]);

	coeffsB[0] += adatpB[0] * sign;
	coeffsB[1] += adatpB[1] * sign;
	coeffsB[2] += adatpB[2] * sign;
	coeffsB[3] += adatpB[3] * sign;
	coeffsB[4] += adatpB[4] * sign;
	//
	for (uint8_t i = 3; i > 0; i--)
	{
		cacheA[i] = cacheA[i - 1];
		adatpA[i] = adatpA[i - 1];
	}
	for (uint8_t i = 4; i > 0; i--)
	{
		cacheB[i] = cacheB[i - 1];
		adatpB[i] = adatpB[i - 1];
	}

	return out;
}


void ApePrediction::ApePredictionFilterMono(int32_t* Y)
{
	*Y = predictor[0].DoFilter(*Y, 0);
}

void ApePrediction::ApePredictionFilterStero(int32_t* Y, int32_t* X)
{
	*Y = predictor[0].DoFilter(*Y, xOld);
	*X = predictor[1].DoFilter(*X, *Y);
	xOld = *X;
}


#endif