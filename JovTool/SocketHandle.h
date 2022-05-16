#pragma once

#include "Thread.h"

class SocketHandle:public Thread
{
private:
	SOCKET *socket;
	char *msg;
	char *buff;
	int outTime;
	DWORD finishCount;
public:
	SocketHandle(SOCKET *sc, char **p_msg, char **p_buff, int out=0);
	~SocketHandle(void);

public:
	virtual void Execute(void);

	void setSocket(SOCKET *sc);
	DWORD getFinishCount();
};

