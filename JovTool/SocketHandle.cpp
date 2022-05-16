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
		//���ó�ʱʱ��
		if(outTime > 0){
			//���÷��ͳ�ʱ
			//setsockopt(*socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&tim,sizeof(int));
			//���ý��ճ�ʱ
			int ret = setsockopt(*socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&outTime,sizeof(int));
			TRACE(_T("\n setsockopt: %d"), ret);
		}
		DWORD beginCount = GetTickCount();

		TRACE(_T("\n before send: %d"), GetTickCount());
		send(*socket, msg, strlen(msg), 0);
		
		TRACE(_T("\n after send: %d"), GetTickCount());
		//�������������whileѭ���������ݣ�����Ҫ�����ر����������Ȼ���������ܻ�ȡ��recv.
		shutdown(*socket, 1);
		//SHUT_RD��0�����ر�sockfd�ϵĶ����ܣ���ѡ�������sockfd���ж���������������ܡ�
		//SHUT_WR��1�����ر�sockfd��д���ܣ���ѡ�������sockfd����д�������������͡�
		//SHUT_RDWR��2�����ر�sockfd�Ķ�д���ܡ��������ͺͽ���(�� close() һ��)��
		
		TRACE(_T("\n before recv: %d"), GetTickCount());
		//recv()�ĵ��ĸ���������ΪMSG_WAITALLʱ��������ģʽ�²��ȵ�ָ����Ŀ�����ݲ��᷵�أ����ǳ�ʱʱ�䵽�������������˿����ӣ�������ָ������Ŀ��һ����ȵ���ʱ
		int ret = 0;
		char tempbuf[LENGTH_1M] = { 0 };
		while (ret = recv(*socket, tempbuf, LENGTH_1M, 0)) //�˴����û�ȡbuff�ĳ��ȣ�strlen����sizeof����ȡ����
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