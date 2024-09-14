#if 1
#include"testInner.h"



#if 1
//thread class
#include <windows.h>
#include <mutex>
#include <condition_variable>



typedef unsigned long MTF_ThreadResult_t;
typedef MTF_ThreadResult_t(*MTF_StartRoutine_CB)(void*);

class MTF_Thread_c
{
public:
	MTF_Thread_c() {};
	~MTF_Thread_c() {};
public:
	static void* ThreadCreate(MTF_StartRoutine_CB startRoutine, void* arg) {
		HANDLE handle = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)startRoutine, (LPVOID)arg, 0, NULL);
		return (void*)handle;
	}
	static void ThreadDestroy(void* t) {
		CloseHandle(t);
	}
	static void* MutexCreate() {
		return new std::mutex();
	}
	static void MutexDestroy(void* mutex) {
		delete mutex;
	}
	static void MutexLock(void* mutex) {
		((std::mutex*)mutex)->lock();
	}
	static void MutexUnlock(void* mutex) {
		((std::mutex*)mutex)->unlock();
	}
#if 1
	static void* CondCreate() {
		void* cv = new std::condition_variable();
		return cv;
	}
	static void CondDestroy(void* cv) {
		delete cv;
	}
	static void CondBroadcast(void* cv, void* m){
		std::unique_lock<std::mutex> lck(*(std::mutex*)m);
		((std::condition_variable*)cv)->notify_all();
	}
	static void CondWait(void* cv, void* m) {
		std::unique_lock<std::mutex> lck(*(std::mutex*)m);
		((std::condition_variable*)cv)->wait(lck);
	}
	static void CondSignal(void* cv, void* m){
		std::unique_lock<std::mutex> lck(*(std::mutex*)m);
		((std::condition_variable*)cv)->notify_one();
	}
#endif
private:

private:
	//void* _thread;
};
#endif




#if 1

typedef struct {
	int threadId;
	void* mutex;
	void* conV;
}TaskUsearData_t;

static int flag = 0;
class Task_c
{
public:
	Task_c() {};
	Task_c(int id) { _id = id; }
	~Task_c() {}
public:
	int _id=0;
	TaskUsearData_t _userData;
private:
	static const int _stateNum = 4;
	static int _state[_stateNum];
public:	
	static void task(Task_c* param) {
#if 0
		LOG("<%d> id = %d", param->_userData.threadId,param->_id);
#else
		if (param->_id == 1)
			for (int i = 0; i < 10 * 1024; i++);

		_state[param->_id % _stateNum] = 0;
		if (param->_id == 0) {
			goto Exit;
		}
		while (_state[(param->_id - 1) % _stateNum] != 1) {
			MTF_Thread_c::CondWait(param->_userData.conV, param->_userData.mutex);
		}
Exit:
		MTF_Thread_c::CondSignal(param->_userData.conV, param->_userData.mutex);
		LOG("<id:%d><task:%d>", param->_userData.threadId, param->_id);
		if (param->_id == 0)
			flag = 2;
		if (param->_id == 0)
			flag = 1;
		//if (param->_id == 1)
			//int a = 1;
		_state[param->_id % _stateNum] = 1;
#endif
	}
};
int Task_c::_state[Task_c::_stateNum];


#endif
#include <deque>


typedef struct {
	int id;
	void* mutex;
	void* conV;
	std::deque<Task_c*> *tasks;
}WorkThreadParamUser;

static int constVal = 0;
static MTF_ThreadResult_t WorkThread(void* paramIn) {
#if 1
	WorkThreadParamUser* param = (WorkThreadParamUser*)paramIn;
	while (1) {
		MTF_Thread_c::MutexLock(param->mutex);
#if 0
		while (param->tasks->size()<1);
#else
		if (param->tasks->size() < 1)
			break;
#endif
		Task_c* task = param->tasks->front();
		param->tasks->pop_front();
		task->_userData = { param->id ,param->mutex , param->conV };
		MTF_Thread_c::MutexUnlock(param->mutex);
		task->task(task);
		delete task;
	}
#else
#if 0
	WorkThreadParamUser* param = (WorkThreadParamUser*)paramIn;
	while (1) {
		MTF_Thread_c::MutexLock(param->mutex);
		constVal = param->id;
		MTF_Thread_c::MutexUnlock(param->mutex);
		if (constVal != param->id)
			int a = 1;
	}
#endif

#endif
	return 0;
}


#if 1

bool testThread()
{
	LOG();
	std::deque<Task_c*> tasks;
	const int taskNum = 20;
	for (int n = 0; n < taskNum; n++) {
		tasks.push_back(new Task_c(n));
	}
	const int threadNum = 2;
	void* threadPtr[threadNum];
	WorkThreadParamUser threadParam[threadNum];
	void *mutex = MTF_Thread_c::MutexCreate();
	void *conV = MTF_Thread_c::CondCreate();
	for (int n = 0; n < threadNum; n++) {
		threadParam[n] = {n,mutex,conV ,&tasks};
		threadPtr[n] = MTF_Thread_c::ThreadCreate(WorkThread, &threadParam[n]);
	}
	while (1);
	for (int n = 0; n < threadNum; n++) {
		MTF_Thread_c::ThreadDestroy(threadPtr[n]);
	}
	return 0;
}
#endif
#endif