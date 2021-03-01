// LocalIndexMngDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "LocalIndexMngDlg.h"
#include "LocalIndexSetDlg.h"
#include "KeyGuy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLocalIndexMngDlg property page

IMPLEMENT_DYNCREATE(CLocalIndexMngDlg, CPropertyPage)

CLocalIndexMngDlg::CLocalIndexMngDlg() : CPropertyPage(CLocalIndexMngDlg::IDD)
{
	//{{AFX_DATA_INIT(CLocalIndexMngDlg)
	m_IndexDesc = _T("");
	//}}AFX_DATA_INIT
	m_pLcIdxSet		= NULL;
	m_lLcIdxSetNum	= 0;
}

CLocalIndexMngDlg::~CLocalIndexMngDlg()
{
}

void CLocalIndexMngDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLocalIndexMngDlg)
	DDX_Control(pDX, IDC_LCINDEXMNG_LIST, m_IndexList);
	DDX_Text(pDX, IDC_LCINDEXMNG_EDIT, m_IndexDesc);
	DDX_Control(pDX, IDC_HINT_STATIC, m_CStatus);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLocalIndexMngDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CLocalIndexMngDlg)
	ON_BN_CLICKED(IDC_LCINDEXMNG_ADDBUTTON, OnLcindexmngAddbutton)
	ON_BN_CLICKED(IDC_LCINDEXMNG_CHGBUTTON, OnLcindexmngChgbutton)
	ON_BN_CLICKED(IDC_LCINDEXMNG_DELBUTTON, OnLcindexmngDelbutton)
	ON_BN_CLICKED(IDC_RECALC_BUTTON, OnRecalcButton)
	ON_BN_CLICKED(IDC_VIEWCURHQ, OnViewcurhq)
	ON_NOTIFY(NM_DBLCLK, IDC_LCINDEXMNG_LIST, OnDblclkLcindexmngList)
	ON_BN_CLICKED(IDC_RECALCALL_BUTTON, OnRecalcallButton)
	ON_NOTIFY(NM_CLICK, IDC_LCINDEXMNG_LIST, OnClickLcindexmngList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLocalIndexMngDlg message handlers

BOOL CLocalIndexMngDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	m_CStatus.SetTextColor(RGB(240,0,0));

	char IndexListHeadStr[][10] = {"品种代码","品种名称","样本个数","作者"};
	int IndexListHeadWid[] = {60,60,60,60};

	m_ImageList.Create(1,20,ILC_COLOR8|ILC_MASK,1,1);
	m_IndexList.SetImageList(&m_ImageList,LVSIL_SMALL);
	m_IndexList.SetExtendedStyle(m_IndexList.GetExtendedStyle()|LVS_EX_FULLROWSELECT);

	int i=0;
	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;

	if(g_pLcIdxMng)
		m_lLcIdxSetNum = g_pLcIdxMng->GetIdxSet(m_pLcIdxSet);

	for(i=0; i<4;i++)
	{
		lvColumn.pszText	= _F(IndexListHeadStr[i]);
		lvColumn.cx			= IndexListHeadWid[i];
		lvColumn.iSubItem	= i;
		m_IndexList.InsertColumn (i,&lvColumn);
	}

	FillList();
	FillEdit();

	return TRUE;
}

void CLocalIndexMngDlg::OnLcindexmngAddbutton() 
{
	if(g_pLcIdxMng==NULL) return;
	if(g_pLcIdxMng->GetIdxNum()>=LOCALINDEX_MAXSPACE)
	{
		TDX_MessageBox(m_hWnd,"组合品种个数超过限制.",MB_OK);
		return;
	}

	UpdateData(TRUE);
	CLocalIndexSetDlg dlg;
	if(dlg.DoModal()==IDOK)
	{
		if(g_pLcIdxMng->AddLcIndex(&(dlg.m_LcIdxSet),dlg.m_pLcIdxStk,dlg.m_LcIdxStkNum))
		{
			m_lLcIdxSetNum = g_pLcIdxMng->GetIdxSet(m_pLcIdxSet);
			if(m_lLcIdxSetNum>0)
				g_pLcIdxMng->ReCalc(m_lLcIdxSetNum-1);
		}
	}
	g_pLcIdxMng->SaveAll();
	g_pLcIdxMng->LoadAll();
	g_pStockIO->ReAddLc(SZ);
	m_lLcIdxSetNum = g_pLcIdxMng->GetIdxSet(m_pLcIdxSet);
	FillList();
	m_IndexList.SetItemState(m_IndexList.GetItemCount()-1,LVIS_SELECTED,LVIS_SELECTED);
	UpdateData(FALSE);

	FillEdit();
}

void CLocalIndexMngDlg::OnLcindexmngChgbutton() 
{
	if(g_pLcIdxMng==NULL || m_lLcIdxSetNum<=0) return;

	UpdateData(TRUE);
	POSITION pos = m_IndexList.GetFirstSelectedItemPosition();
	if(pos == NULL)
	{
		TDX_MessageBox(m_hWnd,"请选择列表中的品种!",MB_OK|MB_ICONERROR);
		return;
	}
	int nItem = m_IndexList.GetNextSelectedItem(pos);
	if(nItem<0 || nItem>=m_IndexList.GetItemCount())
	{
		TDX_MessageBox(m_hWnd,"请选择列表中的品种!",MB_OK|MB_ICONERROR);
		return;
	}
	DWORD nIndex = m_IndexList.GetItemData(nItem);
	if(nIndex<0 || nIndex>=m_lLcIdxSetNum)
	{
		FillList();
		return;
	}
	
	CLocalIndexSetDlg dlg;
	dlg.SetIdxSet(&(m_pLcIdxSet[nIndex]),nIndex);
	if(dlg.DoModal()==IDOK)
	{
		g_pLcIdxMng->ChgLcIndex(&(dlg.m_LcIdxSet),nIndex,dlg.m_pLcIdxStk,dlg.m_LcIdxStkNum);
		g_pLcIdxMng->ReCalc(nIndex);
	}
	g_pLcIdxMng->SaveAll();
	g_pLcIdxMng->LoadAll();
	g_pStockIO->ReAddLc(SZ);
	m_lLcIdxSetNum = g_pLcIdxMng->GetIdxSet(m_pLcIdxSet);
	FillList();
	m_IndexList.SetItemState(nItem,LVIS_SELECTED,LVIS_SELECTED);
	UpdateData(FALSE);

	FillEdit();
}

void CLocalIndexMngDlg::OnLcindexmngDelbutton() 
{
	if(g_pLcIdxMng==NULL || m_lLcIdxSetNum<=0) return;

	UpdateData(TRUE);
	POSITION pos = m_IndexList.GetFirstSelectedItemPosition();
	if(pos == NULL)
	{
		TDX_MessageBox(m_hWnd,"请选择列表中的品种!",MB_OK|MB_ICONERROR);
		return;
	}
	int nItem = m_IndexList.GetNextSelectedItem(pos);
	if(nItem<0 || nItem>=m_IndexList.GetItemCount())
	{
		TDX_MessageBox(m_hWnd,"请选择列表中的品种!",MB_OK|MB_ICONERROR);
		return;
	}
	DWORD nIndex = m_IndexList.GetItemData(nItem);
	if(nIndex<0 || nIndex>=m_lLcIdxSetNum)
	{
		FillList();
		return;
	}

	g_pLcIdxMng->DelLcIndex(nIndex);
	g_pLcIdxMng->LoadAll();
	g_pStockIO->ReAddLc(SZ);
	m_lLcIdxSetNum = g_pLcIdxMng->GetIdxSet(m_pLcIdxSet);

	FillList();
	if(nItem>=m_IndexList.GetItemCount()) m_IndexList.SetItemState(m_IndexList.GetItemCount()-1,LVIS_SELECTED,LVIS_SELECTED);
	else m_IndexList.SetItemState(nItem,LVIS_SELECTED,LVIS_SELECTED);
	UpdateData(FALSE);

	FillEdit();
}

void CLocalIndexMngDlg::FillList()
{
	if(g_pLcIdxMng==NULL) return;
	long i = 0;
	int nItem=0;
	CString tmpstr;
	m_IndexList.SetRedraw(FALSE);
	m_IndexList.DeleteAllItems();
	for(i=0;i<m_lLcIdxSetNum;i++)
	{
		nItem = m_IndexList.InsertItem(i,m_pLcIdxSet[i].Code);
		m_IndexList.SetItemData(nItem,i);
		m_IndexList.SetItemText(nItem,1,m_pLcIdxSet[i].Name);
		tmpstr.Format("%d",m_pLcIdxSet[i].nUnitNum);
		m_IndexList.SetItemText(nItem,2,tmpstr);
		m_IndexList.SetItemText(nItem,3,m_pLcIdxSet[i].Author);
	}
	m_IndexList.SetRedraw(TRUE);
	UpdateData(FALSE);
}

void CLocalIndexMngDlg::FillEdit()
{
	if(g_pLcIdxMng==NULL) return;

	POSITION pos = m_IndexList.GetFirstSelectedItemPosition();
	if(pos == NULL)	return;
	int nItem = m_IndexList.GetNextSelectedItem(pos);
	if(nItem<0 || nItem>=m_IndexList.GetItemCount()) return;
	DWORD nIndex = m_IndexList.GetItemData(nItem);
	if(nIndex<0 || nIndex>=m_lLcIdxSetNum)
	{
		m_IndexDesc = "";
		UpdateData(FALSE);
		return;
	}
	m_IndexDesc = m_pLcIdxSet[nIndex].Descript;
	UpdateData(FALSE);
}

void CLocalIndexMngDlg::OnRecalcButton() 
{
	if(g_pLcIdxMng==NULL || m_lLcIdxSetNum<=0) return;

	UpdateData(TRUE);
	POSITION pos = m_IndexList.GetFirstSelectedItemPosition();
	if(pos == NULL)
	{
		TDX_MessageBox(m_hWnd,"请选择列表中的品种!",MB_OK|MB_ICONERROR);
		return;
	}
	int nItem = m_IndexList.GetNextSelectedItem(pos);
	if(nItem<0 || nItem>=m_IndexList.GetItemCount())
	{
		TDX_MessageBox(m_hWnd,"请选择列表中的品种!",MB_OK|MB_ICONERROR);
		return;
	}
	DWORD nIndex = m_IndexList.GetItemData(nItem);
	if(nIndex<0 || nIndex>=m_lLcIdxSetNum)
	{
		FillList();
		return;
	}	
	g_pLcIdxMng->ReCalc(nIndex);
}

void CLocalIndexMngDlg::OnViewcurhq() 
{
	if(g_pLcIdxMng==NULL) return;

	UpdateData(TRUE);
	POSITION pos = m_IndexList.GetFirstSelectedItemPosition();
	if(pos == NULL)	return;
	int nItem = m_IndexList.GetNextSelectedItem(pos);
	if(nItem<0 || nItem>=m_IndexList.GetItemCount()) return;
	DWORD nIndex = m_IndexList.GetItemData(nItem);
	if(nIndex<0 || nIndex>=m_lLcIdxSetNum)
	{
		FillList();
		return;
	}
	
	char tmpGPValue[80];
	tmpGPValue[0] = '0';
	strcpy(tmpGPValue+1,m_pLcIdxSet[nIndex].Code);
	CKeyGuy::OnCommonProcess(0,tmpGPValue);
}

void CLocalIndexMngDlg::OnDblclkLcindexmngList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnLcindexmngChgbutton();
	
	*pResult = 0;
}

void CLocalIndexMngDlg::OnRecalcallButton() 
{
	g_pLcIdxMng->ReCalc();
}

void CLocalIndexMngDlg::OnClickLcindexmngList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	if(g_pLcIdxMng==NULL || m_lLcIdxSetNum<=0) return;

	UpdateData(TRUE);
	FillEdit();
	*pResult = 0;
}
