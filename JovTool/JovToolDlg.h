
// JovToolDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "HttpClient.h"
#include "CatchPack.h"

// CJovToolDlg dialog
class CJovToolDlg : public CDialogEx
{
// Construction
public:
	CJovToolDlg(CWnd* pParent = NULL);	// standard constructor
	~CJovToolDlg();

// Dialog Data
	enum { IDD = IDD_JOVTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
	HttpClient *m_client;
	int m_encode;
	int m_linuxPacket;

	CCatchPack *m_catchPack;
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void AdjustControl(); //调整控件位置
	int GetCStringLength(int encode, int uFlag, CString str);
	void SetSendLength();
public:
	afx_msg void OnBnClickedSend();
	afx_msg void OnBnClickedStartServer();
	afx_msg void OnBnClickedClear();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEnChangeRecvData();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CEdit m_consume;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	CEdit m_sendData;
	CEdit m_recvData;
	afx_msg void OnBnClickedPost();
	afx_msg void OnBnClickedGet();
	afx_msg void OnCbnSelchangeEncode();
	afx_msg void OnBnClickedFormat();
	afx_msg void OnBnClickedLinuxcheck();
	afx_msg void OnEnChangeSendData();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedTranslate();
	afx_msg void OnBnClickedOpen();
	afx_msg void OnBnClickedCodeByte();
	afx_msg void OnBnClickedDecode();
	afx_msg void OnBnClickedCatchpack();
};
