#include "stdafx.h"
#include "webHandler.h"

#include "include/base/cef_bind.h"
#include "include/cef_app.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"


#ifdef _WIN64
#include "include/cef_parser.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"

namespace {

	webHandler* g_instance = nullptr;

	// Returns a data: URI with the specified contents.
	std::string GetDataURI(const std::string& data, const std::string& mime_type) {
		return "data:" + mime_type + ";base64," +
			CefURIEncode(CefBase64Encode(data.data(), data.size()), false)
			.ToString();
	}

}  // namespace

webHandler::webHandler(bool use_views)
	: use_views_(use_views), is_closing_(false) {
	DCHECK(!g_instance);
	g_instance = this;
	initX = 200;
	initY = 100;
}

webHandler::~webHandler() {
	g_instance = nullptr;
}

// static
webHandler* webHandler::GetInstance() {
	return g_instance;
}

void webHandler::OnTitleChange(CefRefPtr<CefBrowser> browser,
	const CefString& title) {
	CEF_REQUIRE_UI_THREAD();

	if (use_views_) {
		// Set the title of the window using the Views framework.
		CefRefPtr<CefBrowserView> browser_view =
			CefBrowserView::GetForBrowser(browser);
		if (browser_view) {
			CefRefPtr<CefWindow> window = browser_view->GetWindow();
			if (window)
				window->SetTitle(title);
		}
	} else {
		// Set the title of the window using platform APIs.
		PlatformTitleChange(browser, title);
	}
}

void webHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();

	// Add to the list of existing browsers.
	browser_list_.push_back(browser);
	if (!GetBrowser())   {
		base::AutoLock lock_scope(lock_);
		// We need to keep the main child window, but not popup windows
		browser_ = browser;
		CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
		::MoveWindow(hwnd, 0, 0 + 50, initX, initY - 50, TRUE);
	}
}

bool webHandler::DoClose(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();

	// Closing the main window requires special handling. See the DoClose()
	// documentation in the CEF header for a detailed destription of this
	// process.
	if (browser_list_.size() == 1) {
		// Set a flag to indicate that the window close should be allowed.
		is_closing_ = true;
	}

	// Allow the close. For windowed browsers this will result in the OS close
	// event being sent.
	return false;
}

void webHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();

	// Remove from the list of existing browsers.
	BrowserList::iterator bit = browser_list_.begin();
	for (; bit != browser_list_.end(); ++bit) {
		if ((*bit)->IsSame(browser)) {
			browser_list_.erase(bit);
			break;
		}
	}

	if (browser_list_.empty()) {
		// All browser windows have closed. Quit the application message loop.
		CefQuitMessageLoop();
	}
	base::AutoLock lock_scope(lock_);   //这一句包括下一句一定要有，否则就会在退出程序时报错，进程无法释放
	// Free the browser pointer so that the browser can be destroyed
	browser_ = NULL;
}

void webHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	ErrorCode errorCode,
	const CefString& errorText,
	const CefString& failedUrl) {
	CEF_REQUIRE_UI_THREAD();

	// Don't display an error for downloaded files.
	if (errorCode == ERR_ABORTED)
		return;

	// Display a load error message using a data: URI.
	std::stringstream ss;
	ss << "<html><body bgcolor=\"white\">"
		"<h2>Failed to load URL "
		<< std::string(failedUrl) << " with error " << std::string(errorText)
		<< " (" << errorCode << ").</h2></body></html>";

	frame->LoadURL(GetDataURI(ss.str(), "text/html"));
}

void webHandler::CloseAllBrowsers(bool force_close) {
	if (!CefCurrentlyOn(TID_UI)) {
		// Execute on the UI thread.
		CefPostTask(TID_UI, base::Bind(&webHandler::CloseAllBrowsers, this,
			force_close));
		return;
	}

	if (browser_list_.empty())
		return;

	BrowserList::const_iterator it = browser_list_.begin();
	for (; it != browser_list_.end(); ++it)
		(*it)->GetHost()->CloseBrowser(force_close);
}

CefRefPtr<CefBrowser> webHandler::GetBrowser() const
{
	base::AutoLock lock_scope(lock_);
	return browser_;
}

void webHandler::PlatformTitleChange(CefRefPtr<CefBrowser> browser,
	const CefString& title) {
	CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
	if (hwnd)
		SetWindowText(hwnd, std::wstring(title).c_str());
}

#else
namespace {
	webHandler* g_instance = NULL;

	// Returns a data: URI with the specified contents.
	/*std::string GetDataURI(const std::string& data, const std::string& mime_type) {
	return "data:" + mime_type + ";base64," +
	CefURIEncode(CefBase64Encode(data.data(), data.size()), false)
	.ToString();
	}*/
}

webHandler::webHandler()
	: is_closing_(false) {
	DCHECK(!g_instance);
	g_instance = this;
	initX = 200;
	initY = 100;
}


webHandler::~webHandler()
{
	g_instance = NULL;
}

// static
webHandler* webHandler::GetInstance() {
	return g_instance;
}

//void webHandler::PlatformTitleChange(CefRefPtr<CefBrowser> browser,
//	const CefString& title) {
//	CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
//	if (hwnd)
//		SetWindowText(hwnd, std::wstring(title).c_str());
//}

void webHandler::OnTitleChange(CefRefPtr<CefBrowser> browser,
	const CefString& title) {
	CEF_REQUIRE_UI_THREAD();

	//if (use_views_) {
	//	// Set the title of the window using the Views framework.
	//	CefRefPtr<CefBrowserView> browser_view =
	//		CefBrowserView::GetForBrowser(browser);
	//	if (browser_view) {
	//		CefRefPtr<CefWindow> window = browser_view->GetWindow();
	//		if (window)
	//			window->SetTitle(title);
	//	}
	//}
	//else {
	//	// Set the title of the window using platform APIs.
	//	PlatformTitleChange(browser, title);
	//}
}

void webHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();

	// Add to the list of existing browsers.
	browser_list_.push_back(browser);
	if (!GetBrowser())   {
		base::AutoLock lock_scope(lock_);
		// We need to keep the main child window, but not popup windows
		browser_ = browser;
		CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
		::MoveWindow(hwnd, 0, 0 + 50, initX, initY - 50, TRUE);
	}
}

bool webHandler::DoClose(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();

	// Closing the main window requires special handling. See the DoClose()
	// documentation in the CEF header for a detailed destription of this
	// process.
	if (browser_list_.size() == 1) {
		// Set a flag to indicate that the window close should be allowed.
		is_closing_ = true;
	}

	// Allow the close. For windowed browsers this will result in the OS close
	// event being sent.
	return false;
}

void webHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();
	// Remove from the list of existing browsers.
	BrowserList::iterator bit = browser_list_.begin();
	for (; bit != browser_list_.end(); ++bit) {
		if ((*bit)->IsSame(browser)) {
			browser_list_.erase(bit);
			break;
		}
	}

	if (browser_list_.empty()) {
		// All browser windows have closed. Quit the application message loop.
		CefQuitMessageLoop();
	}
	base::AutoLock lock_scope(lock_);   //这一句包括下一句一定要有，否则就会在退出程序时报错，进程无法释放
	// Free the browser pointer so that the browser can be destroyed
	browser_ = NULL;
}

void webHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	ErrorCode errorCode,
	const CefString& errorText,
	const CefString& failedUrl) {
	CEF_REQUIRE_UI_THREAD();

	// Don't display an error for downloaded files.
	if (errorCode == ERR_ABORTED)
		return;

	// Display a load error message.
	std::stringstream ss;
	ss << "<html><body bgcolor=\"white\">"
		"<h2>Failed to load URL " << std::string(failedUrl) <<
		" with error " << std::string(errorText) << " (" << errorCode <<
		").</h2></body></html>";
	frame->LoadString(ss.str(), failedUrl);
}

void webHandler::CloseAllBrowsers(bool force_close) {
	if (!CefCurrentlyOn(TID_UI)) {
		// Execute on the UI thread.
		CefPostTask(TID_UI,
			base::Bind(&webHandler::CloseAllBrowsers, this, force_close));
		return;
	}

	if (browser_list_.empty())
		return;

	BrowserList::const_iterator it = browser_list_.begin();
	for (; it != browser_list_.end(); ++it)
		(*it)->GetHost()->CloseBrowser(force_close);
}

CefRefPtr<CefBrowser> webHandler::GetBrowser() const
{
	base::AutoLock lock_scope(lock_);
	return browser_;
}
#endif
