// FormulaPackDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "FormulaPackDlg.h"
#include "OutStruct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFormulaPackDlg dialog


CFormulaPackDlg::CFormulaPackDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFormulaPackDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFormulaPackDlg)
	m_PackName = _T("");
	m_bSavePeriod = FALSE;
	//}}AFX_DATA_INIT
	m_bBand = FALSE;
	
	m_nFmlPack = g_nFmlPack;
	memcpy(m_FmlPack,g_FmlPack,MAX_FMLPACK_NUM*sizeof(FMLPACK));
	m_nFml2Stk = g_nFml2Stk;
	memcpy(m_Fml2Stk,g_Fml2Stk,MAX_BANDSTOCK_NUM*sizeof(FML2STK));

	memset(&m_SvFmlPack,0,sizeof(FMLPACK));
	m_pStk = NULL;
}


void CFormulaPackDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFormulaPackDlg)
	DDX_Control(pDX, IDC_TAB_FORMULAPACK, m_Tab);
	DDX_Control(pDX, IDC_LIST_FORMULAPACK, m_List);
	DDX_Text(pDX, IDC_EDIT_NEWPACK, m_PackName);
	DDX_Check(pDX, IDC_CHECK_SAVEPERIOD, m_bSavePeriod);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFormulaPackDlg, CDialog)
	//{{AFX_MSG_MAP(CFormulaPackDlg)
	ON_BN_CLICKED(IDC_BUTTON_ADDDEL, OnButtonAddDel)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_FORMULAPACK, OnSelchangeTabFormulapack)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_FORMULAPACK, OnDblclkListFormulapack)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFormulaPackDlg message handlers

BOOL CFormulaPackDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	//股票列表框
	m_ImageList.Create(16,16,ILC_COLOR8|ILC_MASK,2,1);
	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_PACK));
	m_ImageList.Add(hIcon);
	hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_GP));
	m_ImageList.Add(hIcon);
	m_List.SetImageList(&m_ImageList,LVSIL_SMALL);
	m_List.SetExtendedStyle (LVS_EX_FULLROWSELECT);

	m_Tab.InsertItem(0,_F("指标模板信息"));
	if(!m_bBand && !m_bNew)
		m_Tab.InsertItem(1,_F("证券绑定信息"));
	m_Tab.SetCurSel(0);

	FillListHead();
	FillList();

	if(m_bBand)
	{
		GetDlgItem(IDC_BUTTON_ADDDEL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_NEWPACK)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECK_SAVEPERIOD)->ShowWindow(SW_HIDE);
		SetWindowText(_F("当前证券绑定到指标模板"));
	}
	else
	{
		GetDlgItem(IDC_BUTTON_ADDDEL)->ShowWindow(SW_SHOW);
		if(m_bNew)
		{
			SetWindowText(_F("另存为指标模板"));
			GetDlgItem(IDC_EDIT_NEWPACK)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_CHECK_SAVEPERIOD)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BUTTON_ADDDEL)->SetWindowText(_F("保存"));
			GetDlgItem(IDOK)->SetWindowText(_F("替换"));
		}
		else
		{
			GetDlgItem(IDC_EDIT_NEWPACK)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_CHECK_SAVEPERIOD)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BUTTON_ADDDEL)->SetWindowText(_F("删除"));
		}
	}
	
	return TRUE;
}

void CFormulaPackDlg::OnButtonAddDel() 
{
	int i=0,j=0;
	int nSelNo = 0;
	int nItemNo = 0;
	UpdateData(TRUE);
	if(m_bBand) 
		return;
	if(m_bNew)				//新增方案(非替换)
	{
		if(!m_bSavePeriod) m_SvFmlPack.nPeriod = -1;
		m_PackName.TrimLeft();
		m_PackName.TrimRight();
		if(m_PackName.GetLength()==0)
		{
			TDX_MessageBox(m_hWnd,"请输入新增的模板名称!",MB_OK|MB_ICONEXCLAMATION);
			return;
		}
		memset(m_SvFmlPack.aPackName,0,32);
		strncpy(m_SvFmlPack.aPackName,LPCSTR(m_PackName),31);
		for(i=0;i<m_nFmlPack;i++)
		{
			if(stricmp(m_PackName,m_FmlPack[i].aPackName)==0) 
				break;
		}
		if(i<m_nFmlPack)		//重名,调用覆盖
		{
			for(j=0;j<m_List.GetItemCount();j++)
			{
				if(i-m_List.GetItemData(j)==0) 
					m_List.SetItemState(j,LVIS_SELECTED,LVIS_SELECTED);
				else 
					m_List.SetItemState(j,~LVIS_SELECTED,LVIS_SELECTED);
			}
			OnOK();
			return;
		}
		if(m_nFmlPack>=MAX_FMLPACK_NUM)
		{
			CString tmpInfo;
			tmpInfo.Format("最多允许建立%d个模板!\r\n请使用模板管理功能整理!",MAX_FMLPACK_NUM);
			TDX_MessageBox(m_hWnd,tmpInfo,MB_OK|MB_ICONEXCLAMATION);
			return;
		}
		memcpy(&(m_FmlPack[m_nFmlPack]),&m_SvFmlPack,sizeof(FMLPACK));
		memcpy(&(g_FmlPack[m_nFmlPack]),&m_SvFmlPack,sizeof(FMLPACK));
		m_nFmlPack++;
		g_nFmlPack++;
		g_CF.SaveFormulaPack();
		CDialog::OnOK();
	}
	else					//删除方案或者绑定
	{
		POSITION pos = m_List.GetFirstSelectedItemPosition();
		if(pos == NULL)
		{
			TDX_MessageBox(m_hWnd,"请在列表中进行选择!",MB_OK|MB_ICONEXCLAMATION);
			return;
		}
		nSelNo = m_List.GetNextSelectedItem(pos);
		if(nSelNo<0 || nSelNo>=m_List.GetItemCount()) return;
		nItemNo = m_List.GetItemData(nSelNo);
		switch(m_Tab.GetCurSel())
		{
		case 0:
			if(nItemNo<0 || nItemNo>=m_nFmlPack) return;
			//删除与该模板相关的绑定
			j = 0;
			for(i=0;i<m_nFml2Stk;i++)
			{
				if(stricmp(m_Fml2Stk[i].aPackName,m_FmlPack[nItemNo].aPackName)==0) 
					continue;
				if(i>j) 
					memcpy(&(m_Fml2Stk[j]),&(m_Fml2Stk[i]),sizeof(FML2STK));
				j++;
			}
			m_nFml2Stk = j;
			//
			if(nItemNo<m_nFmlPack-1) memmove(&(m_FmlPack[nItemNo]),&(m_FmlPack[nItemNo+1]),(m_nFmlPack-1-nItemNo)*sizeof(FMLPACK));
			m_nFmlPack--;
			memset(&(m_FmlPack[m_nFmlPack]),0,sizeof(FMLPACK));
			break;
		case 1:
			if(nItemNo<0 || nItemNo>=m_nFml2Stk) return;
			if(nItemNo<m_nFml2Stk-1) 
				memmove(&(m_Fml2Stk[nItemNo]),&(m_Fml2Stk[nItemNo+1]),(m_nFml2Stk-1-nItemNo)*sizeof(FML2STK));
			m_nFml2Stk--;
			memset(&(m_Fml2Stk[m_nFml2Stk]),0,sizeof(FML2STK));
			break;
		default:return;
		}
		FillList();
		if(nSelNo>=m_List.GetItemCount()) 
			nSelNo = m_List.GetItemCount()-1;
		if(nSelNo>=0) 
			m_List.SetItemState(nSelNo,LVIS_SELECTED,LVIS_SELECTED);
	}
}

void CFormulaPackDlg::OnOK()
{
	int i=0,j=0;
	int nSelNo = 0;
	int nItemNo = 0;
	UpdateData(TRUE);
	if(m_bNew)				//替换方案
	{
		POSITION pos = m_List.GetFirstSelectedItemPosition();
		if(pos == NULL)
		{
			TDX_MessageBox(m_hWnd,"请在列表中选中要替换的模板!",MB_OK|MB_ICONEXCLAMATION);
			return;
		}
		if(TDX_MessageBox(m_hWnd,"确定要替换吗?",MB_YESNO|MB_ICONQUESTION)!=IDYES) 
			return;
		nSelNo = m_List.GetNextSelectedItem(pos);
		if(nSelNo<0 || nSelNo>=m_List.GetItemCount()) 
			return;
		nItemNo = m_List.GetItemData(nSelNo);
		if(!m_bSavePeriod) m_SvFmlPack.nPeriod = -1;
		memcpy(m_SvFmlPack.aPackName,m_FmlPack[nItemNo].aPackName,32*sizeof(char));
		memcpy(&(m_FmlPack[nItemNo]),&m_SvFmlPack,sizeof(FMLPACK));
		FillList();
		if(nSelNo>=m_List.GetItemCount()) 
			nSelNo = m_List.GetItemCount()-1;
		if(nSelNo>=0) 
			m_List.SetItemState(nSelNo,LVIS_SELECTED,LVIS_SELECTED);
	}
	else if(m_bBand)		//绑定方案
	{
		POSITION pos = m_List.GetFirstSelectedItemPosition();
		if(pos == NULL)
		{
			TDX_MessageBox(m_hWnd,"请在列表中选中要绑定的模板!",MB_OK|MB_ICONEXCLAMATION);
			return;
		}
		if(m_pStk==NULL) 
			return;
		if(m_nFml2Stk >= MAX_BANDSTOCK_NUM) 
			return;
		nSelNo = m_List.GetNextSelectedItem(pos);
		if(nSelNo<0 || nSelNo>=m_List.GetItemCount()) 
			return;
		nItemNo = m_List.GetItemData(nSelNo);
		m_Fml2Stk[m_nFml2Stk].setcode = m_pStk->setcode;
		strncpy(m_Fml2Stk[m_nFml2Stk].code,m_pStk->Code,12);
		memcpy(m_Fml2Stk[m_nFml2Stk].aPackName,m_FmlPack[nItemNo].aPackName,32*sizeof(char));
		m_nFml2Stk++;
	}
	g_nFmlPack = m_nFmlPack;
	memcpy(g_FmlPack,m_FmlPack,MAX_FMLPACK_NUM*sizeof(FMLPACK));
	g_nFml2Stk = m_nFml2Stk;
	memcpy(g_Fml2Stk,m_Fml2Stk,MAX_BANDSTOCK_NUM*sizeof(FML2STK));
	g_CF.SaveFormulaPack();
	g_CF.SaveFormulaBandStock();
	CDialog::OnOK();
}

void CFormulaPackDlg::OnCancel() 
{
	CDialog::OnCancel();
}

void CFormulaPackDlg::OnSelchangeTabFormulapack(NMHDR* pNMHDR, LRESULT* pResult) 
{
	FillListHead();
	FillList();
	*pResult = 0;
}

void CFormulaPackDlg::FillListHead()
{
	int nTab = m_Tab.GetCurSel();
	m_List.SetRedraw(FALSE);
	int i = 0;
	CHeaderCtrl *pHeadCtrl = m_List.GetHeaderCtrl();
	if(pHeadCtrl)
	{
		int nColumnCount = pHeadCtrl->GetItemCount();
		for(i = 0;i < nColumnCount;i++) 
			m_List.DeleteColumn(0);
	}
	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	if(nTab==0)
	{
		lvColumn.pszText = _F("模板名称");
		lvColumn.cx = 100;
		lvColumn.iSubItem = 0;
		lvColumn.fmt = LVCFMT_LEFT;
		m_List.InsertColumn (0,&lvColumn);
		lvColumn.pszText = _F("组成内容");
		lvColumn.cx = 270;
		lvColumn.iSubItem = 1;
		m_List.InsertColumn (1,&lvColumn);
	}
	else
	{
		lvColumn.pszText = _F("证券名称");
		lvColumn.cx = 130;
		lvColumn.iSubItem = 0;
		lvColumn.fmt = LVCFMT_LEFT;
		m_List.InsertColumn (0,&lvColumn);
		lvColumn.pszText = _F("绑定模板");
		lvColumn.cx = 230;
		lvColumn.iSubItem = 1;
		m_List.InsertColumn (1,&lvColumn);
	}
	m_List.SetRedraw(TRUE);
}

void CFormulaPackDlg::FillList()
{
	int nTab = m_Tab.GetCurSel();
	int i=0,j=0;
	int nItem = 0;
	CString tmpstr;
	m_List.SetRedraw(FALSE);
	m_List.DeleteAllItems();
	if(nTab==0)
	{
		for(i=0;i<m_nFmlPack;i++)
		{
			nItem = m_List.InsertItem(i,m_FmlPack[i].aPackName,0);
			m_List.SetItemData(nItem,i);			
			tmpstr.Format("周期:%s",GetTdxPeriodStr(m_FmlPack[i].nPeriod,"未绑定"));
			tmpstr += "主:";
			if(m_FmlPack[i].lRectNum==0 || m_FmlPack[i].aFormular[0][0]=='\0') 
				tmpstr+="无";
			else 
				tmpstr += m_FmlPack[i].aFormular[0];
			tmpstr += " 副:";
			if(m_FmlPack[i].lRectNum<=1) 
				tmpstr+="无";
			else
			{
				for(j=1;j<m_FmlPack[i].lRectNum;j++)
				{
					tmpstr+=m_FmlPack[i].aFormular[j];
					tmpstr+=",";
				}
			}
			m_List.SetItemText(nItem,1,_F(tmpstr));
		}
	}
	else
	{
		MemStkInfo *pStkInfo = NULL;
		for(i=0;i<m_nFml2Stk;i++)
		{
			pStkInfo = g_pStockIO->Get_MemStkInfo(m_Fml2Stk[i].code,m_Fml2Stk[i].setcode);
			if(pStkInfo)
				tmpstr.Format("%s(%s)",pStkInfo->Name,pStkInfo->Code);
			else
				tmpstr="空";
			nItem = m_List.InsertItem(i,_F(tmpstr),1);
			m_List.SetItemData(nItem,i);
			m_List.SetItemText(nItem,1,m_Fml2Stk[i].aPackName);
		}
	}
	m_List.SetRedraw(TRUE);
}

void CFormulaPackDlg::SetMode(BOOL bNew,BOOL bBand)
{
	m_bNew	= bNew;
	m_bBand = bBand;
}

void CFormulaPackDlg::SetSvFmlPack(FMLPACK &SvFmlPack)
{
	memcpy(&m_SvFmlPack,&SvFmlPack,sizeof(FMLPACK));
}

void CFormulaPackDlg::SetStock(MemStkInfo *pStk)
{
	m_pStk = pStk;
}

void CFormulaPackDlg::OnDblclkListFormulapack(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(m_bNew || m_bBand) 
		OnOK();
	
	*pResult = 0;
}
