#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <iostream>
#include <thread>
#include <Windows.h>
#include <queue>
#include <string>
#include <mutex>


#include"test1.h"

using namespace std;



//thread thr;
//returnIfErrC(false, !thr.Start(Name(), [](void* priv) {((Element*)priv)->Loop(); }, this, _stack_size));
//thr.detach();


//thread
#if 0
HANDLE coutMutex;
DWORD WINAPI MyThread(LPVOID lpParamter)
{
	WaitForSingleObject(coutMutex, INFINITE);
	cout << "MyThread, input: " << *(int*)lpParamter << endl;
	ReleaseMutex(coutMutex);
	//for (int i = 0; i < 2; i++)
	//{
	//	cout << "[" << *(int*)lpParamter << "]" << i << endl;
	//}

	return 0L;
}

void func()
{
	int input[10];
	coutMutex = CreateMutex(NULL, FALSE, NULL);
	for (int i = 0; i < 10; i++)
	{
		input[i] = i;
		HANDLE hThread = CreateThread(NULL, 0, MyThread, &input[i], 0, NULL);
	}

	//HANDLE hThread = CreateThread(NULL, 0, MyThread, &input, 0, NULL);
	//WaitForSingleObject(hThread, INFINITE);
	system("PAUSE");
}
#endif


//thread pool
#if 0

#define CORE_THREAD_NUM 8
#define ALIVE_TIME 5000

typedef void(*fp_return_void)(void* paramter);


class Task
{
private:
	fp_return_void fp;
	void* paramter;
	Task(fp_return_void fp, void* paramter) :
		fp(fp),
		paramter(paramter)
	{
	}
	void run()
	{
		fp(paramter);
	}
public:
	friend class KyThreadPool;

};


class KyThreadPool
{
private:
	queue<Task> task_queue;
	static DWORD WINAPI SingleThread(LPVOID lpParamter);
	void AddOneThread();

	int CoreThreadNum = CORE_THREAD_NUM;
	int NowThreadNUm = 0;
	int ThreadEndFlag = 0;
	int AliveTime = ALIVE_TIME;

	HANDLE TaskQueue_mutex;
	HANDLE NowThreadNum_mutex;
	HANDLE ActiveThreadNum_mutex;
	int ActiveThreadNum;

public:
	KyThreadPool()
	{
		TaskQueue_mutex = CreateMutex(NULL, FALSE, NULL);
		NowThreadNum_mutex = CreateMutex(NULL, FALSE, NULL);
	}
	KyThreadPool(int CoreThreadNum, int AliveTime)
	{
		this->CoreThreadNum = CoreThreadNum;
		this->AliveTime = AliveTime;

		TaskQueue_mutex = CreateMutex(NULL, FALSE, NULL);
		NowThreadNum_mutex = CreateMutex(NULL, FALSE, NULL);
	}
	void AddTask(fp_return_void function, void* arg);
	void WaitAndEnd(void);
	void PoolRestart(void);

};
DWORD WINAPI KyThreadPool::SingleThread(LPVOID lpParamter)
{
	DWORD waitStartTime = GetTickCount();
	KyThreadPool* pool = (KyThreadPool*)lpParamter;
	while (1)
	{
		if (!pool->task_queue.empty()
			&& pool->ThreadEndFlag == 0)
		{
			WaitForSingleObject(pool->TaskQueue_mutex, INFINITE);
			if (!pool->task_queue.empty())
			{
				Task T = pool->task_queue.front();
				pool->task_queue.pop();
				pool->ActiveThreadNum++;
				ReleaseMutex(pool->TaskQueue_mutex);

				T.run();
				waitStartTime = GetTickCount();

				WaitForSingleObject(pool->ActiveThreadNum_mutex, INFINITE);
				pool->ActiveThreadNum--;
				ReleaseMutex(pool->ActiveThreadNum_mutex);
			}
			else
				ReleaseMutex(pool->TaskQueue_mutex);
		}
		else if (pool->ThreadEndFlag == 1
			|| GetTickCount() - waitStartTime > pool->AliveTime)
		{
			WaitForSingleObject(pool->NowThreadNum_mutex, INFINITE);
			pool->NowThreadNUm--;
			ReleaseMutex(pool->NowThreadNum_mutex);
			return 0L;
		}
	}
	return 0L;
}
void KyThreadPool::AddOneThread()
{
	HANDLE hThread = CreateThread(NULL, 0, SingleThread, this, 0, NULL);
	WaitForSingleObject(NowThreadNum_mutex, INFINITE);
	NowThreadNUm++;
	ReleaseMutex(NowThreadNum_mutex);
}


void KyThreadPool::AddTask(fp_return_void function, void* arg)
{
	WaitForSingleObject(TaskQueue_mutex, INFINITE);
	Task task(function, arg);
	this->task_queue.push(task);
	ReleaseMutex(TaskQueue_mutex);

	if (task_queue.size() > 0
		&& NowThreadNUm < CoreThreadNum)
	{
		AddOneThread();
	}
}
void KyThreadPool::WaitAndEnd(void)
{
	while (ActiveThreadNum > 0 || task_queue.empty());
	ThreadEndFlag = 1;
}
void KyThreadPool::PoolRestart(void)
{
	ThreadEndFlag = 0;
	for (int i = 0; i < task_queue.size() && i < CoreThreadNum; i++)
	{
		AddOneThread();
	}
}
HANDLE cout_mutex;
void TestTask(void* lpParamter)
{
	int* pmd = (int*)lpParamter;
	WaitForSingleObject(cout_mutex, INFINITE);
	cout << "Task" << to_string(pmd[0]) << "is working" << endl;
	ReleaseMutex(cout_mutex);

	Sleep(1000);

	WaitForSingleObject(cout_mutex, INFINITE);
	cout << "Task" << to_string(pmd[0]) << "finished" << endl;
	ReleaseMutex(cout_mutex);
}

void func()
{
	KyThreadPool pooTest;
	int a[40];

	for (int i = 0; i < 15; i++)
	{
		a[i] = i;
		pooTest.AddTask(TestTask, a + i);
	}
	pooTest.WaitAndEnd();
	cout << "-----------------------all Task finished" << endl;

	pooTest.PoolRestart();
	for (int i = 0; i < 15; i++)
	{
		a[i] = i;
		pooTest.AddTask(TestTask, a + i);
	}
	pooTest.WaitAndEnd();
	cout << "-----------------------all Task finished again" << endl;
}

#endif



//thread
#if 1

mutex mtx;

void threadFunction(int& n)
{
	//cout << "Thread " << this_thread::get_id() << " start" << endl;
	for (int i = 0; i < 100; ++i)
	{
		//mtx.lock();
		n++;
		//mtx.unlock();
	}
}
int num = 0;
void func()
{
	thread mythread[500];
	for (thread& a : mythread)
	{
		a = thread(threadFunction, ref(num));
		a.join();
	}

	cout << "num = " << num << endl;
	cout << "Main thread exits" << endl;

}

#endif

void testThread()
{
	func();
}





