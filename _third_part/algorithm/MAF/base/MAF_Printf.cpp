#include<stdarg.h>
#include "MAF_Printf.h"

#ifdef _WIN32
#include<stdio.h>
#define printf_inner(fmt,...)		printf(fmt,##__VA_ARGS__)
#else
#define printf_ori(fmt,...)		
#endif

MAF_Printf::MAF_Printf()
{

}
MAF_Printf::~MAF_Printf()
{
}

void MAF_Printf::Printf(const char* _Format, ...)
{
    char buf[256];
    va_list args;
    va_start(args, _Format);
    vsprintf_s(buf, 256, _Format, args);
    va_end(args);
    printf_inner(buf);
}
