// SimOptDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "SimOptDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSimOptDlg dialog


CSimOptDlg::CSimOptDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSimOptDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSimOptDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	nCurSelScheme = CurrentScheme;
	m_nMinn = g_nMinnNum;
	m_nDayn = g_nDaynNum;
}


void CSimOptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSimOptDlg)
	DDX_Control(pDX, IDC_SCHEME_COMBO, m_Scheme_Combo);
	DDX_Text(pDX, IDC_MINN, m_nMinn);
	DDV_MinMaxUInt(pDX, m_nMinn, 1, 200);
	DDX_Text(pDX, IDC_DAYN, m_nDayn);
	DDV_MinMaxUInt(pDX, m_nDayn, 1, 300);
	DDX_Control(pDX, IDC_ZS3_STATIC, m_ZS3_Static);
	DDX_Control(pDX, IDC_ZS2_STATIC, m_ZS2_Static);
	DDX_Control(pDX, IDC_ZS1_STATIC, m_ZS1_Static);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSimOptDlg, CDialog)
	//{{AFX_MSG_MAP(CSimOptDlg)
	ON_BN_CLICKED(IDC_ZS1_BUTTON, OnZs1Button)
	ON_BN_CLICKED(IDC_ZS2_BUTTON, OnZs2Button)
	ON_BN_CLICKED(IDC_ZS3_BUTTON, OnZs3Button)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSimOptDlg message handlers

BOOL CSimOptDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	for(int i = 0; i < SchemeNum; i++)
		m_Scheme_Combo.InsertString(i,_F(SchemeStr[i]));	//SchemeStrÎªÒ»È«¾Ö±äÁ¿
	
	UI = Scheme[CurrentScheme];
	m_Scheme_Combo.SetCurSel(CurrentScheme);
	nCurSelScheme = CurrentScheme;

	m_ZS1_Static.SetTextColor(RGB(0,64,255));
	m_ZS2_Static.SetTextColor(RGB(0,64,255));
	m_ZS3_Static.SetTextColor(RGB(0,64,255));
	m_ZS1_Static.SetWindowText(g_StatusGP[0].lpScript);
	m_ZS2_Static.SetWindowText(g_StatusGP[1].lpScript);
	m_ZS3_Static.SetWindowText(g_StatusGP[2].lpScript);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

#include "SelectGPDlg.h"
#include "InputName.h"
void CSimOptDlg::OnZs1Button() 
{
	CSelectGPDlg dlg;
	dlg.bSingleSelect = TRUE;
	if(dlg.DoModal() == IDOK)
	{
		if(dlg.SelectedGPNum > 0)
		{
			MemStkInfo *tmpInfo = (*g_pStockIO)[dlg.SelectedGPIndex[0]];
			if(tmpInfo)
			{
				CInputName dlg;
				dlg.HintInfo = "ÇëÊäÈëÏÔÊ¾×Ö´®:";
				dlg.Caption = "ÏÔÊ¾×Ö´®";
				CString tmpStr=tmpInfo->Name;
				dlg.m_NewName = tmpStr.Left(8);
				if(dlg.DoModal()==IDOK)
				{
					strncpy(g_StatusGP[0].lpScript,dlg.m_NewName, 19);
					g_StatusGP[0].lpScript[19]=0;
					g_StatusGP[0].setcode=tmpInfo->setcode;
					strncpy(g_StatusGP[0].Code,tmpInfo->Code,SH_CODE_LEN);
					GetDlgItem(IDC_ZS1_STATIC)->SetWindowText(g_StatusGP[0].lpScript);
				}
			}
		}
	}
}

void CSimOptDlg::OnZs2Button() 
{
	CSelectGPDlg dlg;
	dlg.bSingleSelect = TRUE;
	if(dlg.DoModal() == IDOK)
	{
		if(dlg.SelectedGPNum > 0)
		{
			MemStkInfo *tmpInfo = (*g_pStockIO)[dlg.SelectedGPIndex[0]];
			if(tmpInfo)
			{
				CInputName dlg;
				dlg.HintInfo = "ÇëÊäÈëÏÔÊ¾×Ö´®:";
				dlg.Caption = "ÏÔÊ¾×Ö´®";
				CString tmpStr=tmpInfo->Name;
				dlg.m_NewName = tmpStr.Left(8);
				if(dlg.DoModal()==IDOK)
				{
					strncpy(g_StatusGP[1].lpScript,dlg.m_NewName,19);
					g_StatusGP[1].lpScript[19]=0;
					g_StatusGP[1].setcode=tmpInfo->setcode;
					strncpy(g_StatusGP[1].Code,tmpInfo->Code,SH_CODE_LEN);
					GetDlgItem(IDC_ZS2_STATIC)->SetWindowText(g_StatusGP[1].lpScript);
				}
			}
		}
	}
}

void CSimOptDlg::OnZs3Button() 
{
	CSelectGPDlg dlg;
	dlg.bSingleSelect = TRUE;
	if(dlg.DoModal() == IDOK)
	{
		if(dlg.SelectedGPNum > 0)
		{
			MemStkInfo *tmpInfo = (*g_pStockIO)[dlg.SelectedGPIndex[0]];
			if(tmpInfo)
			{
				CInputName dlg;
				dlg.HintInfo = "ÇëÊäÈëÏÔÊ¾×Ö´®:";
				dlg.Caption = "ÏÔÊ¾×Ö´®";
				CString tmpStr=tmpInfo->Name;
				dlg.m_NewName = tmpStr.Left(8);
				if(dlg.DoModal()==IDOK)
				{
					strncpy(g_StatusGP[2].lpScript,dlg.m_NewName,19);
					g_StatusGP[2].lpScript[19]=0;
					g_StatusGP[2].setcode=tmpInfo->setcode;
					strncpy(g_StatusGP[2].Code,tmpInfo->Code,SH_CODE_LEN);
					GetDlgItem(IDC_ZS3_STATIC)->SetWindowText(g_StatusGP[2].lpScript);
				}
			}
		}
	}	
}

void CSimOptDlg::OnOK() 
{
	UpdateData(TRUE);
	nCurSelScheme = m_Scheme_Combo.GetCurSel();
	
	CDialog::OnOK();
}
