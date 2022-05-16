#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include"pcap/pcap.h"
#include "Protocol.h"
#include "utilities.h"

// CCatchPack 对话框

class CCatchPack : public CDialogEx
{
	DECLARE_DYNAMIC(CCatchPack)

public:
	CCatchPack(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCatchPack();

	void setEncode(int encode);
	int initCatchPack();
	int startCatchPack();
	int updateTree(int index);
	int updateEdit(int index);
	int saveFile();
	int readFile(CString path);

	//////////////////////////////////////////////［my data］/////////////////////////////////////////////
	int devCount;
	struct pktcount npacket;				//各类数据包计数
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_if_t *alldev;
	pcap_if_t *dev;
	pcap_t *adhandle;
	pcap_dumper_t *dumpfile;
	char filepath[512];							//	文件保存路径
	char filename[64];							//	文件名称	
	int m_countPack;				//包计数
	int m_encode;					//编码
	char m_contentStr[512];			//过滤字符串
	CPtrList m_localDataList;				//保存被本地化后的数据包
	CPtrList m_netDataList;					//保存从网络中直接获取的数据包
	HANDLE m_ThreadHandle;			//线程

	CPtrList m_pktList;							//捕获包所存放的链表

// 对话框数据
	enum { IDD = IDD_CATCHPACK };

protected:
	HICON m_hIcon;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_network;
	CComboBox m_protocol;
	CEdit m_content;
	CListCtrl m_listPack;
	CTreeCtrl m_treePack;
	CEdit m_packContent;
	CButton m_startCatch;
	CButton m_stopCatch;
	CButton m_savePack;
	CButton m_openPack;

	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedStop();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedOpen();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg HCURSOR OnQueryDragIcon();
};
