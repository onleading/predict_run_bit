// LocalExternMngDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "LocalExternMngDlg.h"
#include "LocalExternSetDlg.h"
#include "KeyGuy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLocalExternMngDlg property page

IMPLEMENT_DYNCREATE(CLocalExternMngDlg, CPropertyPage)

CLocalExternMngDlg::CLocalExternMngDlg() : CPropertyPage(CLocalExternMngDlg::IDD)
{
	//{{AFX_DATA_INIT(CLocalExternMngDlg)
	//}}AFX_DATA_INIT
}

CLocalExternMngDlg::~CLocalExternMngDlg()
{
}

void CLocalExternMngDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLocalExternMngDlg)
	DDX_Control(pDX, IDC_LCINDEXMNG_LIST, m_ExternList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLocalExternMngDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CLocalExternMngDlg)
	ON_BN_CLICKED(IDC_LCEXTERNMNG_ADDBUTTON, OnLcexternmngAddbutton)
	ON_BN_CLICKED(IDC_LCEXTERNMNG_CHGBUTTON, OnLcexternmngChgbutton)
	ON_BN_CLICKED(IDC_LCEXTERNMNG_DELBUTTON, OnLcexternmngDelbutton)
	ON_BN_CLICKED(IDC_VIEWCURHQ, OnViewcurhq)
	ON_BN_CLICKED(IDC_RECALC_BUTTON, OnRecalcButton)
	ON_NOTIFY(NM_DBLCLK, IDC_LCINDEXMNG_LIST, OnDblclkLcindexmngList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLocalExternMngDlg message handlers

void CLocalExternMngDlg::OnOK() 
{
	CPropertyPage::OnOK();
}

void CLocalExternMngDlg::OnCancel() 
{
	CPropertyPage::OnCancel();
}

void CLocalExternMngDlg::OnLcexternmngAddbutton() 
{
	if(g_pLcIdxMng==NULL) return;
	if(g_pLcIdxMng->GetExtNum()>=LOCALINDEX_MAXSPACE)
	{
		TDX_MessageBox(m_hWnd,"外部品种个数超过限制.",MB_OK);
		return;
	}

	UpdateData(TRUE);
	CLocalExternSetDlg dlg;
	if(dlg.DoModal()==IDOK)
	{
		g_pLcIdxMng->AddLcExt(&(dlg.m_LcExtSet));
		m_lLcExtSetNum = g_pLcIdxMng->GetExtSet(m_pLcExtSet);
		if(m_lLcExtSetNum>0) g_pLcIdxMng->ReLoad(m_lLcExtSetNum-1);
	}
	g_pLcIdxMng->SaveAll();
	g_pLcIdxMng->LoadAll();
	g_pStockIO->ReAddLc(SZ);
	m_lLcExtSetNum = g_pLcIdxMng->GetExtSet(m_pLcExtSet);
	FillList();
	m_ExternList.SetItemState(m_ExternList.GetItemCount()-1,LVIS_SELECTED,LVIS_SELECTED);
	UpdateData(FALSE);
}

void CLocalExternMngDlg::OnLcexternmngChgbutton() 
{
	if(g_pLcIdxMng==NULL || m_lLcExtSetNum<=0) return;

	UpdateData(TRUE);
	POSITION pos = m_ExternList.GetFirstSelectedItemPosition();
	if(pos == NULL)
	{
		TDX_MessageBox(m_hWnd,"请选择列表中的品种!",MB_OK|MB_ICONERROR);
		return;
	}
	int nItem = m_ExternList.GetNextSelectedItem(pos);
	if(nItem<0 || nItem>=m_ExternList.GetItemCount())
	{
		TDX_MessageBox(m_hWnd,"请选择列表中的品种!",MB_OK|MB_ICONERROR);
		return;
	}
	DWORD nIndex = m_ExternList.GetItemData(nItem);
	if(nIndex<0 || nIndex>=m_lLcExtSetNum)
	{
		FillList();
		return;
	}
	
	CLocalExternSetDlg dlg;
	dlg.SetExtSet(&(m_pLcExtSet[nIndex]),nIndex);
	if(dlg.DoModal()==IDOK)
	{
		g_pLcIdxMng->ChgLcExt(&(dlg.m_LcExtSet),nIndex);
		if(g_pLcIdxMng->ReLoad(nIndex))
			g_pLcIdxMng->SaveAll();
	}
	else g_pLcIdxMng->SaveAll();
	g_pLcIdxMng->LoadAll();
	g_pStockIO->ReAddLc(SZ);
	m_lLcExtSetNum = g_pLcIdxMng->GetExtSet(m_pLcExtSet);
	FillList();
	m_ExternList.SetItemState(nItem,LVIS_SELECTED,LVIS_SELECTED);
	UpdateData(FALSE);
}

void CLocalExternMngDlg::OnLcexternmngDelbutton() 
{
	if(g_pLcIdxMng==NULL || m_lLcExtSetNum<=0) return;

	UpdateData(TRUE);
	POSITION pos = m_ExternList.GetFirstSelectedItemPosition();
	if(pos == NULL)
	{
		TDX_MessageBox(m_hWnd,"请选择列表中的品种!",MB_OK|MB_ICONERROR);
		return;
	}
	int nItem = m_ExternList.GetNextSelectedItem(pos);
	if(nItem<0 || nItem>=m_ExternList.GetItemCount())
	{
		TDX_MessageBox(m_hWnd,"请选择列表中的品种!",MB_OK|MB_ICONERROR);
		return;
	}
	DWORD nIndex = m_ExternList.GetItemData(nItem);
	if(nIndex<0 || nIndex>=m_lLcExtSetNum)
	{
		FillList();
		return;
	}

	g_pLcIdxMng->DelLcExt(nIndex);
	g_pLcIdxMng->LoadAll();
	g_pStockIO->ReAddLc(SZ);
	m_lLcExtSetNum = g_pLcIdxMng->GetExtSet(m_pLcExtSet);

	FillList();
	if(nItem>=m_ExternList.GetItemCount()) m_ExternList.SetItemState(m_ExternList.GetItemCount()-1,LVIS_SELECTED,LVIS_SELECTED);
	else m_ExternList.SetItemState(nItem,LVIS_SELECTED,LVIS_SELECTED);
	UpdateData(FALSE);
}

void CLocalExternMngDlg::OnRecalcButton() 
{
	if(g_pLcIdxMng==NULL || m_lLcExtSetNum<=0) return;

	UpdateData(TRUE);
	POSITION pos = m_ExternList.GetFirstSelectedItemPosition();
	if(pos == NULL)
	{
		TDX_MessageBox(m_hWnd,"请选择列表中的品种!",MB_OK|MB_ICONERROR);
		return;
	}
	int nItem = m_ExternList.GetNextSelectedItem(pos);
	if(nItem<0 || nItem>=m_ExternList.GetItemCount())
	{
		TDX_MessageBox(m_hWnd,"请选择列表中的品种!",MB_OK|MB_ICONERROR);
		return;
	}
	DWORD nIndex = m_ExternList.GetItemData(nItem);
	if(nIndex<0 || nIndex>=m_lLcExtSetNum)
	{
		FillList();
		return;
	}

	g_pLcIdxMng->ReLoad(nIndex);
}

void CLocalExternMngDlg::OnViewcurhq() 
{
	if(g_pLcIdxMng==NULL) return;

	UpdateData(TRUE);
	POSITION pos = m_ExternList.GetFirstSelectedItemPosition();
	if(pos == NULL)	return;
	int nItem = m_ExternList.GetNextSelectedItem(pos);
	if(nItem<0 || nItem>=m_ExternList.GetItemCount()) return;
	DWORD nIndex = m_ExternList.GetItemData(nItem);
	if(nIndex<0 || nIndex>=m_lLcExtSetNum)
	{
		FillList();
		return;
	}
	
	char tmpGPValue[80];
	tmpGPValue[0] = '0';
	strcpy(tmpGPValue+1,m_pLcExtSet[nIndex].Code);
	CKeyGuy::OnCommonProcess(0,tmpGPValue);
}

BOOL CLocalExternMngDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	char IndexListHeadStr[][10] = {"品种代码","品种名称"};
	int IndexListHeadWid[] = {60,60};

	m_ImageList.Create(1,20,ILC_COLOR8|ILC_MASK,1,1);
	m_ExternList.SetImageList(&m_ImageList,LVSIL_SMALL);
	m_ExternList.SetExtendedStyle(m_ExternList.GetExtendedStyle()|LVS_EX_FULLROWSELECT);

	int i=0;
	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;

	if(g_pLcIdxMng)
		m_lLcExtSetNum = g_pLcIdxMng->GetExtSet(m_pLcExtSet);

	for(i=0;i<2;i++)
	{
		lvColumn.pszText	= _F(IndexListHeadStr[i]);
		lvColumn.cx			= IndexListHeadWid[i];
		lvColumn.iSubItem	= i;
		m_ExternList.InsertColumn (i,&lvColumn);
	}
	FillList();
	
	return TRUE;
}

void CLocalExternMngDlg::FillList()
{
	if(g_pLcIdxMng==NULL) return;
	long i = 0;
	int nItem=0;
	m_ExternList.SetRedraw(FALSE);
	m_ExternList.DeleteAllItems();
	for(i=0;i<m_lLcExtSetNum;i++)
	{
		nItem = m_ExternList.InsertItem(i,m_pLcExtSet[i].Code);
		m_ExternList.SetItemData(nItem,i);
		m_ExternList.SetItemText(nItem,1,m_pLcExtSet[i].Name);
	}
	m_ExternList.SetRedraw(TRUE);
	UpdateData(FALSE);
}

void CLocalExternMngDlg::FillEdit()
{
}

void CLocalExternMngDlg::OnDblclkLcindexmngList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnLcexternmngChgbutton();
	
	*pResult = 0;
}
