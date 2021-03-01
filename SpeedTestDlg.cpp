// SpeedTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "SpeedTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpeedTestDlg dialog


CSpeedTestDlg::CSpeedTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSpeedTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSpeedTestDlg)
	//}}AFX_DATA_INIT
	m_Return=-1;
	m_nCurTab=0;
}


void CSpeedTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpeedTestDlg)
	DDX_Control(pDX, IDC_TAB, m_Tab);
	DDX_Control(pDX, IDC_EXPORT, m_ExportCtrl);
	DDX_Control(pDX, IDC_STATUS_STATIC, m_Status);
	DDX_Control(pDX, IDC_LOGOIN, m_LogoinCtrl);
	DDX_Control(pDX, IDC_TESTAGAIN, m_TryAgainCtrl);
	DDX_Control(pDX, IDC_HOSTLIST, m_hostlist);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSpeedTestDlg, CDialog)
	//{{AFX_MSG_MAP(CSpeedTestDlg)
	ON_BN_CLICKED(IDC_TESTAGAIN, OnTestagain)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_LOGOIN, OnLogoin)
	ON_NOTIFY(NM_DBLCLK, IDC_HOSTLIST, OnDblclkHostlist)
	ON_BN_CLICKED(IDC_EXPORT, OnExport)
	ON_BN_CLICKED(IDC_RETURN,OnCancel)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, OnSelchangeTab)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpeedTestDlg message handlers

void CSpeedTestDlg::OnCancel() 
{
	CDialog::OnCancel();
}

extern DWORD WINAPI	AfxNLBThread(LPVOID lParam);
extern DWORD WINAPI	AfxLevel2NLBThread(LPVOID lParam);
extern DWORD WINAPI	AfxWTNLBThread(LPVOID lParam);

#define ONEBATCH_TEST	3
void CSpeedTestDlg::ProceedBatchTest()
{
	TDXWHOST *pHost=&g_HQHost;
	if(g_bNewJyUseAdvHQHost)
		pHost=&g_L2HQHost;
	if(m_nCurTab==1)
		pHost=&g_WTHost;

	HANDLE	m_hNLBThreads[150]={NULL};
	DWORD	TmpThreadID;

	int		i,j;
	int		nTestNum = 0;
	for(i=0;i<pHost->HostNum&&nTestNum<ONEBATCH_TEST;i++)
	{
		if(g_NLBInfo[i].bProcTest) 
			continue;
		++nTestNum;
		g_NLBInfo[i].ClientNum=-1;
		g_NLBInfo[i].Diff_TickCnt=MAXLONG;
		if(m_nCurTab==1)
			m_hNLBThreads[i] = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)AfxWTNLBThread,(LPVOID)i,0,&TmpThreadID);
		else if(g_bNewJyUseAdvHQHost)
			m_hNLBThreads[i] = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)AfxLevel2NLBThread,(LPVOID)i,0,&TmpThreadID);
		else
			m_hNLBThreads[i] = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)AfxNLBThread,(LPVOID)i,0,&TmpThreadID);
		Sleep(1);
	}
	vector<HANDLE> vWait;
	for(i=0;i<pHost->HostNum;i++)
	{
		if(m_hNLBThreads[i]!=NULL)
		{
			vWait.push_back(m_hNLBThreads[i]);
			g_NLBInfo[i].bProcTest = TRUE;
		}
	}
	//没了
	if(vWait.empty()) 
	{
		KillTimer(2);
		m_TryAgainCtrl.EnableWindow(TRUE);
		m_ExportCtrl.EnableWindow(TRUE);
		m_TryAgainCtrl.SetWindowText(_F("开始测试"));
		m_Status.SetText("");
		return;
	}

	int nNLBAck = 0;
	while (vWait.size()>0 && nNLBAck<ONEBATCH_TEST)
	{
		PeekMsg();
		DWORD nRet = WaitForMultipleObjects(vWait.size(), &vWait[0], FALSE, 1000);			//每次等1s超时
		PeekMsg();
		if(nRet==WAIT_TIMEOUT || nRet<WAIT_OBJECT_0 || nRet>(WAIT_OBJECT_0+pHost->HostNum-1)) break;
		int nWaitXh = nRet - WAIT_OBJECT_0;
		int nThreadXh = -1;
		for(i=0;i<150;i++)
		{
			if(vWait[nWaitXh]&&vWait[nWaitXh]==m_hNLBThreads[i])
			{
				nThreadXh = i;
				break;
			}
		}
		if(nThreadXh<0) 
		{
			++nNLBAck;
			continue;
		}
		DWORD nExitCode;
		BOOL bGetCode = GetExitCodeThread(m_hNLBThreads[nThreadXh], &nExitCode);
		if(bGetCode && STILL_ACTIVE!=nExitCode)
		{
			CloseHandle(m_hNLBThreads[nThreadXh]);
			m_hNLBThreads[nThreadXh] = NULL;
			
			vWait.clear();
			for(i=0;i<pHost->HostNum;i++)
			{
				if(m_hNLBThreads[i]!=NULL)
					vWait.push_back(m_hNLBThreads[i]);
			}
		}
		++nNLBAck;
	}
	for(i=0;i<pHost->HostNum;i++)
	{
		CHQComm_NLB::CloseThreadSocket(i);
		if(m_hNLBThreads[i]==NULL) continue;
		DWORD nExitCode;
		BOOL bGetCode = GetExitCodeThread(m_hNLBThreads[i], &nExitCode);
		if(!bGetCode||STILL_ACTIVE==nExitCode)
		{
			if (::WaitForSingleObject(m_hNLBThreads[i],300)!=WAIT_OBJECT_0 )
			{
				//不能强制去掉线程，以免造成半连接(三次握手没有完成)
				//::TerminateThread(m_hNLBThreads[i], 0);
			}
		}
		CloseHandle(m_hNLBThreads[i]);
		m_hNLBThreads[i] = NULL;
	}
	//找出负载最轻的主站
	aSort.RemoveAll();
	double tmpqz=0;
	for(j=0;j<pHost->HostNum;j++)
	{
		int n=aSort.GetSize();
		Sort tmpSort;
		tmpSort.nIndex=j;
		if(g_NLBInfo[j].Diff_TickCnt!=MAXLONG)
		{
			tmpSort.yali=g_NLBInfo[j].ClientNum*100/g_NLBInfo[j].MaxConnectNum;
			if(tmpSort.yali>90)
				tmpSort.zonghe=long(g_NLBInfo[j].Diff_TickCnt/10.0*7+tmpSort.yali*3);
			else if(tmpSort.yali>80)
				tmpSort.zonghe=long(g_NLBInfo[j].Diff_TickCnt/10.0*8+tmpSort.yali*2);
			else
				tmpSort.zonghe=long(g_NLBInfo[j].Diff_TickCnt/10.0*9+tmpSort.yali*1);
			tmpqz=(tmpSort.zonghe*(100-pHost->weight[j]))/100.0;
			tmpSort.zonghe=(long)tmpqz;
		}
		for(int q=0;q<n;q++)
		{
			if(tmpSort.zonghe<aSort[q].zonghe)
				break;
		}
		aSort.InsertAt(q,tmpSort);
	}
	
	m_hostlist.SetRedraw(FALSE);
	m_hostlist.DeleteAllItems();
	CString tmpStr;
	for(j=0;j<pHost->HostNum;j++)
	{
		if(g_NLBInfo[aSort[j].nIndex].Diff_TickCnt==MAXLONG)
		{
			m_hostlist.InsertItem(j,_F(pHost->HostName[aSort[j].nIndex]),2);
			m_hostlist.SetItemText(j,1,IntToStr(pHost->weight[aSort[j].nIndex]));
			m_hostlist.SetItemText(j,2,_F("--"));
			m_hostlist.SetItemText(j,3,_F("--"));
			if(!g_NLBInfo[aSort[j].nIndex].bProcTest)
				m_hostlist.SetItemText(j,4,_F("--"));
			else 
				m_hostlist.SetItemText(j,4,_F("连接超时"));
		}
		else
		{
			m_hostlist.InsertItem(j,_F(pHost->HostName[aSort[j].nIndex]),1);
			m_hostlist.SetItemText(j,1,IntToStr(pHost->weight[aSort[j].nIndex]));
			tmpStr.Format("%d",g_NLBInfo[aSort[j].nIndex].Diff_TickCnt);
			m_hostlist.SetItemText(j,2,tmpStr);
			tmpStr.Format("%ld",aSort[j].yali);
			m_hostlist.SetItemText(j,3,tmpStr);
			tmpStr.Format("%ld",aSort[j].zonghe);
			if(aSort[j].zonghe<50)
				tmpStr+="(非常好)";
			else if(aSort[j].zonghe<100)
				tmpStr+="(较好)";
			else if(aSort[j].zonghe<300)
				tmpStr+="(一般)";
			else if(aSort[j].zonghe<500)
				tmpStr+="(较慢)";
			else 
				tmpStr+="(非常慢)";
			m_hostlist.SetItemText(j,4,_F(tmpStr));
		}
	}
	m_hostlist.SetRedraw(TRUE);
	m_hostlist.SetItemState(0,LVIS_FOCUSED|LVIS_SELECTED,LVIS_FOCUSED|LVIS_SELECTED);
	//继续
	SetTimer(2,500,NULL);
}

void CSpeedTestDlg::OnTestagain()
{
	m_TryAgainCtrl.EnableWindow(FALSE);
	m_ExportCtrl.EnableWindow(FALSE);
	m_TryAgainCtrl.SetWindowText(_F("测试中.."));
	m_Status.SetText(_F("请您耐心等待测试结果..."));

	TDXWHOST *pHost=&g_HQHost;
	if(g_bNewJyUseAdvHQHost)
		pHost=&g_L2HQHost;
	if(m_nCurTab==1)
		pHost=&g_WTHost;
	for(int i=0;i<pHost->HostNum;i++)
	{
		g_NLBInfo[i].bProcTest = FALSE;
		g_NLBInfo[i].ClientNum=-1;
		g_NLBInfo[i].Diff_TickCnt=MAXLONG;
	}

	ProceedBatchTest();
}

BOOL CSpeedTestDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_Tab.InsertItem(0,_F("行情主站测速"));
#ifdef OEM_NEWJY
	m_Tab.InsertItem(1,_F("交易中心测速"));
	GetHostInfoFromFile(NULL,NULL,&g_WTHost,NULL,NULL,NULL,NULL,TRUE,g_WSXHStr+g_strYxhjCfg);
#endif
	m_Tab.SetCurSel(m_nCurTab);
	
	m_hostlist.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	m_imagelist.Create(16,16,ILC_COLOR24|ILC_MASK,2,1);
	CBitmap tmpbm;
	tmpbm.LoadBitmap(IDB_IMAGELIST_QJTJ);
	m_imagelist.Add(&tmpbm,RGB(0,0,255));
	m_hostlist.SetImageList(&m_imagelist,LVSIL_SMALL);

	static char *BigVolColStr[5] = {"主站名称","权重","网络延时(ms)","主站负载(%)","综合值"};
	static int BigVolColWidth[5] = {155,40,85,80,90};
	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	for(int i = 0; i < 5;i++)
	{
		lvColumn.pszText = _F(BigVolColStr[i]);
		lvColumn.cx = BigVolColWidth[i];
		lvColumn.iSubItem = i;
		m_hostlist.InsertColumn (i,&lvColumn);
	}

	InitList();
	SetTimer(1,100,NULL);
	m_LogoinCtrl.SetWindowText(_F("使用所选"));

	return TRUE;
}

void CSpeedTestDlg::InitList()
{
	m_hostlist.DeleteAllItems();
	TDXWHOST *pHost=&g_HQHost;
	if(g_bNewJyUseAdvHQHost)
		pHost=&g_L2HQHost;
	if(m_nCurTab==1)
		pHost=&g_WTHost;
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT|LVIF_STATE|LVIF_IMAGE;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iSubItem = 0;
	lvItem.iImage=2;
	for(int i=0;i<pHost->HostNum;i++)
	{
		lvItem.iItem = i;
		lvItem.pszText = _F(pHost->HostName[i]);
		m_hostlist.InsertItem (&lvItem);
		
		m_hostlist.SetItemText(i,1,IntToStr(pHost->weight[i]));
		m_hostlist.SetItemText(i,2,"....");
		m_hostlist.SetItemText(i,3,"....");
		m_hostlist.SetItemText(i,4,"....");
	}
}

void CSpeedTestDlg::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent==1)	
	{
		KillTimer(1);
		OnTestagain();
	}
	if(nIDEvent==2)
	{
		KillTimer(2);
		ProceedBatchTest();
	}
	CDialog::OnTimer(nIDEvent);
}

void CSpeedTestDlg::OnLogoin() 
{
	int nSel=m_hostlist.GetNextItem(-1,LVNI_SELECTED);	//如果返回-1表示没有行被选中
	if(nSel==-1)
	{
		TDX_MessageBox(m_hWnd,"请在列表中选择合适的主站登录!",MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	else
	{
		m_Return=aSort[nSel].nIndex;
		OnOK();
	}
}

void CSpeedTestDlg::OnDblclkHostlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	if(m_nCurTab==0)
		OnLogoin();
}

void CSpeedTestDlg::OnExport() 
{
	CString tmpFilename;
	CFileDialog file(FALSE,"*.txt","",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,_F("文本文件(*.txt)|*.txt|所有文件(*.*)|*.*||"));
	if(file.DoModal()==IDOK)
	{
		tmpFilename=file.GetPathName();
		BOOL bRet = ListCtrl2Txt(&m_hostlist,tmpFilename);
		if(bRet && TDX_MessageBox(m_hWnd,"导出成功,是否打开此文件?",MB_ICONQUESTION|MB_OKCANCEL)==IDOK)
		{
			CString tmpStr;
			tmpStr.Format("\"%s\"",tmpFilename);
			::ShellExecute(NULL,NULL,tmpStr,NULL,NULL,SW_SHOW);
		}		
	}	
}

void CSpeedTestDlg::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_nCurTab=m_Tab.GetCurSel();
	if(m_nCurTab!=0)
		GetDlgItem(IDC_LOGOIN)->ShowWindow(SW_HIDE);
	else
		GetDlgItem(IDC_LOGOIN)->ShowWindow(SW_SHOW);
	InitList();
	
	*pResult = 0;
}
