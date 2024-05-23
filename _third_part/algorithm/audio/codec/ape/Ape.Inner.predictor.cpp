#if 1
#include"Algo.Printer.h"
#include"ApeOri.h"
#include"Ape.Inner.func.h"




static int32_t ApePredictionFilter(ApePredictor* p, int32_t A, int32_t B)
{
	int32_t predictionA, predictionB, sign;
	int32_t out;
	p->cacheA[0] = p->lastA;
	p->cacheA[1] = p->cacheA[0] - p->cacheA[1];


	predictionA = p->cacheA[0] * p->coeffsA[0]
		+ p->cacheA[1] * p->coeffsA[1]
		+ p->cacheA[2] * p->coeffsA[2]
		+ p->cacheA[3] * p->coeffsA[3];

	p->cacheB[0] = B - ((p->BOld * 31) >> 5);
	p->BOld = B;
	p->cacheB[1] = p->cacheB[0] - p->cacheB[1];

	predictionB = p->cacheB[0] * p->coeffsB[0]
		+ p->cacheB[1] * p->coeffsB[1]
		+ p->cacheB[2] * p->coeffsB[2]
		+ p->cacheB[3] * p->coeffsB[3]
		+ p->cacheB[4] * p->coeffsB[4];

	p->lastA = A + ((predictionA + (predictionB >> 1)) >> 10);

	out = p->lastA + ((p->outOld * 31) >> 5);
	p->outOld = out;

	//adaptive
	sign = APE_SIGN(A);
	/*****************************/
	p->adatpA[0] = APE_SIGN(p->cacheA[0]);
	p->adatpA[1] = APE_SIGN(p->cacheA[1]);

	p->coeffsA[0] += p->adatpA[0] * sign;
	p->coeffsA[1] += p->adatpA[1] * sign;
	p->coeffsA[2] += p->adatpA[2] * sign;
	p->coeffsA[3] += p->adatpA[3] * sign;


	p->adatpB[0] = APE_SIGN(p->cacheB[0]);
	p->adatpB[1] = APE_SIGN(p->cacheB[1]);

	p->coeffsB[0] += p->adatpB[0] * sign;
	p->coeffsB[1] += p->adatpB[1] * sign;
	p->coeffsB[2] += p->adatpB[2] * sign;
	p->coeffsB[3] += p->adatpB[3] * sign;
	p->coeffsB[4] += p->adatpB[4] * sign;
	//
	for (uint8_t i = 3; i > 0; i--)
	{
		p->cacheA[i] = p->cacheA[i - 1];
		p->adatpA[i] = p->adatpA[i - 1];
	}
	for (uint8_t i = 4; i > 0; i--)
	{
		p->cacheB[i] = p->cacheB[i - 1];
		p->adatpB[i] = p->adatpB[i - 1];
	}

	return out;
}

void ApePredictionFilterMono(ApePrediction* prediction, int32_t* Y)
{
	*Y = ApePredictionFilter(&prediction->predictor[0], *Y, 0);
}

void ApePredictionFilterStero(ApePrediction* prediction, int32_t* Y, int32_t* X)
{
	*Y = ApePredictionFilter(&prediction->predictor[0], *Y, prediction->xOld);
	*X = ApePredictionFilter(&prediction->predictor[1], *X, *Y);
	prediction->xOld = *X;
}




#endif