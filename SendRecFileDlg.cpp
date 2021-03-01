// SendRecFileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "SendRecFileDlg.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSendRecFileDlg dialog


CSendRecFileDlg::CSendRecFileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSendRecFileDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSendRecFileDlg)
	//}}AFX_DATA_INIT
	m_nFlag = 0;
	m_ItemNum = 0;
	memset(m_Item,0,sizeof(struct SendRec_Item)*MAX_SENDRECV_ITEM);
	m_strPassword[0]=0;
}


void CSendRecFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendRecFileDlg)
	DDX_Control(pDX, IDC_PROGRESS_TEXT, m_static);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
	DDX_Control(pDX, IDC_FILE_LIST, m_listfile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSendRecFileDlg, CDialog)
	//{{AFX_MSG_MAP(CSendRecFileDlg)
	ON_WM_PAINT()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FILE_LIST, OnItemchangedFileList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSendRecFileDlg message handlers

BOOL CSendRecFileDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_listfile.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);

	strcpy(m_strPassword,g_strPassword);
	strcpy(m_strLoginName,g_strLoginName);
#ifdef OEM_NEWJY
	if(g_GC.bHasLevel2Engine)
	{
		strcpy(m_strLoginName,g_L2UserName);
		strcpy(m_strPassword,g_L2Password);
		if(g_nL2Type==0)
			m_strPassword[0]=0;
	}
#endif				

	if(1 == m_nFlag) 
	{
		SetWindowText(_F("下载个性数据"));
		GetDlgItem(IDOK)->SetWindowText(_F("下载"));
		m_static.SetWindowText(_F("准备下载"));
	}
	else 
	{
		SetWindowText(_F("上传个性数据"));
		GetDlgItem(IDOK)->SetWindowText(_F("上传"));
		m_static.SetWindowText(_F("准备上传"));
	}	
	//
	m_listfile.InsertColumn(0,_F("数据项"),LVCFMT_CENTER,120);
	m_listfile.InsertColumn(1,_F("文件大小(KB)"),LVCFMT_CENTER,90);
	m_listfile.InsertColumn(2,_F("状态"),LVCFMT_CENTER,70);
	
	if(1 == m_nFlag) 
	{
		GetDownList();
	}
	else 
	{
		//确定上传的栏目
		strcpy(m_Item[0].ItemName,"自选股");
		strcpy(m_Item[0].ItemPath,"selfbk\\self.blk");
		m_Item[0].ItemFileSize = GetLocalFileSize(HomePath+"selfbk\\self.blk");
		strcpy(m_Item[1].ItemName,"标记数据");
		strcpy(m_Item[1].ItemPath,"mark.dat");
		m_Item[1].ItemFileSize = GetLocalFileSize(g_WSXHStr+"mark.dat");
		m_ItemNum = 2;
	}	
	for(int i=0;i<m_ItemNum;i++)
	{
		m_listfile.InsertItem(i,_F(m_Item[i].ItemName));
		CString strtmp;
		strtmp.Format("%.2f",m_Item[i].ItemFileSize/1024.0);
		m_listfile.SetItemText(i,1,strtmp);
		m_listfile.SetItemText(i,2,"");
		m_listfile.SetItemData(i,i);
	}
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	return TRUE;
}

void CSendRecFileDlg::GetDownList()
{
	m_ItemNum = 0;
	VUserComm * pUser = NewUserCommModule();
	if(pUser)
	{
		BOOL bUserHostEverConnect = TRUE;
		BOOL bJRSConnected = FALSE;
		char MngRight = 0;
		char UserType = 0;
		char UserHostQSID[11] = {0};
		char strUserType[9] = {0};
		char strUserCharacter[9] = {0};
		char strUserLimits[20] = {0};
		pUser->InitUserComm(&bUserHostEverConnect,&bJRSConnected,&MngRight,&UserType,UserHostQSID,strUserType,strUserCharacter,strUserLimits,g_OEM.bEmbedWTVersion);
		pUser->SetServerProperties(&g_ProxyInfo, g_UserHostIP,g_nUserHostPort,g_BakUserHostIP,g_nBakUserHostPort,g_BakUserHostIP2,g_nBakUserHostPort2);
		pUser->SetProductInfo(g_GC.productnbbs,g_GC.bIsSHLevel2,g_GC.bIsSZLevel2);		
		m_static.SetWindowText(_F("连接服务器..."));
		if(pUser->Connect(g_UserHostIP,g_nUserHostPort))
		{
			char lpszfilelist[MAX_LISTBUFLEN] = {0};
			if(pUser->GetFileList(m_strLoginName,m_strPassword,lpszfilelist)) 
			{
				CString strlist = lpszfilelist+sizeof(short);
				CString strtmp;
				int nFind = strlist.Find("$#$");
				int ntmp0,ntmp1;
				while(nFind >= 0)
				{				
					ntmp0 = strlist.Find("$$");
					if(ntmp0 >= 0)
					{
						strncpy(m_Item[m_ItemNum].ItemName,strlist.Left(ntmp0),49);
						ntmp1 = strlist.Find("$$",ntmp0+2);
						if(ntmp1>=0)
						{
							strtmp = strlist.Mid(ntmp0+2,ntmp1-ntmp0-2);
							strncpy(m_Item[m_ItemNum].ItemPath,strtmp,99);
							strtmp = strlist.Mid(ntmp1+2,nFind-ntmp1-2);
							m_Item[m_ItemNum].ItemFileSize = atol(strtmp);
							m_ItemNum++;
							if(m_ItemNum>=MAX_SENDRECV_ITEM)
								break;
						}
					}
					strlist = strlist.Mid(nFind+3);
					nFind = strlist.Find("$#$");
				}
			}
		}
		m_static.SetWindowText(_F("就绪"));
		DelUserCommModule(pUser);
	}
}

void CSendRecFileDlg::OnOK() 
{
	UpdateData(TRUE);

	if(1 == m_nFlag)
	{
		if(TDX_MessageBox(m_hWnd,"从服务器下载这些数据后，将会覆盖本地的自选股和标识数据，\r\n\r\n确定从服务器端下载吗?",MB_ICONQUESTION|MB_OKCANCEL|MB_DEFBUTTON2)==IDCANCEL)
			return;
	}
	
	short nCount = m_listfile.GetItemCount();

	CString str,itemname,filepath,filename;
	int nTotalLen,nlen,nSendLen,iNo;
	VUserComm * pUser = NewUserCommModule();
	if(!pUser)
	{
		TDX_MessageBox(m_hWnd,"模块执行失败!",MB_OK|MB_ICONERROR);
		return;
	}
	BOOL bUserHostEverConnect = TRUE;
	BOOL bJRSConnected = FALSE;
	char MngRight = 0;
	char UserType = 0;
	char UserHostQSID[11] = {0};
	char strUserType[9] = {0};
	char strUserCharacter[9] = {0};
	char strUserLimits[20] = {0};
	pUser->InitUserComm(&bUserHostEverConnect,&bJRSConnected,&MngRight,&UserType,UserHostQSID,strUserType,strUserCharacter,strUserLimits,g_OEM.bEmbedWTVersion);
	pUser->SetServerProperties(&g_ProxyInfo, g_UserHostIP,g_nUserHostPort,g_BakUserHostIP,g_nBakUserHostPort,g_BakUserHostIP2,g_nBakUserHostPort2);
	pUser->SetProductInfo(g_GC.productnbbs,g_GC.bIsSHLevel2,g_GC.bIsSZLevel2);

	m_static.SetWindowText(_F("连接服务器..."));
	if(!pUser->Connect(g_UserHostIP,g_nUserHostPort))
	{
		m_static.SetWindowText(_F(pUser->GetErrorStr()));
		DelUserCommModule(pUser);
		return;
	}
	if(0 == m_nFlag)
	{		
		for(short i=0;i<nCount;i++)
		{
			if(m_listfile.GetCheck(i))
			{
				m_listfile.SetItemText(i,2,_F("上传中.."));

				iNo = m_listfile.GetItemData(i);
				itemname = m_Item[iNo].ItemName;
				filename = m_Item[iNo].ItemPath;
				filepath = g_WSXHStr+filename;

				nTotalLen = m_Item[iNo].ItemFileSize;

				if(nTotalLen<1 || nTotalLen>1024*1024)
				{
					str.Format("[%s]文件大小不合法，不能上传!",itemname);
					TDX_MessageBox(m_hWnd,str,MB_OK|MB_ICONINFORMATION);
					m_static.SetWindowText(_F(str));
					m_listfile.SetItemText(i,2,_F("不能上传"));
					continue;
				}
				m_progress.SetRange(0,99);
				m_progress.SetPos(0);
				nlen = pUser->SendFile(m_strLoginName,m_strPassword,itemname,filepath,filename);
				if(nlen <= 0)
				{
					m_static.SetWindowText(_F("上传失败"));
					m_listfile.SetItemText(i,2,_F("上传失败"));
					pUser->Disconnect();
					DelUserCommModule(pUser);
					return;
				}
				nSendLen = nlen;
				while (nSendLen < nTotalLen)
				{
					str.Format("%s (%d/%d)",itemname,nSendLen,nTotalLen);
					m_static.SetWindowText(_F(str));
					m_progress.SetPos((nSendLen-1)*100/nTotalLen);
					nlen = pUser->SendFile(m_strLoginName,itemname,filename,nSendLen);
					if(nlen <= 0)
					{
						m_static.SetWindowText(_F("上传失败"));
						m_listfile.SetItemText(i,2,_F("上传失败"));
						m_progress.SetPos(0);
						pUser->Disconnect();
						DelUserCommModule(pUser);
						return;
					}
					nSendLen += nlen;
				}
				str.Format("[%s]上传完成",itemname);
				m_listfile.SetItemText(i,2,_F("上传完成"));
				m_static.SetWindowText(_F(str));
				m_progress.SetPos(0);	
			}			
		}
	}
	else if(1 == m_nFlag)
	{
		for(short i=0;i<nCount;i++)
		{
			if(m_listfile.GetCheck(i))
			{
				m_listfile.SetItemText(i,2,_F("下载中.."));
				iNo = m_listfile.GetItemData(i);
				itemname = m_Item[iNo].ItemName;
				filename = m_Item[iNo].ItemPath;
				filepath = g_WSXHStr+filename;

				nTotalLen = m_Item[iNo].ItemFileSize;

				m_progress.SetRange(0,99);
				m_progress.SetPos(0);

				nlen = pUser->GetFile(m_strLoginName,m_strPassword,filename,filepath);
				if(nlen <= 0)
				{
					m_static.SetWindowText(_F("下载失败"));
					m_listfile.SetItemText(i,2,_F("下载失败"));
					m_progress.SetPos(0);
					pUser->Disconnect();
					DelUserCommModule(pUser);
					return;
				}
				nSendLen = nlen;
				while (nSendLen < nTotalLen)
				{
					str.Format("%s(%d/%d)",itemname,nSendLen,nTotalLen);
					m_static.SetWindowText(_F(str));
					m_progress.SetPos((nSendLen-1)*100/nTotalLen);
					nlen = pUser->GetFile(m_strLoginName,m_strPassword,filename,filepath,nSendLen);
					if(nlen <= 0)
					{
						m_static.SetWindowText("下载失败");
						m_listfile.SetItemText(i,2,_F("下载失败"));
						m_progress.SetPos(0);
						pUser->Disconnect();
						DelUserCommModule(pUser);
						return;
					}
					nSendLen += nlen;
				}
				str.Format("[%s]下载完成",itemname);
				m_listfile.SetItemText(i,2,_F("下载完成"));
				m_static.SetWindowText(_F(str));
				m_progress.SetPos(0);	
				//
				if(stricmp(itemname,"自选股")==0)
				{
					LoadColorfulBlockGP();
					g_bBlockModified = TRUE;	
				}
				else if(stricmp(itemname,"标记数据")==0)
					g_pStockIO->LoadIndentInfo();
			}
		}
	}
	pUser->Disconnect();
	DelUserCommModule(pUser);
}

#define PAINT_TITLE(_TITLE,_TITLEINFO)	\
	CPaintDC dc(this);\
	CRect totalrc,rc,rc2;\
	GetClientRect(&totalrc);\
	GetDlgItem(IDC_PIC_STATIC)->GetClientRect(&rc);\
	rc2=rc;\
	rc2.left=0;\
	rc2.right=totalrc.right;\
	dc.FillSolidRect(rc2,RGB(255,255,255));\
	CPen pen1,*poldpen=NULL;\
	pen1.CreatePen(PS_SOLID,1,RGB(128,128,128));\
	poldpen=dc.SelectObject(&pen1);\
	dc.MoveTo(rc2.left,rc2.bottom);\
	dc.LineTo(rc2.right,rc2.bottom);\
	dc.SetBkMode(TRANSPARENT);\
	CFont font1,font2,*poldfont=NULL;\
	font1.CreateFont(12, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET,OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,DEFAULT_PITCH|FF_DONTCARE, "宋体");\
	font2.CreateFont(12, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,DEFAULT_PITCH|FF_DONTCARE, "宋体");\
	poldfont=dc.SelectObject(&font1);\
	dc.SetTextColor(RGB(0,0,0));\
	dc.TextOut(8,6,_TITLE);\
	poldfont=dc.SelectObject(&font2);\
	dc.TextOut(20,24,_TITLEINFO);\
	dc.SelectObject(poldfont);\
	dc.SelectObject(poldpen);\
	font1.DeleteObject();\
	font2.DeleteObject();

void CSendRecFileDlg::OnPaint() 
{
	if(0 == m_nFlag)
	{	
		PAINT_TITLE(_F("上传数据"),_F("上传用户个性化数据文件"))
	}
	else
	{
		PAINT_TITLE(_F("下载数据"),_F("下载用户个性化数据文件"))
	}
}


void CSendRecFileDlg::OnItemchangedFileList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int nCount = m_listfile.GetItemCount();
	for(int i=0;i<nCount;i++)
	{
		if(m_listfile.GetCheck(i))
		{
			GetDlgItem(IDOK)->EnableWindow(TRUE);
			return;
		}
	}
	GetDlgItem(IDOK)->EnableWindow(FALSE);

	*pResult = 0;
}
