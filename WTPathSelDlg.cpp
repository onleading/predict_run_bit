// WTPathSelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "WTPathSelDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWTPathSelDlg dialog


CWTPathSelDlg::CWTPathSelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWTPathSelDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWTPathSelDlg)
	//}}AFX_DATA_INIT
	m_nSelExeIndex = 0;
	m_bFromFileCfg = TRUE;

	m_strWTName[0][0]=0;
	m_strWTName[1][0]=0;
	m_strWTName[2][0]=0;
	m_nWTNum=0;
}


void CWTPathSelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWTPathSelDlg)
	DDX_Control(pDX, IDC_WTPATHLIST, m_ExePathList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWTPathSelDlg, CDialog)
	//{{AFX_MSG_MAP(CWTPathSelDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_WTPATHLIST, OnDblclkWtpathlist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWTPathSelDlg message handlers
char	*title[] = {"程序描述",NULL};
int		nWidth[] = {240};
BOOL CWTPathSelDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();	
	m_ExePathList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	
	m_ImageList.Create(16,16,ILC_COLOR8|ILC_MASK,1,1);
	m_ExePathList.SetImageList(&m_ImageList,LVSIL_SMALL);

	LVCOLUMN	lvc;	
	lvc.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvc.fmt = LVCFMT_LEFT;
	for (int i = 0; title[i] != NULL; i++)
	{
		lvc.pszText = title[i];
		lvc.cx = nWidth[i];
		m_ExePathList.InsertColumn(i,&lvc);
	}
	HICON hNoneIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_PLUGIN));
	if(m_bFromFileCfg)
	{
		SHFILEINFO shFileInfo;
		int nExeIndex = 0; char wtStr[64]={0}, path[256]={0}, name[128]={0},caption[128]={0};
		int nGetRes = GetPrivateProfileString("EXTERN","WTPATH","*****",path,255,g_WSXHStr+g_strYxhjCfg);
		int nWtStr = GetPrivateProfileString("EXTERN","WTNAME","",name,255,g_WSXHStr+g_strYxhjCfg);
		while(nGetRes>0)
		{
			//得到文件的图标
			SHGetFileInfo(path, 0, &shFileInfo, sizeof(SHFILEINFO), SHGFI_ICON|SHGFI_SMALLICON);
			if(shFileInfo.hIcon)
				m_ImageList.Add(shFileInfo.hIcon);
			else
				m_ImageList.Add(hNoneIcon);
			LV_ITEM lvItem;
			lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
			lvItem.state = 0;
			lvItem.stateMask = 0;
			lvItem.iImage = nExeIndex;
			lvItem.iSubItem = 0;
			lvItem.iItem = nExeIndex;
			if(m_nSelExeIndex==nExeIndex)
				lvItem.state |= LVIS_SELECTED;
			sprintf(caption," %s",nWtStr>0?name:"主委托程序");
			lvItem.pszText = caption;
			m_ExePathList.InsertItem(&lvItem);
			m_ExePathList.SetItemData(nExeIndex,nExeIndex++);

			sprintf(wtStr,"WTPATH%d", nExeIndex);
			nGetRes = GetPrivateProfileString("EXTERN",wtStr,"",path,255,g_WSXHStr+g_strYxhjCfg);
			sprintf(wtStr,"WTNAME%d", nExeIndex);
			nWtStr = GetPrivateProfileString("EXTERN",wtStr,"",name,255,g_WSXHStr+g_strYxhjCfg);
		}
	}
	else
	{
		m_ImageList.Add(hNoneIcon);
		for(int i=0;i<m_nWTNum;i++)
		{
			LV_ITEM lvItem;
			lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
			lvItem.state = 0;
			lvItem.stateMask = 0;
			lvItem.iImage = 0;
			lvItem.iSubItem = 0;
			lvItem.iItem = i;
			if(m_nSelExeIndex==i)
				lvItem.state |= LVIS_SELECTED;
			lvItem.pszText = m_strWTName[i];
			m_ExePathList.InsertItem(&lvItem);
			m_ExePathList.SetItemData(i,i);
		}
	}
	return TRUE;
}

void CWTPathSelDlg::OnOK() 
{
	POSITION pos = m_ExePathList.GetFirstSelectedItemPosition ();
	if(pos)
	{
		int nItem = m_ExePathList.GetNextSelectedItem (pos);
		m_nSelExeIndex = m_ExePathList.GetItemData(nItem);
	}
	
	CDialog::OnOK();
}

void CWTPathSelDlg::OnDblclkWtpathlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnOK();
	
	*pResult = 0;
}
