#include"testInner.h"

class MyClass
{
public:
	MyClass();
	~MyClass();

private:
	int _a = 1;
private:
	int func();
};
MyClass::MyClass()
{
	_a = 2;
}

MyClass::~MyClass()
{
}
int MyClass::func()
{
	return _a;
}

void func()
{
	
	printf("size:%d\n", sizeof(MyClass));
}



bool testDemo()
{
	func();
	return 1;
}