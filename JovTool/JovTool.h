
// JovTool.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "webHandler.h"

// CJovToolApp:
// See JovTool.cpp for the implementation of this class
//
#define TIMER_CONSUME		0x01  //���׺�ʱ��ʱ��

class CJovToolApp : public CWinApp
{
public:
	CJovToolApp();
	~CJovToolApp();

// Overrides
public:
	virtual BOOL InitInstance();
	webHandler *handler;
// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CJovToolApp theApp;