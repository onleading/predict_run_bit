// MyDetailDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "MyDetailDlg.h"
#include "ParamTestDlg.h"
#include "TestZBDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyDetailDlg dialog
CMyDetailDlg::CMyDetailDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMyDetailDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMyDetailDlg)
	//}}AFX_DATA_INIT
	m_pParent=(CDialog*)pParent;
}

void CMyDetailDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyDetailDlg)
	DDX_Control(pDX, IDC_DETAIL, m_detailList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMyDetailDlg, CDialog)
	//{{AFX_MSG_MAP(CMyDetailDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyDetailDlg message handlers

BOOL CMyDetailDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_detailList.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	int i;
	CString		tmpStr;
	LVCOLUMN	lvc;
	TCHAR *title[] = {"时间", "类型", "价格(元)", "交易量(股)", "收益(元)",NULL};
	int		nWidth[] = {120,60,60,80,80};
	
	m_ImageList.Create(16,16,ILC_COLOR8|ILC_MASK,1,1);
	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_GP));
	m_ImageList.Add(hIcon);
	
	m_detailList.SetImageList(&m_ImageList,LVSIL_SMALL);
	
	lvc.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvc.fmt = LVCFMT_CENTER;
	for (i = 0; title[i] != NULL; i++)
	{
		lvc.pszText = title[i];
		lvc.cx = nWidth[i];
		m_detailList.InsertColumn(i,&lvc);
	}
	CString sTitle;
	sTitle.Format("%s交易明细",m_sName);
	SetWindowText(sTitle);
	FillDetail();
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CMyDetailDlg::FillDetail()
{
	int nItemCount=m_testDetails.size();
	CString tmpStr;
	LV_ITEM lvi;
	
	for(int i = 0; i < nItemCount; i++)
	{
		lvi.mask = LVIF_TEXT;
		lvi.iItem = i;
		lvi.iSubItem = 0;
		testDetail tmp=m_testDetails[i];
		tmpStr.Format("%d年%d月%d日",tmp.nTime.year,tmp.nTime.month,tmp.nTime.day);
		lvi.pszText=(char*)(LPCSTR)tmpStr;
		
		m_detailList.InsertItem(&lvi);
	}	
	// set item text for additional columns
	for(i=0;i<nItemCount;i++)
	{
		testDetail tmp=m_testDetails[i];
		if(tmp.iToDo==3) tmpStr="满足条件";
		else tmpStr=tmp.iToDo>0?"买入":(tmp.iToDo?"清仓":"卖出");
		m_detailList.SetItemText(i,1,tmpStr);
		tmpStr.Format("%.2f",tmp.fPrice);
		m_detailList.SetItemText(i,2,tmpStr);
		if(tmp.lCount==0) tmpStr="";
		else tmpStr.Format("%d",tmp.lCount);
		m_detailList.SetItemText(i,3,tmpStr);
		if(tmp.fValue>-COMPPREC&&tmp.fValue<COMPPREC)
		{
			tmpStr="";
		}
		else
		{
			tmpStr.Format("%.2f",tmp.fValue);
		}
		m_detailList.SetItemText(i,4,tmpStr);
	}
}

void CMyDetailDlg::OnCancel()
{
//	((CParamTestDlg*)m_pParent)->DeleteDetail();
//	((CTestZBDlg*)m_pParent)->DeleteDetail();
	DestroyWindow();
}

void CMyDetailDlg::PostNcDestroy() 
{
	delete this;
	CDialog::PostNcDestroy();
}
