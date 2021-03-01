// MarkMgn.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "MarkMgn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const char *strMark[MAX_GROUPMARK] = {"①","②","③","④","⑤","⑥","文字"};
/////////////////////////////////////////////////////////////////////////////
// CMarkMgn dialog


CMarkMgn::CMarkMgn(CWnd* pParent /*=NULL*/)
	: CDialog(CMarkMgn::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMarkMgn)
	m_bCheck1 = FALSE;
	m_bCheck2 = FALSE;
	m_bCheck3 = FALSE;
	m_bCheck4 = FALSE;
	m_bCheck5 = FALSE;
	m_bCheck6 = FALSE;
	m_bCheck7 = FALSE;
	//}}AFX_DATA_INIT
	for(int i=0;i<MAX_GROUPMARK;i++) m_bShowMark[i] = FALSE;
	m_nStkNum = 0;
	for(i=0;i<MAXGPNUM;i++) m_nStkIndex[i] = -1;
}


void CMarkMgn::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMarkMgn)
	DDX_Control(pDX, IDC_GPLIST, m_GPList);
	DDX_Check(pDX, IDC_CHECK1, m_bCheck1);
	DDX_Check(pDX, IDC_CHECK2, m_bCheck2);
	DDX_Check(pDX, IDC_CHECK3, m_bCheck3);
	DDX_Check(pDX, IDC_CHECK4, m_bCheck4);
	DDX_Check(pDX, IDC_CHECK5, m_bCheck5);
	DDX_Check(pDX, IDC_CHECK6, m_bCheck6);
	DDX_Check(pDX, IDC_CHECK7, m_bCheck7);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMarkMgn, CDialog)
	//{{AFX_MSG_MAP(CMarkMgn)
	ON_BN_CLICKED(IDC_BATCHADD, OnBatchadd)
	ON_BN_CLICKED(IDC_SELECTALL, OnSelectall)
	ON_BN_CLICKED(IDC_SELECTNONE, OnSelectnone)
	ON_BN_CLICKED(IDC_BATCHMARK, OnBatchmark)
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	ON_BN_CLICKED(IDC_CHECK2, OnCheck2)
	ON_BN_CLICKED(IDC_CHECK3, OnCheck3)
	ON_BN_CLICKED(IDC_CHECK4, OnCheck4)
	ON_BN_CLICKED(IDC_CHECK5, OnCheck5)
	ON_BN_CLICKED(IDC_CHECK6, OnCheck6)
	ON_NOTIFY(NM_DBLCLK, IDC_GPLIST, OnDblclkGplist)
	ON_BN_CLICKED(IDC_CHECK7, OnCheck7)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_MARK_CANCEL, OnCancelMark)
	ON_COMMAND_RANGE(ID_MARK_ADD1, ID_MARK_ADD10, OnMarkStockFlag)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMarkMgn message handlers
char *MarkGPStr[3] = {"股票代码","股票名称","标记"};
int   MarkGPWidth[3] = {66,70,38};

BOOL CMarkMgn::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//股票列表框
	m_GPList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);

	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_CENTER;
	for(int i = 0; i < 3;i++)
	{
		lvColumn.pszText = _F(MarkGPStr[i]);
		lvColumn.cx = MarkGPWidth[i];
		lvColumn.iSubItem = i;
		m_GPList.InsertColumn (i,&lvColumn);
	}
	
	FillGpList();

	return TRUE;
}

void CMarkMgn::FillGpList()
{
	m_nStkNum = 0;
	for(int i=0;i<MAXGPNUM;i++) m_nStkIndex[i] = -1;
	m_GPList.DeleteAllItems();
	
	for(i=0;i<MAX_GROUPMARK;i++)
	{
		if(!m_bShowMark[i]) continue;
		
		for(int j = 0;j < g_pStockIO->GetStockNum(ALL_ALL);j++)
		{
			MemStkInfo *pInfo = (*g_pStockIO)[j];
			if(!pInfo) continue;
			
			if(pInfo->fl_flag==i+1)
			{
				LV_ITEM lvItem;
				lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
				lvItem.state = 0;
				lvItem.stateMask = 0;
				lvItem.iSubItem = 0;

				lvItem.iItem = m_nStkNum;
				lvItem.iImage = 0;
				lvItem.pszText = pInfo->Code;
				m_GPList.InsertItem(&lvItem);
				m_GPList.SetItemText(m_nStkNum,1,pInfo->Name);
				m_GPList.SetItemText(m_nStkNum,2,strMark[i]);
				m_GPList.SetItemData(m_nStkNum,j);

				m_nStkIndex[m_nStkNum] = j;
				++m_nStkNum;
			}
		}
	}
}

void CMarkMgn::OnOK() 
{
}

#include "GetBlockDlg.h"
void CMarkMgn::OnBatchadd() 
{
	CGetBlockDlg dlg;
	dlg.m_bGetBlock = FALSE;
	int stknum = 0;
	for(int i=0;i < m_nStkNum;i++)
	{
		if(m_GPList.GetCheck(i))
			dlg.m_nStkIndex[stknum++] = m_nStkIndex[i];
	}
	if(stknum > 0) //只有当选中一个时，才会弹出对话框
	{
		dlg.m_nStkNum = stknum;
		if(dlg.DoModal()==IDOK)
			g_bBlockModified = TRUE;
	}
	else
		TDX_MessageBox(m_hWnd, "请在左边勾选需要加入板块的证券.",MB_ICONINFORMATION);
}

void CMarkMgn::OnSelectall() 
{
	for(int i = 0;i < m_nStkNum;i++)
		m_GPList.SetCheck(i,1);
}

void CMarkMgn::OnSelectnone() 
{
	for(int i = 0;i < m_nStkNum;i++)
		m_GPList.SetCheck(i,0);	
}

void CMarkMgn::OnBatchmark() 
{
	CNewMenu showMore;
	showMore.CreatePopupMenu();
	showMore.AppendMenu(0, ID_MARK_CANCEL, _F("取消标记"));
	showMore.AppendMenu(MF_SEPARATOR);
	showMore.AppendMenu(0, ID_MARK_ADD1, _F("标记①"));
	showMore.AppendMenu(0, ID_MARK_ADD2, _F("标记②"));
	showMore.AppendMenu(0, ID_MARK_ADD3, _F("标记③"));
	showMore.AppendMenu(0, ID_MARK_ADD4, _F("标记④"));
	showMore.AppendMenu(0, ID_MARK_ADD5, _F("标记⑤"));
	showMore.AppendMenu(0, ID_MARK_ADD6, _F("标记⑥"));
	showMore.AppendMenu(0, ID_MARK_ADD7, _F("标记文字"));

	CRect showMoreRect;
	GetDlgItem(IDC_BATCHMARK)->GetWindowRect(&showMoreRect);
	showMore.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, showMoreRect.left, showMoreRect.bottom, this);
}

void CMarkMgn::OnCancelMark()
{
	BOOL bHasCheck=FALSE;
	for(int i=0;i < m_nStkNum;i++)
	{
		if(m_GPList.GetCheck(i))
		{
			bHasCheck=TRUE;
			int nCurGpIndex = m_nStkIndex[i];
			if(nCurGpIndex<0) continue;
			MemStkInfo *tempStkInfo = (*g_pStockIO)[nCurGpIndex];
			if(!tempStkInfo) continue;
			MarkStockFlag(tempStkInfo, 0);
		}
	}
	if(!bHasCheck)
	{
		TDX_MessageBox(m_hWnd, "请在左边勾选需要取消标记的证券.",MB_ICONINFORMATION);
		m_GPList.SetFocus();
		return;
	}
	else
		FillGpList();
	theViewManager.pActiveView->PostMessage(WM_PAINT);
}

#include "TipMarkDlg.h"
void CMarkMgn::OnMarkStockFlag(UINT nID)
{
	int nFlag = nID - ID_MARK_ADD1+1;
	if(nFlag<=0||nFlag>10) return;
	BOOL bHasCheck=FALSE;
	for(int i=0;i < m_nStkNum;i++)
	{
		if(m_GPList.GetCheck(i))
		{
			bHasCheck=TRUE;
			int nCurGpIndex = m_nStkIndex[i];
			if(nCurGpIndex<0) return;
			MemStkInfo *tempStkInfo = (*g_pStockIO)[nCurGpIndex];
			if(!tempStkInfo) return;
			if(nFlag==MAX_GROUPMARK)
			{
				CTipMarkDlg dlg;
				dlg.m_pStock = tempStkInfo;
				if(dlg.DoModal()==IDOK)
					MarkStockTipFlag(tempStkInfo, dlg.m_strTip);
			}
			else 	MarkStockFlag(tempStkInfo, (char)nFlag);
		}
	}
	if(!bHasCheck)
	{
		TDX_MessageBox(m_hWnd, "请在左边勾选需要标记的证券.",MB_ICONINFORMATION);
		m_GPList.SetFocus();
		return;
	}
	else
		FillGpList();
	theViewManager.pActiveView->PostMessage(WM_PAINT);
}

void CMarkMgn::OnCheck1() 
{
	UpdateData(TRUE);
	m_bShowMark[0] = m_bCheck1;
	FillGpList();
}

void CMarkMgn::OnCheck2() 
{
	UpdateData(TRUE);
	m_bShowMark[1] = m_bCheck2;	
	FillGpList();
}

void CMarkMgn::OnCheck3() 
{
	UpdateData(TRUE);
	m_bShowMark[2] = m_bCheck3;	
	FillGpList();
}

void CMarkMgn::OnCheck4() 
{
	UpdateData(TRUE);
	m_bShowMark[3] = m_bCheck4;	
	FillGpList();
}

void CMarkMgn::OnCheck5() 
{
	UpdateData(TRUE);
	m_bShowMark[4] = m_bCheck5;	
	FillGpList();
}

void CMarkMgn::OnCheck6() 
{
	UpdateData(TRUE);
	m_bShowMark[5] = m_bCheck6;
	FillGpList();
}

void CMarkMgn::OnCheck7() 
{
	UpdateData(TRUE);
	m_bShowMark[6] = m_bCheck7;
	FillGpList();	
}

void CMarkMgn::OnDblclkGplist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	POSITION pos = m_GPList.GetFirstSelectedItemPosition ();
	if(pos)
	{
		int nItem = m_GPList.GetNextSelectedItem (pos);
		if(nItem>=0)
		{
			int GPIndex = m_nStkIndex[nItem];
			if(GPIndex < 0) return;
			ToZst((*g_pStockIO)[GPIndex]->Code,(*g_pStockIO)[GPIndex]->setcode,FALSE);
		}
	}	
	*pResult = 0;
}

