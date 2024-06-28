#pragma once
#include"Ape.Inner.basic.h"
#include"Ape.Inner.rollbuffer.h"
/** Filters applied to the decoded data */
class ApeNNFilter {
public:
    ApeNNFilter() {}
    ~ApeNNFilter() {}
public:
    void Init(u8* buf,i32 lenBytes,i32 orderLenBytes);
    void DoFilter(int32_t* inOut, uint16_t order, uint8_t fracbits, uint16_t version);
private:
    ApeRollBuffer coeffs;       ///< actual coefficients used in filtering
    ApeRollBuffer adaptCoeffs;  ///< adaptive filter coefficients used for correcting of actual filter coefficients
    ApeRollBuffer delay;        ///< filtered values
    i32 avg;
};

class ApeNNFilters {
public:
    ApeNNFilters() {};
    ~ApeNNFilters() {};
public:
    void Reset();
    void Init(MemoryManger_c* MM, u16 fSet, u16 fileVersion);
    void DoFilter(i16 ch,int32_t* inOut);

private:
    ApeNNFilter filters[APE_MAX_CHANNELS][APE_FILTER_LEVELS];
    i16* filterbuf[APE_MAX_CHANNELS][APE_FILTER_LEVELS];
    u16 _fset;
    u16 _fileVersion;
};

