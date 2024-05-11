#if 1
//#include<string.h>
//#include<stdint.h>
//#include<stdio.h>
//#include<stdarg.h>
#include<iostream>
#include<string>
//#include<vector>

#if 1
using namespace std;
class P
{
public:
	P() {};
	~P() {};
public:
};

class C:public P
{
public:
	C() {};
	~C() {};
public:
	//operator bool()const;
	//bool Valid()const;

	operator bool() const{
		//val = 1;
		return true;
	}
	//std::string *operator ->()
	//std::string& operator *()

	void func(){
		val = 1;
	}
public:
	int val;
};


#endif


bool testDemo()
{
	char x[4] = { 1,2,3,4 };
	//int24_t i24(*((int*)x));
	//int24_t *i24=(int24_t*)x;
	//int i24=*((int24_t*)x);
	//int i24_1 = *((int24_t*)x + 1);
	//int24_t x24(15);
	//int24_t x24 = 15;
	//int24_t x24_2 = 16;
	//int x32 = (int)x24_2;



	return 0;
}

#endif