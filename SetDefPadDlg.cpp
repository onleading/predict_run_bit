// SetDefPadDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "SetDefPadDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetDefPadDlg dialog


CSetDefPadDlg::CSetDefPadDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetDefPadDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetDefPadDlg)
	m_bDefCusPad = g_DefCusPadName[0]!='\0';
	//}}AFX_DATA_INIT
}


void CSetDefPadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetDefPadDlg)
	DDX_Control(pDX, IDC_ALLCUSPADLIST, m_PadList);
	DDX_Check(pDX, IDC_CHECKCUSPAD, m_bDefCusPad);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetDefPadDlg, CDialog)
	//{{AFX_MSG_MAP(CSetDefPadDlg)
	ON_BN_CLICKED(IDC_CHECKCUSPAD, OnCheckcuspad)
	ON_NOTIFY(NM_DBLCLK, IDC_ALLCUSPADLIST, OnDblclkAllcuspadlist)
	ON_NOTIFY(NM_CLICK, IDC_ALLCUSPADLIST, OnDblclkAllcuspadlist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetDefPadDlg message handlers

char *CusPadStr[3] = {"版面简称","中文名称","快捷键"};
int   CusPadWidth[3] = {80,100,48};

BOOL CSetDefPadDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ImageList.Create(16,16,ILC_COLOR8|ILC_MASK,1,0);
	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_GP));
	m_ImageList.Add(hIcon);
	HICON hIconSpec = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_GP_SELECTED));
	m_ImageList.Add(hIconSpec);
	m_PadList.SetImageList(&m_ImageList,LVSIL_SMALL);
	m_PadList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	
	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	for(int i = 0; i < 3;i++)
	{
		lvColumn.pszText = _F(CusPadStr[i]);
		lvColumn.cx = CusPadWidth[i];
		lvColumn.iSubItem = i;
		m_PadList.InsertColumn (i,&lvColumn);
	}

	FillPadList();
	m_PadList.EnableWindow(m_bDefCusPad);
	
	return TRUE;
}

void CSetDefPadDlg::FillPadList()
{	
	m_PadList.DeleteAllItems();
	
	LV_ITEM lvItem;
	
	DynaCmdTable *pCmdInfo = NULL;
	BOOL	bSetSel = FALSE;
	for(int i=0;i < g_vDynaCmdTab.size();i++)
	{
		lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
		lvItem.state = 0;
		lvItem.stateMask = 0;
		lvItem.iSubItem = 0;

		lvItem.iItem = i;
		lvItem.iImage = 0;
		pCmdInfo = &g_vDynaCmdTab[i];
		if(strcmp(pCmdInfo->ChName, g_DefCusPadName)==0)
		{
			lvItem.iImage = 1;
			lvItem.state |= LVIS_SELECTED;
			bSetSel = TRUE;
		}
		lvItem.pszText = pCmdInfo?pCmdInfo->EnName:"-";
		m_PadList.InsertItem(&lvItem);
		m_PadList.SetItemText(i,1,pCmdInfo?pCmdInfo->ChName:"-");
		char nKey[16];
		Num2KeyString(nKey, pCmdInfo?pCmdInfo->nCmdNum:0); CString key = "."+(CString)nKey;
		m_PadList.SetItemText(i,2,pCmdInfo?key:"-");
		m_PadList.SetItemData(i,pCmdInfo?(DWORD)pCmdInfo:NULL);
	}
	if(!bSetSel) m_PadList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
}

void CSetDefPadDlg::OnCheckcuspad()
{
	UpdateData(TRUE);
	FillPadList();
	m_PadList.EnableWindow(m_bDefCusPad);
}

void CSetDefPadDlg::OnOK()
{
	if(!m_bDefCusPad)
		memset(&g_DefCusPadName, 0, 64);
	else
	{
		POSITION pos = m_PadList.GetFirstSelectedItemPosition ();
		if(pos)
		{
			int nItem = m_PadList.GetNextSelectedItem (pos);
			DynaCmdTable *pCmdInfo = (DynaCmdTable*)m_PadList.GetItemData(nItem);
			if(pCmdInfo == NULL) return;
			strcpy(g_DefCusPadName ,pCmdInfo->ChName);
		}
	}
	if(m_bDefCusPad)
		TDX_MessageBox(m_hWnd,"提示:你设置的初始版面下次系统进入后才能生效!",MB_OK|MB_ICONINFORMATION);

	CDialog::OnOK();
}

void CSetDefPadDlg::OnDblclkAllcuspadlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	POSITION pos = m_PadList.GetFirstSelectedItemPosition ();
	if(pos)
	{
		int nItem = m_PadList.GetNextSelectedItem (pos);
		DynaCmdTable *pCmdInfo = (DynaCmdTable*)m_PadList.GetItemData(nItem);
		if(pCmdInfo == NULL) return;
		strcpy(g_DefCusPadName ,pCmdInfo->ChName);
	}
	FillPadList();
	
	*pResult = 0;
}
