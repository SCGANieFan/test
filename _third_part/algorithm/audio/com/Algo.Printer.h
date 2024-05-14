#pragma once
#include"Algo.Type.h"

#if 1
#include<stdio.h>
#include<stdarg.h>
namespace Algo {
	class Printer {
	public:
		Printer() {};
		~Printer() {};
	public:
		static void Printf(const i8* _Format, ...) {
			i8 buf[256];
			va_list args;
			va_start(args, _Format);
			vsprintf(buf, _Format, args);
			va_end(args);
			printf(buf);
		}
	};

#define ALGO_PRINT(fmt,...)		Printer::Printf("[%s](%d)" fmt "\n",__func__, __LINE__, ##__VA_ARGS__)
}
#endif

