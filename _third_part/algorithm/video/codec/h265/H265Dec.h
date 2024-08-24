#pragma once

#include"Algo.Type.h"
#include"Algo.Memory.h"
#include"Algo.Printer.h"
using namespace Algo;

class H265Dec_c
{
public:
    H265Dec_c(){}
    ~H265Dec_c(){}

public:
    b1 Init();
    b1 Receive(u8 *in,i32 inByte);
    b1 Generate(u8 *out,i32 *outByte);
    b1 DeInit();
private:
    void *_hd;
    b1 _check_hash=false;
    i32 _framecnt=0;
};
