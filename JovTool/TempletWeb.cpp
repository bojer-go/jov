#include "stdafx.h"
#include "TempletWeb.h"

#include "webHandler.h"
#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/wrapper/cef_helpers.h"


TempletWeb::TempletWeb()
{
}


TempletWeb::~TempletWeb()
{
}
void TempletWeb::OnBeforeCommandLineProcessing(const CefString& process_type,
	CefRefPtr<CefCommandLine> command_line)
{
	command_line->AppendSwitch("--single-process");
}
void TempletWeb::OnContextInitialized() {

}