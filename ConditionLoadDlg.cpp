// ConditionLoadDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "ConditionLoadDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConditionLoadDlg dialog


CConditionLoadDlg::CConditionLoadDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConditionLoadDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConditionLoadDlg)
	//}}AFX_DATA_INIT
	ConFlag = 0;
}


void CConditionLoadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConditionLoadDlg)
	DDX_Control(pDX, IDC_CONDITIONFILE_LIST, m_ConditionFile_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConditionLoadDlg, CDialog)
	//{{AFX_MSG_MAP(CConditionLoadDlg)
	ON_LBN_DBLCLK(IDC_CONDITIONFILE_LIST, OnDblclkConditionfileList)
	ON_BN_CLICKED(ID_DELETEIT, OnDeleteit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConditionLoadDlg message handlers

BOOL CConditionLoadDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_ConditionFile_List.SetItemHeight(0,16);

	FillList(0);

	return TRUE;
}

void CConditionLoadDlg::FillList(int whichsel)
{
	m_ConditionFile_List.ResetContent();
	CString tmpFixStr;
	if(ConFlag == 0)	tmpFixStr = "*.cos";
	else if(ConFlag==1)	tmpFixStr = "*.cus";
	else				tmpFixStr = "*.czs";
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile (g_WSXHStr+tmpFixStr, &fd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do 
		{
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) 
				m_ConditionFile_List.AddString(fd.cFileName);
		}while (::FindNextFile (hFind, &fd));
		::FindClose (hFind);	
	}
	if(whichsel >= 0)
		m_ConditionFile_List.SetCurSel(whichsel);
}

void CConditionLoadDlg::OnOK() 
{
	int nSelFile = m_ConditionFile_List.GetCurSel();
	if(nSelFile == LB_ERR)
		return;
	m_ConditionFile_List.GetText (nSelFile,ReturnStr);
	CDialog::OnOK();
}

void CConditionLoadDlg::OnDblclkConditionfileList() 
{
	OnOK();	
}

void CConditionLoadDlg::OnDeleteit()
{
	CString tempStr;
	int nSelFile = m_ConditionFile_List.GetCurSel();
	if(nSelFile != LB_ERR)
	{
		m_ConditionFile_List.GetText (nSelFile,tempStr);
		CString tempFileName = g_WSXHStr+tempStr;
		try
		{
			CFile::Remove(tempFileName);
		}
		catch(...)
		{
			TDX_MessageBox(m_hWnd,"ÎÄ¼þÉ¾³ý´íÎó!",MB_OK|MB_ICONEXCLAMATION);
			return;
		}
	}
	FillList(max(nSelFile-1,0));
}
