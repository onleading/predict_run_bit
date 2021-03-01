// BlockGpDel.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "BlockGpDel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBlockGpDel dialog


CBlockGpDel::CBlockGpDel(CWnd* pParent /*=NULL*/)
	: CDialog(CBlockGpDel::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBlockGpDel)
	//}}AFX_DATA_INIT
}


void CBlockGpDel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBlockGpDel)
	DDX_Control(pDX, IDC_GPLIST, m_BlockList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBlockGpDel, CDialog)
	//{{AFX_MSG_MAP(CBlockGpDel)
	ON_BN_CLICKED(IDC_BATCHDELETE, OnBatchdelete)
	ON_BN_CLICKED(IDC_SELECTALL, OnSelectall)
	ON_BN_CLICKED(IDC_SELECTNONE, OnSelectnone)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBlockGpDel message handlers

char *BatchBKStr[2] = {"板块简称","板块名称"};
int   BatchBKWidth[2] = {65,80};

BOOL CBlockGpDel::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	//板块列表框
	m_BlockList.SetExtendedStyle(LVS_EX_CHECKBOXES);
	
	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_CENTER;
	for(int i = 0; i < 2;i++)
	{
		lvColumn.pszText = _F(BatchBKStr[i]);
		lvColumn.cx = BatchBKWidth[i];
		lvColumn.iSubItem = i;
		m_BlockList.InsertColumn (i,&lvColumn);
	}
	
	FillBlockList(m_CurGPCode,m_CurSetCode);
		
	if(m_nBlock==0)
	{
		TDX_MessageBox(m_hWnd,"该股票不属于任何自定义板块",MB_OK|MB_ICONINFORMATION);
		PostMessage(WM_COMMAND,IDCANCEL);
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
void CBlockGpDel::FillBlockList(CString CurGPCode,short CurSetCode)
{
	m_BlockList.SetRedraw(FALSE);
	m_BlockList.DeleteAllItems();

	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iSubItem = 0;
	
	short nIDIndex = g_pStockIO->GetIndex(CurGPCode, CurSetCode);
	short indexbuff[MAXGPNUM]; memset(indexbuff,-1,MAXGPNUM*sizeof(short));
	int nBlockNum = 0, i, j;
	m_nBlock = 0;
	for(i = 0;i < g_nBlockNum+2;i++)
	{
		int gpnum = FiltBlockCode(i,indexbuff);
		for(j=0;j<gpnum;j++)
		{
			if(indexbuff[j]==nIDIndex)
				break;
		}
		if(j < gpnum)
		{
			lvItem.iItem = m_nBlock;
			lvItem.iImage = 0;
			if(i==0)
				lvItem.pszText = "self";
			else if(i==1)
				lvItem.pszText = "calc";
			else
				lvItem.pszText = g_BlockInfo[i-2].KeyGuyCode;
			m_BlockList.InsertItem(&lvItem);
			if(i==0)
				m_BlockList.SetItemText(m_nBlock,1,_F("自选股"));
			else if(i==1)
				m_BlockList.SetItemText(m_nBlock,1,_F("股票池"));
			else
				m_BlockList.SetItemText(m_nBlock,1,g_BlockInfo[i-2].Name);
			m_BlockList.SetItemData(m_nBlock,i);
			m_BlockList.SetCheck(m_nBlock,TRUE);
			m_BlockList.SetItemState(m_nBlock,LVIS_FOCUSED,LVIS_FOCUSED);
			m_nBlock++;
		}
	}
	GetDlgItem(IDC_BATCHDELETE)->EnableWindow(m_nBlock>0);
	m_BlockList.SetRedraw(TRUE);
}

void CBlockGpDel::OnBatchdelete() 
{
	BOOL bFindOne = FALSE;
	for(int i=0;i < m_nBlock;i++)
	{
		if(m_BlockList.GetCheck(i))
		{
			bFindOne = TRUE;
			break;
		}
	}
	if(!bFindOne) return;
	
	short nIDIndex = g_pStockIO->GetIndex(m_CurGPCode, m_CurSetCode);
	for(i=0;nIDIndex>=0 && i < m_nBlock;i++)
	{
		if(m_BlockList.GetCheck(i))
		{
			int BlockNo=m_BlockList.GetItemData(i);
			if(BlockNo==0)
				DeleteGPFromBlock(TYPE_ZXG,0,nIDIndex);
			else if(BlockNo==1)
				DeleteGPFromBlock(TYPE_TJG,0,nIDIndex);
			else
				DeleteGPFromBlock(-1,BlockNo-2,nIDIndex);
		}
	}
	g_bBlockModified = TRUE;
	if(theViewManager.pActiveView)
		theViewManager.pActiveView->PostMessage(UM_BLOCKMODIFIED,0,0);//在GridView中马上出现
	CDialog::OnCancel();
}

void CBlockGpDel::OnSelectall() 
{
	for(int i = 0;i < m_nBlock;i++)
		m_BlockList.SetCheck(i,1);
}

void CBlockGpDel::OnSelectnone() 
{
	for(int i = 0;i < m_nBlock;i++)
		m_BlockList.SetCheck(i,0);	
}
