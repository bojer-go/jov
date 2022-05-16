
// JovTool.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "JovTool.h"
#include "JovToolDlg.h"
#include "include/cef_sandbox_win.h"
#include "include/wrapper/cef_helpers.h"
#include "include/cef_app.h"
#include "TempletWeb.h"
#include "surf.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CJovToolApp

BEGIN_MESSAGE_MAP(CJovToolApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CJovToolApp construction

CJovToolApp::CJovToolApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	void* sandbox_info = NULL;
	CefMainArgs main_args(AfxGetApp()->m_hInstance);
	CefRefPtr<TempletWeb> app(new TempletWeb);
	CefSettings settings;
	settings.no_sandbox = true;
	settings.multi_threaded_message_loop = true;
#ifdef _WIN64
	handler = new webHandler(false);
#else
	handler = new webHandler();
#endif

	CefInitialize(main_args, settings, app.get(), sandbox_info);
}
CJovToolApp::~CJovToolApp()
{
#ifdef _WIN64
	//CefShutdown();
	delete handler;
#else
	CefShutdown();
	delete handler;
#endif
}

// The one and only CJovToolApp object

CJovToolApp theApp;


// CJovToolApp initialization

BOOL CJovToolApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CJovToolDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Delete the shell manager created above.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

