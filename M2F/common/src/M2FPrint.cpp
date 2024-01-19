#include <stdio.h>
#include <stdarg.h>
#include"./M2FPrint.h"

#define m2f_printf_ori(fmt,...)		printf(fmt,##__VA_ARGS__)
//#define m2f_printf(fmt,...)		printf(fmt "\n",##__VA_ARGS__)

M2FPrint::M2FPrint()
{


}
M2FPrint::~M2FPrint()
{


}

void M2FPrint::Print(const char* _Format, void* args)
{
    char buf[256];
    vsprintf_s(buf, 256, _Format, (va_list)args);
    m2f_printf_ori(buf);
}


void M2FPrint::PrintInner(const char* _Format, ...) {
    char buf[256];
    va_list args;
    va_start(args, _Format);
    vsprintf_s(buf, 256, _Format, args);
    va_end(args);
    m2f_printf_ori(buf);
}

void M2FPrint::PrintInner(char* buf) {
    m2f_printf_ori(buf);
}
