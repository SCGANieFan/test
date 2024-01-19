#ifndef __M2F_MEMORY_MANAGER__H_
#define __M2F_MEMORY_MANAGER__H_

#include"../common/M2FCommonInterface.h"
class M2FMemoryManger:public M2FObject
{
public:
	M2FMemoryManger();
	~M2FMemoryManger();
	void start();
	void end(unsigned long long duration_ms);
	void printInfo();


	const char* getName();


public:
	//bool isRun;
	unsigned long long curkcps;
	unsigned long long avekcps;
	unsigned long long frame;	
private:
	char name[10];
	unsigned long long ticksStart;
	unsigned long long ticksEnd;
	unsigned long long ticksCurr;
	unsigned long long totalticks;
	unsigned long long totalduration_0p1ms;
};



#endif