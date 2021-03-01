// AuthPassMng.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "AuthPassMng.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAuthPassMng dialog


CAuthPassMng::CAuthPassMng(CWnd* pParent /*=NULL*/)
	: CDialog(CAuthPassMng::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAuthPassMng)
	//}}AFX_DATA_INIT
}


void CAuthPassMng::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAuthPassMng)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAuthPassMng, CDialog)
	//{{AFX_MSG_MAP(CAuthPassMng)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_BN_CLICKED(IDC_MODIFY, OnModify)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAuthPassMng message handlers

void CAuthPassMng::OnOK() 
{
}

void CAuthPassMng::OnApply() 
{
	ShellExecute(NULL,"open",wtdef.UrlApplyAuthPass,NULL,NULL,SW_SHOWNORMAL);
}

void CAuthPassMng::OnModify() 
{
	ShellExecute(NULL,"open",wtdef.UrlModifyAuthPass,NULL,NULL,SW_SHOWNORMAL);
}

BOOL CAuthPassMng::OnInitDialog() 
{
	CDialog::OnInitDialog();
		
	return TRUE;
}
