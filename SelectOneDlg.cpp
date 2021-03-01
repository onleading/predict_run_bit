// SelectOneDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "SelectOneDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectOneDlg dialog


CSelectOneDlg::CSelectOneDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectOneDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectOneDlg)
	m_strHintStr = _T("");
	//}}AFX_DATA_INIT
	m_nCurSel = 0;
	m_Item.clear();
}


void CSelectOneDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectOneDlg)
	DDX_Control(pDX, IDC_LIST1, m_List);
	DDX_Text(pDX, IDC_HINT_STATIC, m_strHintStr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectOneDlg, CDialog)
	//{{AFX_MSG_MAP(CSelectOneDlg)
	ON_LBN_DBLCLK(IDC_LIST1, OnDblclkList1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectOneDlg message handlers

void CSelectOneDlg::AddString(CString tmpStr)
{
   tag_SelectItem tmp;
   strncpy(tmp.ItemStr,tmpStr,250);
   tmp.ItemStr[250]=0;
   m_Item.push_back(tmp);
}


void CSelectOneDlg::OnOK() 
{
	m_nCurSel=m_List.GetCurSel();
	if(m_nCurSel<0) return;
	
	CDialog::OnOK();
}

BOOL CSelectOneDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	for(int i=0;i<m_Item.size();i++)
		m_List.AddString(m_Item[i].ItemStr);
	m_List.SetCurSel(m_nCurSel);

	UpdateData(FALSE);
	return TRUE;
}

void CSelectOneDlg::OnDblclkList1() 
{
	OnOK();	
}
