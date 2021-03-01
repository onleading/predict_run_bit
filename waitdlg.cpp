// waitdlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "waitdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWaitDlg dialog


CWaitDlg::CWaitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWaitDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWaitDlg)
	//}}AFX_DATA_INIT
	m_strCaption=" ÇëµÈ´ý...";
	m_strContent="";
	m_nWaitSeconds=60;
}


void CWaitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWaitDlg)
	DDX_Control(pDX, IDC_TIME_STATIC, m_Time_Static);
	DDX_Control(pDX, IDC_HINT_STATIC, m_Hint_Static);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWaitDlg, CDialog)
	//{{AFX_MSG_MAP(CWaitDlg)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWaitDlg message handlers

BOOL CWaitDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_Hint_Static.SetTextColor(RGB(255,0,0));
	m_Hint_Static.SetWindowText(m_strContent);
	m_Time_Static.SetTextColor(RGB(0,0,255));

	SetWindowText(g_strPreName+m_strCaption);

	SetTimer(0x823,1000,NULL);
	return TRUE;
}

void CWaitDlg::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent==0x823)
	{
		if(m_nWaitSeconds>0)
		{
			m_nWaitSeconds--;
			CString tmpStr;
			tmpStr.Format("%02d:%02d",m_nWaitSeconds/60,m_nWaitSeconds%60);
			m_Time_Static.SetText(tmpStr);
			if(m_nWaitSeconds==0)
				CDialog::OnOK();
			return;
		}
	}
	CDialog::OnTimer(nIDEvent);
}

void CWaitDlg::OnCancel() 
{
}
