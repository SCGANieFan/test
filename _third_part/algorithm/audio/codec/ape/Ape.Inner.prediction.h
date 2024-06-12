#pragma once
#include"Ape.Inner.basic.h"

class ApePredictor
{
public:
    ApePredictor() {};
    ~ApePredictor() {};
public:
    void Init(){
        ALGO_MEM_SET(this, 0, sizeof(ApePredictor));
        static const i32 initial_coeffs[4] = {
            360, 317, -109, 98
        };
        ALGO_MEM_CPY(coeffsA, initial_coeffs, sizeof(initial_coeffs));
    }
    int32_t DoFilter(int32_t A, int32_t B);
private:
    i32 cacheA[4];
    i32 coeffsA[4];
    i32 adatpA[4];

    i32 cacheB[5]__ALIGN8__;
    i32 coeffsB[5]__ALIGN8__;  ///< adaption coefficients
    i32 adatpB[5]__ALIGN8__;

    i32 lastA;
    i32 BOld;
    i32 outOld;
};

class ApePrediction
{
public:
    ApePrediction() {};
    ~ApePrediction() {};
public:
    void Init() {
        for (uint16_t ch = 0; ch < APE_MAX_CHANNELS; ch++) {
            predictor[ch].Init();
        }
        xOld = 0;
    }
    void ApePredictionFilterMono(int32_t* Y);
    void ApePredictionFilterStero(int32_t* Y, int32_t* X);
private:
    ApePredictor predictor[APE_MAX_CHANNELS];
    i32 xOld;
};
