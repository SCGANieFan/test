#ifndef __M2F_MANAGEMENT__H_
#define __M2F_MANAGEMENT__H_

//#include"./common/M2FCommonInterface.h"
#include"./M2FMipsMonitor.h"


class M2FManagement:virtual public M2FCommon
{
	//functions
public:
	M2FManagement();
	M2FManagement(void* p, m2f_uint32 size);
	~M2FManagement();

	//MipsMonitor
	int MipsMonitorCreat(const char *name);
	M2FMipsMonitor* getMipsMonitor(char* name);

protected:
private:
	//variable
public:
protected:
private:
	M2FMipsMonitor* mipsMonitorCur;
	unsigned char mipsMonitorsNum;
	M2FMipsMonitor* mipsMonitors[10];


};





#endif