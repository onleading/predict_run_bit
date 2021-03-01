// UrgentDlg.cpp : implementation file

#include "stdafx.h"
#include "tdxw.h"
#include "UrgentDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUrgentDlg dialog


CUrgentDlg::CUrgentDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUrgentDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUrgentDlg)
	m_strContent = _T("");
	m_strTitle = _T("");
	//}}AFX_DATA_INIT
	m_bHQHostUrgent = TRUE;
	m_bWeb = FALSE;
	m_bCheckUrgent = g_GH.bCheckUrgent;
	m_bEnterPop = TRUE;
	if(g_OEM.bUrgentUseQSIcon)
		m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	else
		m_hIcon = AfxGetApp()->LoadIcon(IDI_INFO_POP);

	m_nMulNum=0;
	memset(m_MulUrgent,0,MAX_MULURGENT_NUM*sizeof(struct Mul_Urgent));
	m_FullRect.SetRectEmpty();
	m_FullRect=m_NoFullRect;
}


void CUrgentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUrgentDlg)
	DDX_Control(pDX, IDC_MUL_TAB, m_MulTab);
	DDX_Control(pDX, IDC_CONTENT_EDIT, m_RichEditEx);
	DDX_Check(pDX, IDC_CHECKTODAY, m_bCheckUrgent);
	DDX_Control(pDX, IDC_EXPLORER1, m_IE);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUrgentDlg, CDialog)
	//{{AFX_MSG_MAP(CUrgentDlg)
	ON_NOTIFY( EN_LINK, IDC_CONTENT_EDIT, OnRichEditExLink )
	ON_MESSAGE(SHOWINFO_MESSAGE,OnShowInfoMsg)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_CHECKTODAY, OnChecktoday)
	ON_BN_CLICKED(IDC_SAVEIT, OnSaveit)
	ON_NOTIFY(TCN_SELCHANGE, IDC_MUL_TAB, OnSelchangeMulTab)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUrgentDlg message handlers

void CUrgentDlg::OnOK()
{
}

BOOL CUrgentDlg::OnInitDialog()
{
	CDialog::OnInitDialog();	
	SetIcon(m_hIcon,TRUE);
	SetIcon(m_hIcon,FALSE);

	if(m_strTitle.GetLength()>0)
		SetWindowText(m_strTitle);
	
	CString tmpStr;
	tmpStr.Format("此通告在%04d年%02d月%02d日以后不再提示",lEndDate/10000,(lEndDate%10000)/100,(lEndDate%10000)%100);
	GetDlgItem(IDC_ENDDATE_STATIC)->SetWindowText(_F(tmpStr));
	//应华西证券要求，这个有效期不再显示
	GetDlgItem(IDC_ENDDATE_STATIC)->ShowWindow(SW_HIDE);

	if(!m_bEnterPop)
		GetDlgItem(IDC_CHECKTODAY)->ShowWindow(SW_HIDE);
	
#ifndef PERSONAL
	GetDlgItem(IDC_SAVEIT)->ShowWindow(SW_HIDE);
#endif
	if(g_OEM.bGtjaEmbedWTVersion)
		GetDlgItem(IDC_SAVEIT)->ShowWindow(SW_HIDE);

	unsigned mask = ::SendMessage(m_RichEditEx.m_hWnd, EM_GETEVENTMASK, 0, 0);
	::SendMessage(m_RichEditEx.m_hWnd, EM_SETEVENTMASK, 0, mask | ENM_LINK);
	::SendMessage(m_RichEditEx.m_hWnd, EM_AUTOURLDETECT, true, 0);  //自动检测URL

	CRect tmpRect,tmpFullRect;
	m_MulTab.GetClientRect(&tmpRect);
	m_RichEditEx.GetWindowRect(&tmpFullRect);
	ScreenToClient(&tmpFullRect);
	m_NoFullRect=m_FullRect=tmpFullRect;
	m_NoFullRect.top=tmpRect.bottom-1;

	PostMessage(SHOWINFO_MESSAGE,0,0);
	return TRUE;
}

void CUrgentDlg::ShowContent(CString strContent,BOOL bWeb)
{
	if(m_nMulNum>0)
		m_MulTab.ShowWindow(SW_SHOW);
	else
		m_MulTab.ShowWindow(SW_HIDE);
	if(bWeb)
	{
		m_bWeb=TRUE;
		GetDlgItem(IDC_SAVEIT)->ShowWindow(SW_HIDE);
		m_RichEditEx.ShowWindow(SW_HIDE);
		if(m_nMulNum>0)
			m_IE.MoveWindow(m_NoFullRect);
		else
			m_IE.MoveWindow(m_FullRect);
		m_IE.SetSilent(TRUE);
		TransferToRealURL(strContent,NULL);
		m_IE.Navigate(strContent,NULL,NULL,NULL,NULL);
	}
	else
	{
		m_RichEditEx.ShowWindow(SW_SHOW);
		if(m_nMulNum>0)
			m_RichEditEx.MoveWindow(m_NoFullRect);
		else
			m_RichEditEx.MoveWindow(m_FullRect);
		m_IE.MoveWindow(0,0,0,0);
		m_RichEditEx.SetWindowText(strContent);
	}
}

void CUrgentDlg::OnShowInfoMsg(WPARAM wParam,LPARAM lParam)
{
	try
	{
		TrimString(m_strContent);
		if(m_strContent.Left(4)=="http" || m_strContent.Left(4)=="HTTP")
		{
			ShowContent(m_strContent,TRUE);
			return;
		}
		else if(m_strContent.Left(6)=="<META>")
		{
			int i,nStartPos = m_strContent.Find("</META>",0);
			if(nStartPos>0)
			{
				if(m_bHQHostUrgent)	//对于行情通告才支持分栏
				{
					char tmpStr[101];
					CString strIndexRow=m_strContent.Mid(6,nStartPos-6);
					GetStr(strIndexRow,m_MulUrgent[0].Title,40,1,',');
					m_nMulNum=1;
					for(i=1;i<MAX_MULURGENT_NUM;i++)
					{
						GetStr(strIndexRow,tmpStr,100,i+1,',');
						if(tmpStr[0]==0) break;
						GetStr(tmpStr,m_MulUrgent[i].Title,40,1,'=');
						GetStr(tmpStr,m_MulUrgent[i].FileName,40,2,'=');
						m_nMulNum++;
					}
					for(i=0;i<m_nMulNum;i++)
						m_MulTab.InsertItem(i,m_MulUrgent[i].Title);
					m_MulTab.SetCurSel(0);
				}
				m_strContent=m_strContent.Mid(nStartPos+7);
				TrimString(m_strContent);		
				if(m_strContent.Left(4)=="http" || m_strContent.Left(4)=="HTTP")
				{
					ShowContent(m_strContent,TRUE);
					return;
				}
			}
		}
		ShowContent(m_strContent,FALSE);
	}
	catch(...)
	{
	}
}

extern void ProcessRichEditExLink(CRichEditCtrl *pCtrl,NMHDR*  in_pNotifyHeader,LRESULT* out_pResult);
void CUrgentDlg::OnRichEditExLink(NMHDR*  in_pNotifyHeader,LRESULT* out_pResult)
{
	ProcessRichEditExLink(&m_RichEditEx,in_pNotifyHeader,out_pResult);
}

int CUrgentDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	PeekMsg();	//这里不加PeekMsg,IE控件会吸收按钮,造成死锁 ???
	return 0;
}

void CUrgentDlg::OnChecktoday() 
{
	UpdateData(TRUE);
	g_GH.bCheckUrgent = m_bCheckUrgent;
}

void CUrgentDlg::OnSaveit() 
{
	CString tmpFileTitle;
	tmpFileTitle.Format("%ld通告",g_lOpenRq);
	CString tmpFileName;
	CFileDialog FileDialog(FALSE,NULL,tmpFileTitle+".txt",
							OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXTENSIONDIFFERENT,
							_F("文本文件(*.txt)|*.txt||"));
	if (FileDialog.DoModal() == IDOK)
	{		
		tmpFileName=FileDialog.GetPathName();
		if(tmpFileName.Find(".",0)<0)
			tmpFileName+=".txt";
		CFile theFile;
		if(theFile.Open(tmpFileName,CFile::modeCreate|CFile::modeWrite|CFile::shareDenyNone))
		{
			theFile.Write(m_strContent,m_strContent.GetLength());
			theFile.Close();
		}
		else
			TDX_MessageBox(m_hWnd,"存盘文件失败!",MB_OK|MB_ICONERROR);
	}
}

void CUrgentDlg::OnSelchangeMulTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int nCur=m_MulTab.GetCurSel();
	if(nCur<0) return;
	
	CString tmpContent=_F("无内容");
	if(nCur==0)
		tmpContent=m_strContent;
	else if(nCur>0)
	{
		g_nDataSyncType=DATASYNC_FILE;
		g_strDataSyncFile=m_MulUrgent[nCur].FileName;
		AfxGetMainWnd()->SendMessage(WM_COMMAND,ID_DATASYNC);
		CFile theFile;
		if(theFile.Open(g_WSXHStr+"LData\\cache\\"+g_strDataSyncFile,CFile::modeRead|CFile::shareDenyNone))
		{
			int nLen=theFile.GetLength();
			if(nLen>0)
			{
				char *pBuf = new char[nLen+1];
				theFile.Read(pBuf,nLen);
				pBuf[nLen]=0;
				tmpContent=pBuf;
				delete pBuf;
			}
			theFile.Close();
		}
	}
	if(tmpContent.Left(4)=="http" || tmpContent.Left(4)=="HTTP")
		ShowContent(tmpContent,TRUE);
	else
		ShowContent(tmpContent,FALSE);
	
	*pResult = 0;
}

void ShowUrgentDlg(int flag,int whichurgent)	//flag URGENT_INIT:进入程序时报的 URGENT_DURING:中途报的
{
	if(flag==URGENT_INIT || whichurgent==URGENT_BG)
	{
		if(g_GH.bCheckUrgent && g_GH.lCheckUrgentDate==g_iToday)
			return;
	}
	CFile	urgentFile;
	CString tmpFileName;
	if(whichurgent==URGENT_USER) //用户管理服务器的通告
		tmpFileName.Format("%stmp\\user_urgent.dat",g_WSXHStr);
	else if(whichurgent==URGENT_BG)
		tmpFileName.Format("%stmp\\bg_urgent.dat",g_WSXHStr);
	else if(whichurgent==URGENT_TDXFREE)
		tmpFileName.Format("%shq_cache\\tdx_urgent.dat",g_WSXHStr);
	else
		tmpFileName.Format("%stmp\\urgent.dat",g_WSXHStr);
	if(urgentFile.Open(tmpFileName,CFile::modeRead|CFile::shareDenyNone))
	{
		if(urgentFile.GetLength() == sizeof(struct Cfg_Struct))
		{
			struct Cfg_Struct CfgStruct;
			urgentFile.Read(&CfgStruct,sizeof(struct Cfg_Struct));
			CfgStruct.NotifyStr[90] = 0;
			CfgStruct.UrgentSource[30] = 0;
			CfgStruct.UrgentContent[2048] = 0;
			AllTrim(CfgStruct.UrgentContent);	//将内容紧缩
			CTime time = CTime::GetCurrentTime();
			long lNowTime = time.GetYear()*10000+time.GetMonth()*100+time.GetDay();
			if(lNowTime <= CfgStruct.EndTime && strlen(CfgStruct.UrgentContent) > 0)
			{
				if(flag==URGENT_INIT)
					g_GH.lCheckUrgentDate=g_iToday;
				try
				{
					CUrgentDlg dlg(g_pMainWin);
					if(whichurgent==URGENT_USER || whichurgent==URGENT_BG || whichurgent==URGENT_TDXFREE)
						dlg.m_bHQHostUrgent = FALSE;
					else
						dlg.m_bHQHostUrgent = TRUE;
					dlg.m_strTitle = _F(CfgStruct.UrgentSource);
					dlg.m_strContent = _F(CfgStruct.UrgentContent);
					dlg.lEndDate = CfgStruct.EndTime;
					if(flag==URGENT_DURING)
					{
						dlg.m_bEnterPop = FALSE;
						TDXBeep(0);
					}
					dlg.DoModal();
				}
				catch(...)
				{
				}
			}
		}
		urgentFile.Close();
	}
}

