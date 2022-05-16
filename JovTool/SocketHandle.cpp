#include "StdAfx.h"
#include "SocketHandle.h"


SocketHandle::SocketHandle(SOCKET *sc, char **p_msg, char **p_buff, int out)
{
	socket = sc;
	msg = *p_msg;
	buff = *p_buff;
	outTime = out;
	finishCount = 0;
}


SocketHandle::~SocketHandle(void)
{
}

void SocketHandle::setSocket(SOCKET *sc)
{	
	socket = sc;
}

void SocketHandle::Execute(void)
{
	if(!m_bTerminated)
	{
		//设置超时时间
		if(outTime > 0){
			//设置发送超时
			//setsockopt(*socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&tim,sizeof(int));
			//设置接收超时
			int ret = setsockopt(*socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&outTime,sizeof(int));
			TRACE(_T("\n setsockopt: %d"), ret);
		}
		DWORD beginCount = GetTickCount();

		TRACE(_T("\n before send: %d"), GetTickCount());
		send(*socket, msg, strlen(msg), 0);
		
		TRACE(_T("\n after send: %d"), GetTickCount());
		//如果服务器采用while循环接收数据，则需要主动关闭输出流，不然会阻塞不能获取到recv.
		shutdown(*socket, 1);
		//SHUT_RD（0）：关闭sockfd上的读功能，此选项将不允许sockfd进行读操作。不允许接受。
		//SHUT_WR（1）：关闭sockfd的写功能，此选项将不允许sockfd进行写操作。不允许发送。
		//SHUT_RDWR（2）：关闭sockfd的读写功能。不允许发送和接受(和 close() 一样)。
		
		TRACE(_T("\n before recv: %d"), GetTickCount());
		//recv()的第四个参数设置为MSG_WAITALL时，在阻塞模式下不等到指定数目的数据不会返回，除非超时时间到，若服务器不端开连接，读不到指定的数目则一定会等到超时
		int ret = 0;
		char tempbuf[LENGTH_1M] = { 0 };
		while (ret = recv(*socket, tempbuf, LENGTH_1M, 0)) //此处不好获取buff的长度，strlen或者sizeof都获取不了
		{
			if (ret > 0){
				strcat(buff, tempbuf);
			}
		}

		TRACE(_T("\n after recv: %d "), GetTickCount());

		TRACE(_T("\n WSAGetLastError: %d recv: %d \n"), WSAGetLastError(), ret);//WSAETIMEDOUT
		
		if(strlen(buff) <=0 || (ret == -1 && errno == EAGAIN)){
			sprintf(buff, "%08d%s", strlen("recv timeout..."), "recv timeout...");
		}
		finishCount = GetTickCount() - beginCount;
	}
}

DWORD SocketHandle::getFinishCount()
{
	return finishCount;
}