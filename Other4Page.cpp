// Other4Page.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "Other4Page.h"
#include "UFxt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COther4Page dialog

IMPLEMENT_DYNCREATE(COther4Page, CPropertyPage)

COther4Page::COther4Page(): CPropertyPage(COther4Page::IDD)
{
	//{{AFX_DATA_INIT(COther4Page)
	//}}AFX_DATA_INIT
	m_bPillarBSChart = g_GH.bPillarBSChart;
	m_bBoldZSTLine = g_GH.bBoldZSTLine;
	m_bLeftHQInfo = g_GH.bLeftHQInfo;
	m_bBigStatusBar = g_GH.bBigStatusBar;
	m_bBigHq = g_GH.bBigHq;
	m_nShowJunj = g_GH.nShowJUNJ;
	m_bLocalData = g_GH.bLocalData;

	m_bHasInitKNum = (g_GH.nInitKNum>=MIN_SHOWNUM);
	m_nInitKNum = m_bHasInitKNum?max(MIN_SHOWNUM, g_GH.nInitKNum):120;
	m_bHasInitKNum2 = (g_GH.nInitKNum2>=MIN_SHOWNUM);
	m_nInitKNum2 = m_bHasInitKNum2?max(MIN_SHOWNUM, g_GH.nInitKNum2):120;

	m_bShowKGap = g_GH.bShowKGap;
	m_bShowMmpChg = g_GH.bShowMmpChg;
	m_bShowBS = g_GH.bShowBS;
	m_bPinAssistPad = g_GH.bPinAssistPad;
	m_bShowTPPriceInZST = g_GH.bShowTPPriceInZST;
	m_bDiffVolColor = g_GH.bDiffVolColor;	
	m_bEmptyZstRight = g_GH.bEmptyZSTRight;
	m_bZsShowAverage = g_GH.bZsShowAverage;
	m_bZsShowLX = g_GH.bZsShowLX;
	m_bDenseZSTAxis = g_GH.bDenseZSTAxis;
	m_bHq10Bottom = g_GH.bHq10Bottom;	
}

COther4Page::~COther4Page()
{
	
}

void COther4Page::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COther4Page)
	DDX_Check(pDX, IDC_DIFFVOLCOLOR_CHECK, m_bDiffVolColor);
	DDX_Control(pDX, IDC_ZS3_STATIC, m_ZS3_Static);
	DDX_Control(pDX, IDC_ZS2_STATIC, m_ZS2_Static);
	DDX_Control(pDX, IDC_ZS1_STATIC, m_ZS1_Static);
	DDX_Check(pDX, IDC_PILLARBSCHART_CHECK, m_bPillarBSChart);
	DDX_Check(pDX, IDC_BIGHQ_CHECK, m_bBigHq);
	DDX_Check(pDX, IDC_BOLDZSTLINE_CHECK, m_bBoldZSTLine);
	DDX_Check(pDX, IDC_LEFTHQINFO_CHECK, m_bLeftHQInfo);
	DDX_Check(pDX, IDC_BIGSTATUSBAR_CHECK, m_bBigStatusBar);

	DDX_Text(pDX, IDC_KNUM_EDIT, m_nInitKNum);
	DDX_Check(pDX, IDC_FXTKNUM, m_bHasInitKNum);
	DDX_Text(pDX, IDC_KNUM2_EDIT, m_nInitKNum2);
	DDX_Check(pDX, IDC_FXTKNUM2, m_bHasInitKNum2);

	DDX_Check(pDX, IDC_SHOW_BS, m_bShowBS);
	DDX_Check(pDX, IDC_CHECK_SHOWGAP, m_bShowKGap);
	DDX_Radio(pDX, IDC_RADIO1, m_nShowJunj);
	DDX_Check(pDX, IDC_CHECK_MMPCHANGE, m_bShowMmpChg);
	DDX_Check(pDX, IDC_PINAPAD, m_bPinAssistPad);
	DDX_Check(pDX, IDC_SHOWTPPRICEINZST_CHECK, m_bShowTPPriceInZST);
	DDX_Check(pDX, IDC_CHECK_LOCALDATA, m_bLocalData);
	DDX_Check(pDX, IDC_EMPTYZSTRIGHT_CHECK, m_bEmptyZstRight);
	DDX_Check(pDX, IDC_ZSSHOWAVERAGE,m_bZsShowAverage);
	DDX_Check(pDX, IDC_ZSSHOWLX,m_bZsShowLX);
	DDX_Check(pDX, IDC_ZSTDENSEAXIS,m_bDenseZSTAxis);
	DDX_Check(pDX, IDC_HQ10BOTTOM, m_bHq10Bottom);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COther4Page, CPropertyPage)
	//{{AFX_MSG_MAP(COther4Page)
	ON_BN_CLICKED(IDC_ZS1_BUTTON, OnZs1Button)
	ON_BN_CLICKED(IDC_ZS2_BUTTON, OnZs2Button)
	ON_BN_CLICKED(IDC_ZS3_BUTTON, OnZs3Button)
	ON_BN_CLICKED(IDC_PILLARBSCHART_CHECK, OnPillarChartCheck)
	ON_BN_CLICKED(IDC_BIGHQ_CHECK, OnBighqCheck)
	ON_BN_CLICKED(IDC_FXTKNUM, OnFxtknum)
	ON_BN_CLICKED(IDC_FXTKNUM2, OnFxtknum2)
	ON_BN_CLICKED(IDC_CHECK_LOCALDATA, OnCheckLocaldata)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COther4Page message handlers
BOOL COther4Page::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_ZS1_Static.SetTextColor(RGB(0,64,255));
	m_ZS2_Static.SetTextColor(RGB(0,64,255));
	m_ZS3_Static.SetTextColor(RGB(0,64,255));
	m_ZS1_Static.SetWindowText(g_StatusGP[0].lpScript);
	m_ZS2_Static.SetWindowText(g_StatusGP[1].lpScript);
	m_ZS3_Static.SetWindowText(g_StatusGP[2].lpScript);

	GetDlgItem(IDC_PILLARBSCHART_CHECK)->EnableWindow(!m_bBigHq);
	GetDlgItem(IDC_RADIO1)->EnableWindow(!m_bBigHq&&!m_bPillarBSChart);
	GetDlgItem(IDC_RADIO2)->EnableWindow(!m_bBigHq&&!m_bPillarBSChart);
	GetDlgItem(IDC_RADIO3)->EnableWindow(!m_bBigHq&&!m_bPillarBSChart);
	GetDlgItem(IDC_KNUM_EDIT)->EnableWindow(m_bHasInitKNum);
	GetDlgItem(IDC_KNUM2_EDIT)->EnableWindow(m_bHasInitKNum2);
	
	return TRUE;
}

#include "SelectGPDlg.h"
#include "InputName.h"
void COther4Page::OnZs1Button() 
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
				dlg.HintInfo = "请输入显示字串:";
				dlg.Caption = "显示字串";
				CString tmpStr=tmpInfo->Name;
				dlg.m_NewName = tmpStr.Left(8);
				if(dlg.DoModal()==IDOK)
				{
					strncpy(g_StatusGP[0].lpScript,dlg.m_NewName, 19);
					g_StatusGP[0].lpScript[4]=0;
					g_StatusGP[0].setcode=tmpInfo->setcode;
					strncpy(g_StatusGP[0].Code,tmpInfo->Code,SH_CODE_LEN);
					GetDlgItem(IDC_ZS1_STATIC)->SetWindowText(g_StatusGP[0].lpScript);
				}
			}
		}
	}
}

void COther4Page::OnZs2Button() 
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
				dlg.HintInfo = "请输入显示字串:";
				dlg.Caption = "显示字串";
				CString tmpStr=tmpInfo->Name;
				dlg.m_NewName = tmpStr.Left(8);
				if(dlg.DoModal()==IDOK)
				{
					strncpy(g_StatusGP[1].lpScript,dlg.m_NewName,19);
					g_StatusGP[1].lpScript[4]=0;
					g_StatusGP[1].setcode=tmpInfo->setcode;
					strncpy(g_StatusGP[1].Code,tmpInfo->Code,SH_CODE_LEN);
					GetDlgItem(IDC_ZS2_STATIC)->SetWindowText(g_StatusGP[1].lpScript);
				}
			}
		}
	}
}

void COther4Page::OnZs3Button() 
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
				dlg.HintInfo = "请输入显示字串:";
				dlg.Caption = "显示字串";
				CString tmpStr=tmpInfo->Name;
				dlg.m_NewName = tmpStr.Left(8);
				if(dlg.DoModal()==IDOK)
				{
					strncpy(g_StatusGP[2].lpScript,dlg.m_NewName,19);
					g_StatusGP[2].lpScript[4]=0;
					g_StatusGP[2].setcode=tmpInfo->setcode;
					strncpy(g_StatusGP[2].Code,tmpInfo->Code,SH_CODE_LEN);
					GetDlgItem(IDC_ZS3_STATIC)->SetWindowText(g_StatusGP[2].lpScript);
				}
			}
		}
	}	
}

void COther4Page::OnOK() 
{
	UpdateData(TRUE);

	if(m_nInitKNum<MIN_SHOWNUM || m_nInitKNum2<MIN_SHOWNUM)
		TDX_MessageBox(m_hWnd,"初始K线数设得太小，请重新设置",MB_OK|MB_ICONINFORMATION);
	CString	Ini = g_WSXHStr+g_strUserCfg;	
	WritePrivateProfileString("Other","BigStatusBar",m_bBigStatusBar?"1":"0",Ini);

	CPropertyPage::OnOK();
}

void COther4Page::OnBighqCheck() 
{
	UpdateData(TRUE);
	GetDlgItem(IDC_PILLARBSCHART_CHECK)->EnableWindow(!m_bBigHq);
	GetDlgItem(IDC_RADIO1)->EnableWindow(!m_bBigHq);
	GetDlgItem(IDC_RADIO2)->EnableWindow(!m_bBigHq);
	GetDlgItem(IDC_RADIO3)->EnableWindow(!m_bBigHq);	
}

void COther4Page::OnPillarChartCheck()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_RADIO1)->EnableWindow(!m_bPillarBSChart);
	GetDlgItem(IDC_RADIO2)->EnableWindow(!m_bPillarBSChart);
	GetDlgItem(IDC_RADIO3)->EnableWindow(!m_bPillarBSChart);	
}

void COther4Page::OnFxtknum() 
{
	UpdateData(TRUE);
	GetDlgItem(IDC_KNUM_EDIT)->EnableWindow(m_bHasInitKNum);
}

void COther4Page::OnFxtknum2() 
{
	UpdateData(TRUE);
	GetDlgItem(IDC_KNUM2_EDIT)->EnableWindow(m_bHasInitKNum2);
}

void COther4Page::OnCheckLocaldata() 
{
	UpdateData(TRUE);
	if(m_bLocalData && TDX_MessageBox(m_hWnd,"使用本地数据模式时,分析图中的日/周/月/季/年线数据将直接使用盘后下载数据,\r\n请注意保持本地数据的正确完整.\r\n\r\n确认打开此选项吗?[不推荐使用]",MB_YESNO|MB_ICONINFORMATION)==IDNO)
	{
		m_bLocalData=FALSE;
		UpdateData(FALSE);
	}
}
