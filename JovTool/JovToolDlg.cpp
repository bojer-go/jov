
// JovToolDlg.cpp : implementation file
//
#include "stdafx.h"

#include "JovTool.h"
#include "JovToolDlg.h"
#include "afxdialogex.h"
#include <locale.h>
#include "SocketHandle.h"

#include "include\cef_browser.h"
#include "include\cef_command_line.h"
#include "webHandler.h"
#include "surf.h"
#include "simple_app.h"
#include "tinystr.h"
#include "tinyxml.h"
#include "json\json.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;//注意__FILE__是双下划线
#endif 

// CAboutDlg dialog used for App About

#ifdef _DEBUG
#pragma comment(lib, "json_vc71_libmtd.lib")
#else
#pragma comment(lib, "json_vc71_libmt.lib")
#endif

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CJovToolDlg dialog




CJovToolDlg::CJovToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CJovToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_client = new HttpClient();
	m_encode = 0;
	m_linuxPacket = 0;
	m_catchPack = NULL;
}

CJovToolDlg::~CJovToolDlg()
{
	delete m_client;
	if (m_catchPack)
		delete m_catchPack;
}

void CJovToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONSUME, m_consume);
	DDX_Control(pDX, IDC_SEND_DATA, m_sendData);
	DDX_Control(pDX, IDC_RECV_DATA, m_recvData);
}

BEGIN_MESSAGE_MAP(CJovToolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START_SERVER, &CJovToolDlg::OnBnClickedStartServer)
	ON_BN_CLICKED(IDC_SEND, &CJovToolDlg::OnBnClickedSend)	
	ON_BN_CLICKED(IDC_CLEAR, &CJovToolDlg::OnBnClickedClear)
	ON_WM_SIZE()
	ON_EN_CHANGE(IDC_RECV_DATA, &CJovToolDlg::OnEnChangeRecvData)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_POST, &CJovToolDlg::OnBnClickedPost)
	ON_BN_CLICKED(IDC_GET, &CJovToolDlg::OnBnClickedGet)
	ON_CBN_SELCHANGE(IDC_ENCODE, &CJovToolDlg::OnCbnSelchangeEncode)
	ON_BN_CLICKED(IDC_FORMAT, &CJovToolDlg::OnBnClickedFormat)
	ON_BN_CLICKED(IDC_LINUXCHECK, &CJovToolDlg::OnBnClickedLinuxcheck)
	ON_EN_CHANGE(IDC_SEND_DATA, &CJovToolDlg::OnEnChangeSendData)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_TRANSLATE, &CJovToolDlg::OnBnClickedTranslate)
	ON_BN_CLICKED(IDC_OPEN, &CJovToolDlg::OnBnClickedOpen)
	ON_BN_CLICKED(IDC_CODEBYTE, &CJovToolDlg::OnBnClickedCodeByte)
	ON_BN_CLICKED(IDC_DECODE, &CJovToolDlg::OnBnClickedDecode)
	ON_BN_CLICKED(IDC_CATCHPACK, &CJovToolDlg::OnBnClickedCatchpack)
END_MESSAGE_MAP()


// CJovToolDlg message handlers

BOOL CJovToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CDialogEx::SetBackgroundImage(IDB_BGM);

	SetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE,
		GetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE)|WS_EX_LAYERED);
	::SetLayeredWindowAttributes(this->GetSafeHwnd(),RGB(100,255,0), 230, LWA_ALPHA);

	CEdit *p_Edit;
	p_Edit = (CEdit*)GetDlgItem(IDC_SEND_DATA);
	p_Edit->SetLimitText(UINT_MAX);//设置显示最大字符数

	p_Edit = (CEdit*)GetDlgItem(IDC_RECV_DATA);
	p_Edit->SetLimitText(UINT_MAX);//设置显示最大字符数

	SetDlgItemTextW(IDC_LENGTH, _T("8"));

	CComboBox *pComboBox = ((CComboBox*)GetDlgItem(IDC_ENCODE));
	pComboBox->AddString(_T("UTF-8"));
	pComboBox->AddString(_T("GBK"));
	//pComboBox->AddString(_T("GB18030"));
	pComboBox->SetCurSel(0);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CJovToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CJovToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CJovToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CJovToolDlg::OnBnClickedStartServer()
{
	// TODO: Add your control notification handler code here

}

void CJovToolDlg::OnBnClickedSend()
{
	// TODO: Add your control notification handler code here
	SOCKET client;
	SOCKADDR_IN servAddr;
	WSADATA wsaData;  
	CString ip;
	CString port;
	CString length;
	CString outtime;
	char addr[20]={0};
	char buff[LENGTH_1M] = {0};
	char sendBuff[LENGTH_1M] = {0};
	char msg[LENGTH_1M] = {0};
	char utfmsg[LENGTH_1M] = {0};
	char format[64] = {0};
	int headLen = 8;
	CString sendMsg;

	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){
		AfxMessageBox(_T("Failed to load Winsock！"));
		return;
	}
	char* old_locale = _strdup( setlocale(LC_CTYPE,NULL) );
	setlocale( LC_CTYPE, "chs" );

	GetDlgItem(IDC_IP)->GetWindowText(ip);
	GetDlgItem(IDC_PORT)->GetWindowText(port);
	GetDlgItem(IDC_LENGTH)->GetWindowText(length);
	GetDlgItem(IDC_SEND_DATA)->GetWindowText(sendMsg);
	GetDlgItem(IDC_OUTTIME)->GetWindowText(outtime);

	if(ip.IsEmpty() || port.IsEmpty()){
		AfxMessageBox(_T("未输入地址信息! "));
		return;
	}
	S2C(addr,ip);
	
	if(sendMsg.IsEmpty()){
		AfxMessageBox(_T("请输入发送数据! "));
		return;
	}
	if (m_linuxPacket == 1)
		sendMsg.Replace(_T("\r\n"), _T("\n"));

	S2C(sendBuff, sendMsg);

	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(_wtoi(port));
	servAddr.sin_addr.S_un.S_addr = inet_addr(addr); 

	client = socket(AF_INET, SOCK_STREAM, 0);
	if(SOCKET_ERROR == client){
		AfxMessageBox(_T("create socket error! "));
		return;
	}
	if(connect(client, (struct sockaddr*)&servAddr, sizeof(servAddr)) == INVALID_SOCKET){
		AfxMessageBox(_T("connect failed"));
		return;
	}
	
	if (length.IsEmpty())
		headLen = 8;
	else
		headLen = _wtoi(length);
	if(headLen < 0)
		headLen = 8;

	switch(headLen){
	case 0:{
		sprintf(format, "%s", "%s");
		break;}
	case 1:{
		sprintf(format, "%s", "%01d%s");
		break;}
	case 2:{
		sprintf(format, "%s", "%02d%s");
		break;}
	case 3:{
		sprintf(format, "%s", "%03d%s");
		break;}
	case 4:{
		sprintf(format, "%s", "%04d%s");
		break;}
	case 5:{
		sprintf(format, "%s", "%05d%s");
		break;}
	case 6:{
		sprintf(format, "%s", "%06d%s");
		break;}
	case 7:{
		sprintf(format, "%s", "%07d%s");
		break;}
	case 8:{
		sprintf(format, "%s", "%08d%s");
		break;}
	case 9:{
		sprintf(format, "%s", "%09d%s");
		break;}
	case 10:{
		sprintf(format, "%s", "%010d%s");
		break;}
	default:{
		sprintf(format, "%s", "%08d%s");
		break;}
	}
	
	int temp = strlen(sendBuff);
	TRACE(_T("msg LENGTH:%d"), temp);

	if (m_encode == 1)
		UnicodeToGBK(sendMsg, utfmsg);
	else 
		UnicodeToUtf8(sendMsg, utfmsg);

	temp = strlen(utfmsg);
	TRACE(_T("utf-msg LENGTH_:%d"), temp);

	if(headLen == 0)
		sprintf(msg, format, utfmsg);
	else
		sprintf(msg, format, strlen(utfmsg), utfmsg);
	temp = strlen(msg);

	char *msg1 = msg;	//取数组的指针不好处理，用指针表示
	char *buff1 = buff;

	//SetTimer(TIMER_CONSUME, 1, NULL);
	//报文发送接收采用子线程处理
	SocketHandle handle = SocketHandle(&client, &msg1, &buff1, _wtoi(outtime)); //使用栈空间就够了 处理线程不再开辟堆空间
	handle.Start();
	handle.Join();	//强制主线程等待，等待子线程执行完毕
	//KillTimer(TIMER_CONSUME);

	//这儿不好用timer  因为主线程在等待，子线程执行完后才会到主线程，所以非要时间滚动的话可以使用在子线程使用消息通知
	CString sDateTime;
	sDateTime.Format(_T(" %d "), handle.getFinishCount());
	m_consume.SetWindowText(sDateTime);

	char recvLen[32] = {0};
	strncpy(recvLen, buff, headLen);
	int rvLen = atoi(recvLen);
	if(headLen == 0)
		rvLen = strlen(buff);
	char buff2[LENGTH_1M] = {0};
	strncpy(buff2, buff + headLen, rvLen);

	wchar_t recv[LENGTH_1M] = {0}; 
	if (m_encode == 1)
		GBKToUnicode(buff2, recv);
	else
		Utf8ToUnicode(buff2, recv);

	//CMarkup xml;
	//xml.SetDoc(recv);
	//xml.Save(_T("respose.xml"));

	CString fromatResponse = _T("");
	/*
	TiXmlDocument xmlDocument;
	char tempRes[LENGTH_1M] = { 0 };
	S2C(tempRes, recv);
	xmlDocument.Parse(tempRes, 0, TIXML_ENCODING_UTF8);
	//xmlDocument.SaveFile("response.xml");

	TiXmlPrinter printer;
	xmlDocument.Accept(&printer);
	fromatResponse = printer.CStr();
	fromatResponse.Replace(_T("\n"), _T("\r\n"));
	*/
	fromatResponse = CString(recv);
	SetDlgItemTextW(IDC_RECV_DATA, fromatResponse);

	closesocket(client);  
    WSACleanup();  

	setlocale( LC_CTYPE, old_locale );
	free( old_locale );//还原区域设定
}

void CJovToolDlg::OnBnClickedClear()
{
	// TODO: Add your control notification handler code here
	//SetDlgItemTextW(IDC_SEND_DATA, CString(""));
	SetDlgItemTextW(IDC_RECV_DATA, CString(""));
	SetDlgItemTextW(IDC_CONSUME, CString(""));
	//KillTimer(TIMER_CONSUME);
}


void CJovToolDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	AdjustControl();
}


void CJovToolDlg::OnEnChangeRecvData()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


void CJovToolDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if(TIMER_CONSUME == nIDEvent)
	{	
		CString sDateTime;
		sDateTime.Format(_T(" %d "), GetTickCount());
		m_consume.SetWindowText(sDateTime);
	}

	CDialogEx::OnTimer(nIDEvent);
}


BOOL CJovToolDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	//ctrl+w 打开浏览器
	if (pMsg->message == WM_KEYDOWN
		&& (pMsg->wParam == 'w' || pMsg->wParam == 'W')
		&& GetAsyncKeyState(VK_CONTROL))
	{
		surf web;
		web.DoModal();
		return TRUE;	//注意该return不能少，否则关闭窗口后可能会出现断言错误
	}
	if (pMsg->message == WM_KEYDOWN
		&& (pMsg->wParam == 'p' || pMsg->wParam == 'P')
		&& GetAsyncKeyState(VK_CONTROL))
	{
		if (m_catchPack == NULL){
			m_catchPack = new CCatchPack();
			m_catchPack->Create(IDD_CATCHPACK);
			m_catchPack->setEncode(m_encode);
		}
		else{
			delete m_catchPack;
			m_catchPack = new CCatchPack();
			m_catchPack->Create(IDD_CATCHPACK);
			m_catchPack->setEncode(m_encode);
		}
		m_catchPack->ShowWindow(SW_SHOWNORMAL);
		return TRUE;	//注意该return不能少，否则关闭窗口后可能会出现断言错误
	}
	if (pMsg->message == WM_KEYDOWN)
	{
		BOOL b = GetKeyState(VK_CONTROL) & 0x80;
		CWnd *pActiveWnd = GetFocus();
		DWORD nWndID = 0;
		if (pActiveWnd)
		{
			nWndID = pActiveWnd->GetDlgCtrlID();//获取光标所在的框
		}
		if (b && (pMsg->wParam == 'a' || pMsg->wParam == 'A'))
		{
			SendDlgItemMessage(nWndID, EM_SETSEL, 0, -1);
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


int CJovToolDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	return 0;
}

void CJovToolDlg::AdjustControl()
{
	CRect rect;
	GetClientRect(&rect);

	//控件右移
	CRect temp;
	if (m_sendData.GetSafeHwnd()){
		m_sendData.GetWindowRect(&temp);
		ScreenToClient(temp);
		temp.right = rect.right - 12;
		m_sendData.MoveWindow(temp);
	}
	if (m_recvData.GetSafeHwnd()){
		m_recvData.GetWindowRect(&temp);
		ScreenToClient(temp);
		temp.right = rect.right - 12;
		temp.bottom = rect.bottom - 12;
		m_recvData.MoveWindow(temp);
	}
	CEdit *length = (CEdit*)GetDlgItem(IDC_SEND_DATA);
	if (length->GetSafeHwnd()){
		GetDlgItem(IDC_SEND_LENGTH)->GetWindowRect(&temp);
		ScreenToClient(temp);
		temp.left = rect.right - 80;
		temp.right = rect.right - 12;
		GetDlgItem(IDC_SEND_LENGTH)->MoveWindow(temp);
	}
}

int CJovToolDlg::GetCStringLength(int encode, int uFlag, CString str){
	char sendDest[LENGTH_1M] = { 0 };

	if (uFlag == 1)
		str.Replace(_T("\r\n"), _T("\n"));

	if (encode == 1)
		UnicodeToGBK(str, sendDest);
	else
		UnicodeToUtf8(str, sendDest);

	return strlen(sendDest);
}

void CJovToolDlg::SetSendLength(){
	CString sendData;
	GetDlgItem(IDC_SEND_DATA)->GetWindowText(sendData);
	int finalLength = GetCStringLength(m_encode, m_linuxPacket, sendData);

	CString str;
	str.Format(_T("%d"), finalLength);
	SetDlgItemTextW(IDC_SEND_LENGTH, str);
	UpdateWindow();
}

void CJovToolDlg::OnBnClickedPost()
{
	// TODO:  在此添加控件通知处理程序代码
	CString ip;
	CString outtime;
	CString sendMsg;
	CString contentType;
	CString response;

	GetDlgItem(IDC_IP)->GetWindowText(ip);
	GetDlgItem(IDC_SEND_DATA)->GetWindowText(sendMsg);
	GetDlgItem(IDC_PORT)->GetWindowText(contentType);
	DWORD beginCount = GetTickCount();
	DWORD endCount = 0;
	if (m_linuxPacket == 1)
		sendMsg.Replace(_T("\r\n"), _T("\n"));
	if (m_client->doPost(ip, contentType, m_encode, sendMsg, response) == SUCCESS)
	{
		endCount = GetTickCount();
		CString fromatResponse = _T("");
		/*
		TiXmlDocument xmlDocument;
		char tempRes[LENGTH_1M] = { 0 };
		S2C(tempRes, response);
		xmlDocument.Parse(tempRes, 0, TIXML_ENCODING_UTF8);
		//xmlDocument.SaveFile("response.xml");

		TiXmlPrinter printer;
		xmlDocument.Accept(&printer);
		fromatResponse = printer.CStr();
		fromatResponse.Replace(_T("\n"), _T("\r\n"));
		*/
		fromatResponse = CString(response);
		SetDlgItemTextW(IDC_RECV_DATA, fromatResponse);
	}
	else
	{
		endCount = GetTickCount();
		if (response.IsEmpty())
			SetDlgItemTextW(IDC_RECV_DATA, CString(_T("调用错误")));
		else
			SetDlgItemTextW(IDC_RECV_DATA, response);
	}
		
	CString sDateTime;
	sDateTime.Format(_T(" %d "), (endCount - beginCount));
	m_consume.SetWindowText(sDateTime);
}


void CJovToolDlg::OnBnClickedGet()
{
	// TODO:  在此添加控件通知处理程序代码
	CString ip;
	CString outtime;
	CString sendMsg;
	CString contentType;
	CString response;

	GetDlgItem(IDC_IP)->GetWindowText(ip);
	GetDlgItem(IDC_SEND_DATA)->GetWindowText(sendMsg);
	GetDlgItem(IDC_PORT)->GetWindowText(contentType);
	DWORD beginCount = GetTickCount();
	DWORD endCount = 0;
	if (m_linuxPacket == 1)
		sendMsg.Replace(_T("\r\n"), _T("\n"));
	if (m_client->doGet(ip, contentType, m_encode, sendMsg, response) == SUCCESS)
	{
		endCount = GetTickCount();
		CString fromatResponse = _T("");
		/*
		TiXmlDocument xmlDocument;
		char tempRes[LENGTH_1M] = { 0 };
		S2C(tempRes, response);
		xmlDocument.Parse(tempRes, 0, TIXML_ENCODING_UTF8);
		//xmlDocument.SaveFile("response.xml");

		TiXmlPrinter printer;
		xmlDocument.Accept(&printer);
		fromatResponse = printer.CStr();
		fromatResponse.Replace(_T("\n"), _T("\r\n"));
		*/
		fromatResponse = CString(response);
		SetDlgItemTextW(IDC_RECV_DATA, fromatResponse);
	}
	else
	{
		endCount = GetTickCount();
		if (response.IsEmpty())
			SetDlgItemTextW(IDC_RECV_DATA, CString(_T("调用错误")));
		else
			SetDlgItemTextW(IDC_RECV_DATA, response);
	}
	CString sDateTime;
	sDateTime.Format(_T(" %d "), (GetTickCount() - beginCount));
	m_consume.SetWindowText(sDateTime);
}


void CJovToolDlg::OnCbnSelchangeEncode()
{
	// TODO:  在此添加控件通知处理程序代码
	CComboBox *pEncode = ((CComboBox*)GetDlgItem(IDC_ENCODE));
	m_encode = pEncode->GetCurSel();

	if (m_catchPack){
		m_catchPack->setEncode(m_encode);
	}
	SetSendLength();
}


void CJovToolDlg::OnBnClickedFormat()
{
	// TODO:  在此添加控件通知处理程序代码
	CString response;
	CString fromatResponse;
	int packType = 0;
	GetDlgItem(IDC_RECV_DATA)->GetWindowText(response);
	fromatResponse = response;
	if (response.IsEmpty()){
		AfxMessageBox(_T("没有报文不要乱点！"));
		return;
	}

	if (response.Left(1) == _T("{"))
		packType = 1;

	//
	int pos = response.Find(_T("<?xml")) + response.Find(_T("<?XML"));
	if (1 != packType && pos != -1){
		AfxMessageBox(_T("未知报文格式！"));
		return;
	}

	if (packType == 1){
		char temp[LENGTH_1M] = { 0 };
		char des[LENGTH_1M] = { 0 };
		response.Replace(_T("\t"), _T(""));
		response.Replace(_T("\r\n"), _T(""));
		S2C(temp, response);
		formatJson(temp, des);
		fromatResponse = des;
	}
	else{
		TiXmlDocument xmlDocument;
		char temp[LENGTH_1M] = { 0 };
		S2C(temp, response);
		xmlDocument.Parse(temp, 0, TIXML_ENCODING_UTF8);
		xmlDocument.SaveFile("response.xml");

		TiXmlPrinter printer;
		xmlDocument.Accept(&printer);
		fromatResponse = printer.CStr();
		fromatResponse.Replace(_T("\n"), _T("\r\n"));
	}
	
	SetDlgItemTextW(IDC_RECV_DATA, fromatResponse);
}


void CJovToolDlg::OnBnClickedLinuxcheck()
{
	// TODO:  在此添加控件通知处理程序代码
	CButton* pBtn = (CButton*)GetDlgItem(IDC_LINUXCHECK);
	int state = pBtn->GetCheck();

	if (state == 1)
		m_linuxPacket = 1;
	else
		m_linuxPacket = 0;

	SetSendLength();
}


void CJovToolDlg::OnEnChangeSendData()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	SetSendLength();
}


HBRUSH CJovToolDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	UINT id = pWnd->GetDlgCtrlID();
	if (id == IDC_LINUXCHECK)
	{
		pDC->SetBkMode(TRANSPARENT);

		CRect rc;
		pWnd->GetWindowRect(&rc);
		ScreenToClient(&rc);

		CDC* dc = GetDC();

		pDC->BitBlt(0, 0, rc.Width(), rc.Height(), dc, rc.left, rc.top, SRCCOPY);	//把父窗口背景图片先画到按钮上

		ReleaseDC(dc);

		hbr = (HBRUSH) ::GetStockObject(NULL_BRUSH);
	}

	return hbr;
}

void CJovToolDlg::OnBnClickedTranslate()
{
	// TODO:  在此添加控件通知处理程序代码
	CString ip;
	CString outtime;
	CString sendMsg;
	CString contentType;
	CString response;

	char *baidu = "http://api.fanyi.baidu.com/api/trans/vip/translate?";
	char myurl[1000] = { 0 };
	contentType = _T("application/x-www-form-urlencoded");
	GetDlgItem(IDC_SEND_DATA)->GetWindowText(sendMsg);

	char *appid = "youid";   
	char *secretKey = "youkey";
	char msg[LENGTH_1M] = { 0 };
	char signMsg[LENGTH_1M] = { 0 };
	S2C(signMsg, sendMsg);
	if (m_encode == 1)
		UnicodeToGBK(sendMsg, msg);
	else
		UnicodeToUtf8(sendMsg, msg);

	char *from = "auto";          
	char to[10] = { 0 };
	char salt[60]; //随机数

	if (strlen(msg) == sendMsg.GetLength())
		strcpy(to, "zh");
	else
		strcpy(to, "en");

	srand((unsigned)time(NULL));
	sprintf(salt, "%d", (int)rand());

	char sign[520] = { 0 };
	strcat(sign, appid);
	strcat(sign, msg); //根据百度文档，签名时不做utf编码
	strcat(sign, salt);
	strcat(sign, secretKey);

	char md[100] = { 0 };
	md5(md, sign);

	strcat(myurl, baidu);
	strcat(myurl, "appid=");
	strcat(myurl, appid);
	strcat(myurl, "&q=");
	strcat(myurl, signMsg);
	strcat(myurl, "&from=");
	strcat(myurl, from);
	strcat(myurl, "&to=");
	strcat(myurl, to);
	strcat(myurl, "&salt=");
	strcat(myurl, salt);
	strcat(myurl, "&sign=");
	strcat(myurl, md);
	TRACE(_T("url:%s"), CString(myurl));

	if (m_encode == 1)
		UnicodeToGBK(CString(myurl), msg);
	else
		UnicodeToUtf8(CString(myurl), msg);
	ip = msg;

	DWORD beginCount = GetTickCount();
	DWORD endCount = 0;

	if (m_client->doPost(ip, contentType, m_encode, NULL, response) == SUCCESS)
	{
		endCount = GetTickCount();
		CString fromatResponse = _T("");

		Json::Reader reader;
		Json::Value root;
		Json::Value result;
		char msg[LENGTH_1M] = { 0 };
		if (m_encode == 1)
			UnicodeToGBK(response, msg);
		else
			UnicodeToUtf8(response, msg);

		fromatResponse = CString(response);

		//在解析json时已经自动把unicode编码转换为UTF-8
		if (reader.parse(msg, root)){
			//string trans = root["trans_result"].toStyledString();
			//fromatResponse = trans.c_str(); //未开通词典，这儿精准显示dst即可

			Json::Int size = root["trans_result"].size();
			for (INT i = 0; i < size; i++){
				result = root["trans_result"][i]["dst"];
				fromatResponse = result.asCString();
			}
		}
		
		/* 识别unicode编码
		int nCount = fromatResponse.Replace(_T("\\u"), _T(" 0x"));
		wchar_t* buf = new wchar_t[nCount + 1];
		memset(buf, 0, sizeof(wchar_t)*(nCount + 1));
		int nIndex = 0;
		TCHAR seps[] = _T(" ");
		TCHAR* token = _tcstok(fromatResponse.GetBuffer(fromatResponse.GetLength()), seps);
		while (NULL != token)
		{
			buf[nIndex++] = _tcstoul(token, NULL, 16);
			token = _tcstok(NULL, seps);
		}
		delete[] buf;
		buf = NULL;
		*/
		
		SetDlgItemTextW(IDC_RECV_DATA, fromatResponse);
	}
	else
	{
		endCount = GetTickCount();
		SetDlgItemTextW(IDC_RECV_DATA, CString(_T("调用错误")));
	}

	CString sDateTime;
	sDateTime.Format(_T(" %d "), (endCount - beginCount));
	m_consume.SetWindowText(sDateTime);
}


void CJovToolDlg::OnBnClickedOpen()
{
	// TODO:  在此添加控件通知处理程序代码
	CFileDialog pkgDlg(TRUE);  //设为TRUE将创建打开对话框，否则将创建保存对话框
	CString pathName;
	CString pkg;
	if (pkgDlg.DoModal() == IDOK)
	{
		pathName = pkgDlg.GetPathName();//将文件字符串保存为在CString中

		CFile file;
		file.Open(pathName, CFile::modeRead);
		int length = file.GetLength();
		char pBuf[LENGTH_1M] = { 0 };

		file.Read(pBuf, length);
		file.Close();
		wchar_t recv[LENGTH_1M] = { 0 };

		//FFFE-Unicode
		if (m_encode == 0)
			Utf8ToUnicode(pBuf, recv);
		else
			GBKToUnicode(pBuf, recv);

		SetDlgItemTextW(IDC_SEND_DATA, recv);

		SetSendLength();

		UpdateWindow();//更新窗口
		//SetTimer(1, 300, NULL);   //setTimer（）函数是一个定时器函数，时间一到就会自动调用Ontimer函数
	}
}


void CJovToolDlg::OnBnClickedCodeByte()
{
	// TODO:  在此添加控件通知处理程序代码
	CString sendMsg;
	CString response;

	GetDlgItem(IDC_SEND_DATA)->GetWindowText(sendMsg);

	if (m_linuxPacket == 1)
		sendMsg.Replace(_T("\r\n"), _T("\n"));

	char msg[LENGTH_1M] = { 0 };
	char msg2[LENGTH_1M] = { 0 };
	int signMsg[LENGTH_1M] = { 0 };
	char resp[LENGTH_1M] = { 0 };
	char resp2[LENGTH_1M] = { 0 };
	char temp[10] = { 0 };

	if (m_encode == 1)
		UnicodeToGBK(sendMsg, msg);
	else
		UnicodeToUtf8(sendMsg, msg);

	CString returnstr;
	int no;
	//unicode编码
	for (int i = 0; i < sendMsg.GetLength(); i++){
		no = sendMsg.GetAt(i);
		sprintf(temp, "%04X,", no);
		returnstr += CString(temp);
	}
	returnstr.Delete(returnstr.GetLength() - 1, 1);

	strcat(resp, "字节流:\r\n");
	for (int i = 0; i < strlen(msg); i++){
		signMsg[i] = (int)(msg[i]);

		//字节流
		sprintf(temp, "%d", signMsg[i]);
		strcat(resp, temp);
		strcat(resp, ",");

		//16进制字节流
		sprintf(temp, "%02X", (unsigned char)signMsg[i]);
		strcat(resp2, temp);
		strcat(resp2, ",");
	}
	resp[strlen(resp) - 1] = 0;
	resp2[strlen(resp2) - 1] = 0;

	strcat(resp, "\r\n\r\n字节16进制:\r\n");
	strcat(resp, resp2);

	response = CString(resp);

	response = response + _T("\r\n\r\nUnicode:\r\n") + returnstr;

	SetDlgItemTextW(IDC_RECV_DATA, response);
}


void CJovToolDlg::OnBnClickedDecode()
{
	// TODO:  在此添加控件通知处理程序代码
	CString sendMsg;
	CString response;
	int codeNo = 0; //0-字节流 1-十六进制字节流 2-unicode
	char separate = ','; //默认为逗号分隔
	char msg[LENGTH_1M] = { 0 };

	GetDlgItem(IDC_SEND_DATA)->GetWindowText(sendMsg);
	if (m_linuxPacket == 1)
		sendMsg.Replace(_T("\r\n"), _T("\n"));

	S2C(msg, sendMsg);
	for (int i=0; i < strlen(msg); i++){
		if (msg[i] == ','){
			separate = ',';
			break;
		}
		else if (msg[i] == ' '){
			separate = ' ';
			break;
		}
		else if (msg[i] == '|'){
			separate = '|';
			break;
		}
		else if (msg[i] == '	'){
			separate = '	';
			break;
		}
		else if (msg[i] == '&'){
			separate = '&';
			break;
		}
		else if (msg[i] == '#'){
			separate = '#';
			break;
		}
	}

	int count = -1;
	for (int pos = 0; pos != -1; count++)
		pos = sendMsg.Find(separate, pos + 1);

	//先根据长度大概确认解码的为什么类型
	if (sendMsg.GetLength() == (2 * (count + 1) + count))
		codeNo = 1;
	else if (sendMsg.GetLength() == (4 * (count + 1) + count))
		codeNo = 2;
	else
		codeNo = 0;

	if (sendMsg.GetLength() > (4 * (count + 1) + count)){
		AfxMessageBox(_T("非字节流、十六进制、unicode码，并且非以英文逗号分隔的解不了码！"));
		return;
	}
		

	if (codeNo == 1){
		//该判断不一定准
		if (sendMsg.FindOneOf(_T("a")) > 0 || sendMsg.FindOneOf(_T("b")) > 0 || sendMsg.FindOneOf(_T("c")) > 0 || sendMsg.FindOneOf(_T("d")) > 0 || sendMsg.FindOneOf(_T("e")) > 0 || sendMsg.FindOneOf(_T("f")) > 0
			|| sendMsg.FindOneOf(_T("A")) > 0 || sendMsg.FindOneOf(_T("B")) > 0 || sendMsg.FindOneOf(_T("C")) > 0 || sendMsg.FindOneOf(_T("D")) > 0 || sendMsg.FindOneOf(_T("E")) > 0 || sendMsg.FindOneOf(_T("F")) > 0){
			codeNo = 1;
		}
		else
			codeNo = 0; //防止正好全为是两位的字节流
	}
	
	//判断unicode，正好为4位，字节流只能负号开头
	if (codeNo == 2){
		int pos = -1;
		pos = sendMsg.Find(_T("-"));
		if (pos > -1)
			codeNo == 0;
	}

	
	char msgArray[LENGTH_1M][5] = { 0 };
	char resp[LENGTH_1M] = { 0 };
	wchar_t recv[LENGTH_1M] = { 0 };
	wchar_t tempmsg[8] = { 0 };
	wchar_t unioMsg[8] = { 0 };
	wchar_t *pp = sendMsg.GetBuffer();

	char *p = msg;
	int k = 0;
	int j = -1;
	int i = 0;
	char temp[1] = { 0 };

	
	if (codeNo == 0){
		for (i; i < strlen(msg); i++){
			if (msg[i] == separate){
				strncpy(msgArray[k], p, i - j - 1);
				resp[k] = atoi(msgArray[k]);

				j = i;
				k++;
				p = msg + i + 1;
			}
		}
	}
	else if(codeNo == 1){
		for (i; i < strlen(msg); i++){
			if (msg[i] == separate){
				//若为16进制，需要把16进制转换为byte
				strncpy(msgArray[k], p, i - j - 1);
				resp[k] = HexToByte(msgArray[k]);

				j = i;
				k++;
				p = msg + i + 1;
			}
		}
	}
	else if (codeNo == 2){
		for (i; i < sendMsg.GetLength(); i++){
			if (sendMsg.GetAt(i) == separate){
				int unicodeInt = 0;
				memset(unioMsg, 0, sizeof(unioMsg));
				unioMsg[0] = '0';
				unioMsg[1] = 'x';
				wcsncpy(tempmsg, pp, i - j - 1);
				wcscat(unioMsg, tempmsg);
				StrToIntExW(unioMsg, STIF_SUPPORT_HEX, &unicodeInt);

				response += (wchar_t)unicodeInt;

				j = i;
				k++;
				pp = sendMsg.GetBuffer() + i + 1;
			}
		}
	}

	if (codeNo == 0){
		strncpy(msgArray[k], p, i - j);
		resp[k] = atoi(msgArray[k]);
	}
	else if(codeNo == 1){
		strncpy(msgArray[k], p, i - j);
		resp[k] = HexToByte(msgArray[k]);
	}
	else if (codeNo == 2){
		int unicodeInt = 0;
		memset(unioMsg, 0, sizeof(unioMsg));
		unioMsg[0] = '0';
		unioMsg[1] = 'x';
		wcsncpy(tempmsg, pp, i - j - 1);
		wcscat(unioMsg, tempmsg);
		StrToIntExW(unioMsg, STIF_SUPPORT_HEX, &unicodeInt);

		response += (wchar_t)unicodeInt;
	}

	
	if (codeNo == 0 || codeNo == 1){
		if (m_encode == 1)
			GBKToUnicode(resp, recv);
		else
			Utf8ToUnicode(resp, recv);

		response = CString(recv);
	}
	
	SetDlgItemTextW(IDC_RECV_DATA, response);
}


void CJovToolDlg::OnBnClickedCatchpack()
{
#ifndef _WIN64
	// TODO:  在此添加控件通知处理程序代码
	if (m_catchPack == NULL){
		m_catchPack = new CCatchPack();
		m_catchPack->Create(IDD_CATCHPACK);
		m_catchPack->setEncode(m_encode);
	}
	
	m_catchPack->ShowWindow(SW_SHOWNORMAL);
#endif
}
