#pragma once

#include "resource.h"
#include "afxwin.h"

class WebBrowser;
// surf 对话框

class surf : public CDialogEx
{
	DECLARE_DYNAMIC(surf)

public:
	surf(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~surf();

// 对话框数据
	enum { IDD = IDD_WEB };

protected:
	HICON m_hIcon;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnBnClickedSearch();
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	WebBrowser *pWebBrowser;
	afx_msg void OnPaint();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg HCURSOR OnQueryDragIcon();
	CEdit m_Url;
};
