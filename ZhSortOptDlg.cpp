 // ZhSortOptDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "ZhSortOptDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ZhSortOptDlg dialog


ZhSortOptDlg::ZhSortOptDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ZhSortOptDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(ZhSortOptDlg)
	m_nShowNum = 1;
	//}}AFX_DATA_INIT
	m_aShowSort = NULL;
}


void ZhSortOptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ZhSortOptDlg)
	DDX_Control(pDX, IDC_SHOWSORTLIST, m_showSortList);
	DDX_Control(pDX, IDC_ALLSORTLIST, m_allSortList);
	DDX_Radio(pDX, IDC_RADIO9, m_nShowNum);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ZhSortOptDlg, CDialog)
	//{{AFX_MSG_MAP(ZhSortOptDlg)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
	ON_BN_CLICKED(IDC_RADIO4, OnRadio4)
	ON_BN_CLICKED(IDC_RADIO9, OnRadio9)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ZhSortOptDlg message handlers

extern void MakeZHSortTitle(char (*TypeStr)[25]);

BOOL ZhSortOptDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	MakeZHSortTitle(TypeStr);

	if(!m_aShowSort) return FALSE;

	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.pszText = "DXRJ";
	lvColumn.cx = 120;
	lvColumn.iSubItem = 0;
	m_allSortList.InsertColumn (0,&lvColumn);	//插入一列
	m_showSortList.InsertColumn (0,&lvColumn);	//插入一列
	m_allSortList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_showSortList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	
	m_ImageList.Create(16,16,ILC_COLOR8|ILC_MASK,1,1);
	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_INDEX));
	m_ImageList.Add(hIcon);
	m_allSortList.SetImageList(&m_ImageList,LVSIL_SMALL);
	m_showSortList.SetImageList(&m_ImageList,LVSIL_SMALL);
	
	Fill2List();
	
	UpdateData(FALSE);
	return TRUE;
}

void ZhSortOptDlg::Fill2List()
{
	LockWindowUpdate();
	m_allSortList.DeleteAllItems();
	m_showSortList.DeleteAllItems();

	int nShowSort;
	if(m_nShowNum == 0) nShowSort = 9;
	else nShowSort = 4;

	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT|LVIF_STATE|LVIF_IMAGE;
	lvItem.state = 0;
	lvItem.iImage = 0;
	lvItem.stateMask = 0;
	lvItem.iSubItem = 0;
	for(int i = 0;i < 9;i++)
	{
		lvItem.iItem = i;
		lvItem.pszText = _F(TypeStr[i]);
		m_allSortList.InsertItem (&lvItem);
	}
	for(int j = 0;j < nShowSort;j++)
	{
		lvItem.iItem = j;
		lvItem.pszText = _F(TypeStr[m_aShowSort[j]]);
		m_showSortList.InsertItem (&lvItem);
	}
	m_allSortList.SetItemState(0,LVIS_FOCUSED,LVIS_FOCUSED);
	m_allSortList.SetItemState(0,LVIS_SELECTED,LVIS_SELECTED);
	m_showSortList.SetItemState(0,LVIS_FOCUSED,LVIS_FOCUSED);
	m_showSortList.SetItemState(0,LVIS_SELECTED,LVIS_SELECTED);

	UpdateData(FALSE);

	UnlockWindowUpdate();
}

void ZhSortOptDlg::OnAddButton() 
{
	int tmp;
	POSITION pos = m_allSortList.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		int iNo = m_allSortList.GetNextSelectedItem(pos);
		POSITION pos2 = m_showSortList.GetFirstSelectedItemPosition();	
		if(pos2!=NULL)
		{
			int iNo2 = m_showSortList.GetNextSelectedItem(pos2);
			char *pStr = TypeStr[iNo];
			if(!pStr) return;
			for(int i=0;i<9;i++)
				if(m_aShowSort[i]==iNo)	break;
			tmp = m_aShowSort[iNo2];
			m_aShowSort[iNo2] = m_aShowSort[i];
			m_aShowSort[i] = tmp;
			Fill2List();
		}
	}
}

void ZhSortOptDlg::OnRadio4() 
{
	UpdateData(TRUE);
	Fill2List();
}

void ZhSortOptDlg::OnRadio9() 
{
	UpdateData(TRUE);
	Fill2List();
}
