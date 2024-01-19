#include <string>

#include"M2FManagement.h"

M2FManagement::M2FManagement()
{
	this->HeapInit(NULL, 0);
	this->mipsMonitorCur = NULL;
	this->mipsMonitorsNum = 0;
	for (int i = 0; i < 10; i++)
	{
		this->mipsMonitors[i] = NULL;
	}
}
M2FManagement::M2FManagement(void* p, m2f_uint32 size)
{
	this->HeapInit(p, size);
	this->mipsMonitorCur = NULL;
	this->mipsMonitorsNum = 0;
	for (int i = 0; i < 10; i++)
	{
		this->mipsMonitors[i] = NULL;
	}
}
M2FManagement::~M2FManagement()
{
	for (int i = 0; i < 10; i++)
	{
		if (this->mipsMonitors[i])
		{
			this->mipsMonitors[i]->~M2FMipsMonitor();
			this->Free(this->mipsMonitors[i]);
		}
		else
			break;
	}
}

int M2FManagement::MipsMonitorCreat(const char* name)
{
	if (this->mipsMonitorsNum >= 10)
		return -1;
	this->mipsMonitorCur = (M2FMipsMonitor*)this->MAlloc(sizeof(M2FMipsMonitor));
	this->mipsMonitors[this->mipsMonitorsNum] = new(this->mipsMonitorCur)M2FMipsMonitor(name);
	this->mipsMonitorsNum++;
	return 0;
}

M2FMipsMonitor* M2FManagement::getMipsMonitor(char* name)
{
	for (int i = 0; i < this->mipsMonitorsNum; i++)
	{
		if (!strcmp(name, this->mipsMonitors[i]->getName()))
			return this->mipsMonitors[i];
	}
	return NULL;
}





