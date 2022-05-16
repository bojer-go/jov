// CatchPack.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "JovTool.h"
#include "CatchPack.h"
#include "afxdialogex.h"

#ifndef _WIN64
DWORD WINAPI catchPackThread(LPVOID lpParameter);
#pragma comment(lib,"Iphlpapi.lib")
// CCatchPack �Ի���

IMPLEMENT_DYNAMIC(CCatchPack, CDialogEx)

CCatchPack::CCatchPack(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCatchPack::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CCatchPack::~CCatchPack()
{
	if (NULL != m_ThreadHandle)
		TerminateThread(this->m_ThreadHandle, -1);

	this->m_ThreadHandle = NULL;
	m_localDataList.RemoveAll();
	m_netDataList.RemoveAll();
}

void CCatchPack::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NETWORK, m_network);
	DDX_Control(pDX, IDC_PROTOCOL, m_protocol);
	DDX_Control(pDX, IDC_CONTENT, m_content);
	DDX_Control(pDX, IDC_LIST1, m_listPack);
	DDX_Control(pDX, IDC_TREE1, m_treePack);
	DDX_Control(pDX, IDC_PACKCONTENT, m_packContent);
	DDX_Control(pDX, IDC_STARTCATCH, m_startCatch);
	DDX_Control(pDX, IDC_STOPCATCH, m_stopCatch);
	DDX_Control(pDX, IDC_SAVEPACK, m_savePack);
	DDX_Control(pDX, IDC_OPENPACK, m_openPack);
}


BEGIN_MESSAGE_MAP(CCatchPack, CDialogEx)
	ON_BN_CLICKED(IDC_START, &CCatchPack::OnBnClickedStart)
	ON_BN_CLICKED(IDC_STOP, &CCatchPack::OnBnClickedStop)
	ON_BN_CLICKED(IDC_SAVE, &CCatchPack::OnBnClickedSave)
	ON_BN_CLICKED(IDC_OPEN, &CCatchPack::OnBnClickedOpen)
	ON_WM_SIZE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CCatchPack::OnLvnItemchangedList1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST1, &CCatchPack::OnNMCustomdrawList1)
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CCatchPack ��Ϣ�������


void CCatchPack::OnBnClickedStart()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (this->m_localDataList.IsEmpty() == FALSE)
	{
		if (MessageBox(_T("ȷ�ϲ��������ݣ�"), _T("����"), MB_YESNO) == IDNO)
		{
			this->saveFile();
		}
	}

	this->m_countPack = 1;													//���¼���
	this->m_localDataList.RemoveAll();				//ÿ��һ��ʼ�ͽ���ǰ���������յ�
	this->m_netDataList.RemoveAll();
	memset(&(this->npacket), 0, sizeof(struct pktcount));

	if (this->startCatchPack()<0)
		return;
	this->m_listPack.DeleteAllItems();
	this->m_treePack.DeleteAllItems();
	this->m_packContent.SetWindowTextW(_T(""));
	this->m_startCatch.EnableWindow(FALSE);
	this->m_stopCatch.EnableWindow(TRUE);
	this->m_savePack.EnableWindow(FALSE);

	CString temp;
	m_content.GetWindowTextW(temp);
	UnicodeToUtf8(temp, this->m_contentStr);
}


void CCatchPack::OnBnClickedStop()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (NULL == this->m_ThreadHandle)
		return;
	if (TerminateThread(this->m_ThreadHandle, -1) == 0)
	{
		MessageBox(_T("�ر��̴߳������Ժ�����"));
		return;
	}
	this->m_ThreadHandle = NULL;
	this->m_startCatch.EnableWindow(TRUE);
	this->m_stopCatch.EnableWindow(FALSE);
	this->m_savePack.EnableWindow(TRUE);
}


void CCatchPack::OnBnClickedSave()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (this->saveFile()<0)
		return;
}


void CCatchPack::OnBnClickedOpen()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	this->m_listPack.DeleteAllItems();
	this->m_countPack = 1;													//�б����¼���
	this->m_localDataList.RemoveAll();				//ÿ��һ��ʼ�ͽ���ǰ���������յ�
	this->m_netDataList.RemoveAll();
	memset(&(this->npacket), 0, sizeof(struct pktcount));//������������

	//���ļ��Ի���
	CFileDialog   FileDlg(TRUE, _T(".lix"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT);
	FileDlg.m_ofn.lpstrInitialDir = _T("c:\\");
	if (FileDlg.DoModal() == IDOK)
	{
		int ret = this->readFile(FileDlg.GetPathName());
		if (ret < 0)
			return;
	}
}


void CCatchPack::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO:  �ڴ˴������Ϣ����������
	CRect rect;
	GetClientRect(&rect);

	//�ؼ�����
	CRect temp;
	if (m_listPack.GetSafeHwnd()){
		m_listPack.GetWindowRect(&temp);
		ScreenToClient(temp);
		temp.right = rect.right - 12;
		m_listPack.MoveWindow(temp);
	}
	if (m_packContent.GetSafeHwnd()){
		m_packContent.GetWindowRect(&temp);
		ScreenToClient(temp);
		temp.right = rect.right - 12;
		temp.bottom = rect.bottom - 12;
		m_packContent.MoveWindow(temp);
	}
	if (m_treePack.GetSafeHwnd()){
		m_treePack.GetWindowRect(&temp);
		ScreenToClient(temp);
		temp.bottom = rect.bottom - 12;
		m_treePack.MoveWindow(temp);
	}

}


BOOL CCatchPack::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_listPack.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES | LVS_EX_DOUBLEBUFFER);

	m_listPack.InsertColumn(0, _T("���"), 3, 50);                        //1��ʾ�ң�2��ʾ�У�3��ʾ��
	m_listPack.InsertColumn(1, _T("ʱ��"), 3, 130);
	m_listPack.InsertColumn(2, _T("����"), 3, 72);
	m_listPack.InsertColumn(3, _T("ԴMAC��ַ"), 3, 140);
	m_listPack.InsertColumn(4, _T("Ŀ��MAC��ַ"), 3, 140);
	m_listPack.InsertColumn(5, _T("Э��"), 3, 70);
	m_listPack.InsertColumn(6, _T("ԴIP��ַ"), 3, 145);
	m_listPack.InsertColumn(7, _T("Ŀ��IP��ַ"), 3, 145);

	m_network.AddString(_T("��ѡ������"));
	m_protocol.AddString(_T("��ѡ��Э��"));

	if (initCatchPack()<0)
		return FALSE;

	/*��ʼ���ӿ��б�*/
	for (dev = alldev; dev; dev = dev->next)
	{
		if (dev->description)
			m_network.AddString(CString(dev->description));  //////////////////////////////Problem 1�ַ�������
	}

	/*��ʼ�����˹����б�*/
	m_protocol.AddString(_T("tcp"));
	m_protocol.AddString(_T("udp"));
	m_protocol.AddString(_T("ip"));
	m_protocol.AddString(_T("icmp"));
	m_protocol.AddString(_T("arp"));

	m_network.SetCurSel(0);
	m_protocol.SetCurSel(0);

	m_stopCatch.EnableWindow(FALSE);
	m_savePack.EnableWindow(FALSE);

	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

void CCatchPack::setEncode(int encode)
{
	m_encode = encode;
}

int CCatchPack::initCatchPack()
{
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
	//�õ��ṹ���С,����GetAdaptersInfo����
	unsigned long stSize = sizeof(IP_ADAPTER_INFO);
	//����GetAdaptersInfo����,���pIpAdapterInfoָ�����;����stSize��������һ��������Ҳ��һ�������
	int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);

	devCount = 0;
	if (pcap_findalldevs(&alldev, errbuf) == -1)
		//if (pcap_findalldevs_ex("rpcap://",NULL, &alldev, errbuf) == -1)
		return -1;
	for (dev = alldev; dev; dev = dev->next)
		devCount++;
	return 0;
}

//��ʼ����
int CCatchPack::startCatchPack()
{
	int if_index, filter_index, count;
	u_int netmask;
	struct bpf_program fcode;

	initCatchPack();

	//��ýӿں͹���������
	if_index = this->m_network.GetCurSel();
	filter_index = this->m_protocol.GetCurSel();

	if (0 == if_index || CB_ERR == if_index)
	{
		MessageBox(_T("��ѡ������"));
		return -1;
	}
	if (CB_ERR == filter_index)
	{
		MessageBox(_T("������ѡ�����"));
		return -1;
	}

	/*���ѡ�е������ӿ�*/
	dev = alldev;
	for (count = 0; count<if_index - 1; count++)
		dev = dev->next;

	if ((adhandle = pcap_open_live(dev->name,	// �豸��
		65536,											//�������ݰ�����																					
		1,													// ����ģʽ (��0��ζ���ǻ���ģʽ)
		1000,												// ����ʱ����
		errbuf											// ������Ϣ
		)) == NULL)
	{
		MessageBox(_T("�޷��򿪽ӿڣ�" + CString(dev->description)));
		pcap_freealldevs(alldev);
		return -1;
	}

	/*����Ƿ�Ϊ��̫��*/
	if (pcap_datalink(adhandle) != DLT_EN10MB)
	{
		MessageBox(_T("�ⲻ�ʺ��ڷ���̫��������!"));
		pcap_freealldevs(alldev);
		return -1;
	}

	if (dev->addresses != NULL)
		netmask = ((struct sockaddr_in *)(dev->addresses->netmask))->sin_addr.S_un.S_addr;
	else
		netmask = 0xffffff;

	//���������
	if (0 == filter_index)
	{
		char filter[] = "";
		if (pcap_compile(adhandle, &fcode, filter, 1, netmask) <0)
		{
			MessageBox(_T("�﷨�����޷����������"));
			pcap_freealldevs(alldev);
			return -1;
		}
	}
	else{
		CString str;
		char *filter;
		int len, x;
		this->m_protocol.GetLBText(filter_index, str);
		len = str.GetLength() + 1;
		filter = (char*)malloc(len);
		for (x = 0; x<len; x++)
		{
			filter[x] = str.GetAt(x);
		}
		if (pcap_compile(adhandle, &fcode, filter, 1, netmask) <0)
		{
			MessageBox(_T("�﷨�����޷����������"));
			pcap_freealldevs(alldev);
			return -1;
		}
	}


	//���ù�����
	if (pcap_setfilter(adhandle, &fcode)<0)
	{
		MessageBox(_T("���ù���������"));
		pcap_freealldevs(alldev);
		return -1;
	}

	/* �������ݰ��洢·��*/
	CFileFind file;
	char thistime[30];
	struct tm *ltime;
	memset(filepath, 0, 512);
	memset(filename, 0, 64);

	if (!file.FindFile(_T("SavedData")))
	{
		CreateDirectory(_T("SavedData"), NULL);
	}

	time_t nowtime;
	time(&nowtime);
	ltime = localtime(&nowtime);
	strftime(thistime, sizeof(thistime), "%Y%m%d %H%M%S", ltime);
	strcpy(filepath, "SavedData\\");
	strcat(filename, thistime);
	strcat(filename, ".lix");

	strcat(filepath, filename);
	dumpfile = pcap_dump_open(adhandle, filepath);
	if (dumpfile == NULL)
	{
		MessageBox(_T("�ļ���������"));
		return -1;
	}

	pcap_freealldevs(alldev);

	/*�������ݣ��½��̴߳���*/
	LPDWORD threadCap = NULL;
	m_ThreadHandle = CreateThread(NULL, 0, catchPackThread, this, 0, threadCap);
	if (m_ThreadHandle == NULL)
	{
		int code = GetLastError();
		CString str;
		str.Format(_T("�����̴߳��󣬴���Ϊ%d."), code);
		MessageBox(str);
		return -1;
	}
	return 1;
}

DWORD WINAPI catchPackThread(LPVOID lpParameter)
{
	int res, nItem;
	struct tm *ltime;
	CString timestr, buf, srcMac, destMac;
	time_t local_tv_sec;
	struct pcap_pkthdr *header;									  //���ݰ�ͷ
	const u_char *pkt_data = NULL, *pData = NULL;     //�������յ����ֽ�������
	u_char *ppkt_data;

	CCatchPack *pthis = (CCatchPack*)lpParameter;
	if (NULL == pthis->m_ThreadHandle)
	{
		MessageBox(NULL, _T("�߳̾������"), _T("��ʾ"), MB_OK);
		return -1;
	}

	while ((res = pcap_next_ex(pthis->adhandle, &header, &pkt_data)) >= 0)
	{
		if (res == 0)				//��ʱ
			continue;

		struct datapkt *data = (struct datapkt*)malloc(sizeof(struct datapkt));
		memset(data, 0, sizeof(struct datapkt));

		if (NULL == data)
		{
			MessageBox(NULL, _T("�ռ��������޷������µ����ݰ�"), _T("Error"), MB_OK);
			return -1;
		}

		//������������������ݰ����ڴ���Χ��
		if (analyze_frame(pkt_data, data, &(pthis->npacket))<0)
			continue;

		//�������ݹ���
		if (analyze_content(pkt_data, data, pthis->m_contentStr)< 0)
			continue;

		//�����ݰ����浽�򿪵��ļ���
		if (pthis->dumpfile != NULL)
		{
			pcap_dump((unsigned char*)pthis->dumpfile, header, pkt_data);
		}


		//�����ػ��������װ��һ�������У��Ա����ʹ��		
		ppkt_data = (u_char*)malloc(header->len);
		memcpy(ppkt_data, pkt_data, header->len);

		pthis->m_localDataList.AddTail(data);
		pthis->m_netDataList.AddTail(ppkt_data);

		/*Ԥ�������ʱ�䡢����*/
		data->len = header->len;								//��·���յ������ݳ���
		local_tv_sec = header->ts.tv_sec;
		ltime = localtime(&local_tv_sec);
		data->time[0] = ltime->tm_year + 1900;
		data->time[1] = ltime->tm_mon + 1;
		data->time[2] = ltime->tm_mday;
		data->time[3] = ltime->tm_hour;
		data->time[4] = ltime->tm_min;
		data->time[5] = ltime->tm_sec;

		/*Ϊ�½��յ������ݰ���listControl���½�һ��item*/
		buf.Format(_T("%d"), pthis->m_countPack);
		nItem = pthis->m_listPack.InsertItem(pthis->m_countPack, buf);

		/*��ʾʱ���*/
		timestr.Format(_T("%d/%d/%d  %d:%d:%d"), data->time[0],
			data->time[1], data->time[2], data->time[3], data->time[4], data->time[5]);
		pthis->m_listPack.SetItemText(nItem, 1, timestr);
		//pthis->m_listPack.setitem

		/*��ʾ����*/
		buf.Empty();
		buf.Format(_T("%d"), data->len);
		pthis->m_listPack.SetItemText(nItem, 2, buf);

		/*��ʾԴMAC*/
		buf.Empty();
		buf.Format(_T("%02X-%02X-%02X-%02X-%02X-%02X"), data->ethh->src[0], data->ethh->src[1],
			data->ethh->src[2], data->ethh->src[3], data->ethh->src[4], data->ethh->src[5]);
		pthis->m_listPack.SetItemText(nItem, 3, buf);

		/*��ʾĿ��MAC*/
		buf.Empty();
		buf.Format(_T("%02X-%02X-%02X-%02X-%02X-%02X"), data->ethh->dest[0], data->ethh->dest[1],
			data->ethh->dest[2], data->ethh->dest[3], data->ethh->dest[4], data->ethh->dest[5]);
		pthis->m_listPack.SetItemText(nItem, 4, buf);

		/*���Э��*/
		pthis->m_listPack.SetItemText(nItem, 5, CString(data->pktType));

		/*���ԴIP*/
		buf.Empty();
		if (0x0806 == data->ethh->type)
		{
			buf.Format(_T("%d.%d.%d.%d"), data->arph->ar_srcip[0],
				data->arph->ar_srcip[1], data->arph->ar_srcip[2], data->arph->ar_srcip[3]);
		}
		else if (0x0800 == data->ethh->type) {
			struct  in_addr in;
			in.S_un.S_addr = data->iph->saddr;
			buf = CString(inet_ntoa(in));
		}
		else if (0x86dd == data->ethh->type){
			int n;
			for (n = 0; n<8; n++)
			{
				if (n <= 6)
					buf.AppendFormat(_T("%02x:"), data->iph6->saddr[n]);
				else
					buf.AppendFormat(_T("%02x"), data->iph6->saddr[n]);
			}
		}
		pthis->m_listPack.SetItemText(nItem, 6, buf);

		/*���Ŀ��IP*/
		buf.Empty();
		if (0x0806 == data->ethh->type)
		{
			buf.Format(_T("%d.%d.%d.%d"), data->arph->ar_destip[0],
				data->arph->ar_destip[1], data->arph->ar_destip[2], data->arph->ar_destip[3]);
		}
		else if (0x0800 == data->ethh->type){
			struct  in_addr in;
			in.S_un.S_addr = data->iph->daddr;
			buf = CString(inet_ntoa(in));
		}
		else if (0x86dd == data->ethh->type){
			int n;
			for (n = 0; n<8; n++)
			{
				if (n <= 6)
					buf.AppendFormat(_T("%02x:"), data->iph6->daddr[n]);
				else
					buf.AppendFormat(_T("%02x"), data->iph6->daddr[n]);
			}
		}
		pthis->m_listPack.SetItemText(nItem, 7, buf);

		/*�԰�����*/
		pthis->m_countPack++;

	}
	return 1;
}

//������Ϣ
int CCatchPack::updateEdit(int index)
{
	POSITION localpos, netpos;
	localpos = this->m_localDataList.FindIndex(index);
	netpos = this->m_netDataList.FindIndex(index);

	struct datapkt* local_data = (struct datapkt*)(this->m_localDataList.GetAt(localpos));
	u_char * net_data = (u_char*)(this->m_netDataList.GetAt(netpos));

	CString buf;
	print_packet_hex(net_data, local_data->len, &buf);

	this->m_packContent.SetWindowTextW(buf);

	return 1;
}

//�������οؼ�
int CCatchPack::updateTree(int index)
{
	POSITION localpos;
	CString str;
	int i;

	this->m_treePack.DeleteAllItems();

	localpos = this->m_localDataList.FindIndex(index);
	struct datapkt* local_data = (struct datapkt*)(this->m_localDataList.GetAt(localpos));

	HTREEITEM root = this->m_treePack.GetRootItem();
	str.Format(_T("���յ��ĵ�%d�����ݰ�"), index + 1);
	HTREEITEM data = this->m_treePack.InsertItem(str, root);

	/*����֡����*/
	HTREEITEM frame = this->m_treePack.InsertItem(_T("��·������"), data);
	//ԴMAC
	str.Format(_T("ԴMAC��"));
	for (i = 0; i<6; i++)
	{
		if (i <= 4)
			str.AppendFormat(_T("%02x-"), local_data->ethh->src[i]);
		else
			str.AppendFormat(_T("%02x"), local_data->ethh->src[i]);
	}
	this->m_treePack.InsertItem(str, frame);
	//Ŀ��MAC
	str.Format(_T("Ŀ��MAC��"));
	for (i = 0; i<6; i++)
	{
		if (i <= 4)
			str.AppendFormat(_T("%02x-"), local_data->ethh->dest[i]);
		else
			str.AppendFormat(_T("%02x"), local_data->ethh->dest[i]);
	}
	this->m_treePack.InsertItem(str, frame);
	//����
	str.Format(_T("���ͣ�0x%02x"), local_data->ethh->type);
	this->m_treePack.InsertItem(str, frame);

	/*����IP��ARP��IPv6���ݰ�*/
	if (0x0806 == local_data->ethh->type)							//ARP
	{
		HTREEITEM arp = this->m_treePack.InsertItem(_T("ARPЭ��ͷ"), data);
		str.Format(_T("Ӳ�����ͣ�%d"), local_data->arph->ar_hrd);
		this->m_treePack.InsertItem(str, arp);
		str.Format(_T("Э�����ͣ�0x%02x"), local_data->arph->ar_pro);
		this->m_treePack.InsertItem(str, arp);
		str.Format(_T("Ӳ����ַ���ȣ�%d"), local_data->arph->ar_hln);
		this->m_treePack.InsertItem(str, arp);
		str.Format(_T("Э���ַ���ȣ�%d"), local_data->arph->ar_pln);
		this->m_treePack.InsertItem(str, arp);
		str.Format(_T("�����룺%d"), local_data->arph->ar_op);
		this->m_treePack.InsertItem(str, arp);

		str.Format(_T("���ͷ�MAC��"));
		for (i = 0; i<6; i++)
		{
			if (i <= 4)
				str.AppendFormat(_T("%02x-"), local_data->arph->ar_srcmac[i]);
			else
				str.AppendFormat(_T("%02x"), local_data->arph->ar_srcmac[i]);
		}
		this->m_treePack.InsertItem(str, arp);

		str.Format(_T("���ͷ�IP��"), local_data->arph->ar_hln);
		for (i = 0; i<4; i++)
		{
			if (i <= 2)
				str.AppendFormat(_T("%d."), local_data->arph->ar_srcip[i]);
			else
				str.AppendFormat(_T("%d"), local_data->arph->ar_srcip[i]);
		}
		this->m_treePack.InsertItem(str, arp);

		str.Format(_T("���շ�MAC��"), local_data->arph->ar_hln);
		for (i = 0; i<6; i++)
		{
			if (i <= 4)
				str.AppendFormat(_T("%02x-"), local_data->arph->ar_destmac[i]);
			else
				str.AppendFormat(_T("%02x"), local_data->arph->ar_destmac[i]);
		}
		this->m_treePack.InsertItem(str, arp);

		str.Format(_T("���շ�IP��"), local_data->arph->ar_hln);
		for (i = 0; i<4; i++)
		{
			if (i <= 2)
				str.AppendFormat(_T("%d."), local_data->arph->ar_destip[i]);
			else
				str.AppendFormat(_T("%d"), local_data->arph->ar_destip[i]);
		}
		this->m_treePack.InsertItem(str, arp);

	}
	else if (0x0800 == local_data->ethh->type){					//IP

		HTREEITEM ip = this->m_treePack.InsertItem(_T("IPЭ��ͷ"), data);

		str.Format(_T("�汾��%d"), local_data->iph->version);
		this->m_treePack.InsertItem(str, ip);
		str.Format(_T("IPͷ����%d"), local_data->iph->ihl);
		this->m_treePack.InsertItem(str, ip);
		str.Format(_T("�������ͣ�%d"), local_data->iph->tos);
		this->m_treePack.InsertItem(str, ip);
		str.Format(_T("�ܳ��ȣ�%d"), local_data->iph->tlen);
		this->m_treePack.InsertItem(str, ip);
		str.Format(_T("��ʶ��0x%02x"), local_data->iph->id);
		this->m_treePack.InsertItem(str, ip);
		str.Format(_T("��ƫ�ƣ�%d"), local_data->iph->frag_off);
		this->m_treePack.InsertItem(str, ip);
		str.Format(_T("�����ڣ�%d"), local_data->iph->ttl);
		this->m_treePack.InsertItem(str, ip);
		str.Format(_T("Э�飺%d"), local_data->iph->proto);
		this->m_treePack.InsertItem(str, ip);
		str.Format(_T("ͷ��У��ͣ�0x%02x"), local_data->iph->check);
		this->m_treePack.InsertItem(str, ip);

		str.Format(_T("ԴIP��"));
		struct in_addr in;
		in.S_un.S_addr = local_data->iph->saddr;
		str.AppendFormat(CString(inet_ntoa(in)));
		this->m_treePack.InsertItem(str, ip);

		str.Format(_T("Ŀ��IP��"));
		in.S_un.S_addr = local_data->iph->daddr;
		str.AppendFormat(CString(inet_ntoa(in)));
		this->m_treePack.InsertItem(str, ip);

		/*�������ICMP��UDP��TCP*/
		if (1 == local_data->iph->proto)							//ICMP
		{
			HTREEITEM icmp = this->m_treePack.InsertItem(_T("ICMPЭ��ͷ"), data);

			str.Format(_T("����:%d"), local_data->icmph->type);
			this->m_treePack.InsertItem(str, icmp);
			str.Format(_T("����:%d"), local_data->icmph->code);
			this->m_treePack.InsertItem(str, icmp);
			str.Format(_T("���:%d"), local_data->icmph->seq);
			this->m_treePack.InsertItem(str, icmp);
			str.Format(_T("У���:%d"), local_data->icmph->chksum);
			this->m_treePack.InsertItem(str, icmp);

		}
		else if (6 == local_data->iph->proto){				//TCP

			HTREEITEM tcp = this->m_treePack.InsertItem(_T("TCPЭ��ͷ"), data);

			str.Format(_T("  Դ�˿�:%d"), local_data->tcph->sport);
			this->m_treePack.InsertItem(str, tcp);
			str.Format(_T("  Ŀ�Ķ˿�:%d"), local_data->tcph->dport);
			this->m_treePack.InsertItem(str, tcp);
			str.Format(_T("  ���к�:0x%02x"), local_data->tcph->seq);
			this->m_treePack.InsertItem(str, tcp);
			str.Format(_T("  ȷ�Ϻ�:%d"), local_data->tcph->ack_seq);
			this->m_treePack.InsertItem(str, tcp);
			str.Format(_T("  ͷ������:%d"), local_data->tcph->doff);

			HTREEITEM flag = this->m_treePack.InsertItem(_T(" +��־λ"), tcp);

			str.Format(_T("cwr %d"), local_data->tcph->cwr);
			this->m_treePack.InsertItem(str, flag);
			str.Format(_T("ece %d"), local_data->tcph->ece);
			this->m_treePack.InsertItem(str, flag);
			str.Format(_T("urg %d"), local_data->tcph->urg);
			this->m_treePack.InsertItem(str, flag);
			str.Format(_T("ack %d"), local_data->tcph->ack);
			this->m_treePack.InsertItem(str, flag);
			str.Format(_T("psh %d"), local_data->tcph->psh);
			this->m_treePack.InsertItem(str, flag);
			str.Format(_T("rst %d"), local_data->tcph->rst);
			this->m_treePack.InsertItem(str, flag);
			str.Format(_T("syn %d"), local_data->tcph->syn);
			this->m_treePack.InsertItem(str, flag);
			str.Format(_T("fin %d"), local_data->tcph->fin);
			this->m_treePack.InsertItem(str, flag);

			str.Format(_T("  ����ָ��:%d"), local_data->tcph->urg_ptr);
			this->m_treePack.InsertItem(str, tcp);
			str.Format(_T("  У���:0x%02x"), local_data->tcph->check);
			this->m_treePack.InsertItem(str, tcp);
			str.Format(_T("  ѡ��:%d"), local_data->tcph->opt);
			this->m_treePack.InsertItem(str, tcp);
		}
		else if (17 == local_data->iph->proto){				//UDP
			HTREEITEM udp = this->m_treePack.InsertItem(_T("UDPЭ��ͷ"), data);

			str.Format(_T("Դ�˿�:%d"), local_data->udph->sport);
			this->m_treePack.InsertItem(str, udp);
			str.Format(_T("Ŀ�Ķ˿�:%d"), local_data->udph->dport);
			this->m_treePack.InsertItem(str, udp);
			str.Format(_T("�ܳ���:%d"), local_data->udph->len);
			this->m_treePack.InsertItem(str, udp);
			str.Format(_T("У���:0x%02x"), local_data->udph->check);
			this->m_treePack.InsertItem(str, udp);
		}
	}
	else if (0x86dd == local_data->ethh->type){		//IPv6
		HTREEITEM ip6 = this->m_treePack.InsertItem(_T("IPv6Э��ͷ"), data);

		//////////////////////////////////////////////////////////////////////////////////////////
		str.Format(_T("�汾:%d"), local_data->iph6->flowtype);
		this->m_treePack.InsertItem(str, ip6);
		str.Format(_T("������:%d"), local_data->iph6->version);
		this->m_treePack.InsertItem(str, ip6);
		///////////////////////////////////////////////////////////////////////////////////////////
		str.Format(_T("����ǩ:%d"), local_data->iph6->flowid);
		this->m_treePack.InsertItem(str, ip6);
		str.Format(_T("��Ч�غɳ���:%d"), local_data->iph6->plen);
		this->m_treePack.InsertItem(str, ip6);
		str.Format(_T("��һ���ײ�:0x%02x"), local_data->iph6->nh);
		this->m_treePack.InsertItem(str, ip6);
		str.Format(_T("������:%d"), local_data->iph6->hlim);
		this->m_treePack.InsertItem(str, ip6);

		str.Format(_T("Դ��ַ:"));
		int n;
		for (n = 0; n<8; n++)
		{
			if (n <= 6)
				str.AppendFormat(_T("%02x:"), local_data->iph6->saddr[n]);
			else
				str.AppendFormat(_T("%02x"), local_data->iph6->saddr[n]);
		}
		this->m_treePack.InsertItem(str, ip6);

		str.Format(_T("Ŀ�ĵ�ַ:"));
		for (n = 0; n<8; n++)
		{
			if (n <= 6)
				str.AppendFormat(_T("%02x:"), local_data->iph6->saddr[n]);
			else
				str.AppendFormat(_T("%02x"), local_data->iph6->saddr[n]);
		}
		this->m_treePack.InsertItem(str, ip6);

		/*�������ICMPv6��UDP��TCP*/
		if (0x3a == local_data->iph6->nh)							//ICMPv6
		{
			HTREEITEM icmp6 = this->m_treePack.InsertItem(_T("ICMPv6Э��ͷ"), data);

			str.Format(_T("����:%d"), local_data->icmph6->type);
			this->m_treePack.InsertItem(str, icmp6);
			str.Format(_T("����:%d"), local_data->icmph6->code);
			this->m_treePack.InsertItem(str, icmp6);
			str.Format(_T("���:%d"), local_data->icmph6->seq);
			this->m_treePack.InsertItem(str, icmp6);
			str.Format(_T("У���:%d"), local_data->icmph6->chksum);
			this->m_treePack.InsertItem(str, icmp6);
			str.Format(_T("ѡ��-����:%d"), local_data->icmph6->op_type);
			this->m_treePack.InsertItem(str, icmp6);
			str.Format(_T("ѡ��-����%d"), local_data->icmph6->op_len);
			this->m_treePack.InsertItem(str, icmp6);
			str.Format(_T("ѡ��-��·���ַ:"));
			int i;
			for (i = 0; i<6; i++)
			{
				if (i <= 4)
					str.AppendFormat(_T("%02x-"), local_data->icmph6->op_ethaddr[i]);
				else
					str.AppendFormat(_T("%02x"), local_data->icmph6->op_ethaddr[i]);
			}
			this->m_treePack.InsertItem(str, icmp6);

		}
		else if (0x06 == local_data->iph6->nh){				//TCP

			HTREEITEM tcp = this->m_treePack.InsertItem(_T("TCPЭ��ͷ"), data);

			str.Format(_T("  Դ�˿�:%d"), local_data->tcph->sport);
			this->m_treePack.InsertItem(str, tcp);
			str.Format(_T("  Ŀ�Ķ˿�:%d"), local_data->tcph->dport);
			this->m_treePack.InsertItem(str, tcp);
			str.Format(_T("  ���к�:0x%02x"), local_data->tcph->seq);
			this->m_treePack.InsertItem(str, tcp);
			str.Format(_T("  ȷ�Ϻ�:%d"), local_data->tcph->ack_seq);
			this->m_treePack.InsertItem(str, tcp);
			str.Format(_T("  ͷ������:%d"), local_data->tcph->doff);

			HTREEITEM flag = this->m_treePack.InsertItem(_T("��־λ"), tcp);

			str.Format(_T("cwr %d"), local_data->tcph->cwr);
			this->m_treePack.InsertItem(str, flag);
			str.Format(_T("ece %d"), local_data->tcph->ece);
			this->m_treePack.InsertItem(str, flag);
			str.Format(_T("urg %d"), local_data->tcph->urg);
			this->m_treePack.InsertItem(str, flag);
			str.Format(_T("ack %d"), local_data->tcph->ack);
			this->m_treePack.InsertItem(str, flag);
			str.Format(_T("psh %d"), local_data->tcph->psh);
			this->m_treePack.InsertItem(str, flag);
			str.Format(_T("rst %d"), local_data->tcph->rst);
			this->m_treePack.InsertItem(str, flag);
			str.Format(_T("syn %d"), local_data->tcph->syn);
			this->m_treePack.InsertItem(str, flag);
			str.Format(_T("fin %d"), local_data->tcph->fin);
			this->m_treePack.InsertItem(str, flag);

			str.Format(_T("  ����ָ��:%d"), local_data->tcph->urg_ptr);
			this->m_treePack.InsertItem(str, tcp);
			str.Format(_T("  У���:0x%02x"), local_data->tcph->check);
			this->m_treePack.InsertItem(str, tcp);
			str.Format(_T("  ѡ��:%d"), local_data->tcph->opt);
			this->m_treePack.InsertItem(str, tcp);
		}
		else if (0x11 == local_data->iph6->nh){				//UDP
			HTREEITEM udp = this->m_treePack.InsertItem(_T("UDPЭ��ͷ"), data);

			str.Format(_T("Դ�˿�:%d"), local_data->udph->sport);
			this->m_treePack.InsertItem(str, udp);
			str.Format(_T("Ŀ�Ķ˿�:%d"), local_data->udph->dport);
			this->m_treePack.InsertItem(str, udp);
			str.Format(_T("�ܳ���:%d"), local_data->udph->len);
			this->m_treePack.InsertItem(str, udp);
			str.Format(_T("У���:0x%02x"), local_data->udph->check);
			this->m_treePack.InsertItem(str, udp);
		}
	}

	return 1;
}


int CCatchPack::saveFile()
{
	CFileFind find;
	if (NULL == find.FindFile(CString(filepath)))
	{
		MessageBox(_T("�����ļ�����δ֪����"));
		return -1;
	}

	//���ļ��Ի���
	CFileDialog   FileDlg(FALSE, _T(".lix"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT);
	FileDlg.m_ofn.lpstrInitialDir = _T("c:\\");
	if (FileDlg.DoModal() == IDOK)
	{
		CopyFile(CString(filepath), FileDlg.GetPathName(), TRUE);
	}
	return 1;
}

int CCatchPack::readFile(CString path)
{
	int res, nItem, i;
	struct tm *ltime;
	CString timestr, buf, srcMac, destMac;
	time_t local_tv_sec;
	struct pcap_pkthdr *header;									  //���ݰ�ͷ
	const u_char *pkt_data = NULL;     //�������յ����ֽ�������
	u_char *ppkt_data;

	CCatchPack *pthis = this;						//Щ���������lixsinff_CapThread��Ϊ��Լ���������ʱ���pthisָ��
	pcap_t *fp;

	//���ȴ���һ��·��������pcap_open_offline���ļ�ʱ��
	//·����Ҫ��char *���ͣ�������CStringǿ��ת�����char *
	int len = path.GetLength() + 1;							/////////////////////////////////ע����һ��ϸ�ڣ�����Ҫ��1����������
	char* charpath = (char *)malloc(len);
	memset(charpath, 0, len);
	if (NULL == charpath)
		return -1;

	for (i = 0; i<len; i++)
		charpath[i] = (char)path.GetAt(i);

	//������ļ�
	if ((fp = pcap_open_offline( /*(char*)(LPCTSTR)path*/charpath, errbuf)) == NULL)
	{
		MessageBox(_T("���ļ�����") + CString(errbuf));
		return -1;
	}

	while ((res = pcap_next_ex(fp, &header, &pkt_data)) >= 0)
	{
		struct datapkt *data = (struct datapkt*)malloc(sizeof(struct datapkt));
		memset(data, 0, sizeof(struct datapkt));

		if (NULL == data)
		{
			MessageBox(_T("�ռ��������޷������µ����ݰ�"));
			return  -1;
		}

		//������������������ݰ����ڴ���Χ��
		if (analyze_frame(pkt_data, data, &(pthis->npacket))<0)
			continue;

		//�����ػ��������װ��һ�������У��Ա����ʹ��		
		ppkt_data = (u_char*)malloc(header->len);
		memcpy(ppkt_data, pkt_data, header->len);

		pthis->m_localDataList.AddTail(data);
		pthis->m_netDataList.AddTail(ppkt_data);

		/*Ԥ�������ʱ�䡢����*/
		data->len = header->len;								//��·���յ������ݳ���
		local_tv_sec = header->ts.tv_sec;
		ltime = localtime(&local_tv_sec);
		data->time[0] = ltime->tm_year + 1900;
		data->time[1] = ltime->tm_mon + 1;
		data->time[2] = ltime->tm_mday;
		data->time[3] = ltime->tm_hour;
		data->time[4] = ltime->tm_min;
		data->time[5] = ltime->tm_sec;

		/*Ϊ�½��յ������ݰ���listControl���½�һ��item*/
		buf.Format(_T("%d"), pthis->m_countPack);
		nItem = pthis->m_listPack.InsertItem(pthis->m_countPack, buf);

		/*��ʾʱ���*/
		timestr.Format(_T("%d/%d/%d  %d:%d:%d"), data->time[0],
			data->time[1], data->time[2], data->time[3], data->time[4], data->time[5]);
		pthis->m_listPack.SetItemText(nItem, 1, timestr);

		/*��ʾ����*/
		buf.Empty();
		buf.Format(_T("%d"), data->len);
		pthis->m_listPack.SetItemText(nItem, 2, buf);

		/*��ʾԴMAC*/
		buf.Empty();
		buf.Format(_T("%02X-%02X-%02X-%02X-%02X-%02X"), data->ethh->src[0], data->ethh->src[1],
			data->ethh->src[2], data->ethh->src[3], data->ethh->src[4], data->ethh->src[5]);
		pthis->m_listPack.SetItemText(nItem, 3, buf);

		/*��ʾĿ��MAC*/
		buf.Empty();
		buf.Format(_T("%02X-%02X-%02X-%02X-%02X-%02X"), data->ethh->dest[0], data->ethh->dest[1],
			data->ethh->dest[2], data->ethh->dest[3], data->ethh->dest[4], data->ethh->dest[5]);
		pthis->m_listPack.SetItemText(nItem, 4, buf);

		/*���Э��*/
		pthis->m_listPack.SetItemText(nItem, 5, CString(data->pktType));

		/*���ԴIP*/
		buf.Empty();
		if (0x0806 == data->ethh->type)
		{
			buf.Format(_T("%d.%d.%d.%d"), data->arph->ar_srcip[0],
				data->arph->ar_srcip[1], data->arph->ar_srcip[2], data->arph->ar_srcip[3]);
		}
		else  if (0x0800 == data->ethh->type){
			struct  in_addr in;
			in.S_un.S_addr = data->iph->saddr;
			buf = CString(inet_ntoa(in));
		}
		else if (0x86dd == data->ethh->type){
			int i;
			for (i = 0; i<8; i++)
			{
				if (i <= 6)
					buf.AppendFormat(_T("%02x-"), data->iph6->saddr[i]);
				else
					buf.AppendFormat(_T("%02x"), data->iph6->saddr[i]);
			}
		}
		pthis->m_listPack.SetItemText(nItem, 6, buf);

		/*���Ŀ��IP*/
		buf.Empty();
		if (0x0806 == data->ethh->type)
		{
			buf.Format(_T("%d.%d.%d.%d"), data->arph->ar_destip[0],
				data->arph->ar_destip[1], data->arph->ar_destip[2], data->arph->ar_destip[3]);
		}
		else if (0x0800 == data->ethh->type) {
			struct  in_addr in;
			in.S_un.S_addr = data->iph->daddr;
			buf = CString(inet_ntoa(in));
		}
		else if (0x86dd == data->ethh->type){
			int i;
			for (i = 0; i<8; i++)
			{
				if (i <= 6)

					buf.AppendFormat(_T("%02x-"), data->iph6->daddr[i]);
				else
					buf.AppendFormat(_T("%02x"), data->iph6->daddr[i]);
			}
		}
		pthis->m_listPack.SetItemText(nItem, 7, buf);

		/*�԰�����*/
		pthis->m_countPack++;
	}

	pcap_close(fp);

	return 1;
}

void CCatchPack::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	int index;
	index = this->m_listPack.GetHotItem();
	int count = this->m_localDataList.GetCount();
	if (index < 0 || index>(count - 1))
		return;

	this->updateEdit(index);
	this->updateTree(index);
	*pResult = 0;
}


void CCatchPack::OnNMCustomdrawList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	LPNMLVCUSTOMDRAW pNMCD = (LPNMLVCUSTOMDRAW)pNMHDR;
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;

	if (CDDS_PREPAINT == pNMCD->nmcd.dwDrawStage)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (CDDS_ITEMPREPAINT == pNMCD->nmcd.dwDrawStage){
		COLORREF crText;
		char buf[10];
		memset(buf, 0, 10);
		POSITION pos = this->m_localDataList.FindIndex(pNMCD->nmcd.dwItemSpec);
		struct datapkt * local_data = (struct datapkt *)this->m_localDataList.GetAt(pos);
		strcpy(buf, local_data->pktType);

		if (strcmp(buf, "IPV6") == 0)
			crText = RGB(111, 224, 254);
		else if (strcmp(buf, "UDP") == 0)
			crText = RGB(194, 195, 252);
		else if (strcmp(buf, "TCP") == 0)
			crText = RGB(230, 230, 230);
		else if (strcmp(buf, "ARP") == 0)
			crText = RGB(226, 238, 227);
		else if (strcmp(buf, "ICMP") == 0)
			crText = RGB(49, 164, 238);
		else if (strcmp(buf, "HTTP") == 0)
			crText = RGB(238, 232, 180);
		else if (strcmp(buf, "ICMPv6") == 0)
			crText = RGB(189, 254, 76);

		pNMCD->clrTextBk = crText;
		*pResult = CDRF_DODEFAULT;
	}
}


HCURSOR CCatchPack::OnQueryDragIcon()
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ

	return static_cast<HCURSOR>(m_hIcon);
}

#else
IMPLEMENT_DYNAMIC(CCatchPack, CDialogEx)

CCatchPack::CCatchPack(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCatchPack::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CCatchPack::~CCatchPack()
{
	if (NULL != m_ThreadHandle)
		TerminateThread(this->m_ThreadHandle, -1);

	this->m_ThreadHandle = NULL;
	m_localDataList.RemoveAll();
	m_netDataList.RemoveAll();
}
void CCatchPack::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NETWORK, m_network);
	DDX_Control(pDX, IDC_PROTOCOL, m_protocol);
	DDX_Control(pDX, IDC_CONTENT, m_content);
	DDX_Control(pDX, IDC_LIST1, m_listPack);
	DDX_Control(pDX, IDC_TREE1, m_treePack);
	DDX_Control(pDX, IDC_PACKCONTENT, m_packContent);
	DDX_Control(pDX, IDC_STARTCATCH, m_startCatch);
	DDX_Control(pDX, IDC_STOPCATCH, m_stopCatch);
	DDX_Control(pDX, IDC_SAVEPACK, m_savePack);
	DDX_Control(pDX, IDC_OPENPACK, m_openPack);
}


BEGIN_MESSAGE_MAP(CCatchPack, CDialogEx)
	ON_BN_CLICKED(IDC_START, &CCatchPack::OnBnClickedStart)
	ON_BN_CLICKED(IDC_STOP, &CCatchPack::OnBnClickedStop)
	ON_BN_CLICKED(IDC_SAVE, &CCatchPack::OnBnClickedSave)
	ON_BN_CLICKED(IDC_OPEN, &CCatchPack::OnBnClickedOpen)
	ON_WM_SIZE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CCatchPack::OnLvnItemchangedList1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST1, &CCatchPack::OnNMCustomdrawList1)
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()

void CCatchPack::OnBnClickedStart()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	
}


void CCatchPack::OnBnClickedStop()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

}


void CCatchPack::OnBnClickedSave()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

}


void CCatchPack::OnBnClickedOpen()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	
}


void CCatchPack::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO:  �ڴ˴������Ϣ����������
	
}


BOOL CCatchPack::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

void CCatchPack::setEncode(int encode)
{
	m_encode = encode;
}

int CCatchPack::initCatchPack()
{
	
	return 0;
}

//��ʼ����
int CCatchPack::startCatchPack()
{
	
	return 1;
}

DWORD WINAPI catchPackThread(LPVOID lpParameter)
{
	
	return 1;
}

//������Ϣ
int CCatchPack::updateEdit(int index)
{

	return 1;
}

//�������οؼ�
int CCatchPack::updateTree(int index)
{
	
	return 1;
}


int CCatchPack::saveFile()
{

	return 1;
}

int CCatchPack::readFile(CString path)
{
	

	return 1;
}

void CCatchPack::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

}


void CCatchPack::OnNMCustomdrawList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	LPNMLVCUSTOMDRAW pNMCD = (LPNMLVCUSTOMDRAW)pNMHDR;
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

}


HCURSOR CCatchPack::OnQueryDragIcon()
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ

	return static_cast<HCURSOR>(m_hIcon);
}
#endif