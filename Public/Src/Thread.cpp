#include "stdafx.h"
#include "Thread.h"

Thread::Thread(void)
{
	hThread			=	NULL;
	m_bTerminated	=	FALSE;
	m_bFinished		=	FALSE;

	hWaitFor = CreateEvent(NULL, FALSE, FALSE, NULL);
}

Thread::~Thread(void)
{
	if(!m_bFinished)
		Join(5000);

	CloseHandle(hWaitFor);
	CloseHandle(hThread);
}

void Thread::Start()
{
	if(NULL != hThread)
		return;

	OnStart();

	hThread = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc, this, 0, &threadID);
}

void Thread::Resume()
{
	if(NULL == hThread)
		return;

	OnResume();

	::ResumeThread(hThread);
}

void Thread::Suspend()
{
	if(NULL == hThread)
		return;

	OnSuspend();

	::SuspendThread(hThread);
}

void Thread::Terminate(DWORD dwTimeOut)
{
	if(NULL == hThread)
		return;

	//等到线程执行完后返回
	m_bTerminated = TRUE;

	OnTerminate();

	Join(dwTimeOut);
}

void Thread::OnStart()
{
}

void Thread::OnSuspend()
{
}

void Thread::OnResume()
{
}

void Thread::OnTerminate()
{
}

void Thread::Join(DWORD dwTimeOut)
{
	if(NULL == hThread)
		return;

	//判断线程是否已经结束
	DWORD dwExitCode = 0;
    ::GetExitCodeThread(hThread, &dwExitCode);
    if(dwExitCode != STILL_ACTIVE)
    {
        return;
    }

	//如果线程是寿终正寝，直接返回
	if(m_bFinished)
		return;

	//强制主线程阻塞，采用信号量有时会异常
	WaitForSingleObject(hWaitFor, dwTimeOut);
}

DWORD Thread::ThreadProc(void *pThread)
{
	Thread *thread = static_cast<Thread*>(pThread);
	
	if(!thread->m_bTerminated)
		thread->Execute();
	thread->m_bFinished = TRUE;
	SetEvent(thread->hWaitFor);

	return 0;
}