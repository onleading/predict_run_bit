// TjWarnConfDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "TjWarnConfDlg.h"
#include "DescZBDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTjWarnConfDlg dialog


CTjWarnConfDlg::CTjWarnConfDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTjWarnConfDlg::IDD, pParent)
{
	m_bModefied = FALSE;
	//{{AFX_DATA_INIT(CTjWarnConfDlg)
	//}}AFX_DATA_INIT
}


void CTjWarnConfDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTjWarnConfDlg)
	DDX_Control(pDX, IDC_PERIODSEL, m_SelPeriod);
	DDX_Control(pDX, IDC_TJSEL, m_SelTj);
	DDX_Control(pDX, IDC_STATUS, m_CStatus);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTjWarnConfDlg, CDialog)
	//{{AFX_MSG_MAP(CTjWarnConfDlg)
	ON_BN_CLICKED(IDC_TJEXPLAIN, OnTjexplain)
	ON_CBN_SELCHANGE(IDC_TJSEL, OnSelchangeTjsel)
	ON_CBN_SELCHANGE(IDC_PERIODSEL, OnSelchangePeriodsel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTjWarnConfDlg message handlers

BOOL CTjWarnConfDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_CStatus.SetTextColor(RGB(240,0,0));
	m_CStatus.SetText(_F("注意：条件预警在下载的日线数据中进行,请确保数据完整"));
	
	m_SelTj.SetItemHeight(0,16);
	m_SelTj.SetItemHeight(-1,16);
	m_SelPeriod.SetItemHeight(0,16);
	m_SelPeriod.SetItemHeight(-1,16);	

	if(m_bModefied) m_SelTj.EnableWindow(FALSE);

	CRect rc;
	CStatic *pOutInfoStatic = (CStatic*)GetDlgItem(IDC_PARAMINFO); 
	pOutInfoStatic->GetWindowRect(&rc);
	ScreenToClient(&rc);
	rc.DeflateRect(4,4);
	rc.top += 10;
	m_Form.Create(NULL, "", WS_CHILD|WS_VISIBLE, rc, this, ID_CONTROL_VIEW , NULL);
	m_Form.Init(rc);
	m_Form.HideSelf();	

	CString tempStr;
	int nTotalNum = g_pCalc->_CallDTopViewCalc_7(TJ_TYPE);
	for(int i = 0;i < nTotalNum;i++)	//填充条件选股指标列表框
	{
		tempStr.Format("%-8s - %-16s",(*g_pCalc)[TJ_TYPE][i].acCode,(*g_pCalc)[TJ_TYPE][i].acName);
		m_SelTj.AddString(_F(tempStr));
	}
	if(i > 0 && m_warnTjConf.nTjIndexNo<0)
	{
		m_SelTj.SetCurSel(0);
		m_warnTjConf.nTjIndexNo = 0;
	}
	else if(m_warnTjConf.nTjIndexNo>-1)
		m_SelTj.SetCurSel(m_warnTjConf.nTjIndexNo);
	else m_warnTjConf.nTjIndexNo=-1;

	for(i = 0; g_CalcPeriodStr[i] != NULL ;i++)
		m_SelPeriod.AddString(_F(g_CalcPeriodStr[i]));
	m_SelPeriod.SetCurSel (m_warnTjConf.nPeriod);	//默认是日线
	m_warnTjConf.nPeriod = DAY_ST;
	m_Form.SetStatus(TJ_TYPE, m_warnTjConf.nTjIndexNo, DAY_ST);
	m_SelTj.SetItemHeight (0,16);

	UpdateData(FALSE);
	return TRUE;
}

void CTjWarnConfDlg::OnTjexplain() 
{
	if(m_warnTjConf.nTjIndexNo >= 0)
	{
		CDescZBDlg dlg;
		TINDEXINFO	* pTJIndex = g_pCalc->_CallDTopViewCalc_8(TJ_TYPE,(long)m_warnTjConf.nTjIndexNo);
		if(!pTJIndex) return;
		dlg.Init(TJ_TYPE, pTJIndex->acCode);
		dlg.DoModal();
	}
}

void CTjWarnConfDlg::OnSelchangeTjsel() 
{
	int nCurSel = m_SelTj.GetCurSel();
	if(nCurSel != m_warnTjConf.nTjIndexNo)
	{
		m_warnTjConf.nTjIndexNo = nCurSel;
		m_Form.HideSelf();
		m_Form.SetStatus(TJ_TYPE, m_warnTjConf.nTjIndexNo, m_warnTjConf.nPeriod);
	}
}

void CTjWarnConfDlg::OnSelchangePeriodsel() 
{
	m_warnTjConf.nPeriod = m_SelPeriod.GetCurSel();
	//得新设置m_Form的状态
	m_Form.HideSelf();
	m_Form.SetStatus(TJ_TYPE, m_warnTjConf.nTjIndexNo, m_warnTjConf.nPeriod);
}

void CTjWarnConfDlg::OnOK() 
{
	CString tmpStr;
	TINDEXINFO	* pTJIndex = g_pCalc->_CallDTopViewCalc_8(TJ_TYPE,(long)m_warnTjConf.nTjIndexNo);
	if(pTJIndex)
	{
		strcpy(m_warnTjConf.acCode, (LPCSTR)pTJIndex->acCode);
		memset(m_warnTjConf.aTjParam, 0, sizeof(float) * MAX_PARAMNUM);
		if(m_warnTjConf.nTjIndexNo < 0)	return;
		m_Form.GetCurrData(m_warnTjConf.aTjParam);
		CDialog::OnOK();
	}
}
