// PhaseSortDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "PhaseSortDlg.h"
#include "Hisview.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPhaseSortDlg dialog


CPhaseSortDlg::CPhaseSortDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPhaseSortDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPhaseSortDlg)
	m_BgnTime = 0;
	m_EndTime = 0;
	m_nQjType = 0;
	m_nFLType = 0;
	m_bHisViewTQ = TRUE;
	//}}AFX_DATA_INIT
	m_lQJEndDate = 0;
	m_lQJBgnDate = 0;
}


void CPhaseSortDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPhaseSortDlg)
	DDX_Control(pDX, IDC_STATUS_STATIC, m_Status_Static);
	DDX_Control(pDX, IDC_FL_COMBO, m_FLCombo);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_BgnTime);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER2, m_EndTime);
	DDX_Radio(pDX, IDC_RADIO1, m_nQjType);
	DDX_Radio(pDX, IDC_RADIO5, m_nFLType);
	DDX_Check(pDX, IDC_HISVIEWTQ_CHECK, m_bHisViewTQ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPhaseSortDlg, CDialog)
	//{{AFX_MSG_MAP(CPhaseSortDlg)
	ON_BN_CLICKED(IDC_RADIO5, OnRadio5)
	ON_BN_CLICKED(IDC_RADIO6, OnRadio6)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPhaseSortDlg message handlers

BOOL CPhaseSortDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetDateCtrlRange(GetDlgItem(IDC_DATETIMEPICKER1));
	SetDateCtrlRange(GetDlgItem(IDC_DATETIMEPICKER2));
	m_FLCombo.SetItemHeight(0,16);
	m_FLCombo.SetItemHeight(-1,16);
	m_Status_Static.SetTextColor(RGB(240,0,0));

	m_bHisViewTQ = g_bHisViewTQ;

	if(m_lQJEndDate==0) //如果不是外部传入的时间，则用g_lQJEndDate
	{
		//m_lQJEndDate值和m_lQJBgnDate值初始都等于g_lQJEndDate和g_lQJBgnDate
		if(g_lQJEndDate==0)
			g_lQJEndDate = g_lOpenRq;
		m_lQJEndDate	= g_lQJEndDate;
	}
	if(m_lQJBgnDate==0)
	{
		if(g_lQJBgnDate==0)
		{
			CTime theTime(m_lQJEndDate/10000,(m_lQJEndDate%10000)/100,(m_lQJEndDate%10000)%100,0,0,0); 
			CTimeSpan timeSpan(15,0,0,0); 	//15天
			CTime theTime2 = theTime-timeSpan;
			g_lQJBgnDate = theTime2.GetYear()*10000+theTime2.GetMonth()*100+theTime2.GetDay();
		}
		m_lQJBgnDate = g_lQJBgnDate;
	}
	m_BgnTime = LongToTime(m_lQJBgnDate);
	m_EndTime = LongToTime(m_lQJEndDate);
	
	FillCombo();
	UpdateData(FALSE);
	return TRUE;
}

void CPhaseSortDlg::FillCombo()
{
	m_FLCombo.ResetContent();
	int i;
	if(m_nFLType==0)
	{
		for(i=0;i < TOTAL_TYPENUM;i++)
			m_FLCombo.AddString(_F(GPType[i]));
		m_FLCombo.SetCurSel(6);
	}
	else
	{
		for(i=0;i < TOTAL_DY_NUM;i++)
			m_FLCombo.AddString(_F(g_cwAddrTopic[i]));
		for(i=0;i < TOTAL_HY_NUM;i++)
			m_FLCombo.AddString(_F(g_cwHYstr[i]));
		for(i=0;i < g_ConceptBlockNum;i++)
			m_FLCombo.AddString(_F(g_ConceptBlock[i].strBlockName));
		for(i=0;i < g_ZHBBlockNum;i++)
			m_FLCombo.AddString(_F(g_ZHBBlock[i].zhmc));
		for(i=0;i < g_nBlockNum;i++)
			m_FLCombo.AddString(g_BlockInfo[i].Name);
		m_FLCombo.SetCurSel(0);
	}
}

void CPhaseSortDlg::OnOK() 
{
	UpdateData(TRUE);
	int comboSel = m_FLCombo.GetCurSel();
	if(comboSel==CB_ERR) return;

	CTime today = CTime::GetCurrentTime();
	int tempInt1 = m_BgnTime.GetYear();
	int tempInt2 = m_EndTime.GetYear();
	if(tempInt1 < 1990 || tempInt2 < 1990 ||m_BgnTime > today||m_EndTime > today|| m_BgnTime > m_EndTime)
	{
		TDX_MessageBox(m_hWnd,"日期输入不合理,必要时请确认您本机的时间是否正确!\r\n第一日期不能大于第二日期,且都不能大于今日日期(确认你本机时间是正确的)",MB_OK|MB_ICONEXCLAMATION);
		GetDlgItem(IDC_DATETIMEPICKER1)->SetFocus();
		return;
	}

	//关闭HisView
	CloseView("His");
	
	g_bHisViewTQ = m_bHisViewTQ;

	m_lQJBgnDate = TimeToLong(m_BgnTime);
	m_lQJEndDate = TimeToLong(m_EndTime);
	g_lQJBgnDate = m_lQJBgnDate;
	g_lQJEndDate = m_lQJEndDate;

	HisCurType = QJ_REPORT;
	g_nCurQJType = m_nQjType;
	FirstHisGPType = -1;
	if(m_nFLType==0)
		FirstHisGPType = comboSel;
	else
	{
		if(comboSel < TOTAL_DY_NUM)
			FirstHisBlockFlag = DY_START+comboSel;
		else if(comboSel < TOTAL_DY_NUM+TOTAL_HY_NUM)
			FirstHisBlockFlag = HY_START+comboSel-TOTAL_DY_NUM;
		else if(comboSel < TOTAL_DY_NUM+TOTAL_HY_NUM+g_ConceptBlockNum)
			FirstHisBlockFlag = 300+comboSel-TOTAL_DY_NUM-TOTAL_HY_NUM;
		else if(comboSel < TOTAL_DY_NUM+TOTAL_HY_NUM+g_ConceptBlockNum+g_ZHBBlockNum)
			FirstHisBlockFlag = 400+comboSel-TOTAL_DY_NUM-TOTAL_HY_NUM-g_ConceptBlockNum;
		else
			FirstHisBlockFlag = comboSel-TOTAL_DY_NUM-TOTAL_HY_NUM-g_ConceptBlockNum-g_ZHBBlockNum;
	}

	CDialog::OnOK();

	//打开HisView
	OpenNewView("His");
}

void CPhaseSortDlg::OnCancel() 
{
	
	CDialog::OnCancel();
}

void CPhaseSortDlg::OnRadio5() 
{
	UpdateData(TRUE);
	FillCombo();
}

void CPhaseSortDlg::OnRadio6() 
{
	UpdateData(TRUE);
	FillCombo();	
}
