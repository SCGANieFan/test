#ifndef __M2F_MIPS_MONITOR__H_
#define __M2F_MIPS_MONITOR__H_

#include"../common/M2FCommonInterface.h"
class M2FMipsMonitor:public M2FCommon
{
public:
	M2FMipsMonitor();
	M2FMipsMonitor(const char *pStr);
	~M2FMipsMonitor();
	void start();
	void end(unsigned long long duration_ms);
	void printInfo();
	const char* getName();
	m2f_uint32 getAveMips();
public:
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