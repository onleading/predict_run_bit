// FindResultDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "FindResultDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFindResultDlg dialog


CFindResultDlg::CFindResultDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFindResultDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFindResultDlg)
	//}}AFX_DATA_INIT
	m_nStyle = 0;
	m_nFindNum = 0;
	m_nCurSel = -1;
}


void CFindResultDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindResultDlg)
	DDX_Control(pDX, IDC_RESULT_LIST, m_Result_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFindResultDlg, CDialog)
	//{{AFX_MSG_MAP(CFindResultDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_RESULT_LIST, OnDblclkResultList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindResultDlg message handlers

char *	FindColStr[2] =  {"来源","文件名称"};
int		FindColWidth[2] = {120,200};

BOOL CFindResultDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if(m_nStyle==0)	
	{
		m_nCurSel = g_nTxtFindNo;
		m_strFindFileName = "txtfind.dat";
	}
	else			
	{
		m_nCurSel = g_nNewsFindNo;
		m_strFindFileName = "newsfind.dat";
	}
	m_ImageList.Create(16,16,ILC_COLOR8|ILC_MASK,1,1);
	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_FINDED));
	m_ImageList.Add(hIcon);
	m_Result_List.SetImageList(&m_ImageList,LVSIL_SMALL);

	m_Result_List.SetExtendedStyle (LVS_EX_FULLROWSELECT);

	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	for(int i = 0; i < 2;i++)
	{
		lvColumn.pszText = _F(FindColStr[i]);
		lvColumn.cx = FindColWidth[i];
		lvColumn.iSubItem = i;
		m_Result_List.InsertColumn (i,&lvColumn);
	}
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iSubItem = 0;
	lvItem.iImage = 0;

	char tmpLine[255];
	CFile theFile;
	struct tag_FindResult findresult;
	struct tag_FindResult2 findresult2;
	if(theFile.Open(g_WSXHStr+"tmp\\"+m_strFindFileName,CFile::modeRead|CFile::shareDenyNone))
	{
		if(theFile.Read(tmpLine,255) == 255) //正确的头
		{
			GetDlgItem(IDC_INFO_STATIC)->SetWindowText((CString)"\""+tmpLine+_F("\" 检索结果"));
			if(m_nStyle == 0)
			{
				while(theFile.Read(&findresult,sizeof(tag_FindResult))==sizeof(tag_FindResult))
				{
					lvItem.iItem = m_nFindNum;
					if(findresult.which < 0 || findresult.which >= g_TdxCfg.JbmNum)
						findresult.which = 0;
					lvItem.pszText = _F("本地缓存");
					m_Result_List.InsertItem (&lvItem);
					m_Result_List.SetItemText(m_nFindNum,1,findresult.path);
					m_FindResult[m_nFindNum] = findresult;
					m_nFindNum++;
					if(m_nFindNum >= MAX_FINDRESULT) break;
				}
			}
			else
			{
				while(theFile.Read(&findresult2,sizeof(tag_FindResult2))==sizeof(tag_FindResult2))
				{
					lvItem.iItem = m_nFindNum;
					findresult2.whichtype = 0;
					lvItem.pszText = _F("本地缓存");
					m_Result_List.InsertItem (&lvItem);
					m_Result_List.SetItemText(m_nFindNum,1,findresult2.title);
					m_FindResult2[m_nFindNum] = findresult2;
					m_nFindNum++;
					if(m_nFindNum >= MAX_FINDRESULT) break;
				}
			}
		}
		theFile.Close();
	}
	if(m_nFindNum > 0)
	{
		if(m_nCurSel == -1) m_nCurSel = 0;
		m_Result_List.SetItemState(m_nCurSel,LVIS_SELECTED,LVIS_SELECTED);
		m_Result_List.SetItemState(m_nCurSel,LVIS_SELECTED,LVIS_SELECTED);
		m_Result_List.EnsureVisible(m_nCurSel,FALSE);
	}
	return TRUE;
}

void CFindResultDlg::OnOK() 
{
	POSITION pos = m_Result_List.GetFirstSelectedItemPosition();
	if(pos != NULL)
	{
		m_nCurSel = m_Result_List.GetNextSelectedItem(pos);
		if(m_nStyle==0)	g_nTxtFindNo = m_nCurSel;
		else			g_nNewsFindNo = m_nCurSel;
		CDialog::OnOK();
	}		
}

void CFindResultDlg::OnDblclkResultList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnOK();
	*pResult = 0;
}
