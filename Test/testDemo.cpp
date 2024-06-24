#if 1
//#include<string.h>
//#include<stdint.h>
#include<stdio.h>
//#include<stdarg.h>
#include<iostream>
#include<string>
//#include<vector>
#include<assert.h>


void func(int a,int b)
{
	printf("%d\n", a);
}

#define FUNC(...) func(__VA_ARGS__);

bool testDemo()
{
#if 0
	for (int i = 0; i < sizeof(y) / sizeof(y[0]); i++)
	{
		printf("y[i]=%x\n", y[i]);
	}
#endif
	FUNC(1,2);
	return 0;
}

#endif