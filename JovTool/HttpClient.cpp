#include "stdafx.h"
#include "HttpClient.h"
#include "Pub.h"
#include "SocketHandle.h"
#include "zlib.h"

#define  BUFFER_SIZE		1024

#define  NORMAL_CONNECT		INTERNET_FLAG_KEEP_CONNECTION
#define  SECURE_CONNECT		NORMAL_CONNECT | INTERNET_FLAG_SECURE
#define  NORMAL_REQUEST		INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE 
#define  SECURE_REQUEST		NORMAL_REQUEST | INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID

//#pragma comment(lib, "zlibwapi.lib")

//#define segment_size 1460//largest tcp data segment
int ungzip(char* source, int len, char*des)
{
	int ret, have;
	int offset = 0;
	z_stream d_stream;
	Byte compr[LENGTH_1M] = { 0 }, uncompr[LENGTH_1M * 4] = { 0 };
	memcpy(compr, (Byte*)source, len);
	uLong comprLen, uncomprLen;
	comprLen = len;//一开始写成了comprlen=sizeof(compr)以及comprlen=strlen(compr)，后来发现都不对。

	//sizeof(compr)永远都是segment_size，显然不对，strlen(compr)也是不对的，因为strlen只算到\0之前，

	//但是gzip或者zlib数据里\0很多。
	uncomprLen = LENGTH_1M * 4;
	strcpy((char*)uncompr, "garbage");

	d_stream.zalloc = Z_NULL;
	d_stream.zfree = Z_NULL;
	d_stream.opaque = Z_NULL;

	d_stream.next_in = Z_NULL;//inflateInit和inflateInit2都必须初始化next_in和avail_in
	d_stream.avail_in = 0;//deflateInit和deflateInit2则不用

	ret = inflateInit2(&d_stream, 47);
	if (ret != Z_OK)
	{
		printf("inflateInit2error:%d", ret);
		return ret;
	}
	d_stream.next_in = compr;
	d_stream.avail_in = comprLen;
	do
	{
		d_stream.next_out = uncompr;
		d_stream.avail_out = uncomprLen;
		ret = inflate(&d_stream, Z_NO_FLUSH);
		//assert(ret != Z_STREAM_ERROR);
		switch (ret)
		{
		case Z_NEED_DICT:
			ret = Z_DATA_ERROR;
		case Z_DATA_ERROR:
		case Z_MEM_ERROR:
			(void)inflateEnd(&d_stream);
			return ret;
		}
		have = uncomprLen - d_stream.avail_out;
		memcpy(des + offset, uncompr, have);//这里一开始我写成了memcpy(des+offset,d_stream.next_out,have);

		//后来发现这是不对的，因为next_out指向的下次的输出，现在指向的是无有意义数据的内存。见下图

		offset += have;

	} while (d_stream.avail_out == 0);
	inflateEnd(&d_stream);
	memcpy(des + offset, "\0", 1);
	return ret;
}

HttpClient::HttpClient(CString strAgent)
{
	m_pSession = new CInternetSession(strAgent);
	m_pConnection = NULL;
	m_pFile = NULL;
}


HttpClient::~HttpClient(void)
{
	Clear();
	if (NULL != m_pSession)
	{
		m_pSession->Close();
		delete m_pSession;
		m_pSession = NULL;
	}
}

void HttpClient::Clear()
{
	if (NULL != m_pFile)
	{
		m_pFile->Close();
		delete m_pFile;
		m_pFile = NULL;
	}

	if (NULL != m_pConnection)
	{
		m_pConnection->Close();
		delete m_pConnection;
		m_pConnection = NULL;
	}
}

int HttpClient::ExecuteRequest(CString strMethod, CString strUrl, CString contentType, int encode, CString strPostData, CString &strResponse)
{
	CString strServer;
	CString strObject;
	DWORD dwServiceType;
	INTERNET_PORT nPort;
	DWORD dwFlags;

	AfxParseURL(strUrl, dwServiceType, strServer, strObject, nPort);

	if (AFX_INET_SERVICE_HTTP != dwServiceType && AFX_INET_SERVICE_HTTPS != dwServiceType)
	{
		return FAILURE;
	}

	//dwFlags = dwServiceType == AFX_INET_SERVICE_HTTPS ? INTERNET_FLAG_SECURE : 0;

	try
	{
		//打开http连接
		
		m_pConnection = m_pSession->GetHttpConnection(strServer,
			dwServiceType == AFX_INET_SERVICE_HTTP ? NORMAL_CONNECT : SECURE_CONNECT,nPort);
		
		//打开一个http请求
		m_pFile = m_pConnection->OpenRequest(strMethod, strObject,
			NULL, 1, NULL, NULL,
			(dwServiceType == AFX_INET_SERVICE_HTTP ? NORMAL_REQUEST : SECURE_REQUEST));
		
		/*
		m_pConnection = m_pSession->GetHttpConnection(strServer, INTERNET_FLAG_SECURE, nPort, NULL, NULL);
		m_pFile = m_pConnection->OpenRequest(strMethod, strObject,
			NULL, 1, NULL, NULL,
			INTERNET_FLAG_SECURE |
			INTERNET_FLAG_EXISTING_CONNECT |
			INTERNET_FLAG_RELOAD |
			INTERNET_FLAG_NO_CACHE_WRITE |
			INTERNET_FLAG_IGNORE_CERT_DATE_INVALID |
			INTERNET_FLAG_IGNORE_CERT_CN_INVALID);

		m_pFile->QueryOption(INTERNET_OPTION_SECURITY_FLAGS, dwFlags);
		dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA | INTERNET_FLAG_SECURE;
		m_pFile->SetOption(INTERNET_OPTION_SECURITY_FLAGS, dwFlags);
		*/
		CString ct;
		if (contentType.IsEmpty())
			ct = _T("Content-Type: text/plain");
		else
			ct = _T("Content-Type: ") + contentType;
		m_pFile->AddRequestHeaders(_T("Accept: *,*/*"));
		m_pFile->AddRequestHeaders(_T("Accept-Language: zh-cn"));
		m_pFile->AddRequestHeaders(ct);
		m_pFile->AddRequestHeaders(_T("Accept-Encoding: gzip, deflate"));
		if (encode == 0)
			m_pFile->AddRequestHeaders(_T("Charset: UTF-8"));
		else if (encode == 1)
			m_pFile->AddRequestHeaders(_T("Charset: GBK"));
		else if (encode == 2)
			m_pFile->AddRequestHeaders(_T("Charset: GB18030"));

		char utfmsg[LENGTH_1M] = { 0 };
		if (strPostData && !strPostData.IsEmpty()){
			if (encode == 1)
				UnicodeToGBK(strPostData, utfmsg);
			else
				UnicodeToUtf8(strPostData, utfmsg);
			TRACE(_T("发送数据：%d"), strPostData.GetLength());
			TRACE(utfmsg);
		}
		if (strMethod == _T("GET"))
			m_pFile->SendRequest();
		else
			m_pFile->SendRequest(NULL, 0, (LPVOID)utfmsg, strlen(utfmsg));
		//m_pFile->SendRequest(NULL, 0, (LPTSTR)(LPCTSTR)strPostData, strPostData.GetLength());

		char szChars[BUFFER_SIZE + 1] = { 0 };
		char strRawResponse[LENGTH_1M] = { 0 }; //这儿不能用字符数组保存，不好合并
		char recvEncode[LENGTH_1M] = { 0 };
		char *pEncode = NULL;
		pEncode = recvEncode;
		char **ppEncode = &pEncode;
		char *pResponse;
		pResponse = strRawResponse;
		UINT nReaded = 0;
		int responseLength = 0;
		unsigned int realLength = 0;
		while ((nReaded = m_pFile->Read((void*)szChars, BUFFER_SIZE)) > 0)
		{
			szChars[nReaded] = '\0';
			memcpy(pResponse, szChars, nReaded);
			pResponse += nReaded;
			responseLength += nReaded;
			memset(szChars, 0, BUFFER_SIZE + 1); 
		}
		

		CString contentEncode;
		m_pFile->QueryInfo(HTTP_QUERY_CONTENT_ENCODING, contentEncode);
		if (0 == contentEncode.Compare(_T("gzip"))){
			ungzip(strRawResponse, responseLength, recvEncode);
		}else
			memcpy(recvEncode, strRawResponse, responseLength);

		wchar_t recv[LENGTH_1M] = { 0 };
		if (encode == 1)
			GBKToUnicode(recvEncode, recv);
		else
			Utf8ToUnicode(recvEncode, recv);

		strResponse = recv;

		Clear();
	}
	catch (CInternetException* e)
	{
		wchar_t recvEncode[LENGTH_1M] = { 0 };
		Clear();
		DWORD dwErrorCode = e->m_dwError;
		e->GetErrorMessage(recvEncode, LENGTH_1M);
		e->Delete();

		DWORD dwError = GetLastError();

		TRACE(_T("dwError = %d"), dwError, 0);

		strResponse = recvEncode;

		if (ERROR_INTERNET_TIMEOUT == dwErrorCode)
		{
			return OUTTIME;
		}
		else
		{
			return FAILURE;
		}
	}
	return SUCCESS;
}

int HttpClient::doGet(CString strUrl, CString contentType, int encode, CString strPostData, CString &strResponse)
{
	return ExecuteRequest(_T("GET"), strUrl, contentType, encode, strPostData, strResponse);
}

int HttpClient::doPost(CString strUrl, CString contentType, int encode, CString strPostData, CString &strResponse)
{
	return ExecuteRequest(_T("POST"), strUrl, contentType, encode, strPostData, strResponse);
}

