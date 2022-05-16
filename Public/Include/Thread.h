#pragma once


#include <Windows.h>

class Thread
{
public:
	Thread(void);
	~Thread(void);

	static DWORD ThreadProc(void *pThread);
public:
	void Start();		//开始
	void Suspend();		//挂起
	void Resume();		//继续
	//终止，增加超时限制，超过1分钟强制杀线程
	void Terminate(DWORD dwTimeOut = INFINITE);	
	
	virtual void Execute()=0;
public:
	void Join(DWORD dwTimeOut = INFINITE);	//开放terminate函数
protected:
	//子类可重写，相应函数执行前响应的函数
	virtual void OnStart();
	virtual void OnSuspend();
	virtual void OnResume();
	virtual void OnTerminate();
private:
	HANDLE hThread;
	HANDLE hWaitFor;				//等待线程执行完毕
	
	volatile bool m_bFinished;		//线程是否结束
protected:
	DWORD threadID;
	volatile bool m_bTerminated;	//线程是否终止
};