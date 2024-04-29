#include<stdarg.h>
#include "MAF.Printf.h"

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

maf_void MAF_Printf::Printf(const maf_int8* _Format, ...)
{
    maf_int8 buf[256];
    va_list args;
    va_start(args, _Format);
    vsprintf(buf, _Format, args);
    va_end(args);
    printf_inner(buf);
}
