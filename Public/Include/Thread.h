#pragma once


#include <Windows.h>

class Thread
{
public:
	Thread(void);
	~Thread(void);

	static DWORD ThreadProc(void *pThread);
public:
	void Start();		//��ʼ
	void Suspend();		//����
	void Resume();		//����
	//��ֹ�����ӳ�ʱ���ƣ�����1����ǿ��ɱ�߳�
	void Terminate(DWORD dwTimeOut = INFINITE);	
	
	virtual void Execute()=0;
public:
	void Join(DWORD dwTimeOut = INFINITE);	//����terminate����
protected:
	//�������д����Ӧ����ִ��ǰ��Ӧ�ĺ���
	virtual void OnStart();
	virtual void OnSuspend();
	virtual void OnResume();
	virtual void OnTerminate();
private:
	HANDLE hThread;
	HANDLE hWaitFor;				//�ȴ��߳�ִ�����
	
	volatile bool m_bFinished;		//�߳��Ƿ����
protected:
	DWORD threadID;
	volatile bool m_bTerminated;	//�߳��Ƿ���ֹ
};