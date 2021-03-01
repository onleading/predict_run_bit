// OtherPage.cpp : implementation file

#include "stdafx.h"
#include "TdxW.h"
#include "OtherPage.h"
#include "GridView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COtherPage property page

IMPLEMENT_DYNCREATE(COtherPage, CPropertyPage)

int GetTotalMulNumFromIndex(int i)
{
	switch(i)
	{
	case 1:	return 9;	break;
	case 2:	return 16;	break;
	case 3: return 25;	break;
	case 4:	return 6;	break;
	case 5:	return 12;	break;
	case 6:	return 20;	break;
	}
	return 4;
}

int GetIndexFromTotalMulNum(int i)
{
	switch(i)
	{
	case 9:		return 1;	break;
	case 16:	return 2;	break;
	case 25:	return 3;	break;
	case 6:		return 4;	break;
	case 12:	return 5;	break;
	case 20:	return 6;	break;
	}
	return 0;
}

COtherPage::COtherPage() : CPropertyPage(COtherPage::IDD)
{
	m_AutoPageTime = AutoPageTime;
	m_bLongSquare = !g_bLongHighLine;

	m_bXaxis = g_bHasXHidden;
	m_bYaxis = g_bHasYHidden;

	m_nChartNum = GetIndexFromTotalMulNum(g_nNewMulNum);

	m_nHligtNowVol = g_nHligtNowVol;

	m_nZFMin = g_nZFFzNum;
	m_nMinn = g_nMinnNum;
	m_nDayn = g_nDaynNum;

	m_bVolTQ = g_bVolTQ;
	m_bNotifyDownload = g_bNotifyDownload;
	m_bHisViewTQ = g_bHisViewTQ;
	m_nToolBarPos = g_nToolBarPos;
	m_bAutoHideBar = g_bAutoHideBar;
	m_bShowHqMine = g_bShowHqMine;
	m_bDrawBlue = g_bDrawBlue;
	m_bNoXHCol = g_GH.NoXHCol;
	m_bNoVScroll = g_GH.NoVScroll;
	m_bFixedXAxis = g_bFixedXAxis;
	m_nFixedXAxis = g_nFixedXAxis;
	m_bOnlyDownAGData = g_GH.bOnlyDownAGData;

	m_bHlight_HighLow = g_GH.bHlight_HighLow;
	m_bLockHQSort=g_GH.bLockHQSort;
	m_bCodeCanSort = g_GH.bCodeCanSort;
}

COtherPage::~COtherPage()
{
}

void COtherPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COtherPage)
	DDX_Control(pDX, IDC_MULTIGPNUM_COMBO, m_ChartCombo);
	DDX_Text(pDX, IDC_AUTOPAGETIME, m_AutoPageTime);
	DDX_Check(pDX, IDC_XAXIS_CHECK, m_bXaxis);
	DDX_Check(pDX, IDC_YAXIS_CHECK, m_bYaxis);
	DDX_Text(pDX, IDC_ZFMIN, m_nZFMin);
	DDV_MinMaxUInt(pDX, m_nZFMin, 1, 60);
	DDX_Text(pDX, IDC_MINN, m_nMinn);
	DDV_MinMaxUInt(pDX, m_nMinn, 1, 200);
	DDX_Text(pDX, IDC_DAYN, m_nDayn);
	DDV_MinMaxUInt(pDX, m_nDayn, 1, 300);
	DDX_Check(pDX, IDC_VOLTQ_CHECK, m_bVolTQ);
	DDX_Check(pDX, IDC_NOTIFYDOWNLOAD_CHECK, m_bNotifyDownload);
	DDX_Check(pDX, IDC_HISVIEWTQ_CHECK, m_bHisViewTQ);
	DDX_Radio(pDX, IDC_OTHER_BAR1, m_nToolBarPos);
	DDX_Check(pDX, IDC_SHOWHQMINE_CHECK, m_bShowHqMine);
	DDX_Check(pDX, IDC_OTHER_CHECK2, m_bLongSquare);
	DDX_Check(pDX, IDC_DRAWBLUE_CHECK, m_bDrawBlue);
	DDX_Text(pDX, IDC_HLIGHTNOWVOL, m_nHligtNowVol);
	DDV_MinMaxLong(pDX, m_nHligtNowVol, 10, 100000);
	DDX_Check(pDX, IDC_NOXHCOL_CHECK, m_bNoXHCol);
	DDX_Check(pDX, IDC_NOVSCROLL_CHECK, m_bNoVScroll);
	DDX_Check(pDX, IDC_AUTOHIDEBAR_CHECK, m_bAutoHideBar);
	DDX_Text(pDX, IDC_FIXEDXAXISNUM, m_nFixedXAxis);
	DDV_MinMaxInt(pDX, m_nFixedXAxis, 1, 999);
	DDX_Check(pDX, IDC_FIXXAXIS, m_bFixedXAxis);
	DDX_Check(pDX, IDC_ONLYDOWNAGDATA, m_bOnlyDownAGData);
	DDX_Check(pDX, IDC_HLIGHT_HIGHLOW_CHECK, m_bHlight_HighLow);
	DDX_Check(pDX, IDC_LOCKHQSORT_CHECK, m_bLockHQSort);
	DDX_Check(pDX, IDC_CODECANSORT_CHECK, m_bCodeCanSort);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COtherPage, CPropertyPage)
	//{{AFX_MSG_MAP(COtherPage)
	ON_BN_CLICKED(IDC_OTHER_BUTTON1, OnOtherButton1)
	ON_BN_CLICKED(IDC_OTHER_BUTTON3, OnOtherButton3)
	ON_EN_KILLFOCUS(IDC_AUTOPAGETIME, OnKillfocusAutopagetime)
	ON_BN_CLICKED(IDC_DEFWIDTH_BUTTON, OnDefwidthButton)
	ON_BN_CLICKED(IDC_AUTOHIDEBAR_CHECK, OnAutohidebarCheck)
	ON_BN_CLICKED(IDC_VOLTQ_CHECK, OnVolTQCheck)	
	ON_BN_CLICKED(IDC_FIXXAXIS, OnFixxaxis)
	ON_BN_CLICKED(IDC_XAXIS_CHECK, OnXaxisCheck)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COtherPage message handlers

char *l_strMulSelectStr[7]={"4图（2×2）","9图（3×3）","16图（4×4）","25图（5×5）","6图（3×2）","12图（4×3）","20图（5×4）"};

BOOL COtherPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	for(int i=0;i<7;i++)
		m_ChartCombo.AddString(_F(l_strMulSelectStr[i]));
	m_ChartCombo.SetCurSel(m_nChartNum);

	BOOL bStaticToolBar=FALSE;
#if defined(OEM_STAR) || defined(OEM_STAR_HM)
	bStaticToolBar = TRUE;
#else
	if(g_bShowBigToolBar)
		bStaticToolBar = TRUE;
#endif
	if(bStaticToolBar)
	{
		GetDlgItem(IDC_OTHER_BAR1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_OTHER_BAR2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_OTHER_BAR3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_OTHER_BAR4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_AUTOHIDEBAR_CHECK)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC1)->ShowWindow(SW_HIDE);
	}
	else
	{
		GetDlgItem(IDC_OTHER_BAR1)->EnableWindow(!m_bAutoHideBar);
		GetDlgItem(IDC_OTHER_BAR2)->EnableWindow(!m_bAutoHideBar);
		GetDlgItem(IDC_OTHER_BAR3)->EnableWindow(!m_bAutoHideBar);
		GetDlgItem(IDC_OTHER_BAR4)->EnableWindow(!m_bAutoHideBar);
	}
	GetDlgItem(IDC_ZFMIN)->EnableWindow(g_bSupportDownMin5);
	GetDlgItem(IDC_FIXEDXAXISNUM)->EnableWindow(m_bXaxis && m_bFixedXAxis);
	GetDlgItem(IDC_FIXXAXIS)->EnableWindow(m_bXaxis);

	int j,k,nType;
	BOOL bSame;
	/////////////////////////////////////////////
	bSame = TRUE;
	for (i = 0;i < ABGP_COL_NUM;i++) 
	{	
		if( g_aABGPIdx[i] != g_aABGPIdx_Def[i] )
		{
			bSame = FALSE;
			break;
		}
	}
	if(bSame)	GetDlgItem(IDC_OTHER_BUTTON1)->EnableWindow(FALSE);
	/////////////////////////////////////////////
	bSame = TRUE;
	TINDEXINFO *tmpIndex;
	for(nType=ZB_TYPE;nType<=KL_TYPE;nType++)
	{
		for(i = 0;i < g_pCalc->_CallDTopViewCalc_7(nType);i++)
		{
			tmpIndex = (*g_pCalc)[nType]+i;
			for(j = 0;j < tmpIndex->nParaNum;j++)
			for(k = 0;k < 12;k++)
			{
				if(tmpIndex->aPara[j].nValue[k] != tmpIndex->aPara[j].nDefault)
				{
					bSame = FALSE;
					goto findsame;
				}
			}
		}
	}
findsame:;
	if(bSame)	GetDlgItem(IDC_OTHER_BUTTON3)->EnableWindow(FALSE);
	/////////////////////////////////////////////
	bSame = TRUE;
	for(i=0;i<TOTAL_COL;i++)
	{
		if(fabs(g_aAllItemWidth[i]-g_aAllItemWidth_Def[i])>COMPPREC) 
		{
			bSame=FALSE;
			break;
		}
	}
	if(bSame) GetDlgItem(IDC_DEFWIDTH_BUTTON)->EnableWindow(FALSE);
	/////////////////////////////////////////////

	
	UpdateData(FALSE);
	return TRUE;
}

void COtherPage::OnOtherButton1()
{
	if(TDX_MessageBox(m_hWnd,"您确定恢复到缺省栏目顺序吗?",MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION)!=IDYES)
		return;
	int i;
	for (i = 0;i < ABGP_COL_NUM;i++) 
		g_aABGPIdx[i] = g_aABGPIdx_Def[i];
	GetDlgItem(IDC_OTHER_BUTTON1)->EnableWindow(FALSE);
	
	//重画行情视图
	g_bOldGridDomain=FALSE;
	CGridView *tempWnd = (CGridView *)SwitchToView("Grid");
	if(tempWnd)
		tempWnd->SendMessage(UM_GRIDREINIT,0,0);
	g_bOldGridDomain=TRUE;
}

void COtherPage::OnOtherButton3() 
{
	if(TDX_MessageBox(m_hWnd,"您确定重置指标参数吗?",MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION)!=IDYES)
		return;
	int nType,i,j,k,IndexNum;
	TINDEXINFO *tmpIndex;
	for(nType=ZB_TYPE;nType<=KL_TYPE;nType++)
	{
		IndexNum = g_pCalc->_CallDTopViewCalc_7(nType);
		for(i = 0;i < IndexNum;i++)
		{
			tmpIndex = (*g_pCalc)[nType]+i;
			for(j = 0;j < tmpIndex->nParaNum;j++)
			for(k = 0;k < 12;k++)
				tmpIndex->aPara[j].nValue[k] = tmpIndex->aPara[j].nDefault;
		}
	}
	GetDlgItem(IDC_OTHER_BUTTON3)->EnableWindow(FALSE);
}

void COtherPage::OnKillfocusAutopagetime() 
{
	UpdateData(TRUE);
	if(m_AutoPageTime < 1) m_AutoPageTime = 1;
	if(m_AutoPageTime > 600) m_AutoPageTime = 60;
	UpdateData(FALSE);
}

void COtherPage::OnOK() 
{
	UpdateData(TRUE);
	if(m_AutoPageTime < 4) 
	{
		TDX_MessageBox(m_hWnd,"您设置的自动换页时间太小,系统自动恢复成4秒钟!",MB_OK|MB_ICONINFORMATION);
		m_AutoPageTime = 4;
	}
	m_nChartNum = m_ChartCombo.GetCurSel();
	if(m_nChartNum<0)
		return;
	CPropertyPage::OnOK();
}

void COtherPage::OnDefwidthButton() 
{
	if(TDX_MessageBox(m_hWnd,"您确定恢复到缺省栏目宽度吗?",MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION)!=IDYES)
		return;
	memcpy(g_aAllItemWidth,g_aAllItemWidth_Def,TOTAL_COL*sizeof(float));
	GetDlgItem(IDC_DEFWIDTH_BUTTON)->EnableWindow(FALSE);
	if(theViewManager.pActiveView)
		theViewManager.pActiveView->PostMessage(UM_CHANGEGRIDWIDTH,0,0);
}

void COtherPage::OnAutohidebarCheck() 
{
	UpdateData(TRUE);
	GetDlgItem(IDC_OTHER_BAR1)->EnableWindow(!m_bAutoHideBar);
	GetDlgItem(IDC_OTHER_BAR2)->EnableWindow(!m_bAutoHideBar);
	GetDlgItem(IDC_OTHER_BAR3)->EnableWindow(!m_bAutoHideBar);
	GetDlgItem(IDC_OTHER_BAR4)->EnableWindow(!m_bAutoHideBar);
}

void COtherPage::OnVolTQCheck()
{
	TDX_MessageBox(m_hWnd,"成交量复权的含义是:\r\n当进行复权时,成交量也进行相应的复权折算.\r\n成交量复权对于使用某些成本类画线指标有意义,但可能会造成历史当日的换手率不正确.",MB_OK|MB_ICONINFORMATION);
}

void COtherPage::OnFixxaxis() 
{
	UpdateData(TRUE);
	GetDlgItem(IDC_FIXEDXAXISNUM)->EnableWindow(m_bXaxis && m_bFixedXAxis);
}

void COtherPage::OnXaxisCheck() 
{
	UpdateData(TRUE);
	GetDlgItem(IDC_FIXXAXIS)->EnableWindow(m_bXaxis);	
}

HBRUSH COtherPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}
