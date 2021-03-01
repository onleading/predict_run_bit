// DataSyncDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "DataSyncDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDataSyncDlg dialog


CDataSyncDlg::CDataSyncDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDataSyncDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDataSyncDlg)
	//}}AFX_DATA_INIT
	m_nType = DATASYNC_NULL;
	m_nMainID = g_nWideDataMainID;
	m_strDesc = "";

	data_filelen = 0;
	data_pos = 0;
	FileBuffer = NULL;
	m_strSyncFile[0]=0;
}


void CDataSyncDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDataSyncDlg)
	DDX_Control(pDX, IDC_PROGRESS, m_ctlProgress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDataSyncDlg, CDialog)
	//{{AFX_MSG_MAP(CDataSyncDlg)
	ON_MESSAGE(UM_GETDATA_ACK,OnGetDataAck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDataSyncDlg message handlers

BOOL CDataSyncDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if(m_nType == DATASYNC_TEND)
	{
		m_strDesc = "正在更新趋势导航信息...";
		SetWindowText(_F("更新数据"));
		sprintf(m_strSyncFile,"%s.zip",g_strTendName);
	}
	else if(m_nType == DATASYNC_JJ)
	{
		m_strDesc = "正在更新基金基本资料信息...";
		SetWindowText(_F("更新数据"));
		sprintf(m_strSyncFile,"jjbase.zip");
	}
	else if(m_nType == DATASYNC_FILE)
	{
		m_strDesc = "正在更新复合通告信息...";
		SetWindowText(_F("更新数据"));
		strncpy(m_strSyncFile,m_strDataSyncFile,99);
		m_strSyncFile[99]=0;
	}
	else if(m_nType == DATASYNC_ZIPFILE)
	{
		m_strDesc = "正在更新数据文件...";
		SetWindowText(_F("更新数据"));
		strncpy(m_strSyncFile,m_strDataSyncFile,99);
		m_strSyncFile[99]=0;
	}
	GetDlgItem(IDC_STATIC1)->SetWindowText(_F(m_strDesc));
	
	return TryDataReq();
}

void CDataSyncDlg::OnOK() 
{
}

void CDataSyncDlg::OnCancel()
{
	TDEL(FileBuffer);
	g_nWideDataMainID = m_nMainID; //退出前，改变g_nWideDataMainID
	CDialog::OnCancel();
}

void CDataSyncDlg::OnGetDataAck(WPARAM wParam,LPARAM lParam)
{
	if(m_nMainID!=g_AnsHeader.MainID) return;

	if(g_AnsHeader.AssisID==SYNCDATALEN_ASSISID)
	{
		char usemdstr,mdstr[33],local_mdstr[33];
		data_filelen = g_pGetData->GetFileLenACK(&usemdstr,mdstr);
		data_pos = 0;
		if(data_filelen > 0)
		{
			BOOL bNeedGet=TRUE;
			FILE *fpt=fopen(g_WSXHStr+"LData\\cache\\"+m_strSyncFile,"rb");
			if(fpt)
			{
				long local_size = on_filelength(fpt);
				if(local_size==data_filelen)
				{
					if(usemdstr==0 || mdstr[0]==0)
						bNeedGet=FALSE;
					else
					{
						BYTE *buf = new BYTE[data_filelen];
						fread(buf,data_filelen,1,fpt);
						MD5_Buffer(buf,data_filelen,local_mdstr);
						TDELA(buf);
						if(strcmp(mdstr,local_mdstr)==0)
							bNeedGet=FALSE;
					}
				}
				fclose(fpt);
				if(!bNeedGet) 
				{
					if(m_nType==DATASYNC_TEND)
						g_bFYDataAlwaysSync=TRUE;
					else if(m_nType==DATASYNC_JJ)
						g_bFundDataAlwaysSync=TRUE;
					PostMessage(WM_COMMAND,IDCANCEL);
					return;
				}
			}
			if(bNeedGet)
			{
				FileBuffer = new char[data_filelen+30000];
				g_nMainID = MainIDPlus(m_nMainID,SYNCDATA_WIN);
				g_nAssisID = SYNCDATA_ASSISID;
				g_pGetData->GetPathFile(m_strSyncFile,data_pos);
				m_ctlProgress.SetRange32(0,data_filelen);
				m_ctlProgress.SetPos(0);
				return;
			}
		}
		if(m_nType==DATASYNC_TEND)
			g_bFYDataAlwaysSync=TRUE;
		PostMessage(WM_COMMAND,IDCANCEL);
		return;
	}
	else if(g_AnsHeader.AssisID==SYNCDATA_ASSISID)
	{
		long retlen = g_pGetData->GetPathFileACK(FileBuffer+data_pos);
		data_pos+=retlen;
		if(retlen == 30000)
		{
			CString tempStr;
			tempStr.Format("更新数据%3d%%",data_pos*100/data_filelen);
			GetDlgItem(IDC_STATIC1)->SetWindowText(_F(tempStr));
			g_nMainID = MainIDPlus(m_nMainID,SYNCDATA_WIN);
			g_nAssisID = SYNCDATA_ASSISID;
			g_pGetData->GetPathFile(m_strSyncFile,data_pos);
			m_ctlProgress.SetPos(data_pos);
		}
		else
		{
			CFile blockfile;
			if(blockfile.Open(g_WSXHStr+"LData\\cache\\"+m_strSyncFile,CFile::modeCreate|CFile::modeWrite))
			{
				blockfile.Write(FileBuffer,data_filelen);
				blockfile.Close();
				if(m_nType!=DATASYNC_FILE)
				{
					GetDlgItem(IDC_STATIC1)->SetWindowText(_F("正在处理信息..."));
					BOOL bHasEmbwtFile=FALSE,bHasNewHostFile=FALSE;
					UnZipSomeFile(g_WSXHStr+"LData\\cache\\"+m_strSyncFile,g_WSXHStr+"LData\\cache\\",bHasEmbwtFile,bHasNewHostFile); //解压到本地
					if(m_nType==DATASYNC_TEND)
						g_bFYDataAlwaysSync=TRUE;
					else if(m_nType==DATASYNC_JJ)
						g_bFundDataAlwaysSync=TRUE;
				}
			}
			else
				TDX_MessageBox(m_hWnd,"写盘错误!",MB_ICONEXCLAMATION|MB_OK);
			TDEL(FileBuffer);

			PostMessage(WM_COMMAND,IDCANCEL);
		}			
	}
}
//////////////////////////////////////////////////////////////////////////
BOOL CDataSyncDlg::TryDataReq()
{
	g_nMainID = MainIDPlus(m_nMainID,SYNCDATA_WIN);
	g_nAssisID = SYNCDATALEN_ASSISID;
	g_pGetData->GetFileLen(m_strSyncFile);
	return TRUE;
}
