// surf.cpp : 实现文件
//

#include "stdafx.h"
#include "JovTool.h"
#include "surf.h"
#include "afxdialogex.h"
#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/wrapper/cef_helpers.h"
#include "include/cef_app.h"
#include "webHandler.h"
#include "TempletWeb.h"

#ifdef _WIN64
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"

namespace {

	// When using the Views framework this object provides the delegate
	// implementation for the CefWindow that hosts the Views-based browser.
	class SimpleWindowDelegate : public CefWindowDelegate {
	public:
		explicit SimpleWindowDelegate(CefRefPtr<CefBrowserView> browser_view)
			: browser_view_(browser_view) {}

		void OnWindowCreated(CefRefPtr<CefWindow> window) OVERRIDE{
			// Add the browser view and show the window.
			window->AddChildView(browser_view_);
			window->Show();

			// Give keyboard focus to the browser view.
			browser_view_->RequestFocus();
		}

			void OnWindowDestroyed(CefRefPtr<CefWindow> window) OVERRIDE{
			browser_view_ = nullptr;
		}

			bool CanClose(CefRefPtr<CefWindow> window) OVERRIDE{
			// Allow the window to close if the browser says it's OK.
			CefRefPtr<CefBrowser> browser = browser_view_->GetBrowser();
			if (browser)
				return browser->GetHost()->TryCloseBrowser();
			return true;
		}

			CefSize GetPreferredSize(CefRefPtr<CefView> view) OVERRIDE{
			return CefSize(800, 600);
		}

	private:
		CefRefPtr<CefBrowserView> browser_view_;

		IMPLEMENT_REFCOUNTING(SimpleWindowDelegate);
		DISALLOW_COPY_AND_ASSIGN(SimpleWindowDelegate);
	};

	class SimpleBrowserViewDelegate : public CefBrowserViewDelegate {
	public:
		SimpleBrowserViewDelegate() {}

		bool OnPopupBrowserViewCreated(CefRefPtr<CefBrowserView> browser_view,
			CefRefPtr<CefBrowserView> popup_browser_view,
			bool is_devtools) OVERRIDE{
			// Create a new top-level Window for the popup. It will show itself after
			// creation.
			CefWindow::CreateTopLevelWindow(
			new SimpleWindowDelegate(popup_browser_view));

			// We created the Window.
			return true;
		}

	private:
		IMPLEMENT_REFCOUNTING(SimpleBrowserViewDelegate);
		DISALLOW_COPY_AND_ASSIGN(SimpleBrowserViewDelegate);
	};

}  // namespace

#endif

// surf 对话框
CefRefPtr<webHandler> handler = nullptr;

IMPLEMENT_DYNAMIC(surf, CDialogEx)

surf::surf(CWnd* pParent /*=NULL*/)
	: CDialogEx(surf::IDD, pParent)
{
	pWebBrowser = nullptr;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

surf::~surf()
{
	if (pWebBrowser)
		pWebBrowser = nullptr;	
}

void surf::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WEB_URL, m_Url);
}


BEGIN_MESSAGE_MAP(surf, CDialogEx)
//	ON_BN_CLICKED(IDC_SEARCH, &surf::OnBnClickedSearch)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_KEYDOWN()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// surf 消息处理程序


//void surf::OnBnClickedSearch()
//{
//	// TODO:  在此添加控件通知处理程序代码
//	CefRefPtr<CefBrowser> browser = handler->GetBrowser();
//	if (browser){
//		CString url;
//		GetDlgItemText(IDC_WEB_URL, url);
//		if (!url.IsEmpty())
//			browser->GetMainFrame()->LoadURL(CefString(url));
//	}
//}


BOOL surf::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	this->ShowWindow(SW_SHOWMAXIMIZED);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

int surf::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	CefWindowInfo window_info;
	CRect rt;
	GetWindowRect(&rt);
	HWND h = this->GetSafeHwnd();
	window_info.SetAsChild(h, rt);
	//handler = new webHandler();
	handler = webHandler::GetInstance();
	CefBrowserSettings browser_settings;
	std::string url;
	url = "http://www.baidu.com";
#ifdef _WIN64
	CefRefPtr<CefCommandLine> command_line =
		CefCommandLine::GetGlobalCommandLine();

	const bool enable_chrome_runtime =
		command_line->HasSwitch("enable-chrome-runtime");
	if (handler->GetUseViews() && !enable_chrome_runtime) {
		// Create the BrowserView.
		CefRefPtr<CefBrowserView> browser_view = CefBrowserView::CreateBrowserView(
			handler, url, browser_settings, nullptr, nullptr,
			new SimpleBrowserViewDelegate());

		// Create the Window. It will show itself after creation.
		CefWindow::CreateTopLevelWindow(new SimpleWindowDelegate(browser_view));
	}
	else {
		// Create the first browser window.
		CefBrowserHost::CreateBrowser(window_info, handler.get(), url, browser_settings, nullptr, nullptr);
	}
#else
	CefBrowserHost::CreateBrowser(window_info, handler.get(), url, browser_settings, NULL);
#endif

	return 0;
}


void surf::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO:  在此处添加消息处理程序代码
	CRect rect;
	GetClientRect(&rect);

	//控件右移
	CRect temp;
	if (m_Url.GetSafeHwnd()){
		m_Url.GetWindowRect(&temp);
		ScreenToClient(temp);
		temp.right = rect.right-10;
		m_Url.MoveWindow(temp);
	}

	handler->initX = cx;
	handler->initY = cy;
	if (handler != NULL){
		CefRefPtr<CefBrowser> browser = handler->GetBrowser();
		if (browser){
			CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
			::MoveWindow(hwnd, 0, 0 + 50, cx, cy - 50, TRUE);
		}
	}
}


BOOL surf::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		if (GetFocus()->GetDlgCtrlID() == IDC_WEB_URL)
		{
			UpdateData();
			CefRefPtr<CefBrowser> browser = handler->GetBrowser();
			if (browser){
				CString url;
				GetDlgItemText(IDC_WEB_URL, url);
				if (!url.IsEmpty())
					browser->GetMainFrame()->LoadURL(CefString(url));
			}
		}
		return TRUE;
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}


void surf::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}


void surf::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}


HCURSOR surf::OnQueryDragIcon()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	return static_cast<HCURSOR>(m_hIcon);
}
