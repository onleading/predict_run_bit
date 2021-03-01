// GetTxtDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "GetTxtDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGetTxtDlg dialog


CGetTxtDlg::CGetTxtDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGetTxtDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGetTxtDlg)
	m_strContent = _T("");
	//}}AFX_DATA_INIT
}


void CGetTxtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGetTxtDlg)
	DDX_Control(pDX, IDC_CONTENT_EDIT, m_Content_Edit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGetTxtDlg, CDialog)
	//{{AFX_MSG_MAP(CGetTxtDlg)
	ON_BN_CLICKED(IDC_FROMFILE, OnFromfile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGetTxtDlg message handlers

BOOL CGetTxtDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	
	return TRUE;
}

void CGetTxtDlg::OnFromfile() 
{
	CString m_strFilePath;
	CFileDialog FileDialog(TRUE,NULL,NULL,
							OFN_NOCHANGEDIR,
							"文本文件(*.txt)|*.txt|所有文件(*.*)|*.*||");
	if (FileDialog.DoModal() == IDOK)
	{
		m_strFilePath = FileDialog.GetPathName();
		CFile theFile;
		if(theFile.Open(m_strFilePath,CFile::modeRead))
		{
			DWORD tmpLen = theFile.GetLength();
			char *buf = new char[tmpLen+1];
			theFile.Read(buf,tmpLen);
			buf[tmpLen] = 0;
			m_Content_Edit.SetWindowText(buf);
			delete buf;
			theFile.Close();
		}
	}	
}

void CGetTxtDlg::OnOK() 
{
	m_Content_Edit.GetWindowText(m_strContent);
	CDialog::OnOK();
}
