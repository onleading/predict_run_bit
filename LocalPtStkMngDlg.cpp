// LocalPtStkMngDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "LocalPtStkMngDlg.h"
#include "LocalPtStkSetDlg.h"
#include "KeyGuy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLocalPtStkMngDlg property page

IMPLEMENT_DYNCREATE(CLocalPtStkMngDlg, CPropertyPage)

CLocalPtStkMngDlg::CLocalPtStkMngDlg() : CPropertyPage(CLocalPtStkMngDlg::IDD)
{
	//{{AFX_DATA_INIT(CLocalPtStkMngDlg)
	//}}AFX_DATA_INIT
	m_pLcPtSet		= NULL;
	m_lLcPtSetNum	= 0;
}

CLocalPtStkMngDlg::~CLocalPtStkMngDlg()
{
}

void CLocalPtStkMngDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLocalPtStkMngDlg)
	DDX_Control(pDX, IDC_LCPTSTKMNG_LIST, m_PtStkList);
	DDX_Control(pDX, IDC_HINT_STATIC, m_CStatus);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLocalPtStkMngDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CLocalPtStkMngDlg)
	ON_BN_CLICKED(IDC_LCPTSTKMNG_ADDBUTTON, OnLcptstkmngAddbutton)
	ON_BN_CLICKED(IDC_LCPTSTKMNG_CHGBUTTON, OnLcptstkmngChgbutton)
	ON_BN_CLICKED(IDC_LCPTSTKMNG_DELBUTTON, OnLcptstkmngDelbutton)
	ON_BN_CLICKED(IDC_VIEWCURHQ, OnViewcurhq)
	ON_NOTIFY(NM_DBLCLK, IDC_LCPTSTKMNG_LIST, OnDblclkLcptstkmngList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLocalPtStkMngDlg message handlers

BOOL CLocalPtStkMngDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	m_CStatus.SetTextColor(RGB(240,0,0));
	
	char PtListHeadStr[][10] = {"品种代码","品种名称","原始市场","原始代码"};
	int PtListHeadWid[] = {60,60,60,60};

	m_ImageList.Create(1,20,ILC_COLOR8|ILC_MASK,1,1);
	m_PtStkList.SetImageList(&m_ImageList,LVSIL_SMALL);
	m_PtStkList.SetExtendedStyle(m_PtStkList.GetExtendedStyle()|LVS_EX_FULLROWSELECT);

	int i=0;
	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;

	if(g_pLcIdxMng)
		m_lLcPtSetNum = g_pLcIdxMng->GetPtSet(m_pLcPtSet);

	for(i=0;i<4;i++)
	{
		lvColumn.pszText	= _F(PtListHeadStr[i]);
		lvColumn.cx			= PtListHeadWid[i];
		lvColumn.iSubItem	= i;
		m_PtStkList.InsertColumn (i,&lvColumn);
	}
	
	FillList();
	
	return TRUE;
}

void CLocalPtStkMngDlg::OnLcptstkmngAddbutton() 
{
	if(!g_bG2DT)
	{
		TDX_MessageBox(m_hWnd,"暂不支持此功能.",MB_OK|MB_ICONERROR);
		return;
	}
	if(g_pLcIdxMng==NULL) return;
	if(g_pLcIdxMng->GetPtNum()>=LOCALINDEX_MAXSPACE)
	{
		TDX_MessageBox(m_hWnd,"退市品种个数超过限制.",MB_OK|MB_ICONERROR);
		return;
	}

	UpdateData(TRUE);
	CLocalPtStkSetDlg dlg;
	if(dlg.DoModal()==IDOK)
	{
		g_pLcIdxMng->AddLcPt(&(dlg.m_LcPtSet));
		m_lLcPtSetNum = g_pLcIdxMng->GetPtSet(m_pLcPtSet);
	}
	g_pLcIdxMng->SaveAll();
	g_pLcIdxMng->LoadAll();
	g_pStockIO->ReAddLc(SZ);
	m_lLcPtSetNum = g_pLcIdxMng->GetPtSet(m_pLcPtSet);
	FillList();
	m_PtStkList.SetItemState(m_PtStkList.GetItemCount()-1,LVIS_SELECTED,LVIS_SELECTED);
	UpdateData(FALSE);
}

void CLocalPtStkMngDlg::OnLcptstkmngChgbutton() 
{
	if(g_pLcIdxMng==NULL || m_lLcPtSetNum<=0) return;

	UpdateData(TRUE);
	POSITION pos = m_PtStkList.GetFirstSelectedItemPosition();
	if(pos == NULL)
	{
		TDX_MessageBox(m_hWnd,"请选择列表中的品种!",MB_OK|MB_ICONERROR);
		return;
	}
	int nItem = m_PtStkList.GetNextSelectedItem(pos);
	if(nItem<0 || nItem>=m_PtStkList.GetItemCount())
	{
		TDX_MessageBox(m_hWnd,"请选择列表中的品种!",MB_OK|MB_ICONERROR);
		return;
	}
	DWORD nIndex = m_PtStkList.GetItemData(nItem);
	if(nIndex<0 || nIndex>=m_lLcPtSetNum)
	{
		FillList();
		return;
	}
	
	CLocalPtStkSetDlg dlg;
	dlg.SetPtSet(&(m_pLcPtSet[nIndex]),nIndex);
	if(dlg.DoModal()==IDOK)
		g_pLcIdxMng->ChgLcPt(&(dlg.m_LcPtSet),nIndex);
	g_pLcIdxMng->SaveAll();
	g_pLcIdxMng->LoadAll();
	g_pStockIO->ReAddLc(SZ);
	m_lLcPtSetNum = g_pLcIdxMng->GetPtSet(m_pLcPtSet);
	FillList();
	m_PtStkList.SetItemState(nItem,LVIS_SELECTED,LVIS_SELECTED);
	UpdateData(FALSE);
}

void CLocalPtStkMngDlg::OnLcptstkmngDelbutton() 
{
	if(g_pLcIdxMng==NULL || m_lLcPtSetNum<=0) return;

	UpdateData(TRUE);
	POSITION pos = m_PtStkList.GetFirstSelectedItemPosition();
	if(pos == NULL)
	{
		TDX_MessageBox(m_hWnd,"请选择列表中的品种!",MB_OK|MB_ICONERROR);
		return;
	}
	int nItem = m_PtStkList.GetNextSelectedItem(pos);
	if(nItem<0 || nItem>=m_PtStkList.GetItemCount())
	{
		TDX_MessageBox(m_hWnd,"请选择列表中的品种!",MB_OK|MB_ICONERROR);
		return;
	}
	DWORD nIndex = m_PtStkList.GetItemData(nItem);
	if(nIndex<0 || nIndex>=m_lLcPtSetNum)
	{
		FillList();
		return;
	}

	g_pLcIdxMng->DelLcPt(nIndex);
	g_pLcIdxMng->LoadAll();
	g_pStockIO->ReAddLc(SZ);
	m_lLcPtSetNum = g_pLcIdxMng->GetPtSet(m_pLcPtSet);

	FillList();
	if(nItem>=m_PtStkList.GetItemCount()) m_PtStkList.SetItemState(m_PtStkList.GetItemCount()-1,LVIS_SELECTED,LVIS_SELECTED);
	else m_PtStkList.SetItemState(nItem,LVIS_SELECTED,LVIS_SELECTED);
	UpdateData(FALSE);
}

void CLocalPtStkMngDlg::OnViewcurhq() 
{
	if(g_pLcIdxMng==NULL) return;

	UpdateData(TRUE);
	POSITION pos = m_PtStkList.GetFirstSelectedItemPosition();
	if(pos == NULL)	return;
	int nItem = m_PtStkList.GetNextSelectedItem(pos);
	if(nItem<0 || nItem>=m_PtStkList.GetItemCount()) return;
	DWORD nIndex = m_PtStkList.GetItemData(nItem);
	if(nIndex<0 || nIndex>=m_lLcPtSetNum)
	{
		FillList();
		return;
	}
	
	char tmpGPValue[80];
	tmpGPValue[0] = '0';
	strcpy(tmpGPValue+1,m_pLcPtSet[nIndex].Code);
	CKeyGuy::OnCommonProcess(0,tmpGPValue);
}

void CLocalPtStkMngDlg::FillList()
{
	if(g_pLcIdxMng==NULL) return;
	long i = 0;
	int nItem=0;
	CString tmpstr;
	m_PtStkList.SetRedraw(FALSE);
	m_PtStkList.DeleteAllItems();
	for(i=0;i<m_lLcPtSetNum;i++)
	{
		nItem = m_PtStkList.InsertItem(i,m_pLcPtSet[i].Code);
		m_PtStkList.SetItemData(nItem,i);
		m_PtStkList.SetItemText(nItem,1,_F(m_pLcPtSet[i].Name));
		switch(m_pLcPtSet[i].BaseSetcode)
		{
		case SZ:
			m_PtStkList.SetItemText(nItem,2,_F("深圳"));
			break;
		case SH:
			m_PtStkList.SetItemText(nItem,2,_F("上海"));
			break;
		default:
			m_PtStkList.SetItemText(nItem,2,_F("未知"));
			break;
		}
		m_PtStkList.SetItemText(nItem,3,m_pLcPtSet[i].BaseCode);
	}
	m_PtStkList.SetRedraw(TRUE);
	UpdateData(FALSE);
}

void CLocalPtStkMngDlg::OnDblclkLcptstkmngList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnLcptstkmngChgbutton();
	
	*pResult = 0;
}
