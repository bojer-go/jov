#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include"pcap/pcap.h"
#include "Protocol.h"
#include "utilities.h"

// CCatchPack �Ի���

class CCatchPack : public CDialogEx
{
	DECLARE_DYNAMIC(CCatchPack)

public:
	CCatchPack(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCatchPack();

	void setEncode(int encode);
	int initCatchPack();
	int startCatchPack();
	int updateTree(int index);
	int updateEdit(int index);
	int saveFile();
	int readFile(CString path);

	//////////////////////////////////////////////��my data��/////////////////////////////////////////////
	int devCount;
	struct pktcount npacket;				//�������ݰ�����
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_if_t *alldev;
	pcap_if_t *dev;
	pcap_t *adhandle;
	pcap_dumper_t *dumpfile;
	char filepath[512];							//	�ļ�����·��
	char filename[64];							//	�ļ�����	
	int m_countPack;				//������
	int m_encode;					//����
	char m_contentStr[512];			//�����ַ���
	CPtrList m_localDataList;				//���汻���ػ�������ݰ�
	CPtrList m_netDataList;					//�����������ֱ�ӻ�ȡ�����ݰ�
	HANDLE m_ThreadHandle;			//�߳�

	CPtrList m_pktList;							//���������ŵ�����

// �Ի�������
	enum { IDD = IDD_CATCHPACK };

protected:
	HICON m_hIcon;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
