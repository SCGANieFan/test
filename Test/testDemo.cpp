#if 1
#include<string.h>
#include<stdio.h>


class P0
{
public:
	P0() {};
	~P0() {};
public:
	static void Print() { printf("p0\n"); };
	int a = 1;
};

class P1
{
public:
	P1() {};
	~P1() {};
public:
	void Print() { printf("p1\n"); };
	int b = 2;
};

class Child: public P0,  public P1
{
public:
	Child() {};
	~Child() {};
public:
};


bool testDemo()
{
	Child c;
	P0* p0 = (P0*)&c;
	//P1* p1 = (P1*)&c;


	p0->Print();
	((P1*)p0)->Print();
	return 0;
}

#endif