// DescZBDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "DescZBDlg.h"
#include "interface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDescZBDlg dialog


CDescZBDlg::CDescZBDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDescZBDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDescZBDlg)
	m_strDesc = _T("");
	//}}AFX_DATA_INIT
}


void CDescZBDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDescZBDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDescZBDlg, CDialog)
	//{{AFX_MSG_MAP(CDescZBDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDescZBDlg message handlers

void CDescZBDlg::Init(int nSet, TCHAR *acCode)
{
	TINDEXINFO *tmp;
	tmp = g_pCalc->_CallDTopViewCalc_8(nSet, acCode);
	if(!tmp) return;
	m_strDesc = _F(tmp->lpszHelp);
	if(strlen(m_strDesc)==0)
		m_strDesc=_F("无用法注释");
	m_strTitle.Format("[%s]指标用法", acCode);
}

void CDescZBDlg::Init(int nSet, long nIndex, BOOL bIsSpecialLine)
{
	if (bIsSpecialLine)
	{
		m_strDesc = _F(g_SpeclIndex[nIndex].lpszHelp);
		m_strTitle.Format("[%s]指标用法", g_SpeclIndex[nIndex].acCode);
	}
	else
	{
		TINDEXINFO *tmp;
		tmp = g_pCalc->_CallDTopViewCalc_8(nSet, nIndex);
		if(!tmp) return;
		m_strDesc = _F(tmp->lpszHelp);
		m_strTitle.Format("[%s]指标用法", tmp->acCode);
	}
	if(strlen(m_strDesc)==0)
		m_strDesc=_F("无用法注释");
}

BOOL CDescZBDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_strDesc.Replace("\r\n","");
	GetDlgItem(IDC_RICHEDIT1)->SetWindowText("\r\n"+m_strDesc);
	
	SetWindowText(_F(m_strTitle));

	UpdateData(FALSE);
	return TRUE;
}
