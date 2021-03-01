// LinkStkDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "LinkStkDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLinkStkDlg dialog


CLinkStkDlg::CLinkStkDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLinkStkDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLinkStkDlg)
	//}}AFX_DATA_INIT
	m_pCurStkInfo = NULL;
}


void CLinkStkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLinkStkDlg)
	DDX_Control(pDX, IDC_HINT_STATIC, m_Hint_Static);
	DDX_Control(pDX, IDC_LIST1, m_GPList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLinkStkDlg, CDialog)
	//{{AFX_MSG_MAP(CLinkStkDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLinkStkDlg message handlers

char *LinkGPStr[] = {"证券代码","证券名称","类型"};
int   LinkGPWidth[] = {60,110,60};

BOOL CLinkStkDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_Hint_Static.SetTextColor(RGB(255,0,0));

	m_ImageList.Create(16,16,ILC_COLOR4|ILC_MASK,1,0);
	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_GP));
	m_ImageList.Add(hIcon);
	m_GPList.SetImageList(&m_ImageList,LVSIL_SMALL);
	m_GPList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	for(int i = 0; i < 3;i++)
	{
		lvColumn.pszText = _F(LinkGPStr[i]);
		lvColumn.cx = LinkGPWidth[i];
		lvColumn.iSubItem = i;
		m_GPList.InsertColumn (i,&lvColumn);
	}

	CString tmpStr;
	MemStkInfo *tmpInfo=NULL;
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iSubItem = 0;
	int nAddedItem = 0;
	STKRELATION  LinkData[100] = {0};
	short nLinkNum = g_pStockIO->GetRLTData(m_pCurStkInfo, LinkData, 100);
	for(i=0;i < nLinkNum;i++)
	{
		if(strlen(LinkData[i].Code)<1) continue;
		lvItem.iItem = nAddedItem;
		lvItem.iImage = 0;	
		if(strlen(LinkData[i].Code)==5)
		{
			tmpStr=LinkData[i].Code;
			tmpInfo = g_pStockIO->GetDSStkInfo(31,tmpStr);
			if(!tmpInfo)
			{
				tmpInfo = g_pStockIO->GetDSStkInfo(32,tmpStr);
				if(!tmpInfo)
				{
					tmpStr=(CString)"HK"+LinkData[i].Code;
					tmpInfo = g_pStockIO->GetDSStkInfo(31,tmpStr);
				}
			}
		}
		else
			tmpInfo = g_pStockIO->Get_MemStkInfo(LinkData[i].Code, LinkData[i].SetCode);
		lvItem.pszText = LinkData[i].Code;
		m_GPList.InsertItem(&lvItem);
		m_GPList.SetItemText(nAddedItem,1,tmpInfo?tmpInfo->Name:_F(LinkData[i].Name));
		m_GPList.SetItemText(nAddedItem,2,_F(LinkData[i].Typestr));
		m_GPList.SetItemData(nAddedItem,(DWORD)tmpInfo);
		++nAddedItem;
	}
	
	return TRUE;
}


void CLinkStkDlg::OnOK() 
{
	POSITION pos = m_GPList.GetFirstSelectedItemPosition ();
	if(pos)
	{
		int nItem = m_GPList.GetNextSelectedItem (pos);
		MemStkInfo *tmpInfo = (MemStkInfo *)m_GPList.GetItemData(nItem);
		if(tmpInfo) 
			ToZst(tmpInfo->Code,tmpInfo->setcode,FALSE);
	}
}

void CLinkStkDlg::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnOK();
	*pResult = 0;
}
