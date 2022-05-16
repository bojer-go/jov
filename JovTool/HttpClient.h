#pragma once

#include <afxinet.h>
#include <string>

using namespace std;

#define  IE_AGENT  _T("Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727)")

// 操作成功
#define SUCCESS        0
// 操作失败
#define FAILURE        1
// 操作超时 www.it165.net
#define OUTTIME        2

class HttpClient
{
public:
	HttpClient(CString strAgent = IE_AGENT);
	virtual ~HttpClient(void);

	int doGet(CString strUrl, CString contentType, int encode, CString strPostData, CString &strResponse);
	int doPost(CString strUrl, CString contentType, int encode, CString strPostData, CString &strResponse);

private:
	int ExecuteRequest(CString strMethod, CString strUrl, CString contentType, int encode, CString strPostData, CString &strResponse);
	void Clear();

private:
	CInternetSession *m_pSession;
	CHttpConnection *m_pConnection;
	CHttpFile *m_pFile;

};

