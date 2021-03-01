// IniListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IniListDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIniListDlg dialog


CIniListDlg::CIniListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIniListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIniListDlg)
	//}}AFX_DATA_INIT
}


void CIniListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIniListDlg)
	DDX_Control(pDX, IDC_LIST_GNCFG, m_ListINI);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIniListDlg, CDialog)
	//{{AFX_MSG_MAP(CIniListDlg)
	ON_BN_CLICKED(IDC_OPENINI, OnOpenini)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_GNCFG, OnDblclkListGncfg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
WORD CIniListDlg::GetIniFileInfo( CString strFilePath )
{
	char filename[256] = {0};
	struct _finddata_t finddata;
	strcpy( filename, strFilePath );
	strcat( filename,"*.ini" );
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iSubItem = 0;
	lvItem.iImage = 0;
	int	iCount = 0;
	m_ListINI.DeleteAllItems();

	CString strFileName;
	long hFind = ::_findfirst( filename, &finddata );
	if( hFind==-1 )  return iCount;
	while( TRUE )
	{
		strFileName = finddata.name;
		strFileName.TrimLeft();strFileName.TrimRight();
		strFileName.MakeLower();

		char iniPath[MAX_PATH]={0};
		sprintf(iniPath, "%s%s", strFilePath, strFileName);
		char strGNTest[255]={0};
		if(m_nType==0)			//表
			GetPrivateProfileString("REPORT", "LISTTITLE", "", strGNTest, sizeof(strGNTest), iniPath);
		else if(m_nStyle==0)	//静态图
			GetPrivateProfileString("StaticAct", "X_OutName", "", strGNTest, sizeof(strGNTest), iniPath);
		else if(m_nStyle==1)	//动态图
			GetPrivateProfileString("DynaAct", "DynaChart_Fields01", "", strGNTest, sizeof(strGNTest), iniPath);
		else if(m_nStyle==2)	//星空图
			GetPrivateProfileString("StarAct", "StarChart_XOutName01", "", strGNTest, sizeof(strGNTest), iniPath);
		if(strGNTest[0]==0)
		{
			if( ::_findnext(hFind,&finddata)==-1 )  break;
			continue;
		}

		lvItem.iItem = iCount;
		lvItem.pszText = finddata.name;
		m_ListINI.InsertItem( &lvItem );
		
		m_ListINI.SetItemText( iCount, 1,  strGNTest);
		iCount++;
		if( ::_findnext(hFind,&finddata)==-1 )  break;
	}
	::_findclose( hFind );
	return iCount;
}
/////////////////////////////////////////////////////////////////////////////
// CIniListDlg message handlers

BOOL CIniListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	const nColNum=2;
	char *ColStr[nColNum] = {"文件名","功能名称"};
	int ColWidth[nColNum] = {120	,180};
	CString strTmp;
	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	for( int i=0; i<nColNum; i++ )
	{
		lvColumn.pszText = ColStr[i];
		lvColumn.cx = ColWidth[i];
		lvColumn.iSubItem = i;
		m_ListINI.InsertColumn (i,&lvColumn);
	}
	m_ListINI.SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_FLATSB | LVS_EX_GRIDLINES);
				

	UpdateData(FALSE);
	GetIniFileInfo( HomePath+"cwcfg\\" );
	
	return TRUE;
}

void CIniListDlg::OnOpenini() 
{
	int iCurSel = m_ListINI.GetNextItem( -1, LVNI_SELECTED );
	if( iCurSel<0 )
	{
		TDX_MessageBox(m_hWnd,"请先选择要修改的配置文件!",MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	CString strFile = m_ListINI.GetItemText( iCurSel, 0 );
	CString strTemp = strFile;
	strTemp.MakeLower();

	m_strOutIniPath = strFile;

	CDialog::OnOK();
}

void CIniListDlg::OnDblclkListGncfg(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnOpenini();
	
	*pResult = 0;
}
