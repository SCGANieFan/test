//#include"AlgorithmRun.h"
//#include"MM_AudioPlc.h"
class MyClass
{
public:
	MyClass() {};
	~MyClass() {};
public:
	virtual void init(int a) {};
//protected:
protected:
	void init() {};
private:

};
class MyClass2 :public MyClass
{
public:
	MyClass2() {};
	~MyClass2() {};
public:
	virtual void init(int a) override{};
private:

};

bool testDemo()
{
	MyClass *m;
	m = new MyClass2();
	//m->init()
	//m->init(1)

#if 0
	//register
	extern void add32_register(); add32_register();
	extern void add16_register(); add16_register();
	//run
	void* hd32 = AlgorithmCreate("add32");
	void* hd16 = AlgorithmCreate("add16");
	AlgorithmRun(hd16,0,0,0,0);
	AlgorithmRun(hd32,0,0,0,0);
	
	return 1;
#endif
	return 1;
}

