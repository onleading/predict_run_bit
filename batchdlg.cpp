// batchdlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "batchdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBatchDlg dialog


CBatchDlg::CBatchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBatchDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBatchDlg)
	//}}AFX_DATA_INIT
	m_nOwnerType = 0;
	m_nStkNum = 0;
	m_WhichGPType = TYPE_ZXG;
	m_nBlockFlag = -1;

	m_bDisableDelete = TRUE;
	m_bDeleteSuccess = FALSE;
	m_bTopMost = FALSE;
	m_bAllData = FALSE;
}


void CBatchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBatchDlg)
	DDX_Control(pDX, IDC_GPLIST, m_GPList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBatchDlg, CDialog)
	//{{AFX_MSG_MAP(CBatchDlg)
	ON_BN_CLICKED(IDC_BATCHADD, OnBatchadd)
	ON_BN_CLICKED(IDC_BATCHDELETE, OnBatchdelete)
	ON_BN_CLICKED(IDC_SELECTALL, OnSelectall)
	ON_BN_CLICKED(IDC_SELECTNONE, OnSelectnone)
	ON_BN_CLICKED(IDC_BATCHMARK, OnBatchmark)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_MARK_CANCEL, OnCancelMark)
	ON_COMMAND_RANGE(ID_MARK_ADD1, ID_MARK_ADD10, OnMarkStockFlag)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBatchDlg message handlers

char *BatchGPStr[2] = {"股票代码","股票名称"};
int   BatchGPWidth[2] = {75,110};

BOOL CBatchDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	char title[255]={0};
	if(m_bAllData)
		sprintf(title,"批量操作本报表中的所有股票[%d只]",m_nStkNum);
	else
		sprintf(title,"批量操作本屏股票[%d只]",m_nStkNum);
	if(m_nOwnerType==1) 
	{
		sprintf(title,"从剪贴板监控到%d只股票信息",m_nStkNum);
		GetDlgItem(IDC_BATCHDELETE)->ShowWindow(SW_HIDE);
	}
	SetWindowText(_F(title));

	GetDlgItem(IDC_BATCHDELETE)->EnableWindow(!m_bDisableDelete);
	if(!BeUserBlock(m_WhichGPType,m_nBlockFlag))
		GetDlgItem(IDC_BATCHDELETE)->EnableWindow(FALSE);
	//股票列表框
	m_GPList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);

	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	for(int i = 0; i < 2;i++)
	{
		lvColumn.pszText = _F(BatchGPStr[i]);
		lvColumn.cx = BatchGPWidth[i];
		lvColumn.iSubItem = i;
		m_GPList.InsertColumn (i,&lvColumn);
	}
	
	MemStkInfo *tmpInfo;
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iSubItem = 0;
	for(i = 0;i < m_nStkNum;i++)
	{
		lvItem.iItem = i;
		lvItem.iImage = 0;
		tmpInfo = (*g_pStockIO)[m_nStkIndex[i]];
		lvItem.pszText = tmpInfo->Code;
		m_GPList.InsertItem(&lvItem);
		m_GPList.SetItemText(i,1,tmpInfo->Name);
		m_GPList.SetItemData(i,m_nStkIndex[i]);
		if(m_nOwnerType==1) 
			m_GPList.SetCheck(i,TRUE);
	}

	if(m_bTopMost)
		SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
	return TRUE;
}

void CBatchDlg::OnOK() 
{
}

#include "GetBlockDlg.h"
void CBatchDlg::OnBatchadd() 
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
		{
			g_bBlockModified = TRUE;
			CDialog::OnCancel();
		}
	}
	else
		TDX_MessageBox(m_hWnd, "请在左边勾选需要加入板块的证券.",MB_ICONINFORMATION);
}

void CBatchDlg::OnBatchdelete() 
{
	BOOL bFindOne = FALSE;
	for(int i=0;i < m_nStkNum;i++)
	{
		if(m_GPList.GetCheck(i))
		{
			bFindOne = TRUE;
			break;
		}
	}
	if(!bFindOne) return;

	if(TDX_MessageBox(m_hWnd,"你确定从当前板块中删除这些股票吗?",MB_ICONQUESTION|MB_OKCANCEL)==IDCANCEL)
		return;
	for(i=0;i < m_nStkNum;i++)
	{
		if(m_GPList.GetCheck(i))
		{
			if(DeleteGPFromBlock(m_WhichGPType,m_nBlockFlag,m_nStkIndex[i]))
			{
				m_bDeleteSuccess = TRUE;
				CDialog::OnCancel();
			}
		}
	}
		
}

void CBatchDlg::OnSelectall() 
{
	for(int i = 0;i < m_nStkNum;i++)
		m_GPList.SetCheck(i,1);
}

void CBatchDlg::OnSelectnone() 
{
	for(int i = 0;i < m_nStkNum;i++)
		m_GPList.SetCheck(i,0);	
}

void CBatchDlg::OnBatchmark() 
{
	CNewMenu showMore;
	showMore.CreatePopupMenu();
	showMore.AppendMenu(0, ID_MARK_CANCEL, _F("取消标记"));
//	showMore.AppendMenu(MF_SEPARATOR, 0, NULL);
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

void CBatchDlg::OnCancelMark()
{
	BOOL bHasCheck=FALSE;
	for(int i=0;i < m_nStkNum;i++)
	{
		if(m_GPList.GetCheck(i))
		{
			bHasCheck = TRUE;
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
	theViewManager.pActiveView->PostMessage(WM_PAINT);
}

#include "TipMarkDlg.h"
void CBatchDlg::OnMarkStockFlag(UINT nID)
{
	int nFlag = nID - ID_MARK_ADD1+1;
	if(nFlag<=0||nFlag>10) return;

	BOOL bHasCheck=FALSE;
	for(int i=0;i < m_nStkNum;i++)
	{
		if(m_GPList.GetCheck(i))
		{
			bHasCheck = TRUE;
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
			else MarkStockFlag(tempStkInfo, (char)nFlag);
		}
	}
	if(!bHasCheck) 
	{
		TDX_MessageBox(m_hWnd, "请在左边勾选需要标记的证券.",MB_ICONINFORMATION);
		m_GPList.SetFocus();
		return;
	}
	theViewManager.pActiveView->PostMessage(WM_PAINT);
}
