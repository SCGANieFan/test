#include <intrin.h>
#include <chrono>
#include <string>

#include"M2FMipsmonitor.h"

//print
#define M2FMonitorDebug 1
#if M2FMonitorDebug 
#define M2FMonitorPrint(s,...)				this->Print("[M2F_Monitor](%d) " s "\n", __LINE__, ##__VA_ARGS__)
#else
#define M2F_MonitorPrint(s,...)				
#endif

M2FMipsMonitor::M2FMipsMonitor()
{
	
	this->curkcps = 0;
	this->avekcps = 0;
	this->frame = 0;

	this->name[0] = '\0';
	this->ticksStart = 0;
	this->ticksEnd = 0;
	this->ticksCurr = 0;
	this->totalticks = 0;
	this->totalduration_0p1ms = 0;
}
M2FMipsMonitor::M2FMipsMonitor(const char* pStr)
{
	this->curkcps = 0;
	this->avekcps = 0;
	this->frame = 0;

	sprintf_s(this->name, 10, "%s", pStr);
	//this->name = pStr;
	this->ticksStart = 0;
	this->ticksEnd = 0;
	this->ticksCurr = 0;
	this->totalticks = 0;
	this->totalduration_0p1ms = 0;
}

M2FMipsMonitor::~M2FMipsMonitor()
{
}
void M2FMipsMonitor::start()
{
	this->ticksStart = __rdtsc();
}
void M2FMipsMonitor::end(unsigned long long duration_0p1ms)
{
	this->frame++;
	if(this->frame == 1)
		return;
	this->ticksEnd = __rdtsc();
	this->ticksCurr = this->ticksEnd - this->ticksStart;

	this->curkcps = this->ticksCurr / (100 * duration_0p1ms);
	this->totalticks += this->ticksCurr;
	this->totalduration_0p1ms += duration_0p1ms;
	this->avekcps = this->totalticks / (100 * this->totalduration_0p1ms);
	
	this->printInfo();
}
void M2FMipsMonitor::printInfo()
{
	M2FMonitorPrint("%s" "mips[%u]: curkcps:%u, avekcps:%u",
		this->name, (unsigned int)this->frame, (unsigned int)this->curkcps, (unsigned int)this->avekcps);	
}

const char* M2FMipsMonitor::getName()
{
	return this->name;
}

m2f_uint32 M2FMipsMonitor::getAveMips()
{
	return this->avekcps;
}

