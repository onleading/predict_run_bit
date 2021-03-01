// TestZBDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "TestZBDlg.h"
#include "interface.h"
#include "GridView.h"
#include "ComView.h"
#include "TxtView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTestZBDlg dialog


CTestZBDlg::CTestZBDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestZBDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTestZBDlg)
	m_BeginTime = 0;
	m_EndTime = 0;
	m_nDontUseRecentData = -1;
	//}}AFX_DATA_INIT
}


void CTestZBDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestZBDlg)
	DDX_Control(pDX, IDC_EDIT3, m_TestTotal);
	DDX_Control(pDX, IDC_EDIT1, m_GPTotal);
	DDX_Control(pDX, IDC_EDIT7, m_HaveDoneTotal);
	DDX_Control(pDX, IDC_STATICINFO, m_OutInfo);
	DDX_Control(pDX, IDC_COMBO3, m_AimCombo);
	DDX_Control(pDX, IDC_COMBO2, m_PeriodCombo);
	DDX_Control(pDX, IDC_LIST1, m_List);
	DDX_Control(pDX, IDC_PROGRESS1, m_Progress);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_BeginTime);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER2, m_EndTime);
	DDX_Radio(pDX, IDC_RADIO2, m_nDontUseRecentData);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTestZBDlg, CDialog)
	//{{AFX_MSG_MAP(CTestZBDlg)
	ON_BN_CLICKED(IDC_SELECTALL, OnSelectall)
	ON_CBN_SELCHANGE(IDC_COMBO2, OnSelchangeCombo2)
	ON_BN_CLICKED(IDC_RADIO14, OnRadio14)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio14)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestZBDlg message handlers

void CTestZBDlg::OnCancel() 
{
	if(!m_IsCarrying && m_IsCarried)
	{
		GetParent() ->PostMessage (WM_CANCELPRESSED,(WPARAM)this,0);
		g_bDonotClose = FALSE;
	}
	else
		m_IsCarrying = FALSE;
}

BOOL CTestZBDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetDateCtrlRange(GetDlgItem(IDC_DATETIMEPICKER1));
	SetDateCtrlRange(GetDlgItem(IDC_DATETIMEPICKER2));

	m_OutInfo.SetTextColor(RGB(240,0,0));
	m_OutInfo.SetWindowText(_F("����ǰ,��ȷ�����ص�������������."));

	m_AimCombo.SetItemHeight(0,16);
	m_AimCombo.SetItemHeight(-1,16);
	m_PeriodCombo.SetItemHeight(0,16);
	m_PeriodCombo.SetItemHeight(-1,16);

	int i;
	LVCOLUMN	lvc;
	LVITEM		lvi;
	TINDEXINFO	*tmp;
	TCHAR *title[] = {"ָ������", "Ŀ������(%)", "ӯ������", "������", "ָ�����", NULL};
	int		nWidth[] = {80,80,70,70,100};

	m_bSelectAll = FALSE;
	m_nAddedZBNum = 0;
	m_ImageList.Create(16,16,ILC_COLOR8|ILC_MASK,1,1);
	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_INDEX));
	m_ImageList.Add(hIcon);
	m_List.SetImageList(&m_ImageList,LVSIL_SMALL);
	m_List.SetDragType(FALSE);
	DWORD dwStyle = m_List.GetExtendedStyle();
	dwStyle |= LVS_EX_CHECKBOXES;
	m_List.SetExtendedStyle(dwStyle);

	lvc.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvc.fmt = LVCFMT_CENTER;
	for (i = 0; title[i] != NULL; i++)
	{
		lvc.pszText = _F(title[i]);
		lvc.cx = nWidth[i];
		m_List.InsertColumn(i,&lvc);
	}
	i = 0;
	do
	{
		tmp = g_pCalc->_CallDTopViewCalc_8(EXP_TYPE, i);
		lvi.mask = LVIF_TEXT|LVIF_IMAGE;
		lvi.iSubItem = 0;
		if (tmp != NULL)
		{
			lvi.iItem = i;
			lvi.iImage = 0;
			lvi.pszText = tmp->acCode;
			m_List.InsertItem(&lvi);
			m_List.SetItemData(i,65536+tmp->nIndexNo);//��Ϊ����ҷ,��65535Ϊ�������ǽ���ϵͳ��ʽ
		}
		i++;
	}while(tmp != NULL);

	//�����������б��
	for(i = 0; g_CalcPeriodStr[i] != NULL ;i++)
		m_PeriodCombo.AddString (_F(g_CalcPeriodStr[i]));
	m_PeriodCombo.SetCurSel (DAY_ST);	//Ĭ��������
	//���Ŀ�괮
	for(i = 0; i < 8;i++)
		m_AimCombo.AddString(_F(AmiComboStr[i]));
	m_AimCombo.SetCurSel(0);

	for (i = 0; i < MAXCOUNT; i++)
		m_bSelected[i] = 0;
	m_Progress.SetPos(0);	//������������Ϊ1,λ����ʼ��0��
	m_Progress.SetStep(1);
	m_GPTotal.SetWindowText("1");

	m_nCurrentGPNo = GetSYSCurrentGPIndex();
	if(m_nCurrentGPNo == -1) m_nCurrentGPNo = 14; //ȱʡʱȡ"�չ"

	SetWindowText((CString)_F("̽�����ָ��--")+(*g_pStockIO)[m_nCurrentGPNo]->Name );

	CTimeSpan TimeSpan(365, 0, 0, 0);
	m_EndTime = CTime::GetCurrentTime();
	m_BeginTime = m_EndTime - TimeSpan;

	m_nDontUseRecentData = 0;
	GetDlgItem(IDC_DATETIMEPICKER1)->EnableWindow(FALSE);
	GetDlgItem(IDC_DATETIMEPICKER2)->EnableWindow(FALSE);

	m_IsCarrying = FALSE;
	m_IsCarried = TRUE;
	UpdateData(FALSE);
	return TRUE;
}

void CTestZBDlg::OnSelectall() 
{
	m_bSelectAll = !m_bSelectAll;
	for (int i = 0; i < m_List.GetItemCount(); i++)
	{
		m_List.SetCheck(i, m_bSelectAll);
		m_bSelected[i] = m_bSelectAll;
	}
	GetDlgItem(IDC_SELECTALL)->SetWindowText(m_bSelectAll?_F("ȫ��ȡ��"):_F("ȫ��ѡ��"));
	FillGPReport();
}

void CTestZBDlg::FillGPReport()
{
	int i, nPos = 0;
	LVITEM lvi;

	memset(&lvi, 0, sizeof(LVITEM));
	m_List.SetRedraw(FALSE);
	m_List.DeleteAllItems();
	lvi.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
	lvi.state = 0;
	lvi.stateMask = 0;
	lvi.iSubItem = 0;
	for (i = 0; i < g_pCalc->_CallDTopViewCalc_7(EXP_TYPE); i++)
	{
		if (m_bSelected[i])
			m_nAddedZBIndex[nPos++] = i;
		lvi.iItem = i;
		lvi.pszText = g_pCalc->_CallDTopViewCalc_8(EXP_TYPE, i)->acCode;
		lvi.iImage = 0;
		m_List.InsertItem(&lvi);
		m_List.SetItemData(i,i);//��Ϊ����ҷ
		m_List.SetCheck(i,m_bSelected[i]);
	}
	m_List.SetRedraw(TRUE);
	m_List.SetFocus();
	m_nAddedZBNum = nPos;
}

#define CALCBONUS(number, type)\
{\
	CalcBonus(tmp,number, type);\
	CalcedNum++;\
	if(CalcedNum % 50 == 0)\
	{\
		m_Progress.StepIt();\
		tmpStr.Format("%d", CalcedNum);\
		m_HaveDoneTotal.SetWindowText(tmpStr);\
	}\
	if (PeekMyMsg(CalcedNum)) goto CancelLabel;\
}

void CTestZBDlg::OnOK() 
{
	TINDEXINFO	*tmp, IndexBakup;
	CString	tmpStr;
	long	i, j, nAim, tempCalcNum = 0;
	long	lTestCount, lReadNum, nCurrSel = 0;
	int		TotalCalcNum = 0;	//TotalCalcNum����ܹ���Ҫ����Ĵ���
	int		CalcedNum = 0;	//CalcedNum��ŵ�ǰ�Ѿ����������
	NTime	time1, time2;
	BOOL	bDontUseRecentData;

	UpdateData(TRUE);
	bDontUseRecentData = m_nDontUseRecentData;
	memset(&IndexBakup, 0, sizeof(TINDEXINFO));
	if (bDontUseRecentData && m_BeginTime > m_EndTime)
	{
		TDX_MessageBox(m_hWnd,"��ʼ����Ӧ����������֮ǰ!",MB_OK|MB_ICONINFORMATION);
		return;
	}
	m_nPeriod = m_PeriodCombo.GetCurSel();
	if(m_nPeriod==MIN1_ST||m_nPeriod==MINN_ST)
	{
		TDX_MessageBox(m_hWnd,"�ݲ�֧�ִ�����,��ѡ����������!",MB_OK|MB_ICONINFORMATION);
		return;
	}
	if(!CanProcesLocalData(this, m_nPeriod<DAY_ST))
		return;
	for (i = 0; i < m_List.GetItemCount(); i++)
		m_bSelected[i] = m_List.GetCheck(i);
	FillGPReport();
	//���´�������ܹ���Ҫ����Ĵ���
	for(i = 0;i < m_nAddedZBNum;i++)
	{
		tmp = g_pCalc->_CallDTopViewCalc_8(EXP_TYPE, m_nAddedZBIndex[i]);
		if(tmp->nParaNum == 0)
			continue;
		tempCalcNum = 1 + (tmp->aPara[0].nMax - tmp->aPara[0].nMin)/tmp->aPara[0].nStep;
		for(j = 1; j < tmp->nParaNum;j++)
			tempCalcNum *= 1 + (tmp->aPara[j].nMax - tmp->aPara[j].nMin)/tmp->aPara[j].nStep;
		TotalCalcNum += tempCalcNum;
	}

	//�ڶԻ�������ʾ��������
	tmpStr.Format("%d", TotalCalcNum);
	m_TestTotal.SetWindowText(tmpStr);
	m_Progress.SetRange32(0,TotalCalcNum);
	m_Progress.SetStep(50);
	m_Progress.SetPos(0);
	m_OutInfo.SetTextColor(CLR_NOWTESTING);
	m_OutInfo.SetWindowText(_F("���ڽ��в���..."));
	m_IsCarried = FALSE;
	m_IsCarrying = TRUE;
	DisableOther();
	//��Disable����֮������һ�ؼ�Ϊ����,����ESC��������
	GetDlgItem(IDCANCEL)->SetFocus();
	RefreshAllHq();
	g_bDonotClose = TRUE;

	memset(&time1, 0, sizeof(NTime));
	memset(&time2, 0, sizeof(NTime));
	time1.year = m_BeginTime.GetYear();
	time1.month = m_BeginTime.GetMonth();
	time1.day = m_BeginTime.GetDay();
	time2.year = m_EndTime.GetYear();
	time2.month = m_EndTime.GetMonth();
	time2.day = m_EndTime.GetDay();
//�˴�����
	for (lTestCount = 0; lTestCount < m_nAddedZBNum; lTestCount++)
	{
		m_fMaxBonus = -9999999.9f;
		m_nCurrTestZB = m_nAddedZBIndex[lTestCount];
		nAim = m_AimCombo.GetCurSel();
		if (bDontUseRecentData)
			lReadNum = g_pCalc->_CallDTopViewCalc_4((*g_pStockIO)[m_nCurrentGPNo]->Code,(*g_pStockIO)[m_nCurrentGPNo]->setcode, CalcPeriodToTdxPeriod(m_nPeriod), time1, time2,1);
		else
			lReadNum = g_pCalc->_CallDTopViewCalc_4((*g_pStockIO)[m_nCurrentGPNo]->Code,(*g_pStockIO)[m_nCurrentGPNo]->setcode, CalcPeriodToTdxPeriod(m_nPeriod), ASK_ALL,1);
		m_pAnalyData = (LPHISDAT)g_pCalc->_CallDTopViewCalc_15();
		if(lReadNum <= 0 || m_pAnalyData == NULL) continue;
		tmp = g_pCalc->_CallDTopViewCalc_8(EXP_TYPE, m_nCurrTestZB);
		if (tmp == NULL) return;
		memcpy(&IndexBakup, tmp, sizeof(TINDEXINFO));
		switch (tmp->nParaNum)
		{
		case 0:
			CALCBONUS(lReadNum, nAim)
			break;
		case 1:
			for ( tmp->aPara[0].nValue[m_nPeriod] = tmp->aPara[0].nMin;
				tmp->aPara[0].nValue[m_nPeriod] <= tmp->aPara[0].nMax;
				tmp->aPara[0].nValue[m_nPeriod] += tmp->aPara[0].nStep )
					CALCBONUS(lReadNum, nAim)
			break;
		case 2:
			for ( tmp->aPara[0].nValue[m_nPeriod] = tmp->aPara[0].nMin;
				tmp->aPara[0].nValue[m_nPeriod] <= tmp->aPara[0].nMax;
				tmp->aPara[0].nValue[m_nPeriod] += tmp->aPara[0].nStep )
				for ( tmp->aPara[1].nValue[m_nPeriod] = tmp->aPara[1].nMin;
					tmp->aPara[1].nValue[m_nPeriod] <= tmp->aPara[1].nMax;
					tmp->aPara[1].nValue[m_nPeriod] += tmp->aPara[1].nStep )
						CALCBONUS(lReadNum, nAim)
			break;
		case 3:
			for ( tmp->aPara[0].nValue[m_nPeriod] = tmp->aPara[0].nMin;
				tmp->aPara[0].nValue[m_nPeriod] <= tmp->aPara[0].nMax;
				tmp->aPara[0].nValue[m_nPeriod] += tmp->aPara[0].nStep )
				for ( tmp->aPara[1].nValue[m_nPeriod] = tmp->aPara[1].nMin;
					tmp->aPara[1].nValue[m_nPeriod] <= tmp->aPara[1].nMax;
					tmp->aPara[1].nValue[m_nPeriod] += tmp->aPara[1].nStep )
					for ( tmp->aPara[2].nValue[m_nPeriod] = tmp->aPara[2].nMin;
						tmp->aPara[2].nValue[m_nPeriod] <= tmp->aPara[2].nMax;
						tmp->aPara[2].nValue[m_nPeriod] += tmp->aPara[2].nStep )
							CALCBONUS(lReadNum, nAim)
			break;
		case 4:
			for ( tmp->aPara[0].nValue[m_nPeriod] = tmp->aPara[0].nMin;
				tmp->aPara[0].nValue[m_nPeriod] <= tmp->aPara[0].nMax;
				tmp->aPara[0].nValue[m_nPeriod] += tmp->aPara[0].nStep )
				for ( tmp->aPara[1].nValue[m_nPeriod] = tmp->aPara[1].nMin;
					tmp->aPara[1].nValue[m_nPeriod] <= tmp->aPara[1].nMax;
					tmp->aPara[1].nValue[m_nPeriod] += tmp->aPara[1].nStep )
					for ( tmp->aPara[2].nValue[m_nPeriod] = tmp->aPara[2].nMin;
						tmp->aPara[2].nValue[m_nPeriod] <= tmp->aPara[2].nMax;
						tmp->aPara[2].nValue[m_nPeriod] += tmp->aPara[2].nStep )
						for ( tmp->aPara[3].nValue[m_nPeriod] = tmp->aPara[3].nMin;
							tmp->aPara[3].nValue[m_nPeriod] <= tmp->aPara[3].nMax;
							tmp->aPara[3].nValue[m_nPeriod] += tmp->aPara[3].nStep )
								CALCBONUS(lReadNum, nAim)
			break;
		case 5:
			for ( tmp->aPara[0].nValue[m_nPeriod] = tmp->aPara[0].nMin;
				tmp->aPara[0].nValue[m_nPeriod] <= tmp->aPara[0].nMax;
				tmp->aPara[0].nValue[m_nPeriod] += tmp->aPara[0].nStep )
				for ( tmp->aPara[1].nValue[m_nPeriod] = tmp->aPara[1].nMin;
					tmp->aPara[1].nValue[m_nPeriod] <= tmp->aPara[1].nMax;
					tmp->aPara[1].nValue[m_nPeriod] += tmp->aPara[1].nStep )
					for ( tmp->aPara[2].nValue[m_nPeriod] = tmp->aPara[2].nMin;
						tmp->aPara[2].nValue[m_nPeriod] <= tmp->aPara[2].nMax;
						tmp->aPara[2].nValue[m_nPeriod] += tmp->aPara[2].nStep )
						for ( tmp->aPara[3].nValue[m_nPeriod] = tmp->aPara[3].nMin;
							tmp->aPara[3].nValue[m_nPeriod] <= tmp->aPara[3].nMax;
							tmp->aPara[3].nValue[m_nPeriod] += tmp->aPara[3].nStep )
							for ( tmp->aPara[4].nValue[m_nPeriod] = tmp->aPara[4].nMin;
								tmp->aPara[4].nValue[m_nPeriod] <= tmp->aPara[4].nMax;
								tmp->aPara[4].nValue[m_nPeriod] += tmp->aPara[4].nStep )
									CALCBONUS(lReadNum, nAim)
			break;
		case 6:
			for ( tmp->aPara[0].nValue[m_nPeriod] = tmp->aPara[0].nMin;
				tmp->aPara[0].nValue[m_nPeriod] <= tmp->aPara[0].nMax;
				tmp->aPara[0].nValue[m_nPeriod] += tmp->aPara[0].nStep )
				for ( tmp->aPara[1].nValue[m_nPeriod] = tmp->aPara[1].nMin;
					tmp->aPara[1].nValue[m_nPeriod] <= tmp->aPara[1].nMax;
					tmp->aPara[1].nValue[m_nPeriod] += tmp->aPara[1].nStep )
					for ( tmp->aPara[2].nValue[m_nPeriod] = tmp->aPara[2].nMin;
						tmp->aPara[2].nValue[m_nPeriod] <= tmp->aPara[2].nMax;
						tmp->aPara[2].nValue[m_nPeriod] += tmp->aPara[2].nStep )
						for ( tmp->aPara[3].nValue[m_nPeriod] = tmp->aPara[3].nMin;
							tmp->aPara[3].nValue[m_nPeriod] <= tmp->aPara[3].nMax;
							tmp->aPara[3].nValue[m_nPeriod] += tmp->aPara[3].nStep )
							for ( tmp->aPara[4].nValue[m_nPeriod] = tmp->aPara[4].nMin;
								tmp->aPara[4].nValue[m_nPeriod] <= tmp->aPara[4].nMax;
								tmp->aPara[4].nValue[m_nPeriod] += tmp->aPara[4].nStep )
								for ( tmp->aPara[5].nValue[m_nPeriod] = tmp->aPara[5].nMin;
									tmp->aPara[5].nValue[m_nPeriod] <= tmp->aPara[5].nMax;
									tmp->aPara[5].nValue[m_nPeriod] += tmp->aPara[5].nStep )
										CALCBONUS(lReadNum, nAim)
			break;
		}
		while (!m_bSelected[nCurrSel])
			nCurrSel++;
		if(m_fMaxBonus > -9999999.9)
		{
			tmpStr.Format("%3.1f",m_fMaxBonus * 100);
			m_List.SetItemText(nCurrSel,1,tmpStr);
			tmpStr.Format("%d", m_nYLTimes);
			m_List.SetItemText(nCurrSel,2,tmpStr);
			tmpStr.Format("%d", m_nKSTimes);
			m_List.SetItemText(nCurrSel,3,tmpStr);
			tmpStr.Empty();
			if (tmp->nParaNum > 0)
			{
				CString tmp1;
				for(j = 0;j < tmp->nParaNum;j++)
				{
					tmp1.Format("%d,", (int)m_fParaValue[j]);
					tmpStr += tmp1;
				}
				tmpStr.Delete(tmpStr.GetLength()-1);
			}
			m_List.SetItemText(nCurrSel,4,tmpStr);
		}
		else
			m_List.SetItemText(nCurrSel,1,MEANLESS_STR);
		//��ʱ������Ϣ�Ѿ�����,������ԭʼ��IndexBakup���ָ�
		memcpy(&tmp->aPara, &IndexBakup.aPara, sizeof(TPARAINFO) * 6);
		nCurrSel++;
	}
	m_Progress.SetPos(TotalCalcNum);
	tmpStr.Format("%d", TotalCalcNum);
	m_HaveDoneTotal.SetWindowText(tmpStr);
CancelLabel:
	if(m_IsCarrying)
	{
		m_OutInfo.SetTextColor(CLR_TESTED);
		m_OutInfo.SetWindowText(_F("�������!"));
		m_IsCarrying = FALSE;
	}
	EnableOther();
	g_bDonotClose = FALSE;
	m_IsCarried = TRUE;
}

BOOL CTestZBDlg::PeekMyMsg(int TimeOut)
{
	if(TimeOut%100 == 0)
	{
		PeekMsg();
		if( !m_IsCarrying)
		{
			m_Progress.SetPos(0);
			m_OutInfo.SetTextColor(CLR_INTERRUPT);
			SetDlgItemText(IDC_STATICINFO, _F("�����ж�!"));
			return TRUE;
		}
	}
	return FALSE;
}

void CTestZBDlg::EnableOther()
{
	m_PeriodCombo.EnableWindow(TRUE);
	m_AimCombo.EnableWindow(TRUE);
	GetDlgItem(IDOK)->EnableWindow(TRUE);
	GetDlgItem(IDC_SELECTALL)->EnableWindow(TRUE);
	SetDlgItemText(IDCANCEL, _F("�ر�"));
}

void CTestZBDlg::DisableOther()
{
	m_PeriodCombo.EnableWindow(FALSE);
	m_AimCombo.EnableWindow(FALSE);
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	GetDlgItem(IDC_SELECTALL)->EnableWindow(FALSE);
	SetDlgItemText(IDCANCEL, _F("��ֹ����"));
}

//���������
void CTestZBDlg::CalcFormula0(float **OutData, long lReadNum, TINDEXINFO *pIndex)
{
	int		i;
	float	fBonus=0.0, tmpfBonus;	//����,�ðٷ�����ʾ
	BOOL	bFindFirstBuyPoint = FALSE;		//Ҫ���ҵ���һ�����
	BOOL	bMayNew = FALSE;
	BOOL	bAllInvalid = TRUE;	//�жϴ˴μ����Ƿ�ȫΪ��Ч����
	BOOL	bBuy2Zero;		//�ж�������㻹�����������
	float   BuyPrice, SellPrice;	//BuyFlag��ʾ�Ѿ������Ʊ�ı�־,BuyPrice��ʾ�����Ʊ�ļ۸�
	double	dBuyMoney = 0, dSellMoney = 0;	//����ʹ�����ʽ�,����ʹ�����ʽ�
	double	dBuyCount = 0, dSellCount = 0;	//������,������
	LPHISDAT pTmpHisData = m_pAnalyData;
	int		nYLTimes = 0, nKSTimes = 0;

	for (i = 0; i < lReadNum; i++)
	{
		if (!bFindFirstBuyPoint)
			if (fabs(OutData[ENTERLONG][i] - 1) < COMPPREC || fabs(OutData[ENTERSHORT][i] - 1) < COMPPREC)	//һ��Ҫ���ҵ���һ�����
			{
				bFindFirstBuyPoint = TRUE;
				bAllInvalid = FALSE;
			}
			else
				continue;
		if (fabs(OutData[ENTERLONG][i] - 1) < COMPPREC || fabs(OutData[ENTERSHORT][i] - 1) < COMPPREC)	//��ʾ��ǰ���
		{
			if (bMayNew)
			{
				BuyPrice = dBuyMoney / dBuyCount;
				SellPrice = dSellMoney / dSellCount;
				tmpfBonus = (SellPrice - BuyPrice) / BuyPrice;
				if (tmpfBonus >= 0) 
					nYLTimes++;
				else
					nKSTimes++;
				fBonus += tmpfBonus;
				bMayNew = FALSE;
				dBuyCount = 0;
				dSellCount = 0;
				dBuyMoney = 0;
				dSellMoney = 0;
			}
			dBuyCount += pTmpHisData->Amount;	//�����ָ��,���Գɽ������Ȩ��(Ҳ��Amount)
			dBuyMoney += pTmpHisData->Close * dBuyCount;
			pTmpHisData++;
			bBuy2Zero = TRUE;
			continue;
		}
		else
		if (fabs(OutData[EXITLONG][i] - 1) < COMPPREC || fabs(OutData[EXITSHORT][i] - 1) < COMPPREC)	//��ʾ��ǰ����
		{
			dSellCount += pTmpHisData->Amount;
			dSellMoney += pTmpHisData->Close * dSellCount;
			pTmpHisData++;
			bBuy2Zero = FALSE;
			continue;
		}
		else
		{
			if (bBuy2Zero)
				bMayNew = FALSE;
			else
				bMayNew = TRUE;
		}
	}
	if (!bAllInvalid)
	{
		//�������һ�ε�����
		if (dSellCount < COMPPREC)	//����������������
			SellPrice = pTmpHisData->Close;
		else
			SellPrice = dSellMoney / dSellCount;
		if (dBuyCount < COMPPREC)
			BuyPrice = pTmpHisData->Open;
		else
			BuyPrice = dBuyMoney / dBuyCount;
		tmpfBonus = (SellPrice - BuyPrice) / (BuyPrice*2);

		if (tmpfBonus >= 0) 
			nYLTimes++;
		else
			nKSTimes++;
		fBonus += tmpfBonus;

		if ( fBonus > m_fMaxBonus )		//���浱ǰ������漰��ʱ�Ļ��ִ����Ͳ���ֵ,ӯ�����׺Ϳ����״���
		{
			m_nYLTimes = nYLTimes;
			m_nKSTimes = nKSTimes;
			m_fMaxBonus = fBonus;
			for ( i = 0; i < pIndex->nParaNum; i++ )
				m_fParaValue[i] = pIndex->aPara[i].nValue[m_nPeriod];
		}
	}
}

//ƽ���������
void CTestZBDlg::CalcFormula1(float **OutData, long lReadNum, TINDEXINFO *pIndex)
{
	int		i;
	float	fBonus=0.0, tmpfBonus;	//����,�ðٷ�����ʾ
	BOOL	bFindFirstBuyPoint = FALSE;		//Ҫ���ҵ���һ�����
	BOOL	bMayNew = FALSE;
	BOOL	bAllInvalid = TRUE;	//�жϴ˴μ����Ƿ�ȫΪ��Ч����
	BOOL	bBuy2Zero;		//�ж�������㻹�����������
	float   BuyPrice, SellPrice;	//BuyFlag��ʾ�Ѿ������Ʊ�ı�־,BuyPrice��ʾ�����Ʊ�ļ۸�
	double	dBuyMoney = 0, dSellMoney = 0;	//����ʹ�����ʽ�,����ʹ�����ʽ�
	double	dBuyCount = 0, dSellCount = 0;	//������,������
	LPHISDAT pTmpHisData = m_pAnalyData;
	int		nYLTimes = 0, nKSTimes = 0;

	for (i = 0; i < lReadNum; i++)
	{
		if (!bFindFirstBuyPoint)
			if (fabs(OutData[ENTERLONG][i] - 1) < COMPPREC || fabs(OutData[ENTERSHORT][i] - 1) < COMPPREC)	//һ��Ҫ���ҵ���һ�����
			{
				bFindFirstBuyPoint = TRUE;
				bAllInvalid = FALSE;
			}
				else
					continue;
		if (fabs(OutData[ENTERLONG][i] - 1) < COMPPREC || fabs(OutData[ENTERSHORT][i] - 1) < COMPPREC)	//��ʾ��ǰ���
		{
			if (bMayNew)
			{
				BuyPrice = dBuyMoney / dBuyCount;
				SellPrice = dSellMoney / dSellCount;
				tmpfBonus = (SellPrice - BuyPrice) / BuyPrice;
				if (tmpfBonus >= 0) 
					nYLTimes++;
				else
					nKSTimes++;
				fBonus += tmpfBonus;
				bMayNew = FALSE;
				dBuyCount = 0;
				dSellCount = 0;
				dBuyMoney = 0;
				dSellMoney = 0;
			}
			dBuyCount += pTmpHisData->Amount;	//�����ָ��,���Գɽ������Ȩ��(Ҳ��Amount)
			dBuyMoney += pTmpHisData->Close * dBuyCount;
			pTmpHisData++;
			bBuy2Zero = TRUE;
			continue;
		}
		else
		if (fabs(OutData[EXITLONG][i] - 1) < COMPPREC || fabs(OutData[EXITSHORT][i] - 1) < COMPPREC)	//��ʾ��ǰ����
		{
			dSellCount += pTmpHisData->Amount;
			dSellMoney += pTmpHisData->Close * dSellCount;
			pTmpHisData++;
			bBuy2Zero = FALSE;
			continue;
		}
		else
		{
			if (bBuy2Zero)
				bMayNew = FALSE;
			else
				bMayNew = TRUE;
		}
	}
	if (!bAllInvalid)
	{
		//�������һ�ε�����
		if (dSellCount < COMPPREC)	//����������������
			SellPrice = pTmpHisData->Close;
		else
			SellPrice = dSellMoney / dSellCount;
		if (dBuyCount < COMPPREC)
			BuyPrice = pTmpHisData->Open;
		else
			BuyPrice = dBuyMoney / dBuyCount;
		tmpfBonus = (SellPrice - BuyPrice) / (BuyPrice*2);

		if (tmpfBonus >= 0) 
			nYLTimes++;
		else
			nKSTimes++;
		fBonus += tmpfBonus;

		if (nYLTimes+nKSTimes != 0)
			fBonus /= nYLTimes+nKSTimes;
		if (fBonus > m_fMaxBonus )		//���浱ǰ�������ƽ������ʱ�Ļ��ִ����Ͳ���ֵ
		{
			m_nYLTimes = nYLTimes;
			m_nKSTimes = nKSTimes;
			m_fMaxBonus = fBonus;
			for ( i = 0; i < pIndex->nParaNum; i++ )
				m_fParaValue[i] = pIndex->aPara[i].nValue[m_nPeriod];
		}
	}
}

//�۳����ӯ�����������
void CTestZBDlg::CalcFormula2(float **OutData, long lReadNum, TINDEXINFO *pIndex)
{
	int		i;
	float	fBonus=0.0, tmpfBonus, fMaxYL = 0, ftmpMaxBonus = 0;	//����,�ðٷ�����ʾ, fMaxYL -- ���ӯ��
	BOOL	bFindFirstBuyPoint = FALSE;		//Ҫ���ҵ���һ�����
	BOOL	bMayNew = FALSE;
	BOOL	bAllInvalid = TRUE;	//�жϴ˴μ����Ƿ�ȫΪ��Ч����
	BOOL	bBuy2Zero;		//�ж�������㻹�����������
	float   BuyPrice, SellPrice;	//BuyFlag��ʾ�Ѿ������Ʊ�ı�־,BuyPrice��ʾ�����Ʊ�ļ۸�
	double	dBuyMoney = 0, dSellMoney = 0;	//����ʹ�����ʽ�,����ʹ�����ʽ�
	double	dBuyCount = 0, dSellCount = 0;	//������,������
	LPHISDAT pTmpHisData = m_pAnalyData;
	int		nYLTimes = 0, nKSTimes = 0;

	for (i = 0; i < lReadNum; i++)
	{
		if (!bFindFirstBuyPoint)
			if (fabs(OutData[ENTERLONG][i] - 1) < COMPPREC || fabs(OutData[ENTERSHORT][i] - 1) < COMPPREC)	//һ��Ҫ���ҵ���һ�����
			{
				bFindFirstBuyPoint = TRUE;
				bAllInvalid = FALSE;
			}
			else
				continue;
		if (fabs(OutData[ENTERLONG][i] - 1) < COMPPREC || fabs(OutData[ENTERSHORT][i] - 1) < COMPPREC)	//��ʾ��ǰ���
		{
			if (bMayNew)
			{
				BuyPrice = dBuyMoney / dBuyCount;
				SellPrice = dSellMoney / dSellCount;
				tmpfBonus = SellPrice - BuyPrice;
				if (tmpfBonus > fMaxYL)
				{
					fMaxYL = tmpfBonus;
					ftmpMaxBonus = tmpfBonus / BuyPrice;
				}
				if (tmpfBonus >= 0) 
					nYLTimes++;
				else
					nKSTimes++;
				fBonus += tmpfBonus / BuyPrice;
				bMayNew = FALSE;
				dBuyCount = 0;
				dSellCount = 0;
				dBuyMoney = 0;
				dSellMoney = 0;
			}
			dBuyCount += pTmpHisData->Amount;	//�����ָ��,���Գɽ������Ȩ��(Ҳ��Amount)
			dBuyMoney += pTmpHisData->Close * dBuyCount;
			pTmpHisData++;
			bBuy2Zero = TRUE;
			continue;
		}
		else
		if (fabs(OutData[EXITLONG][i] - 1) < COMPPREC || fabs(OutData[EXITSHORT][i] - 1) < COMPPREC)	//��ʾ��ǰ����
		{
			dSellCount += pTmpHisData->Amount;
			dSellMoney += pTmpHisData->Close * dSellCount;
			pTmpHisData++;
			bBuy2Zero = FALSE;
			continue;
		}
		else
		{
			if (bBuy2Zero)
				bMayNew = FALSE;
			else
				bMayNew = TRUE;
		}
	}
	if (!bAllInvalid)
	{
		//�������һ�ε�����
		if (dSellCount < COMPPREC)	//����������������
			SellPrice = pTmpHisData->Close;
		else
			SellPrice = dSellMoney / dSellCount;
		if (dBuyCount < COMPPREC)
			BuyPrice = pTmpHisData->Open;
		else
			BuyPrice = dBuyMoney / dBuyCount;
		tmpfBonus = SellPrice - BuyPrice;
		if (tmpfBonus > fMaxYL)
			ftmpMaxBonus = tmpfBonus / BuyPrice;

		if (tmpfBonus >= 0) 
			nYLTimes++;
		else
			nKSTimes++;
		fBonus += tmpfBonus / BuyPrice;

		fBonus -= ftmpMaxBonus;
		if ( fBonus > m_fMaxBonus )		//���浱ǰ�������ƽ������ʱ�Ļ��ִ����Ͳ���ֵ
		{
			m_nYLTimes = nYLTimes;
			m_nKSTimes = nKSTimes;
			m_fMaxBonus = fBonus;
			for ( i = 0; i < pIndex->nParaNum; i++ )
				m_fParaValue[i] = pIndex->aPara[i].nValue[m_nPeriod];
		}
	}
}

//ƽ��ӯ�����
void CTestZBDlg::CalcFormula3(float **OutData, long lReadNum, TINDEXINFO *pIndex)
{
	int		i;
	float	fBonus=0.0, tmpfBonus = 0;	//����,�ðٷ�����ʾ
	BOOL	bFindFirstBuyPoint = FALSE;		//Ҫ���ҵ���һ�����
	BOOL	bMayNew = FALSE;
	BOOL	bAllInvalid = TRUE;	//�жϴ˴μ����Ƿ�ȫΪ��Ч����
	BOOL	bBuy2Zero;		//�ж�������㻹�����������
	float   BuyPrice, SellPrice;	//BuyFlag��ʾ�Ѿ������Ʊ�ı�־,BuyPrice��ʾ�����Ʊ�ļ۸�
	double	dBuyMoney = 0, dSellMoney = 0;	//����ʹ�����ʽ�,����ʹ�����ʽ�
	long	dBuyCount = 0, dSellCount = 0;	//������,������
	LPHISDAT pTmpHisData = m_pAnalyData;
	int		nYLTimes = 0, nKSTimes = 0;

	for (i = 0; i < lReadNum; i++)
	{
		if (!bFindFirstBuyPoint)
			if (fabs(OutData[ENTERLONG][i] - 1) < COMPPREC || fabs(OutData[ENTERSHORT][i] - 1) < COMPPREC)	//һ��Ҫ���ҵ���һ�����
			{
				bFindFirstBuyPoint = TRUE;
				bAllInvalid = FALSE;
			}
			else
				continue;
		if (fabs(OutData[ENTERLONG][i] - 1) < COMPPREC || fabs(OutData[ENTERSHORT][i] - 1) < COMPPREC)	//��ʾ��ǰ���
		{
			if (bMayNew)
			{
				BuyPrice = dBuyMoney / dBuyCount;
				SellPrice = dSellMoney / dSellCount;
				tmpfBonus = (SellPrice - BuyPrice) / BuyPrice;
				if (tmpfBonus >= 0)
				{
					nYLTimes++;
					fBonus += tmpfBonus;
				}
				else
					nKSTimes++;
				bMayNew = FALSE;
				dBuyCount = 0;
				dSellCount = 0;
				dBuyMoney = 0;
				dSellMoney = 0;
			}
			dBuyCount += pTmpHisData->Amount;	//�����ָ��,���Գɽ������Ȩ��(Ҳ��Amount)
			dBuyMoney += pTmpHisData->Close * dBuyCount;
			pTmpHisData++;
			bBuy2Zero = TRUE;
			continue;
		}
		else
		if (fabs(OutData[EXITLONG][i] - 1) < COMPPREC || fabs(OutData[EXITSHORT][i] - 1) < COMPPREC)	//��ʾ��ǰ����
		{
			dSellCount += pTmpHisData->Amount;
			dSellMoney += pTmpHisData->Close * dSellCount;
			pTmpHisData++;
			bBuy2Zero = FALSE;
			continue;
		}
		else
		{
			if (bBuy2Zero)
				bMayNew = FALSE;
			else
				bMayNew = TRUE;
		}
	}
	if (!bAllInvalid)
	{
		//�������һ�ε�����
		if (dSellCount < COMPPREC)	//����������������
			SellPrice = pTmpHisData->Close;
		else
			SellPrice = dSellMoney / dSellCount;
		if (dBuyCount < COMPPREC)
			BuyPrice = pTmpHisData->Open;
		else
			BuyPrice = dBuyMoney / dBuyCount;
		tmpfBonus = (SellPrice - BuyPrice) / (BuyPrice*2);

		if (tmpfBonus >= 0)
		{
			nYLTimes++;
			fBonus += tmpfBonus;
		}
		else
			nKSTimes++;

		if (nYLTimes+nKSTimes != 0)
			fBonus /= nYLTimes+nKSTimes;
		if (fBonus > m_fMaxBonus)		//���浱ǰ������漰��ʱ�Ļ��ִ����Ͳ���ֵ
		{
			m_nYLTimes = nYLTimes;
			m_nKSTimes = nKSTimes;
			m_fMaxBonus = fBonus;
			for ( i = 0; i < pIndex->nParaNum; i++ )
				m_fParaValue[i] = pIndex->aPara[i].nValue[m_nPeriod];
		}
	}
}

//ƽ��������С��
void CTestZBDlg::CalcFormula4(float **OutData, long lReadNum, TINDEXINFO *pIndex)
{
	int		i;
	float	fBonus=0.0, tmpfBonus = 0;	//����,�ðٷ�����ʾ
	BOOL	bFindFirstBuyPoint = FALSE;		//Ҫ���ҵ���һ�����
	BOOL	bMayNew = FALSE;
	BOOL	bAllInvalid = TRUE;	//�жϴ˴μ����Ƿ�ȫΪ��Ч����
	BOOL	bBuy2Zero;		//�ж�������㻹�����������
	float   BuyPrice, SellPrice;	//BuyFlag��ʾ�Ѿ������Ʊ�ı�־,BuyPrice��ʾ�����Ʊ�ļ۸�
	double	dBuyMoney = 0, dSellMoney = 0;	//����ʹ�����ʽ�,����ʹ�����ʽ�
	double	dBuyCount = 0, dSellCount = 0;	//������,������
	LPHISDAT pTmpHisData = m_pAnalyData;
	int		nYLTimes = 0, nKSTimes = 0;

	for (i = 0; i < lReadNum; i++)
	{
		if (!bFindFirstBuyPoint)
			if (fabs(OutData[ENTERLONG][i] - 1) < COMPPREC || fabs(OutData[ENTERSHORT][i] - 1) < COMPPREC)	//һ��Ҫ���ҵ���һ�����
			{
				bFindFirstBuyPoint = TRUE;
				bAllInvalid = FALSE;
			}
			else
				continue;
		if (fabs(OutData[ENTERLONG][i] - 1) < COMPPREC || fabs(OutData[ENTERSHORT][i] - 1) < COMPPREC)	//��ʾ��ǰ���
		{
			if (bMayNew)
			{
				BuyPrice = dBuyMoney / dBuyCount;
				SellPrice = dSellMoney / dSellCount;
				tmpfBonus = (SellPrice - BuyPrice) / BuyPrice;
				if (tmpfBonus >= 0)
					nYLTimes++;
				else
				{
					nKSTimes++;
					fBonus += tmpfBonus;
				}
				bMayNew = FALSE;
				dBuyCount = 0;
				dSellCount = 0;
				dBuyMoney = 0;
				dSellMoney = 0;
			}
			dBuyCount += pTmpHisData->Amount;	//�����ָ��,���Գɽ������Ȩ��(Ҳ��Amount)
			dBuyMoney += pTmpHisData->Close * dBuyCount;
			pTmpHisData++;
			bBuy2Zero = TRUE;
			continue;
		}
		else
		if (fabs(OutData[EXITLONG][i] - 1) < COMPPREC || fabs(OutData[EXITSHORT][i] - 1) < COMPPREC)	//��ʾ��ǰ����
		{
			dSellCount += pTmpHisData->Amount;
			dSellMoney += pTmpHisData->Close * dSellCount;
			pTmpHisData++;
			bBuy2Zero = FALSE;
			continue;
		}
		else
		{
			if (bBuy2Zero)
				bMayNew = FALSE;
			else
				bMayNew = TRUE;
		}
	}
	if (!bAllInvalid)
	{
		//�������һ�ε�����
		if (dSellCount < COMPPREC)	//����������������
			SellPrice = pTmpHisData->Close;
		else
			SellPrice = dSellMoney / dSellCount;
		if (dBuyCount < COMPPREC)
			BuyPrice = pTmpHisData->Open;
		else
			BuyPrice = dBuyMoney / dBuyCount;
		tmpfBonus = (SellPrice - BuyPrice) / (BuyPrice*2);

		if (tmpfBonus >= 0)
			nYLTimes++;
		else
		{
			nKSTimes++;
			fBonus += tmpfBonus;
		}

		if (nYLTimes+nKSTimes != 0)
			fBonus /= nYLTimes+nKSTimes;
		//����fBonus�Ǹ���,���ƽ��������С������Ҫ��">"��
		if (fBonus > m_fMaxBonus)		//���浱ǰ������漰��ʱ�Ļ��ִ����Ͳ���ֵ
		{
			m_nYLTimes = nYLTimes;
			m_nKSTimes = nKSTimes;
			m_fMaxBonus = fBonus;
			for ( i = 0; i < pIndex->nParaNum; i++ )
				m_fParaValue[i] = pIndex->aPara[i].nValue[m_nPeriod];
		}
	}
}

//�������ӯ�����
void CTestZBDlg::CalcFormula5(float **OutData, long lReadNum, TINDEXINFO *pIndex)
{
	int		i;
	float	fBonus=0.0, tmpfBonus = 0, fYLMax = 0;	//����,�ðٷ�����ʾ
	BOOL	bFindFirstBuyPoint = FALSE;		//Ҫ���ҵ���һ�����
	BOOL	bMayNew = FALSE;
	BOOL	bAllInvalid = TRUE;	//�жϴ˴μ����Ƿ�ȫΪ��Ч����
	BOOL	bBuy2Zero;		//�ж�������㻹�����������
	float   BuyPrice, SellPrice;	//BuyFlag��ʾ�Ѿ������Ʊ�ı�־,BuyPrice��ʾ�����Ʊ�ļ۸�
	double	dBuyMoney = 0, dSellMoney = 0;	//����ʹ�����ʽ�,����ʹ�����ʽ�
	double	dBuyCount = 0, dSellCount = 0;	//������,������
	LPHISDAT pTmpHisData = m_pAnalyData;
	int		nYLTimes = 0, nKSTimes = 0;

	for (i = 0; i < lReadNum; i++)
	{
		if (!bFindFirstBuyPoint)
			if (fabs(OutData[ENTERLONG][i] - 1) < COMPPREC || fabs(OutData[ENTERSHORT][i] - 1) < COMPPREC)	//һ��Ҫ���ҵ���һ�����
			{
				bFindFirstBuyPoint = TRUE;
				bAllInvalid = FALSE;
			}
			else
				continue;
		if (fabs(OutData[ENTERLONG][i] - 1) < COMPPREC || fabs(OutData[ENTERSHORT][i] - 1) < COMPPREC)	//��ʾ��ǰ���
		{
			if (bMayNew)
			{
				BuyPrice = dBuyMoney / dBuyCount;
				SellPrice = dSellMoney / dSellCount;
				tmpfBonus = SellPrice - BuyPrice;
				if (tmpfBonus >= 0)
				{
					nYLTimes++;
					if (tmpfBonus > fYLMax)
					{
						fBonus = tmpfBonus / BuyPrice;
						fYLMax = tmpfBonus;
					}
				}
				else
					nKSTimes++;
				bMayNew = FALSE;
				dBuyCount = 0;
				dSellCount = 0;
				dBuyMoney = 0;
				dSellMoney = 0;
			}
			dBuyCount += pTmpHisData->Amount;	//�����ָ��,���Գɽ������Ȩ��(Ҳ��Amount)
			dBuyMoney += pTmpHisData->Close * dBuyCount;
			pTmpHisData++;
			bBuy2Zero = TRUE;
			continue;
		}
		else
		if (fabs(OutData[EXITLONG][i] - 1) < COMPPREC || fabs(OutData[EXITSHORT][i] - 1) < COMPPREC)	//��ʾ��ǰ����
		{
			dSellCount += pTmpHisData->Amount;
			dSellMoney += pTmpHisData->Close * dSellCount;
			pTmpHisData++;
			bBuy2Zero = FALSE;
			continue;
		}
		else
		{
			if (bBuy2Zero)
				bMayNew = FALSE;
			else
				bMayNew = TRUE;
		}
	}
	if (!bAllInvalid)
	{
		//�������һ�ε�����
		if (dSellCount < COMPPREC)	//����������������
			SellPrice = pTmpHisData->Close;
		else
			SellPrice = dSellMoney / dSellCount;
		if (dBuyCount < COMPPREC)
			BuyPrice = pTmpHisData->Open;
		else
			BuyPrice = dBuyMoney / dBuyCount;
		tmpfBonus = (SellPrice - BuyPrice) / 2;

		if (tmpfBonus >= 0)
		{
			nYLTimes++;
			if (tmpfBonus > fYLMax)
				fBonus = tmpfBonus / BuyPrice;
		}
		else
			nKSTimes++;

		if (fBonus > m_fMaxBonus)		//���浱ǰ������漰��ʱ�Ļ��ִ����Ͳ���ֵ
		{
			m_nYLTimes = nYLTimes;
			m_nKSTimes = nKSTimes;
			m_fMaxBonus = fBonus;
			for ( i = 0; i < pIndex->nParaNum; i++ )
				m_fParaValue[i] = pIndex->aPara[i].nValue[m_nPeriod];
		}
	}
}

//������������С��
void CTestZBDlg::CalcFormula6(float **OutData, long lReadNum, TINDEXINFO *pIndex)
{
	int		i;
	float	fBonus=0.0, tmpfBonus = 0, fKSMax = 0;	//����,�ðٷ�����ʾ
	BOOL	bFindFirstBuyPoint = FALSE;		//Ҫ���ҵ���һ�����
	BOOL	bMayNew = FALSE;
	BOOL	bAllInvalid = TRUE;	//�жϴ˴μ����Ƿ�ȫΪ��Ч����
	BOOL	bBuy2Zero;		//�ж�������㻹�����������
	float   BuyPrice, SellPrice;	//BuyFlag��ʾ�Ѿ������Ʊ�ı�־,BuyPrice��ʾ�����Ʊ�ļ۸�
	double	dBuyMoney = 0, dSellMoney = 0;	//����ʹ�����ʽ�,����ʹ�����ʽ�
	double	dBuyCount = 0, dSellCount = 0;	//������,������
	LPHISDAT pTmpHisData = m_pAnalyData;
	int		nYLTimes = 0, nKSTimes = 0;

	for (i = 0; i < lReadNum; i++)
	{
		if (!bFindFirstBuyPoint)
			if (fabs(OutData[ENTERLONG][i] - 1) < COMPPREC || fabs(OutData[ENTERSHORT][i] - 1) < COMPPREC)	//һ��Ҫ���ҵ���һ�����
			{
				bFindFirstBuyPoint = TRUE;
				bAllInvalid = FALSE;
			}
			else
				continue;
		if (fabs(OutData[ENTERLONG][i] - 1) < COMPPREC || fabs(OutData[ENTERSHORT][i] - 1) < COMPPREC)	//��ʾ��ǰ���
		{
			if (bMayNew)
			{
				BuyPrice = dBuyMoney / dBuyCount;
				SellPrice = dSellMoney / dSellCount;
				tmpfBonus = SellPrice - BuyPrice;
				if (tmpfBonus >= 0)
					nYLTimes++;
				else
				{
					nKSTimes++;
					if (tmpfBonus > fKSMax)
					{
						fBonus = tmpfBonus / BuyPrice;
						fKSMax = tmpfBonus;
					}
				}
				bMayNew = FALSE;
				dBuyCount = 0;
				dSellCount = 0;
				dBuyMoney = 0;
				dSellMoney = 0;
			}
			dBuyCount += pTmpHisData->Amount;	//�����ָ��,���Գɽ������Ȩ��(Ҳ��Amount)
			dBuyMoney += pTmpHisData->Close * dBuyCount;
			pTmpHisData++;
			bBuy2Zero = TRUE;
			continue;
		}
		else
		if (fabs(OutData[EXITLONG][i] - 1) < COMPPREC || fabs(OutData[EXITSHORT][i] - 1) < COMPPREC)	//��ʾ��ǰ����
		{
			dSellCount += pTmpHisData->Amount;
			dSellMoney += pTmpHisData->Close * dSellCount;
			pTmpHisData++;
			bBuy2Zero = FALSE;
			continue;
		}
		else
		{
			if (bBuy2Zero)
				bMayNew = FALSE;
			else
				bMayNew = TRUE;
		}
	}
	if (!bAllInvalid)
	{
		//�������һ�ε�����
		if (dSellCount < COMPPREC)	//����������������
			SellPrice = pTmpHisData->Close;
		else
			SellPrice = dSellMoney / dSellCount;
		if (dBuyCount < COMPPREC)
			BuyPrice = pTmpHisData->Open;
		else
			BuyPrice = dBuyMoney / dBuyCount;
		tmpfBonus = (SellPrice - BuyPrice) / 2;

		if (tmpfBonus >= 0)
			nYLTimes++;
		else
		{
			nKSTimes++;
			if (tmpfBonus > fKSMax)
				fBonus = tmpfBonus / BuyPrice;
		}

		if (fBonus > m_fMaxBonus)		//���浱ǰ������漰��ʱ�Ļ��ִ����Ͳ���ֵ
		{
			m_nYLTimes = nYLTimes;
			m_nKSTimes = nKSTimes;
			m_fMaxBonus = fBonus;
			for ( i = 0; i < pIndex->nParaNum; i++ )
				m_fParaValue[i] = pIndex->aPara[i].nValue[m_nPeriod];
		}
	}
}

//ӯ��ϵ�����
void CTestZBDlg::CalcFormula7(float **OutData, long lReadNum, TINDEXINFO *pIndex)
{
	int		i;
	float	fYLBonus=0.0, fKSBonus = 0.0, tmpfBonus = 0;	//����,�ðٷ�����ʾ
	BOOL	bFindFirstBuyPoint = FALSE;		//Ҫ���ҵ���һ�����
	BOOL	bMayNew = FALSE;
	BOOL	bAllInvalid = TRUE;	//�жϴ˴μ����Ƿ�ȫΪ��Ч����
	BOOL	bBuy2Zero;		//�ж�������㻹�����������
	float   BuyPrice, SellPrice;	//BuyFlag��ʾ�Ѿ������Ʊ�ı�־,BuyPrice��ʾ�����Ʊ�ļ۸�
	double	dBuyMoney = 0, dSellMoney = 0;	//����ʹ�����ʽ�,����ʹ�����ʽ�
	double	dBuyCount = 0, dSellCount = 0;	//������,������
	LPHISDAT pTmpHisData = m_pAnalyData;
	int		nYLTimes = 0, nKSTimes = 0;

	for (i = 0; i < lReadNum; i++)
	{
		if (!bFindFirstBuyPoint)
			if (fabs(OutData[ENTERLONG][i] - 1) < COMPPREC || fabs(OutData[ENTERSHORT][i] - 1) < COMPPREC)	//һ��Ҫ���ҵ���һ�����
			{
				bFindFirstBuyPoint = TRUE;
				bAllInvalid = FALSE;
			}
			else
				continue;
		if (fabs(OutData[ENTERLONG][i] - 1) < COMPPREC || fabs(OutData[ENTERSHORT][i] - 1) < COMPPREC)	//��ʾ��ǰ���
		{
			if (bMayNew)
			{
				BuyPrice = dBuyMoney / dBuyCount;
				SellPrice = dSellMoney / dSellCount;
				tmpfBonus = SellPrice - BuyPrice;
				if (tmpfBonus >= 0)
				{
					nYLTimes++;
					fYLBonus += tmpfBonus;
				}
				else
				{
					nKSTimes++;
					fKSBonus += tmpfBonus;
				}
				bMayNew = FALSE;
				dBuyCount = 0;
				dSellCount = 0;
				dBuyMoney = 0;
				dSellMoney = 0;
			}
			dBuyCount += pTmpHisData->Amount;	//�����ָ��,���Գɽ������Ȩ��(Ҳ��Amount)
			dBuyMoney += pTmpHisData->Close * dBuyCount;
			pTmpHisData++;
			bBuy2Zero = TRUE;
			continue;
		}
		else
		if (fabs(OutData[EXITLONG][i] - 1) < COMPPREC || fabs(OutData[EXITSHORT][i] - 1) < COMPPREC)	//��ʾ��ǰ����
		{
			dSellCount += pTmpHisData->Amount;
			dSellMoney += pTmpHisData->Close * dSellCount;
			pTmpHisData++;
			bBuy2Zero = FALSE;
			continue;
		}
		else
		{
			if (bBuy2Zero)
				bMayNew = FALSE;
			else
				bMayNew = TRUE;
		}
	}
	if (!bAllInvalid)
	{
		//�������һ�ε�����
		if (dSellCount < COMPPREC)	//����������������
			SellPrice = pTmpHisData->Close;
		else
			SellPrice = dSellMoney / dSellCount;
		if (dBuyCount < COMPPREC)
			BuyPrice = pTmpHisData->Open;
		else
			BuyPrice = dBuyMoney / dBuyCount;
		tmpfBonus = (SellPrice - BuyPrice)/2;

		if (tmpfBonus >= 0)
		{
			nYLTimes++;
			fYLBonus += tmpfBonus;
		}
		else
		{
			nKSTimes++;
			fKSBonus += tmpfBonus;
		}

		if (fabs(fYLBonus) < COMPPREC)
			tmpfBonus = fKSBonus;
		else
			tmpfBonus = (fabs(fYLBonus)-fabs(fKSBonus))/(fabs(fYLBonus)+fabs(fKSBonus));
		if (tmpfBonus > m_fMaxBonus)		//���浱ǰ������漰��ʱ�Ļ��ִ����Ͳ���ֵ
		{
			m_nYLTimes = nYLTimes;
			m_nKSTimes = nKSTimes;
			m_fMaxBonus = tmpfBonus;
			for ( i = 0; i < pIndex->nParaNum; i++ )
				m_fParaValue[i] = pIndex->aPara[i].nValue[m_nPeriod];
		}
	}
}

void CTestZBDlg::CalcBonus(TINDEXINFO *pIndex,long ReadNum, int nType)
{
	float *pOutData[MAX_LINEOUT];

	for (int i=0; i<4;i++) 
		pOutData[i] = g_pCalc->_CallDTopViewCalc_13(ReadNum);
	if ( g_pCalc->_CallDTopViewCalc_5(EXP_TYPE, m_nCurrTestZB, pOutData) == 0)	//����ɹ�
	{
		switch (nType)
		{
		case 0://���������
			CalcFormula0(pOutData, ReadNum, pIndex);
			break;
		case 1://ƽ���������
			CalcFormula1(pOutData, ReadNum, pIndex);
			break;
		case 2://�۳����ӯ�����������
			CalcFormula2(pOutData, ReadNum, pIndex);
			break;
		case 3://ƽ��ӯ�����
			CalcFormula3(pOutData, ReadNum, pIndex);
			break;
		case 4://ƽ��������С��
			CalcFormula4(pOutData, ReadNum, pIndex);
			break;
		case 5://�������ӯ�����
			CalcFormula5(pOutData, ReadNum, pIndex);
			break;
		case 6://������������С��
			CalcFormula6(pOutData, ReadNum, pIndex);
			break;
		case 7://ӯ��ϵ�����
			CalcFormula7(pOutData, ReadNum, pIndex);
			break;
		}
	}
	for (i=0; i<4; i++) g_pCalc->_CallDTopViewCalc_14(pOutData[i]);
}

void CTestZBDlg::OnSelchangeCombo2() 
{
	int nSel;

	nSel = m_PeriodCombo.GetCurSel();
	if (nSel != DAY_ST && nSel != WEEK_ST && nSel != MONTH_ST && nSel != DAYN_ST && nSel != SEASON_ST && nSel != YEAR_ST)
	{
		m_nDontUseRecentData = 0;
		GetDlgItem(IDC_RADIO14)->EnableWindow(FALSE);
		UpdateData(FALSE);
	}
	else
		GetDlgItem(IDC_RADIO14)->EnableWindow(TRUE);
	OnRadio14();
}

void CTestZBDlg::OnRadio14() 
{
	UpdateData(TRUE);
	GetDlgItem(IDC_DATETIMEPICKER1)->EnableWindow(m_nDontUseRecentData==1);
	GetDlgItem(IDC_DATETIMEPICKER2)->EnableWindow(m_nDontUseRecentData==1);
}
