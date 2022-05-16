
#include "stdafx.h"
#include "GoogleLog.h"
#include <afxmt.h>
#include <direct.h>  

bool G_IsInit = false;

void InitLog()
{
	/*if(!G_IsInit)
	{
        FLAGS_log_dir = "./";
        FLAGS_max_log_size = 10;

		char cPath[1024];
		getcwd(cPath, MAX_PATH);  

		int iLevel = GetPrivateProfileInt(_T("loglevel"),_T("ufx2ctp"), 0, CString(cPath));
        FLAGS_minloglevel = iLevel;

        google::GetLoggingDirectories();
        google::InitGoogleLogging(FILE_LOG_NAME);

        G_IsInit = true;
    }*/
}

void UnInitLog()
{
	//google::ShutdownGoogleLogging();
}