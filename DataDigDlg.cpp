// DataDigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "DataDigDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDataDigDlg dialog


CDataDigDlg::CDataDigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDataDigDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDataDigDlg)
	//}}AFX_DATA_INIT
	m_nTabSel = 0;
	contentlen = 0;
	clientnum = 0;
	usedclientnum = 0;
	seconds = 0;
	HostName[0] = 0;
	attention_num = 0;
	resonate_num = 0;
	no_attention_num = 0;
	no_resonate_num = 0;
	pContent = NULL;
}


void CDataDigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDataDigDlg)
	DDX_Control(pDX, IDC_CONTENT_EDIT, m_Content_Edit);
	DDX_Control(pDX, IDC_TAB, m_Tab);
	DDX_Control(pDX, IDC_INFO_TATIC, m_Info_Status);
	DDX_Control(pDX, IDC_GP_LIST, m_GPList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDataDigDlg, CDialog)
	//{{AFX_MSG_MAP(CDataDigDlg)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, OnSelchangeTab)
	ON_MESSAGE(UM_GETDATA_ACK,OnGetDataAck)
	ON_BN_CLICKED(IDC_TOZST, OnTozst)
	ON_NOTIFY(NM_DBLCLK, IDC_GP_LIST, OnDblclkGpList)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDataDigDlg message handlers

char *DataDigTabStr[5] = {"当前关注","持续关注","当前冷门","持续冷门","功能解释"};
char *DataDigColStr1[5] = {"名次","股票代码","股票名称","当前关注度","关注比重%"};
char *DataDigColStr2[5] = {"名次","股票代码","股票名称","持续关注度","关注比重%"};
int   DataDigColWidth[5] = {45,75,80,90,80};

BOOL CDataDigDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_Info_Status.SetTextColor(RGB(255,0,0));	
	CString tmpDescr = "在线人气: \r\n行情主站采集客户使用本系统的真实情况,按照一定算法统计出股票的当前关注度和持续关注度.\r\n当前关注度表示股票当前被关注的程度;持续关注度表示股票一直以来被关注的程度.当前关注与当前冷门意义相反,持续关注与持续冷门意义相反.";
	m_Content_Edit.SetWindowText(_F(tmpDescr));

	for(int i=0;i < 5;i++)
		m_Tab.InsertItem(i,_F(DataDigTabStr[i]));
	m_Tab.SetCurSel(0);
	m_nTabSel = 0;

	//股票列表框
	m_ImageList.Create(16,16,ILC_COLOR8|ILC_MASK,1,1);
	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_GP));
	m_ImageList.Add(hIcon);
	m_GPList.SetImageList(&m_ImageList,LVSIL_SMALL);
	m_GPList.SetExtendedStyle (LVS_EX_FULLROWSELECT);

	InitList();
	
	short mainID = 0;
	g_nMainID = MainIDPlus(mainID,DATADIG_WIN);
	g_nAssisID = DATADIG_ASSISID;
	g_pGetData->SendDataDigReq();

	return TRUE;
}

void CDataDigDlg::InitList()
{
	//删除所有列
	int nColumnCount = (m_GPList.GetHeaderCtrl())->GetItemCount();
	for (int i=0;i < nColumnCount;i++)
	   m_GPList.DeleteColumn(0);
	//重新插入列
	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_CENTER;
	for(i = 0; i < 5;i++)
	{
		lvColumn.pszText = (m_nTabSel==0||m_nTabSel==2)?(_F(DataDigColStr1[i])):(_F(DataDigColStr2[i]));
		lvColumn.cx = DataDigColWidth[i];
		lvColumn.iSubItem = i;
		m_GPList.InsertColumn (i,&lvColumn);
	}
}

void CDataDigDlg::FillList()
{
	int gpnum=0,i;
	float TotalRate=0.0;
	short *pIndex;
	unsigned long *pRate;
	if(m_nTabSel==0)
	{
		gpnum = attention_num;	
		pIndex = attention_index;
		pRate = attention_rate;
	}
	else if(m_nTabSel==1)
	{
		gpnum = resonate_num;	
		pIndex = resonate_index;
		pRate = resonate_rate;
	}
	else if(m_nTabSel==2)
	{
		gpnum = no_attention_num;	
		pIndex = no_attention_index;
		pRate = no_attention_rate;
	}
	else if(m_nTabSel==3)
	{
		gpnum = no_resonate_num;	
		pIndex = no_resonate_index;
		pRate = no_resonate_rate;
	}
	for(i=0;i<gpnum;i++)
		TotalRate+=pRate[i]*1.0;
	if(TotalRate<1) TotalRate=1;
	m_GPList.SetRedraw(FALSE);
	m_GPList.DeleteAllItems();	
	MemStkInfo *tmpInfo;
	char tmpStr[80];
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iSubItem = 0;
	for(i = 0;i < gpnum;i++)
	{
		lvItem.iItem = i;
		lvItem.iImage = 0;
		tmpInfo = (*g_pStockIO)[pIndex[i]];
		sprintf(tmpStr,"%d",i+1);
		lvItem.pszText = tmpStr;
		m_GPList.InsertItem(&lvItem);
		m_GPList.SetItemText(i,1,tmpInfo->Code);
		m_GPList.SetItemText(i,2,tmpInfo->Name);
		m_GPList.SetItemText(i,3,IntToStr(pRate[i]));
		m_GPList.SetItemText(i,4,FloatToStr(pRate[i]*100.0/TotalRate));
		m_GPList.SetItemData(i,pIndex[i]);
	}
	m_GPList.SetRedraw(TRUE);

	GetDlgItem(IDOK)->EnableWindow(gpnum>0);
	GetDlgItem(IDC_TOZST)->EnableWindow(gpnum>0);
}

void CDataDigDlg::OnGetDataAck(WPARAM wParam,LPARAM lParam)
{
	TDEL(pContent);
	BOOL bRet = g_pGetData->DataDigACK(clientnum,usedclientnum,seconds,HostName,attention_num,resonate_num,no_attention_num,no_resonate_num,attention_index,resonate_index,no_attention_index,no_resonate_index,attention_rate,resonate_rate,no_attention_rate,no_resonate_rate,contentlen,&pContent);
	if(!bRet) return;

	if(HostName[0]==0) strcpy(HostName,"无");
	CString tmpStr="";
	if(!g_bUserHostEverConnect)
		tmpStr.Format("主站名:%s 当前在线人数:%d 今日总在线数:%d",HostName,clientnum,usedclientnum);
	m_Info_Status.SetText(_F(tmpStr));

	ShowContent();
}

void CDataDigDlg::OnCancel() 
{
	TDEL(pContent);
	GetParent()->PostMessage (WM_CANCELPRESSED,(WPARAM)this,0);	
}

//存为板块
#include "GetBlockDlg.h"
void CDataDigDlg::OnOK() 
{
	int gpnum;
	short *pIndex;
	if(m_nTabSel==0)
	{
		gpnum = attention_num;	
		pIndex = attention_index;
	}
	else if(m_nTabSel==1)
	{
		gpnum = resonate_num;	
		pIndex = resonate_index;
	}
	else if(m_nTabSel==2)
	{
		gpnum = no_attention_num;	
		pIndex = no_attention_index;
	}
	else
	{
		gpnum = no_resonate_num;	
		pIndex = no_resonate_index;
	}
	CGetBlockDlg dlg;
	if(dlg.DoModal() == IDOK)
	{
		if(HasGPInBlockAndContinue(dlg.nSelBlock))
			MakeBlock(pIndex,gpnum,dlg.nSelBlock,TRUE);//生成板块并显示板块
	}
}

void CDataDigDlg::ShowContent()
{
	if(m_nTabSel==4)
	{
		m_GPList.ShowWindow(SW_HIDE);
		m_Content_Edit.ShowWindow(SW_SHOW);
		GetDlgItem(IDOK)->EnableWindow(FALSE);
		GetDlgItem(IDC_TOZST)->EnableWindow(FALSE);
	}
	else
	{
		m_GPList.ShowWindow(SW_SHOW);
		m_Content_Edit.ShowWindow(SW_HIDE);
		InitList();
		FillList();
	}
}

void CDataDigDlg::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int iNo = m_Tab.GetCurSel();
	if(iNo != m_nTabSel)
	{
		m_nTabSel = iNo;
		ShowContent();
	}
	*pResult = 0;
}

void CDataDigDlg::OnTozst() 
{
	POSITION pos = m_GPList.GetFirstSelectedItemPosition ();
	if(pos)
	{
		int nItem = m_GPList.GetNextSelectedItem (pos);
		int GPIndex = m_GPList.GetItemData(nItem);
		if(GPIndex < 0) return;
		ToZst((*g_pStockIO)[GPIndex]->Code,(*g_pStockIO)[GPIndex]->setcode,FALSE);
	}		
}

void CDataDigDlg::OnDblclkGpList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnTozst();	
	*pResult = 0;
}

HBRUSH CDataDigDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if (pWnd->GetDlgCtrlID() == IDC_CONTENT_EDIT)
		return (HBRUSH)GetStockObject(WHITE_BRUSH);

	return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}
