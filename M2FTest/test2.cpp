#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <iostream>
#include <thread>
#include <Windows.h>
#include <queue>
#include <string>
#include <mutex>


#include"test1.h"
//#include"test.h"

using namespace std;
class MyClass
{
public:
	MyClass() {}
	MyClass(int a) :a(a) {};
	~MyClass() {}

public:
	int a;
};

class MyClass2
{
public:
	MyClass2() {}
	MyClass2(int c):cc(c) {}
	~MyClass2() {}

public:
	MyClass cc;
};


void test2()
{
	MyClass2 cc2(1);

	printf("%d\n", cc2.cc.a);

	//for(int i=0;i<len;i++)
	//	printf("[i] %d\n", x[i]);

}





