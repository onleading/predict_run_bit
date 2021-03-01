// OtherWarnPage.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "OtherWarnPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COtherWarnPage property page

IMPLEMENT_DYNCREATE(COtherWarnPage, CPropertyPage)

COtherWarnPage::COtherWarnPage() : CPropertyPage(COtherWarnPage::IDD)
{
	//{{AFX_DATA_INIT(COtherWarnPage)
	m_bNewMax = FALSE;
	m_bNewMin = FALSE;
	m_fDownRatio = 0.0f;
	m_nLBRatio = 0;
	m_fUpRatio = 0.0f;
	m_fTickDiff = 0.0f;
	m_fTickVol = 0.0f;
	m_fTickAmount = 0.0f;
	m_bPopup = FALSE;
	m_bStopClose = FALSE;
	m_bDownRatio = FALSE;
	m_bLB = FALSE;
	m_bTickAmount = FALSE;
	m_bTickDiff = FALSE;
	m_bTickVol = FALSE;
	m_bUpRatio = FALSE;
	m_bTickHugeAmo = FALSE;
	m_nHugeAmoMinute = 0;
	m_nHugeAmoTime = 0;
	m_nBeepFlag = -1;
	m_strWavFile = _T("");
	m_bBigBuyV = FALSE;
	m_bBigSellV = FALSE;
	m_bHighLow = FALSE;
	m_bLowHigh = FALSE;
	//}}AFX_DATA_INIT
}

COtherWarnPage::~COtherWarnPage()
{
}

void COtherWarnPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COtherWarnPage)
	DDX_Check(pDX, IDC_NEWMAXCHECK, m_bNewMax);
	DDX_Check(pDX, IDC_NEWMINCHECK, m_bNewMin);
	DDX_Check(pDX, IDC_BDOWNRATIO, m_bDownRatio);
	DDX_Text(pDX, IDC_DOWNRATIO, m_fDownRatio);
	DDX_Text(pDX, IDC_LBRATIO, m_nLBRatio);
	DDX_Check(pDX, IDC_BUPRATIO, m_bUpRatio);
	DDX_Text(pDX, IDC_UPRATIO, m_fUpRatio);
	DDX_Text(pDX, IDC_TICKDIFF, m_fTickDiff);
	DDX_Text(pDX, IDC_TICKVOL, m_fTickVol);
	DDX_Text(pDX, IDC_TICKAMOUNT, m_fTickAmount);
	DDX_Check(pDX, IDC_POPUPCHECK, m_bPopup);
	DDX_Check(pDX, IDC_STOPCLOSE, m_bStopClose);
	DDX_Check(pDX, IDC_BLB, m_bLB);
	DDX_Check(pDX, IDC_BTICKAMOUNT, m_bTickAmount);
	DDX_Check(pDX, IDC_BTICKDIFF, m_bTickDiff);
	DDX_Check(pDX, IDC_BTICKVOL, m_bTickVol);
	DDX_Check(pDX, IDC_BTICKHUGEAMO, m_bTickHugeAmo);
	DDX_Text(pDX, IDC_HUGEAMOMINUTE, m_nHugeAmoMinute);
	DDX_Text(pDX, IDC_HUGEAMOTIME, m_nHugeAmoTime);
	DDX_Radio(pDX, IDC_RADIO1, m_nBeepFlag);
	DDX_Text(pDX, IDC_WAVFILE_EDIT, m_strWavFile);
	DDV_MaxChars(pDX, m_strWavFile, 78);
	DDX_Check(pDX, IDC_BIGBUYV_CHECK, m_bBigBuyV);
	DDX_Check(pDX, IDC_BIGSELLV_CHECK, m_bBigSellV);
	DDX_Check(pDX, IDC_HIGHLOW, m_bHighLow);
	DDX_Check(pDX, IDC_LOWHIGH, m_bLowHigh);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COtherWarnPage, CPropertyPage)
	//{{AFX_MSG_MAP(COtherWarnPage)
	ON_EN_KILLFOCUS(IDC_UPRATIO, OnKillfocusUpratio)
	ON_EN_KILLFOCUS(IDC_DOWNRATIO, OnKillfocusDownratio)
	ON_EN_KILLFOCUS(IDC_TICKDIFF, OnKillfocusTickdiff)
	ON_EN_KILLFOCUS(IDC_LBRATIO, OnKillfocusLbratio)
	ON_EN_KILLFOCUS(IDC_TICKAMOUNT, OnKillfocusTickamount)
	ON_EN_KILLFOCUS(IDC_TICKVOL, OnKillfocusTickvol)
	ON_BN_CLICKED(IDC_DEFAULT_BUTTON, OnDefaultButton)
	ON_BN_CLICKED(IDC_BUPRATIO, OnBupratio)
	ON_BN_CLICKED(IDC_BDOWNRATIO, OnBdownratio)
	ON_BN_CLICKED(IDC_BTICKDIFF, OnBtickdiff)
	ON_BN_CLICKED(IDC_BLB, OnBlb)
	ON_BN_CLICKED(IDC_BTICKAMOUNT, OnBtickamount)
	ON_BN_CLICKED(IDC_BTICKHUGEAMO, OnBtickhugeamo)
	ON_BN_CLICKED(IDC_BROWSE,OnBrowse)
	ON_BN_CLICKED(IDC_RADIO1, OnBeepRadio)
	ON_BN_CLICKED(IDC_RADIO2, OnBeepRadio)
	ON_BN_CLICKED(IDC_RADIO3, OnBeepRadio)
	ON_BN_CLICKED(IDC_TEST, OnTest)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COtherWarnPage message handlers

BOOL COtherWarnPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_bNewMax =		g_WarnParam.bNewMaxWarn ;
	m_bNewMin =		g_WarnParam.bNewMinWarn ;
	m_bStopClose =	g_WarnParam.bStopClose;
	m_bUpRatio	 =	g_WarnParam.bUpRatio;
	m_bDownRatio =	g_WarnParam.bDownRatio;
	m_bLB		 =	g_WarnParam.bLB;
	m_bTickDiff	 =	g_WarnParam.bTickDiff;
	m_bTickAmount =	g_WarnParam.bTickAmount;
	m_bTickVol	 =	g_WarnParam.bTickVol;
	m_bTickHugeAmo = g_WarnParam.bTickHugeAmo;

	m_fUpRatio =	g_WarnParam.fUpRatio*100 ;
	m_fDownRatio =	g_WarnParam.fDownRatio*100 ;
	m_nLBRatio =	g_WarnParam.nLB;	
	m_fTickDiff =	g_WarnParam.fTickDiff*100 ;
	m_fTickVol =	g_WarnParam.fTickVol*100 ;
	m_fTickAmount =	g_WarnParam.fTickAmount/10000.0;

	m_nHugeAmoMinute = g_WarnParam.nTickAmoMinute;
	m_nHugeAmoTime = g_WarnParam.nTickAmoTime;

	m_bBigBuyV = g_WarnParam.bHasBigBuy;
	m_bBigSellV = g_WarnParam.bHasBigSell;
	m_bHighLow = g_WarnParam.bHighLow;
	m_bLowHigh = g_WarnParam.bLowHigh;
	
	m_bPopup = g_WarnParam.bPopup ;
	m_nBeepFlag = g_WarnParam.nBeepFlag;
	m_strWavFile = g_WarnParam.WavFile;

	DisableSome();
	
	UpdateData(FALSE);

	return TRUE;
}

void COtherWarnPage::DisableSome()
{
	GetDlgItem(IDC_UPRATIO)->EnableWindow(m_bUpRatio);
	GetDlgItem(IDC_DOWNRATIO)->EnableWindow(m_bDownRatio);
	GetDlgItem(IDC_TICKDIFF)->EnableWindow(m_bTickDiff);
	GetDlgItem(IDC_TICKAMOUNT)->EnableWindow(m_bTickAmount);
	GetDlgItem(IDC_LBRATIO)->EnableWindow(m_bLB);
	GetDlgItem(IDC_HUGEAMOMINUTE)->EnableWindow(m_bTickHugeAmo);
	GetDlgItem(IDC_HUGEAMOTIME)->EnableWindow(m_bTickHugeAmo);

	GetDlgItem(IDC_BROWSE)->EnableWindow(m_nBeepFlag==2);
	GetDlgItem(IDC_TEST)->EnableWindow(m_nBeepFlag==2);
	GetDlgItem(IDC_WAVFILE_EDIT)->EnableWindow(m_nBeepFlag==2);	
}

void COtherWarnPage::OnKillfocusUpratio() 
{
	UpdateData(TRUE);
	if(m_fUpRatio < 1.0) m_fUpRatio = 1.0;
	UpdateData(FALSE);
}

void COtherWarnPage::OnKillfocusDownratio() 
{
	UpdateData(TRUE);
	if(m_fDownRatio < 1.0) m_fDownRatio = 1.0;
	UpdateData(FALSE);	
}

void COtherWarnPage::OnKillfocusTickdiff() 
{
	UpdateData(TRUE);
	if(m_fTickDiff < 0.1f) m_fTickDiff = 0.1f;
	UpdateData(FALSE);	
}

void COtherWarnPage::OnKillfocusLbratio() 
{
	UpdateData(TRUE);
	if(m_nLBRatio < 2) m_nLBRatio = 2;
	UpdateData(FALSE);	
}

void COtherWarnPage::OnKillfocusTickamount() 
{
	UpdateData(TRUE);
	if(m_fTickAmount < 10.0f ) m_fTickAmount = 10.0f;
	UpdateData(FALSE);
}

void COtherWarnPage::OnKillfocusTickvol() 
{
	UpdateData(TRUE);
	if(m_fTickVol < 0.001f) m_fTickVol = 0.001f;
	UpdateData(FALSE);	
}

void COtherWarnPage::OnOK() 
{
	UpdateData(TRUE);

	g_WarnParam.bNewMaxWarn	=	m_bNewMax;
	g_WarnParam.bNewMinWarn =	m_bNewMin;
	g_WarnParam.bStopClose  =	m_bStopClose;
	g_WarnParam.bUpRatio	=	m_bUpRatio;
	g_WarnParam.bDownRatio	=	m_bDownRatio;
	g_WarnParam.bTickDiff	=	m_bTickDiff;
	g_WarnParam.bTickAmount	=	m_bTickAmount;
	g_WarnParam.bTickVol	=	m_bTickVol;
	g_WarnParam.bLB			=	m_bLB;
	g_WarnParam.bTickHugeAmo = m_bTickHugeAmo;

	g_WarnParam.fUpRatio	=	m_fUpRatio*0.01;
	g_WarnParam.fDownRatio	=	m_fDownRatio*0.01;
	g_WarnParam.nLB			=	m_nLBRatio;
	g_WarnParam.fTickDiff	=	m_fTickDiff*0.01;
	g_WarnParam.fTickVol	=	m_fTickVol*0.01;	
	g_WarnParam.fTickAmount =	m_fTickAmount*10000.0;

	g_WarnParam.nTickAmoMinute = m_nHugeAmoMinute;
	g_WarnParam.nTickAmoTime = m_nHugeAmoTime;

	g_WarnParam.bHasBigBuy = m_bBigBuyV;
	g_WarnParam.bHasBigSell = m_bBigSellV;
	g_WarnParam.bHighLow = m_bHighLow;
	g_WarnParam.bLowHigh = m_bLowHigh;

	g_WarnParam.bPopup = m_bPopup;
	g_WarnParam.nBeepFlag = m_nBeepFlag;
	strcpy(g_WarnParam.WavFile,m_strWavFile);

	g_nWarnRangeMinute = max(m_nHugeAmoMinute,g_nWarnRangeMinute);
	
	CPropertyPage::OnOK();
}

void COtherWarnPage::OnBrowse()
{
	UpdateData(TRUE);
	CFileDialog FileDialog(TRUE,NULL,NULL,
							OFN_NOCHANGEDIR,
							_F("声音文件(*.wav)|*.wav|全部类型|*.*||"));
	if (FileDialog.DoModal() == IDOK)
	{
		m_strWavFile = FileDialog.GetPathName();
		UpdateData(FALSE);
	}	

}
void COtherWarnPage::OnDefaultButton() 
{
	m_bLB		 = TRUE;
	m_bUpRatio	 = TRUE;
	m_bDownRatio = TRUE;
	m_bTickAmount= TRUE;
	m_bTickDiff	 = TRUE;
	m_bTickVol	 = TRUE;

	m_bNewMax =	 FALSE;
	m_bNewMin =	 FALSE;
	m_bStopClose = TRUE;
	m_bTickHugeAmo = TRUE;

	m_bBigBuyV = TRUE;
	m_bBigSellV = TRUE;
	m_bHighLow = TRUE;
	m_bLowHigh = TRUE;

	m_fUpRatio =	5;	//5%
	m_fDownRatio =	5;	//5%
	m_nLBRatio =	40; //倍
	m_fTickDiff =	1.2f;//1.2%
	m_fTickVol =	0.1f;//0.1%
	m_fTickAmount =	150.0f;//万元
	m_nHugeAmoMinute = 5;
	m_nHugeAmoTime = 3;

	m_bPopup = TRUE;

	DisableSome();
	UpdateData(FALSE);
}

void COtherWarnPage::OnBupratio() 
{
	UpdateData(TRUE);
	GetDlgItem(IDC_UPRATIO)->EnableWindow(m_bUpRatio);
}

void COtherWarnPage::OnBdownratio() 
{
	UpdateData(TRUE);
	GetDlgItem(IDC_DOWNRATIO)->EnableWindow(m_bDownRatio);	
}

void COtherWarnPage::OnBtickdiff() 
{
	UpdateData(TRUE);
	GetDlgItem(IDC_TICKDIFF)->EnableWindow(m_bTickDiff);
}

void COtherWarnPage::OnBlb() 
{
	UpdateData(TRUE);
	GetDlgItem(IDC_LBRATIO)->EnableWindow(m_bLB);
}

void COtherWarnPage::OnBtickamount() 
{
	UpdateData(TRUE);	
	GetDlgItem(IDC_TICKAMOUNT)->EnableWindow(m_bTickAmount);
}

void COtherWarnPage::OnBtickhugeamo() 
{
	UpdateData(TRUE);
	GetDlgItem(IDC_HUGEAMOMINUTE)->EnableWindow(m_bTickHugeAmo);
	GetDlgItem(IDC_HUGEAMOTIME)->EnableWindow(m_bTickHugeAmo);
}

void COtherWarnPage::OnBeepRadio()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_BROWSE)->EnableWindow(m_nBeepFlag==2);
	GetDlgItem(IDC_TEST)->EnableWindow(m_nBeepFlag==2);
	GetDlgItem(IDC_WAVFILE_EDIT)->EnableWindow(m_nBeepFlag==2);	
}

void COtherWarnPage::OnTest() 
{
	UpdateData(TRUE);
	if(m_strWavFile.IsEmpty())
	{
		TDX_MessageBox(NULL,"请选择声音文件!",MB_OK|MB_ICONERROR);
		return;
	}
	::PlaySound(m_strWavFile,NULL,SND_ASYNC);	
}
