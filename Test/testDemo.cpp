#if 1
//#include<string.h>
//#include<stdint.h>
#include<stdio.h>
//#include<stdarg.h>
#include<iostream>
#include<string>
//#include<vector>
#include<assert.h>

class MyClass
{
public:
	MyClass();
	~MyClass();

private:
	static const int len = 10/3;
	int xx[len];
};

MyClass::MyClass()
{
}

MyClass::~MyClass()
{
}

bool testDemo()
{

	MyClass my;

	return 0;
}

#endif