// AdjustParamDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "AdjustParamDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdjustParamDlg dialog


CAdjustParamDlg::CAdjustParamDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAdjustParamDlg::IDD, pParent)
{
	m_nSet = 0;
	m_lIndex = 0;
	m_nPeriod = 0;
	m_pView = NULL;
	//{{AFX_DATA_INIT(CAdjustParamDlg)
	//}}AFX_DATA_INIT
}


void CAdjustParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdjustParamDlg)
	DDX_Control(pDX, IDC_STATICINFO, m_OutStatic);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdjustParamDlg, CDialog)
	//{{AFX_MSG_MAP(CAdjustParamDlg)
	ON_BN_CLICKED(IDC_DELBTN, OnDelbtn)
	ON_BN_CLICKED(IDC_RESUMEBTN, OnResumebtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdjustParamDlg message handlers

void CAdjustParamDlg::Init(int nSet, long lIndex, int nPeriod, CWnd *pCurrView)
{
	m_nSet = nSet;
	m_lIndex = lIndex;
	m_nPeriod = nPeriod;
	m_pView = pCurrView;
}

BOOL CAdjustParamDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CString tmpStr("");
	CRect rc;
	
	m_pIndex = g_pCalc->_CallDTopViewCalc_8(m_nSet, m_lIndex);
	if (m_pIndex == NULL) return FALSE;
	m_OutStatic.GetWindowRect(&rc);
	ScreenToClient(&rc);
	rc.DeflateRect(2,2);
	m_Form.Create(NULL, "", WS_CHILD|WS_VISIBLE, rc, this, ID_CONTROL_VIEW , NULL);
	m_Form.Init(rc, m_pView);
	m_Form.HideSelf();
	m_Form.SetStatus(m_nSet, m_lIndex, m_nPeriod);

	switch (m_nSet)
	{
	case 0:
		tmpStr.Format("[%s]指标参数调整",m_pIndex->acCode);
		GetDlgItem(IDC_DELBTN)->ShowWindow(SW_HIDE);
		break;
	case 2:
		tmpStr.Format("[%s]交易系统指示参数调整",m_pIndex->acCode);
		break;
	case 3:
		tmpStr.Format("[%s]五彩K线指示参数调整",m_pIndex->acCode);
		break;
	}
	SetWindowText(_F(tmpStr));
	return TRUE;
}

void CAdjustParamDlg::OnDelbtn() 
{
	if (m_pView != NULL)
		m_pView->PostMessage(WM_KEYDOWN, VK_DELETE);
}

void CAdjustParamDlg::OnResumebtn() 
{
	for (int i = 0; i < m_pIndex->nParaNum; i++)
		m_pIndex->aPara[i].nValue[m_nPeriod] = m_pIndex->aPara[i].nDefault;
	m_Form.HideSelf();
	m_Form.SetStatus(m_nSet, m_lIndex, m_nPeriod);
	m_pView->PostMessage(UM_DYNCHGPARAM);
}

void CAdjustParamDlg::OnOK() 
{
	m_Form.GetCurrData();
	CDialog::OnOK();
}
