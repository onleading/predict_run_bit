// ModifyAbsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "ModifyAbsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModifyAbsDlg dialog


CModifyAbsDlg::CModifyAbsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModifyAbsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModifyAbsDlg)
	m_fDownBreak = 0.0f;
	m_fUpBreak = 0.0f;
	m_fPriceDiff = 0.0f;
	m_fChangeHand = 0.0f;
	//}}AFX_DATA_INIT
	nStockNo = 0;
	m_nCurSel1 = 0;
	m_nCurSel2 = 0;
	m_hIcon = AfxGetApp()->LoadIcon(IDI_WARNTJCON);
	m_bValidPrice = FALSE;
	m_bHasDeleteBtn = FALSE;
	m_bDeleted = FALSE;
	m_pStkInfo = NULL;
	m_nXSFlag = XS2;
}


void CModifyAbsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModifyAbsDlg)
	DDX_Control(pDX, IDC_GPNAME, m_GPName);
	DDX_Control(pDX, IDC_COMBO2, m_Combo2);
	DDX_Control(pDX, IDC_COMBO1, m_Combo1);
	DDX_Text(pDX, IDC_DOWNBREAK_EDIT, m_fDownBreak);
	DDX_Text(pDX, IDC_UPBREAK_EDIT, m_fUpBreak);
	DDX_Text(pDX, IDC_PRICEDIFF_EDIT, m_fPriceDiff);
	DDX_Text(pDX, IDC_CHANGEHAND_EDIT, m_fChangeHand);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModifyAbsDlg, CDialog)
	//{{AFX_MSG_MAP(CModifyAbsDlg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN2, OnDeltaposSpin2)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_EN_KILLFOCUS(IDC_UPBREAK_EDIT, OnKillfocusUpbreakEdit)
	ON_EN_KILLFOCUS(IDC_DOWNBREAK_EDIT, OnKillfocusDownbreakEdit)
	ON_BN_CLICKED(IDC_ALLTJ, OnAlltj)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN3, OnDeltaposSpin3)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN4, OnDeltaposSpin4)
	ON_EN_CHANGE(IDC_UPBREAK_EDIT, OnChangeUpbreakEdit)
	ON_EN_CHANGE(IDC_DOWNBREAK_EDIT, OnChangeDownbreakEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModifyAbsDlg message handlers

BOOL CModifyAbsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_GPName.SetTextColor(RGB(0,0,255));
	m_GPName.SetFontBold(TRUE);
	
	GetDlgItem(IDC_DELETE)->ShowWindow(m_bHasDeleteBtn?SW_SHOW:SW_HIDE);
	GetDlgItem(IDC_ALLTJ)->ShowWindow(m_bHasDeleteBtn?SW_SHOW:SW_HIDE);
	if(!g_OEM.bEmbedWTVersion)
	{
		GetDlgItem(IDC_STATIC1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMBO1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMBO2)->ShowWindow(SW_HIDE);
	}

	SetIcon(m_hIcon,FALSE);
	SetIcon(m_hIcon,TRUE);

	m_Combo1.AddString(_F("无"));
	m_Combo1.AddString(_F("买入"));
	m_Combo1.AddString(_F("卖出"));
	m_Combo1.SetCurSel(m_nCurSel1);

	m_Combo2.AddString(_F("无"));
	m_Combo2.AddString(_F("买入"));
	m_Combo2.AddString(_F("卖出"));
	m_Combo2.SetCurSel(m_nCurSel2);

	m_pStkInfo = (*g_pStockIO)[nStockNo];
	if(m_pStkInfo)
	{
		CString tmpStr;
		tmpStr.Format("%s [%s]",m_pStkInfo->Name,m_pStkInfo->Code);
		SetDlgItemText(IDC_GPNAME,tmpStr);
		m_nXSFlag = GetXSFlag(m_pStkInfo);
	}
	OnChangeUpBreak();
	OnChangeDownBreak();
	return TRUE;
}

void CModifyAbsDlg::OnOK() 
{
	UpdateData(TRUE);
	m_nCurSel1=m_Combo1.GetCurSel();
	m_nCurSel2=m_Combo2.GetCurSel();
	
	if(m_nCurSel1>0 && m_fUpBreak<COMPPREC)
	{
		TDX_MessageBox(m_hWnd,"请输入上破价!",MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	if(m_nCurSel2>0 && m_fDownBreak<COMPPREC)
	{
		TDX_MessageBox(m_hWnd,"请输入上破价!",MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	if(m_fDownBreak < -COMPPREC || m_fUpBreak < -COMPPREC )
	{
		TDX_MessageBox(m_hWnd,"上破价或下破价必须不小于0!",MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	if(m_fDownBreak>COMPPREC && m_fUpBreak>COMPPREC && m_fDownBreak >= m_fUpBreak )
	{
		TDX_MessageBox(m_hWnd,"上破价不能少于下破价!",MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	if( m_bValidPrice && (m_fDownBreak<COMPPREC && m_fUpBreak<COMPPREC && m_fChangeHand<COMPPREC && m_fPriceDiff<0) )
	{
		TDX_MessageBox(m_hWnd,"请输入价位等条件(可以只输入一种条件)!",MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	if(m_fChangeHand < -COMPPREC || m_fChangeHand > 300)
	{
		TDX_MessageBox(m_hWnd,"换手必须在0到300之间!",MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	if(m_fPriceDiff < -COMPPREC || m_fPriceDiff > 100)
	{
		TDX_MessageBox(m_hWnd,"折/溢率必须在0到100之间!",MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	CDialog::OnOK();
}

void CModifyAbsDlg::FormatFloat(float & ftmp,int xsflag)
{
	if(xsflag==XS3)
		ftmp=((int)(ftmp*1000+0.5))/1000.0;
	else
		ftmp=((int)(ftmp*100+0.5))/100.0;
}

void CModifyAbsDlg::OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	UpdateData(TRUE);
	m_fUpBreak -= pNMUpDown->iDelta*(0.01);
	if(m_fUpBreak<COMPPREC)
		m_fUpBreak = 0;
	UpdateData(FALSE);
	OnChangeUpBreak();

	*pResult = 0;
}

void CModifyAbsDlg::OnDeltaposSpin2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	UpdateData(TRUE);
	m_fDownBreak -= pNMUpDown->iDelta*(0.01);
	if(m_fDownBreak<COMPPREC)
		m_fDownBreak = 0;

	UpdateData(FALSE);
	OnChangeDownBreak();

	*pResult = 0;
}


void CModifyAbsDlg::OnDeltaposSpin3(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	UpdateData(TRUE);

	CString tmpStr;
	GetDlgItem(IDC_UPPER_EDIT)->GetWindowText(tmpStr);
	int ltmp = atol(tmpStr);
	ltmp-=pNMUpDown->iDelta;
	GetDlgItem(IDC_UPPER_EDIT)->SetWindowText(IntToStr(ltmp));

	if(m_pStkInfo)
	{
		m_fUpBreak = m_pStkInfo->Close*(1+ltmp*0.01);
		FormatFloat(m_fUpBreak,m_nXSFlag);
	}
	UpdateData(FALSE);
	
	*pResult = 0;
}

void CModifyAbsDlg::OnDeltaposSpin4(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	UpdateData(TRUE);

	CString tmpStr;
	GetDlgItem(IDC_DOWNPER_EDIT)->GetWindowText(tmpStr);
	int ltmp = atol(tmpStr);
	ltmp-=pNMUpDown->iDelta;
	GetDlgItem(IDC_DOWNPER_EDIT)->SetWindowText(IntToStr(ltmp));

	if(m_pStkInfo)
	{
		m_fDownBreak = m_pStkInfo->Close*(1+ltmp*0.01);
		FormatFloat(m_fDownBreak,m_nXSFlag);
	}
	UpdateData(FALSE);

	*pResult = 0;
}

void CModifyAbsDlg::OnChangeUpBreak()
{
	if(m_fUpBreak<COMPPREC)
		GetDlgItem(IDC_UPPER_EDIT)->SetWindowText("");
	else if(m_pStkInfo && m_pStkInfo->Close>COMPPREC)
	{
		float ftmp=(m_fUpBreak-m_pStkInfo->Close)*100/m_pStkInfo->Close;
		FormatFloat(ftmp,XS2);
		CString tempStr;
		tempStr.Format ("%.2f",ftmp);
		GetDlgItem(IDC_UPPER_EDIT)->SetWindowText(tempStr);
	}
}

void CModifyAbsDlg::OnChangeDownBreak()
{
	if(m_fDownBreak<COMPPREC)
		GetDlgItem(IDC_DOWNPER_EDIT)->SetWindowText("");
	else if(m_pStkInfo && m_pStkInfo->Close>COMPPREC)
	{
		float ftmp=(m_fDownBreak-m_pStkInfo->Close)*100/m_pStkInfo->Close;
		FormatFloat(ftmp,XS2);
		CString tempStr;
		tempStr.Format ("%.2f",ftmp);
		GetDlgItem(IDC_DOWNPER_EDIT)->SetWindowText(tempStr);
	}
}

void CModifyAbsDlg::OnDelete() 
{
	m_bDeleted = TRUE;
	OnCancel();
}

void CModifyAbsDlg::OnKillfocusUpbreakEdit() 
{
	UpdateData(TRUE);
	if(m_fUpBreak<COMPPREC)
		GetDlgItem(IDC_UPBREAK_EDIT)->SetWindowText("0");
}

void CModifyAbsDlg::OnKillfocusDownbreakEdit() 
{
	UpdateData(TRUE);
	if(m_fDownBreak<COMPPREC)
		GetDlgItem(IDC_DOWNBREAK_EDIT)->SetWindowText("0");	
}

void CModifyAbsDlg::OnChangeUpbreakEdit() 
{
	UpdateData(TRUE);
	OnChangeUpBreak();
}

void CModifyAbsDlg::OnChangeDownbreakEdit() 
{
	UpdateData(TRUE);
	OnChangeDownBreak();
}

void CModifyAbsDlg::OnAlltj() 
{
	CDialog::OnCancel();
	if(g_pMainWin)
		g_pMainWin->PostMessage(WM_COMMAND,ID_TJWARNCFG);
}
