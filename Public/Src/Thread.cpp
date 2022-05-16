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

	//�ȵ��߳�ִ����󷵻�
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

	//�ж��߳��Ƿ��Ѿ�����
	DWORD dwExitCode = 0;
    ::GetExitCodeThread(hThread, &dwExitCode);
    if(dwExitCode != STILL_ACTIVE)
    {
        return;
    }

	//����߳����������ޣ�ֱ�ӷ���
	if(m_bFinished)
		return;

	//ǿ�����߳������������ź�����ʱ���쳣
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