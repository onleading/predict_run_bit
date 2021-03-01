// ParamTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "ParamTestDlg.h"
#include "DescZBDlg.h"
#include "SelectGPDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static int CALLBACK MyCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
static int nCurrSortType;
static TCHAR *syTitle[] = {"股票名称","同期涨幅","总收益(%)","盈利次数", "亏损次数","平均收益(%)","去顶收益(%)", "盈利平均(%)","亏损平均(%)","最大收益(%)","最大亏损(%)","交易次数","盈亏系数(%)",NULL};
static int	  nSyWidth[] = {90,80,80,65,65,80,80,80,80,80,80,65,80};
static TCHAR *syTjTitle[] = {"股票名称","同期涨幅","交易次数","总计最大涨幅(%)","平均最大涨幅(%)","总计最小亏损(%)","平均最小亏损(%)","加权均价总盈亏(%)","加权均价平均盈亏(%)","统计周期总盈亏(%)","统计周期平均盈亏(%)",NULL};
static int    nSyTjWidth[] = {90,80,65,120,120,120,120,140,140,140,140};
static TCHAR *ccTitle[] = {"股票名称","总收益(%)","盈利次数", "亏损次数","指标参数",NULL};
static int	  nCcWidth[] = {80, 80, 65, 65, 90};
static TCHAR *ccTjTitle[] = {"股票名称","目标收益(%)","交易次数","指标参数",NULL};
static int    nCcTjWidth[] = {80,80,65,90};
/////////////////////////////////////////////////////////////////////////////
// CParamTestDlg dialog


CParamTestDlg::CParamTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CParamTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CParamTestDlg)
	m_BeginTime = 0;
	m_EndTime = 0;
	m_sParam1 = _T("");
	m_sParam2 = _T("");
	m_sParam3 = _T("");
	m_sParam4 = _T("");
	m_sParam5 = _T("");
	m_sParam6 = _T("");
	m_nDontNeedFee = -1;
	m_nOptimizeParam = -1;
	m_nTestPeriod = 20;
	//}}AFX_DATA_INIT
	m_pShowDetail = NULL;
	for(int i=0;i<13;i++)
	{
		m_showhat[i]=TRUE;
	}
	m_aDetails.reserve(62);
}

CParamTestDlg::~CParamTestDlg()
{
	if (m_pCache != NULL)
		delete m_pCache;
	m_pCache = NULL;
	m_aDetails.clear();
	m_pShowDetail = NULL;
}

void CParamTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CParamTestDlg)
	DDX_Control(pDX, IDC_SHOWMORE, m_showMore);
	DDX_Control(pDX, IDC_EDIT_NUM, m_GPTotal);
	DDX_Control(pDX, IDC_STATICINFO, m_OutInfo);
	DDX_Control(pDX, IDC_COMBO3, m_AimCombo);
	DDX_Control(pDX, IDC_COMBO2, m_PeriodCombo);
	DDX_Control(pDX, IDC_COMBO1, m_NameCombo);
	DDX_Control(pDX, IDC_LIST1, m_List);
	DDX_Control(pDX, IDC_PROGRESS1, m_Progress);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_BeginTime);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER2, m_EndTime);
	DDX_Text(pDX, IDC_EDIT_PARAM1, m_sParam1);
	DDX_Text(pDX, IDC_EDIT_PARAM2, m_sParam2);
	DDX_Text(pDX, IDC_EDIT_PARAM3, m_sParam3);
	DDX_Text(pDX, IDC_EDIT_PARAM4, m_sParam4);
	DDX_Text(pDX, IDC_EDIT_PARAM5, m_sParam5);
	DDX_Text(pDX, IDC_EDIT_PARAM6, m_sParam6);
	DDX_Radio(pDX, IDC_NEEDCOMMISSION, m_nDontNeedFee);
	DDX_Radio(pDX, IDC_OPARAM, m_nOptimizeParam);
	DDX_Text(pDX, IDC_EDIT_PERIOD, m_nTestPeriod);
	DDV_MinMaxInt(pDX, m_nTestPeriod, 2, 9999);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CParamTestDlg, CDialog)
	//{{AFX_MSG_MAP(CParamTestDlg)
	ON_BN_CLICKED(IDC_DESC, OnDesc)
	ON_BN_CLICKED(IDC_ADDGP, OnAddgp)
	ON_BN_CLICKED(IDC_DELGP, OnDelgp)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
	ON_BN_CLICKED(IDC_OPARAM, OnOparam)
	ON_BN_CLICKED(IDC_TESTPARAM, OnTestparam)
	ON_BN_CLICKED(IDC_SHOWMORE, OnShowmore)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST1, OnColumnclickList)
	ON_BN_CLICKED(IDC_SHOWDETAIL, OnShowdetail)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
	ON_BN_CLICKED(IDC_DELGPALL, OnDelgpall)
	ON_BN_CLICKED(IDC_SHOWGGDETAIL, OnShowggdetail)
	ON_BN_CLICKED(IDC_MODGS, OnModgs)
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(IDC_AverageBonus,IDC_YKCoeff,OnSelShowMore1)
	ON_COMMAND_RANGE(IDC_TOLHIGH,IDC_AVEREND,OnSelShowMore2)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CParamTestDlg message handlers

BOOL CParamTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	if(m_nTestType==EXP_TYPE) SetWindowText(_F("交易系统参数测试"));
	else if(m_nTestType==TJ_TYPE) SetWindowText(_F("条件选股参数测试"));
	else return(FALSE);

	GetDlgItem(IDC_SHOWDETAIL)->EnableWindow(FALSE);
	GetDlgItem(IDC_SHOWGGDETAIL)->EnableWindow(FALSE);
	SetDateCtrlRange(GetDlgItem(IDC_DATETIMEPICKER1));
	SetDateCtrlRange(GetDlgItem(IDC_DATETIMEPICKER2));

	m_OutInfo.SetTextColor(RGB(240,0,0));
	m_OutInfo.SetWindowText(_F("测试前,请确保下载的日线数据完整."));

	m_AimCombo.SetItemHeight(0,16);
	m_AimCombo.SetItemHeight(-1,16);
	m_PeriodCombo.SetItemHeight(0,16);
	m_PeriodCombo.SetItemHeight(-1,16);

	//填充计算周期列表框
	for(int i = 0; g_CalcPeriodStr[i] != NULL ;i++)
		m_PeriodCombo.AddString (_F(g_CalcPeriodStr[i]));
	m_PeriodCombo.SetCurSel (DAY_ST);	//默认是日线
	//填充目标串
	for(i = 0; i < 8;i++)
		m_AimCombo.AddString(_F(AmiComboStr[i]));
	m_AimCombo.SetCurSel(0);
	
	m_NameCombo.SetItemHeight(0,16);
	m_NameCombo.SetItemHeight(-1,16);

	CString		tmpStr;
	TINDEXINFO	*tmp;
	m_pCache = NULL;
	m_nAddedGPNum = 0;
	m_nCurrPos = -1;
	m_nYLTimes = 0;
	m_nKSTimes = 0;
	m_ImageList.Create(16,16,ILC_COLOR8|ILC_MASK,1,1);
	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_GP));
	m_ImageList.Add(hIcon);	
	m_List.SetImageList(&m_ImageList,LVSIL_SMALL);

	m_nDontNeedFee = 0;					//默认需要扣除手续费
	m_PeriodCombo.SetCurSel(DAY_ST);	//缺省设置日线为测试周期
	m_AimCombo.SetCurSel(0);
	m_nCalcPeriod = DAY_ST;
	m_GPTotal.SetWindowText("");
	m_Progress.SetPos(0);				//进度条位置起始在0处
	((CComboBox*)GetDlgItem(IDC_COMBO3))->ResetContent();
	if(m_nTestType==EXP_TYPE)
	{
		GetDlgItem(IDC_TYPE)->SetWindowText(_F("交易系统"));
		GetDlgItem(IDC_PERIOD)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_PERIOD)->ShowWindow(SW_HIDE);
		for(i=5;i<13;i++)
		{
			m_showhat[i]=FALSE;
		}
		i = 0;
		do
		{
			tmp = g_pCalc->_CallDTopViewCalc_8(EXP_TYPE, i);
			if (tmp != NULL)
			{
				tmpStr.Format("%s  - %s", tmp->acCode, tmp->acName);
				m_NameCombo.AddString(_F(tmpStr));
			}
			i++;
		}while(tmp != NULL);
		m_AimCombo.AddString(_F("总收益最大化"));
		m_AimCombo.AddString(_F("平均收益最大化"));
		m_AimCombo.AddString(_F("扣除最大赢利后收益最大化"));
		m_AimCombo.AddString(_F("平均赢利最大化"));
		m_AimCombo.AddString(_F("平均亏损最小化"));
		m_AimCombo.AddString(_F("单次最大赢利最大化"));
		m_AimCombo.AddString(_F("单次最大亏损最小化"));
		m_AimCombo.AddString(_F("盈亏系数最大化"));
		m_AimCombo.SetCurSel(0);
	}
	if(m_nTestType==TJ_TYPE)
	{
		GetDlgItem(IDC_SHOWDETAIL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TYPE)->SetWindowText(_F("条件选股"));
		GetDlgItem(IDC_PERIOD)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_PERIOD)->ShowWindow(SW_SHOW);
		for(i=1;i<5;i++)
		{
			m_showhat[2*i+1]=FALSE;
		}
		i = 0;
		do
		{
			tmp = g_pCalc->_CallDTopViewCalc_8(TJ_TYPE, i);
			if (tmp != NULL)
			{
				tmpStr.Format("%s  - %s", tmp->acCode, tmp->acName);
				m_NameCombo.AddString(_F(tmpStr));
			}
			i++;
		}while(tmp != NULL);
		m_AimCombo.AddString(_F("最高涨幅总收益最大化"));
		m_AimCombo.AddString(_F("最高涨幅平均收益最大化"));
		m_AimCombo.AddString(_F("最低亏损总收益最大化"));
		m_AimCombo.AddString(_F("最低亏损平均收益最大化"));
		m_AimCombo.AddString(_F("加权均价总收益最大化"));
		m_AimCombo.AddString(_F("加权均价平均收益最大化"));
		m_AimCombo.AddString(_F("固定周期总收益最大化"));
		m_AimCombo.AddString(_F("固定周期平均收益最大化"));
		m_AimCombo.SetCurSel(1);
	}
	m_NameCombo.SetCurSel(0);
	OnTestparam();
	SetParam();

	CTimeSpan TimeSpan(365, 0, 0, 0);
	m_EndTime = CTime::GetCurrentTime();
	m_BeginTime = m_EndTime - TimeSpan;

	m_IsCarrying = FALSE;
	m_IsCarried = TRUE;
	UpdateData(FALSE);
	return TRUE;
}

void CParamTestDlg::FillLvc()
{
	m_List.SetRedraw(FALSE);
	int nColumnCount = m_List.GetHeaderCtrl()->GetItemCount();	
	for(int i=0;i<nColumnCount;i++)
	{
		m_List.DeleteColumn(0);
	}
	LVCOLUMN  lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvc.fmt = LVCFMT_CENTER;
	int nStrLen = 0;
	if(m_nTestType==EXP_TYPE)
	{
		if(!m_nOptimizeParam)
		{
			for (i = 0; ccTitle[i]!=NULL; i++)
			{
				lvc.pszText = _F(ccTitle[i]);
				lvc.cx = nCcWidth[i];
				m_List.InsertColumn(i,&lvc);
				nStrLen += nCcWidth[i];
			}
		}
		else
		{
			for(i=0;syTitle[i]!=NULL;i++)
			{
				if(m_showhat[i])
				{
					lvc.pszText=_F(syTitle[i]);
					lvc.cx = nSyWidth[i];
					m_List.InsertColumn(i,&lvc);
					nStrLen += nSyWidth[i];
				}
			}
		}
	}
	else if(m_nTestType==TJ_TYPE)
	{
		if(!m_nOptimizeParam)
		{
			for (i = 0; ccTjTitle[i]!=NULL; i++)
			{
				lvc.pszText = _F(ccTjTitle[i]);
				lvc.cx = nCcTjWidth[i];
				m_List.InsertColumn(i,&lvc);
				nStrLen += nCcTjWidth[i];
			}
		}
		else
		{
			for(i=0;syTjTitle[i]!=NULL;i++)
			{
				if(m_showhat[i])
				{
					lvc.pszText=_F(syTjTitle[i]);
					lvc.cx = nSyTjWidth[i];
					m_List.InsertColumn(i,&lvc);
					nStrLen += nSyTjWidth[i];
				}
			}
		}
	}
	CRect rtList;
	m_List.GetWindowRect(&rtList);
	SCROLLINFO scroll;
	m_List.GetScrollInfo(SB_HORZ, &scroll);
	scroll.nMax = nStrLen + 4;
	scroll.nPage = rtList.Width()/3;
	scroll.nMin = rtList.Width()/9;
	scroll.nPos = 0;
	m_List.SetScrollInfo(SB_HORZ, &scroll);
	if(nStrLen>rtList.Width()) 
		m_List.ShowScrollBar(SB_HORZ);
	if(m_nAddedGPNum>0)
		FillGPReport();
}

void CParamTestDlg::SetParam()
{
	TINDEXINFO	*tmp;
	int nPos, nParamCount = 0;
	
	nPos = m_NameCombo.GetCurSel();
	if(m_nTestType==EXP_TYPE) tmp = g_pCalc->_CallDTopViewCalc_8(EXP_TYPE, nPos);
	if(m_nTestType==TJ_TYPE) tmp = g_pCalc->_CallDTopViewCalc_8(TJ_TYPE, nPos);
	GetDlgItem(IDC_EDIT_PARAM1)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_PARAM2)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_PARAM3)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_PARAM4)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_PARAM5)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_PARAM6)->ShowWindow(SW_HIDE);
	if (tmp != NULL)
	{
		nParamCount = tmp->nParaNum;
		if (nParamCount > 0)
		{
			m_sParam1.Format("%.2f", tmp->aPara[0].nValue[m_nCalcPeriod]);
			GetDlgItem(IDC_EDIT_PARAM1)->ShowWindow(SW_SHOW);
		}
		if (nParamCount > 1)
		{
			m_sParam2.Format("%.2f", tmp->aPara[1].nValue[m_nCalcPeriod]);
			GetDlgItem(IDC_EDIT_PARAM2)->ShowWindow(SW_SHOW);
		}
		if (nParamCount > 2)
		{
			m_sParam3.Format("%.2f", tmp->aPara[2].nValue[m_nCalcPeriod]);
			GetDlgItem(IDC_EDIT_PARAM3)->ShowWindow(SW_SHOW);
		}
		if (nParamCount > 3)
		{
			m_sParam4.Format("%.2f", tmp->aPara[3].nValue[m_nCalcPeriod]);
			GetDlgItem(IDC_EDIT_PARAM4)->ShowWindow(SW_SHOW);
		}
		if (nParamCount > 4)
		{
			m_sParam5.Format("%.2f", tmp->aPara[4].nValue[m_nCalcPeriod]);
			GetDlgItem(IDC_EDIT_PARAM5)->ShowWindow(SW_SHOW);
		}
		if (nParamCount > 5)
		{
			m_sParam6.Format("%.2f", tmp->aPara[5].nValue[m_nCalcPeriod]);
			GetDlgItem(IDC_EDIT_PARAM6)->ShowWindow(SW_SHOW);
		}
		UpdateData(FALSE);
	}
}

void CParamTestDlg::OnDesc() 
{
	TINDEXINFO	*tmp;
	CDescZBDlg dlg;
	int nPos;
	nPos = m_NameCombo.GetCurSel();
	if(m_nTestType==EXP_TYPE) tmp = g_pCalc->_CallDTopViewCalc_8(EXP_TYPE, nPos);
	if(m_nTestType==TJ_TYPE) tmp = g_pCalc->_CallDTopViewCalc_8(TJ_TYPE, nPos);
	if(!tmp) return;
	dlg.Init(tmp->nSet, tmp->acCode);
	dlg.DoModal();
}


void CParamTestDlg::OnAddgp() 
{
	CSelectGPDlg dlg;
	dlg.bHasDS = FALSE;
	if( dlg.DoModal() == IDOK )
	{
		int i,j;
		BOOL bFind;
		for(i = 0;i < dlg.SelectedGPNum;i++)
		{
			bFind = FALSE;
			for(j = 0;j < m_nAddedGPNum;j++)
			{
				if(m_nAddedGPIndex[j] == dlg.SelectedGPIndex[i])
				{
					bFind = TRUE;
					break;
				}
			}
			if(!bFind)  m_nAddedGPIndex[m_nAddedGPNum++] = dlg.SelectedGPIndex[i];
		}
		FillGPReport();
	}
	if(m_nAddedGPNum>0) 
	{
		GetDlgItem(IDC_SHOWDETAIL)->EnableWindow(TRUE);
		int selIndex=m_List.GetNextItem(-1,LVNI_SELECTED);
		if(selIndex!=-1)
			GetDlgItem(IDC_SHOWGGDETAIL)->EnableWindow(TRUE);
	}
}

void CParamTestDlg::OnDelgp() 
{
	POSITION pos = m_List.GetFirstSelectedItemPosition();
	if( pos != NULL )
	{
		int nItem,i,j;
		BOOL	bFirst = TRUE;
		while (pos) //先置删除标记-1
		{
			nItem = m_List.GetNextSelectedItem(pos);
			if (bFirst)
			{
				m_nCurrPos = nItem;
				bFirst = FALSE;
			}
			m_nAddedGPIndex[nItem] = -1;
		}
		for(i = m_nAddedGPNum - 1;i >= 0;i--)
		{
			if(m_nAddedGPIndex[i] == -1)
			{
				for(j = i+1;j < m_nAddedGPNum;j++)
					m_nAddedGPIndex[j-1] = m_nAddedGPIndex[j];
				m_nAddedGPNum--;
			}
		}
		FillGPReport();
	}
	if(m_nAddedGPNum<1) 
	{
		GetDlgItem(IDC_SHOWDETAIL)->EnableWindow(FALSE);
		GetDlgItem(IDC_SHOWGGDETAIL)->EnableWindow(FALSE);
	}
}

void CParamTestDlg::FillGPReport()
{
	int i;
	LVITEM lvi;
	CString	tmpStr;

	m_List.SetRedraw(FALSE);
	m_List.DeleteAllItems();
	lvi.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
	lvi.state = 0;
	lvi.stateMask = 0;
	lvi.iSubItem = 0;
	for(i = 0;i < m_nAddedGPNum;i++)
	{
		lvi.iItem = i;
		lvi.iImage = 0;
		lvi.pszText = (*g_pStockIO)[m_nAddedGPIndex[i]]->Name;
		m_List.InsertItem(&lvi);
		m_List.SetItemData(i,MAKELPARAM(0,m_nAddedGPIndex[i]));//仅为了拖曳
	}
	m_List.SetRedraw(TRUE);
	m_List.SetFocus();
	if (m_nCurrPos >= m_List.GetItemCount())
		m_nCurrPos = m_List.GetItemCount() - 1;
	if (m_nCurrPos < 0) m_nCurrPos = 0;
	m_List.SetItemState(m_nCurrPos,LVIS_FOCUSED,LVIS_FOCUSED);
	m_List.SetItemState(m_nCurrPos,LVIS_SELECTED,LVIS_SELECTED);
	tmpStr.Format("%d", m_nAddedGPNum);
	m_GPTotal.SetWindowText(tmpStr);
}

double CParamTestDlg::getsxf(const char *code,short setcode)	//没有算起步价
{
	if (setcode == SZ)
	{
		if (code[0] == 2)		
			return 0.0055;		//深圳B股
		else if (code[0] == '0')
			return 0.0055;      //深圳A股
		else if (code[0] == '1' && (code[0] == '7' || code[0] == '8'))
			return 0.0025;      //深圳基金
	}
	else if (setcode == SH)
	{
		if (code[0] == '7')			
			return 0.000;	//上海新股
		else if (code[0] == '5')	
			return 0.0025;	//上海基金
		else if (code[0] == '9')	
			return 0.0055;	//上海B股
		else if (code[0] == '6')	
			return 0.0065;	//上海A股
	}
	return 0.000;
}

BOOL CParamTestDlg::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_DELETE && pMsg->hwnd == m_List.m_hWnd)
		{
			PostMessage(WM_COMMAND,IDC_DELGP);
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CParamTestDlg::OnCancel() 
{
	if(!m_IsCarrying && m_IsCarried)
	{
		GetParent() ->PostMessage (WM_CANCELPRESSED,(WPARAM)this,0);
		g_bDonotClose = FALSE;
	}
	else
		m_IsCarrying = FALSE;
}

void CParamTestDlg::FindOptParam(int nParam, float *aParam)
{
	TINDEXINFO	*tmp;
	int nOneNeedNum = 0;
	long nPos = m_NameCombo.GetCurSel();
	if(m_nTestType==EXP_TYPE) 
		tmp = g_pCalc->_CallDTopViewCalc_8(EXP_TYPE, nPos);
	if(m_nTestType==TJ_TYPE) 
		tmp = g_pCalc->_CallDTopViewCalc_8(TJ_TYPE, nPos);
	if(!tmp) 
		return;
	ASSERT(nParam == tmp->nParaNum);
	int i, iMax = 0;
	nOneNeedNum = 1 + (tmp->aPara[0].nMax -tmp->aPara[0].nMin)/tmp->aPara[0].nStep;
	for ( i = 1; i < tmp->nParaNum; i++ )
		nOneNeedNum *= 1 + (tmp->aPara[i].nMax - tmp->aPara[i].nMin)/tmp->aPara[i].nStep;
	float fMax = m_pCache[0];
	for(i = 1; i < nOneNeedNum; i++)
	{
		if(m_pCache[i] > fMax) 
		{
			iMax = i;
			fMax = m_pCache[i]; 
		}
	}
	int *nStep = new int[nParam];
	for(i = 0; i<nParam; i++)
	{
		nStep[i] = 1 + (tmp->aPara[i].nMax - tmp->aPara[i].nMin)/tmp->aPara[i].nStep;
	}
	for(i = nParam-1; i >= 0; i--)
	{
		int nDiv = 1;
		for(int j = nParam-1; j>i; j--) nDiv *= nStep[j];
		aParam[i] =tmp->aPara[i].nMin + tmp->aPara[i].nStep*((iMax/nDiv)%nStep[i]);
	}
	delete []nStep;nStep = NULL;
}

#define CALCBONUS(number, type, optIndex)\
{\
	CalcBonus(tmp,number, type, optIndex);\
	CalcedNum++;\
	if(CalcedNum % 50 == 0)\
	{\
		m_Progress.StepIt();\
	}\
	if (PeekMyMsg()) goto CancelLabel;\
}

void CParamTestDlg::OnOK() 
{
	TINDEXINFO	*tmp, IndexBakup;
	CString	tmpStr;
	long	i, j, nPos, nAim, tempCalcNum = 0;
	long	lTestCount, lReadNum;
	int		TotalCalcNum = 0, nOneNeedNum = 0;	//TotalCalcNum存放总共需要计算的次数
	int		CalcedNum = 0;	//CalcedNum存放当前已经计算的数量
	NTime	time1, time2;
	
	UpdateData(TRUE);
	memset(&IndexBakup, 0, sizeof(TINDEXINFO));
	nPos = m_NameCombo.GetCurSel();
	if (m_BeginTime > m_EndTime)
	{
		TDX_MessageBox(m_hWnd,"起始日期应在终至日期之前!",MB_OK|MB_ICONINFORMATION);
		return;
	}
	if(m_nTestType==EXP_TYPE) tmp = g_pCalc->_CallDTopViewCalc_8(EXP_TYPE, nPos);
	if(m_nTestType==TJ_TYPE) tmp = g_pCalc->_CallDTopViewCalc_8(TJ_TYPE, nPos);
	if(!tmp) return;
	memcpy(&IndexBakup, tmp, sizeof(TINDEXINFO));
	if (tmp == NULL) return;
	if (!m_nOptimizeParam&&tmp->nParaNum < 1)
	{
		TDX_MessageBox(m_hWnd,"该专家系统没有参数，不能探索最佳参数!",MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	if(m_nAddedGPNum < 1)
	{
		TDX_MessageBox(m_hWnd,"请加入测试股票！",MB_OK|MB_ICONINFORMATION);
		return;
	}
	nAim = m_AimCombo.GetCurSel();
	m_nCalcPeriod = m_PeriodCombo.GetCurSel();
	if(m_nCalcPeriod==MIN1_ST||m_nCalcPeriod==MINN_ST)
	{
		TDX_MessageBox(m_hWnd,"暂不支持此周期,请选择其它周期!",MB_OK|MB_ICONINFORMATION);
		return;
	}
	if(!CanProcesLocalData(this, m_nCalcPeriod<DAY_ST))
		return;

	FillGPReport();
	//以下两句代码计算一只股票总共需要计算的次数
	nOneNeedNum = 1 + (tmp->aPara[0].nMax -tmp->aPara[0].nMin)/tmp->aPara[0].nStep;
	for ( i = 1; i < tmp->nParaNum; i++ )
		nOneNeedNum *= 1 + (tmp->aPara[i].nMax - tmp->aPara[i].nMin)/tmp->aPara[i].nStep;
	if(nOneNeedNum>100000000)
	{
		TDX_MessageBox(m_hWnd,"参数值相差太远,迭代次数太多,不能进行测试!",MB_OK|MB_ICONINFORMATION);
		return;
	}
	if (m_pCache != NULL)
		delete m_pCache;
	m_pCache = new float[nOneNeedNum];
	memset(m_pCache, 0, nOneNeedNum*sizeof(float));
	TotalCalcNum = nOneNeedNum * m_nAddedGPNum; //支持多个股票
	if(m_nOptimizeParam)
	{
		m_Progress.SetRange32(0,m_nAddedGPNum);
		m_Progress.SetStep(1);
	}
	else
	{
		m_Progress.SetRange32(0,TotalCalcNum);
		m_Progress.SetStep(50);
	}
	m_Progress.SetPos(0);
	m_OutInfo.SetTextColor(CLR_NOWTESTING);
	m_OutInfo.SetWindowText(_F("正在进行测试..."));
	m_IsCarried = FALSE;
	m_IsCarrying = TRUE;
	DisableOther();
	//在Disable其它之后，需设一控件为焦点,否则ESC不起作用
	GetDlgItem(IDCANCEL)->SetFocus();
	RefreshAllHq();
	g_bDonotClose = TRUE;
	
	if(m_nOptimizeParam)
	{
		tmp->aPara[0].nValue[m_nCalcPeriod] = atof(m_sParam1);
		tmp->aPara[1].nValue[m_nCalcPeriod] = atof(m_sParam2);
		tmp->aPara[2].nValue[m_nCalcPeriod] = atof(m_sParam3);
		tmp->aPara[3].nValue[m_nCalcPeriod] = atof(m_sParam4);
		tmp->aPara[4].nValue[m_nCalcPeriod] = atof(m_sParam5);
		tmp->aPara[5].nValue[m_nCalcPeriod] = atof(m_sParam6);
	}
	memset(&time1, 0, sizeof(NTime));
	memset(&time2, 0, sizeof(NTime));
	time1.year = m_BeginTime.GetYear();
	time1.month = m_BeginTime.GetMonth();
	time1.day = m_BeginTime.GetDay();
	time2.year = m_EndTime.GetYear();
	time2.month = m_EndTime.GetMonth();
	time2.day = m_EndTime.GetDay();
	//此处测试
	if(!m_nOptimizeParam)
	{	
		for (lTestCount = 0; lTestCount < m_nAddedGPNum; lTestCount++)
		{
			m_curBonus = -9999999.9f;
			m_fMarkup = -9999999.9f;
			m_fTotalBonus = -9999999.9f;
			m_fAverageBonus = -9999999.9f;
			m_fDivHeadBonus = -9999999.9f;
			m_fAverageYL = -9999999.9f;
			m_fAverageKS = -9999999.9f;
			m_fMaxBonus = -9999999.9f;
			m_fMaxKS = -9999999.9f;
			m_YKCoeff = -9999999.9f;
			m_toTimes = 0;
			m_nYLTimes = 0;
			m_nKSTimes = 0;
			m_pAnalyData=NULL;
			lReadNum = g_pCalc->_CallDTopViewCalc_4((*g_pStockIO)[m_nAddedGPIndex[lTestCount]]->Code,(*g_pStockIO)[m_nAddedGPIndex[lTestCount]]->setcode, CalcPeriodToTdxPeriod(m_nCalcPeriod), time1, time2,1);
			m_pAnalyData = (LPHISDAT)g_pCalc->_CallDTopViewCalc_15();
			if(lReadNum <= 0 || m_pAnalyData == NULL) continue;
			int cacheIndex = 0;
			switch (tmp->nParaNum)
			{
			case 1:
				for (tmp->aPara[0].nValue[m_nCalcPeriod] = tmp->aPara[0].nMin;
				tmp->aPara[0].nValue[m_nCalcPeriod] <= tmp->aPara[0].nMax;
				tmp->aPara[0].nValue[m_nCalcPeriod] += tmp->aPara[0].nStep)
					CALCBONUS(lReadNum, nAim, cacheIndex++)
					break;
			case 2:
				for (tmp->aPara[0].nValue[m_nCalcPeriod] = tmp->aPara[0].nMin;
				tmp->aPara[0].nValue[m_nCalcPeriod] <= tmp->aPara[0].nMax;
				tmp->aPara[0].nValue[m_nCalcPeriod] += tmp->aPara[0].nStep )
					for ( tmp->aPara[1].nValue[m_nCalcPeriod] = tmp->aPara[1].nMin;
					tmp->aPara[1].nValue[m_nCalcPeriod] <= tmp->aPara[1].nMax;
					tmp->aPara[1].nValue[m_nCalcPeriod] += tmp->aPara[1].nStep )
						CALCBONUS(lReadNum, nAim, cacheIndex++)
						break;
			case 3:
				for (tmp->aPara[0].nValue[m_nCalcPeriod] = tmp->aPara[0].nMin;
				tmp->aPara[0].nValue[m_nCalcPeriod] <= tmp->aPara[0].nMax;
				tmp->aPara[0].nValue[m_nCalcPeriod] += tmp->aPara[0].nStep )
					for ( tmp->aPara[1].nValue[m_nCalcPeriod] = tmp->aPara[1].nMin;
					tmp->aPara[1].nValue[m_nCalcPeriod] <= tmp->aPara[1].nMax;
					tmp->aPara[1].nValue[m_nCalcPeriod] += tmp->aPara[1].nStep )
						for ( tmp->aPara[2].nValue[m_nCalcPeriod] = tmp->aPara[2].nMin;
						tmp->aPara[2].nValue[m_nCalcPeriod] <= tmp->aPara[2].nMax;
						tmp->aPara[2].nValue[m_nCalcPeriod] += tmp->aPara[2].nStep )
							CALCBONUS(lReadNum, nAim, cacheIndex++)
							break;
			case 4:
				for (tmp->aPara[0].nValue[m_nCalcPeriod] = tmp->aPara[0].nMin;
				tmp->aPara[0].nValue[m_nCalcPeriod] <= tmp->aPara[0].nMax;
				tmp->aPara[0].nValue[m_nCalcPeriod] += tmp->aPara[0].nStep )
					for ( tmp->aPara[1].nValue[m_nCalcPeriod] = tmp->aPara[1].nMin;
					tmp->aPara[1].nValue[m_nCalcPeriod] <= tmp->aPara[1].nMax;
					tmp->aPara[1].nValue[m_nCalcPeriod] += tmp->aPara[1].nStep )
						for ( tmp->aPara[2].nValue[m_nCalcPeriod] = tmp->aPara[2].nMin;
						tmp->aPara[2].nValue[m_nCalcPeriod] <= tmp->aPara[2].nMax;
						tmp->aPara[2].nValue[m_nCalcPeriod] += tmp->aPara[2].nStep )
							for ( tmp->aPara[3].nValue[m_nCalcPeriod] = tmp->aPara[3].nMin;
							tmp->aPara[3].nValue[m_nCalcPeriod] <= tmp->aPara[3].nMax;
							tmp->aPara[3].nValue[m_nCalcPeriod] += tmp->aPara[3].nStep )
								CALCBONUS(lReadNum, nAim, cacheIndex++)
								break;
			case 5:
				for (tmp->aPara[0].nValue[m_nCalcPeriod] = tmp->aPara[0].nMin;
				tmp->aPara[0].nValue[m_nCalcPeriod] <= tmp->aPara[0].nMax;
				tmp->aPara[0].nValue[m_nCalcPeriod] += tmp->aPara[0].nStep )
					for ( tmp->aPara[1].nValue[m_nCalcPeriod] = tmp->aPara[1].nMin;
					tmp->aPara[1].nValue[m_nCalcPeriod] <= tmp->aPara[1].nMax;
					tmp->aPara[1].nValue[m_nCalcPeriod] += tmp->aPara[1].nStep )
						for ( tmp->aPara[2].nValue[m_nCalcPeriod] = tmp->aPara[2].nMin;
						tmp->aPara[2].nValue[m_nCalcPeriod] <= tmp->aPara[2].nMax;
						tmp->aPara[2].nValue[m_nCalcPeriod] += tmp->aPara[2].nStep )
							for ( tmp->aPara[3].nValue[m_nCalcPeriod] = tmp->aPara[3].nMin;
							tmp->aPara[3].nValue[m_nCalcPeriod] <= tmp->aPara[3].nMax;
							tmp->aPara[3].nValue[m_nCalcPeriod] += tmp->aPara[3].nStep )
								for ( tmp->aPara[4].nValue[m_nCalcPeriod] = tmp->aPara[4].nMin;
								tmp->aPara[4].nValue[m_nCalcPeriod] <= tmp->aPara[4].nMax;
								tmp->aPara[4].nValue[m_nCalcPeriod] += tmp->aPara[4].nStep )
									CALCBONUS(lReadNum, nAim, cacheIndex++)
									break;
			case 6:
				for (tmp->aPara[0].nValue[m_nCalcPeriod] = tmp->aPara[0].nMin;
				tmp->aPara[0].nValue[m_nCalcPeriod] <= tmp->aPara[0].nMax;
				tmp->aPara[0].nValue[m_nCalcPeriod] += tmp->aPara[0].nStep )
					for ( tmp->aPara[1].nValue[m_nCalcPeriod] = tmp->aPara[1].nMin;
					tmp->aPara[1].nValue[m_nCalcPeriod] <= tmp->aPara[1].nMax;
					tmp->aPara[1].nValue[m_nCalcPeriod] += tmp->aPara[1].nStep )
						for ( tmp->aPara[2].nValue[m_nCalcPeriod] = tmp->aPara[2].nMin;
						tmp->aPara[2].nValue[m_nCalcPeriod] <= tmp->aPara[2].nMax;
						tmp->aPara[2].nValue[m_nCalcPeriod] += tmp->aPara[2].nStep )
							for ( tmp->aPara[3].nValue[m_nCalcPeriod] = tmp->aPara[3].nMin;
							tmp->aPara[3].nValue[m_nCalcPeriod] <= tmp->aPara[3].nMax;
							tmp->aPara[3].nValue[m_nCalcPeriod] += tmp->aPara[3].nStep )
								for ( tmp->aPara[4].nValue[m_nCalcPeriod] = tmp->aPara[4].nMin;
								tmp->aPara[4].nValue[m_nCalcPeriod] <= tmp->aPara[4].nMax;
								tmp->aPara[4].nValue[m_nCalcPeriod] += tmp->aPara[4].nStep )
									for ( tmp->aPara[5].nValue[m_nCalcPeriod] = tmp->aPara[5].nMin;
									tmp->aPara[5].nValue[m_nCalcPeriod] <= tmp->aPara[5].nMax;
									tmp->aPara[5].nValue[m_nCalcPeriod] += tmp->aPara[5].nStep )
										CALCBONUS(lReadNum, nAim, cacheIndex++)
										break;
			}
			if(m_toTimes > 0)
			{
				if(m_nTestType==EXP_TYPE)
				{
					tmpStr.Format("%3.1f",m_fTotalBonus * 100);
					m_List.SetItemText(lTestCount,1,tmpStr);
					tmpStr.Format("%d", m_nYLTimes);
					m_List.SetItemText(lTestCount,2,tmpStr);
					tmpStr.Format("%d", m_nKSTimes);
					m_List.SetItemText(lTestCount,3,tmpStr);
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
					m_List.SetItemText(lTestCount,4,tmpStr);
				}
				if(m_nTestType==TJ_TYPE)
				{
					tmpStr.Format("%3.1f",m_fMaxBonus * 100);
					m_List.SetItemText(lTestCount,1,tmpStr);
					tmpStr.Format("%d", m_toTimes);
					m_List.SetItemText(lTestCount,2,tmpStr);
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
					m_List.SetItemText(lTestCount,3,tmpStr);
				}
			}
			else
				m_List.SetItemText(lTestCount,1,MEANLESS_STR);

	}
	m_Progress.SetPos(TotalCalcNum);
	tmpStr.Format("%d", TotalCalcNum);
CancelLabel:
	int nParam = tmp->nParaNum;
	float *aParam = new float[nParam];
	memset(aParam, 0, nParam*sizeof(float));
	FindOptParam(nParam, aParam);
	CString sParams("");
	for(int k=0;k<nParam;k++) sParams += IntToStr((int)aParam[k]) + (k==nParam-1?"":" ,");
	if(m_IsCarrying) 
	{
		m_OutInfo.SetTextColor(CLR_TESTED);
		CString sOutInfo;
		sOutInfo.Format("测试完毕:最优参数为 %s", (LPCSTR)sParams);
		m_OutInfo.SetWindowText(sOutInfo);
		m_IsCarrying = FALSE;
		UpdateData(FALSE);
	}
	}
	else
	{
		int nAver, index=0;
		float aTotalAver[16];
		memset(aTotalAver, 0, 16*sizeof(float));
		for (lTestCount = 0; lTestCount < m_nAddedGPNum; lTestCount++)
		{
			nAver=0;
			for(int i=0;i<8;i++) m_fTJBonus[i]=-9999999.9f;
			m_curBonus = -9999999.9f;
			m_fMarkup = -9999999.9f;
			m_fTotalBonus = -9999999.9f;
			m_fAverageBonus = -9999999.9f;
			m_fDivHeadBonus = -9999999.9f;
			m_fAverageYL = -9999999.9f;
			m_fAverageKS = -9999999.9f;
			m_fMaxBonus = -9999999.9f;
			m_fMaxKS = -9999999.9f;
			m_YKCoeff = -9999999.9f;
			m_toTimes = 0;
			m_nYLTimes = 0;
			m_nKSTimes = 0;
			m_pAnalyData = NULL;
			index=HIWORD(m_List.GetItemData(lTestCount));
			if(index<0) continue;
			strcpy(m_crCurrGPCode, (*g_pStockIO)[index]->Code);
			m_crCurrSetCode = (*g_pStockIO)[index]->setcode;
			lReadNum = g_pCalc->_CallDTopViewCalc_4(m_crCurrGPCode,m_crCurrSetCode,CalcPeriodToTdxPeriod(m_nCalcPeriod), time1, time2,1);
			m_pAnalyData = (LPHISDAT)g_pCalc->_CallDTopViewCalc_15();
			if (m_pAnalyData == NULL) continue;
			if(lReadNum <= 0) continue;
			
			CalcBonus(NULL,lReadNum);
			
			i=1;
			m_Progress.StepIt();
			if(m_toTimes>0)
			{
				if(m_showhat[1])
				{
					aTotalAver[nAver++]+=m_fMarkup * 100;
					tmpStr.Format("%5.1f",m_fMarkup * 100);
					m_List.SetItemText(lTestCount,i++,tmpStr);
				}
				if(m_showhat[2])
				{
					if(m_nTestType==EXP_TYPE)
					{
						aTotalAver[nAver++]+=m_fTotalBonus * 100;
						tmpStr.Format("%5.1f",m_fTotalBonus * 100);
					}
					if(m_nTestType==TJ_TYPE)
					{
						aTotalAver[nAver++]+=m_toTimes;
						tmpStr.Format("  %d", m_toTimes);
					}
					m_List.SetItemText(lTestCount,i++,tmpStr);
				}
				if(m_showhat[3])
				{
					if(m_nTestType==EXP_TYPE)
					{
						aTotalAver[nAver++]+=m_nYLTimes;
						tmpStr.Format("  %d", m_nYLTimes);
					}
					if(m_nTestType==TJ_TYPE)
					{
						aTotalAver[nAver++]+=m_fTJBonus[7] * 100;
						tmpStr.Format("%5.1f",m_fTJBonus[7] * 100);
					}
					m_List.SetItemText(lTestCount,i++,tmpStr);
				}
				if(m_showhat[4])
				{
					aTotalAver[nAver++]+=m_nKSTimes;
					tmpStr.Format("  %d", m_nKSTimes);
					m_List.SetItemText(lTestCount,i++,tmpStr);
				}
				if(m_showhat[5])
				{
					if(m_nTestType==EXP_TYPE)
					{
						aTotalAver[nAver++]+=m_fAverageBonus * 100;
						tmpStr.Format("%5.1f", m_fAverageBonus * 100);
					}
					if(m_nTestType==TJ_TYPE)
					{
						aTotalAver[nAver++]+=m_fTJBonus[0] * 100;
						tmpStr.Format("%5.1f",m_fTJBonus[0] * 100);
					}
					m_List.SetItemText(lTestCount,i++,tmpStr);
				}
				if(m_showhat[6])
				{
					if(m_nTestType==EXP_TYPE)
					{
						aTotalAver[nAver++]+=m_fDivHeadBonus * 100;
						tmpStr.Format("%5.1f", m_fDivHeadBonus * 100);
					}
					if(m_nTestType==TJ_TYPE)
					{
						aTotalAver[nAver++]+=m_fTJBonus[1] * 100;
						tmpStr.Format("%5.1f",m_fTJBonus[1] * 100);
					}
					m_List.SetItemText(lTestCount,i++,tmpStr);
				}
				if(m_showhat[7])
				{
					if(m_nTestType==EXP_TYPE)
					{
						aTotalAver[nAver++]+=m_fAverageYL * 100;
						tmpStr.Format("%5.1f", m_fAverageYL * 100);
					}
					if(m_nTestType==TJ_TYPE)
					{
						aTotalAver[nAver++]+=m_fTJBonus[2] * 100;
						tmpStr.Format("%5.1f",m_fTJBonus[2] * 100);
					}
					m_List.SetItemText(lTestCount,i++,tmpStr);
				}
				if(m_showhat[8])
				{
					if(m_nTestType==EXP_TYPE)
					{
						aTotalAver[nAver++]+=m_fAverageKS * 100;
						tmpStr.Format("%5.1f", m_fAverageKS * 100);
					}
					if(m_nTestType==TJ_TYPE)
					{
						aTotalAver[nAver++]+=m_fTJBonus[3] * 100;
						tmpStr.Format("%5.1f",m_fTJBonus[3] * 100);
					}
					m_List.SetItemText(lTestCount,i++,tmpStr);
				}
				if(m_showhat[9])
				{
					if(m_nTestType==EXP_TYPE)
					{
						aTotalAver[nAver++]+=m_fMaxBonus * 100;
						tmpStr.Format("%5.1f", m_fMaxBonus * 100);
					}
					if(m_nTestType==TJ_TYPE)
					{
						aTotalAver[nAver++]+=m_fTJBonus[4] * 100;
						tmpStr.Format("%5.1f",m_fTJBonus[4] * 100);
					}
					m_List.SetItemText(lTestCount,i++,tmpStr);
				}
				if(m_showhat[10])
				{
					if(m_nTestType==EXP_TYPE)
					{
						aTotalAver[nAver++]+=m_fMaxKS * 100;
						tmpStr.Format("%5.1f", m_fMaxKS * 100);
					}
					if(m_nTestType==TJ_TYPE)
					{
						aTotalAver[nAver++]+=m_fTJBonus[5] * 100;
						tmpStr.Format("%5.1f",m_fTJBonus[5] * 100);
					}
					m_List.SetItemText(lTestCount,i++,tmpStr);
				}
				if(m_showhat[11])
				{
					if(m_nTestType==EXP_TYPE)
					{
						aTotalAver[nAver++]+=m_toTimes;
						tmpStr.Format("  %d", m_toTimes);
					}
					if(m_nTestType==TJ_TYPE)
					{
						aTotalAver[nAver++]+=m_fTJBonus[6] * 100;
						tmpStr.Format("%5.1f",m_fTJBonus[6] * 100);
					}
					m_List.SetItemText(lTestCount,i++,tmpStr);
				}
				if(m_showhat[12])
				{
					aTotalAver[nAver++]+=m_YKCoeff * 100;
					tmpStr.Format("%5.1f", m_YKCoeff * 100);
					m_List.SetItemText(lTestCount,i++,tmpStr);
				}
			}
			else
				m_List.SetItemText(lTestCount,1,MEANLESS_STR);
			
			if (PeekMyMsg()) break;
		}
		for(int j=0;j<16;j++)
		{
			aTotalAver[j]/=m_nAddedGPNum;
		}
		if(m_nAddedGPNum>1)
		{
			LVITEM lvizh;
			lvizh.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
			lvizh.state = 0;
			lvizh.stateMask = 0;
			lvizh.iSubItem = 0;
			lvizh.iItem = m_nAddedGPNum;
			lvizh.iImage = 0;
			lvizh.pszText = "综合平均";
			m_List.InsertItem(&lvizh);
			for(j=0;j<nAver;j++)
			{
				char str[16];
				sprintf(str, "%.2f", aTotalAver[j]);
				m_List.SetItemText(m_nAddedGPNum, j+1, str);
			}
		}
		if(m_IsCarrying)
		{
			m_OutInfo.SetWindowText(_F("测试完毕"));
			m_IsCarrying = FALSE;
			UpdateData(FALSE);
		}
	}
	//此时编译信息已经产生,不能由原始的IndexBakup来恢复
	memcpy(&tmp->aPara, &IndexBakup.aPara, sizeof(TPARAINFO) * 6);
	EnableOther();
	g_bDonotClose = FALSE;
	m_IsCarried = TRUE;
}

void CParamTestDlg::OnSelchangeCombo1() 
{
	TINDEXINFO	*tmp;
	int nPos;

	nPos = m_NameCombo.GetCurSel();
	if(m_nTestType==EXP_TYPE) tmp = g_pCalc->_CallDTopViewCalc_8(EXP_TYPE, nPos);
	if(m_nTestType==TJ_TYPE) tmp = g_pCalc->_CallDTopViewCalc_8(TJ_TYPE, nPos);
	if(!tmp) return;
	SetParam();
	if (tmp->lpszHelp == NULL)
		GetDlgItem(IDC_DESC)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_DESC)->EnableWindow(TRUE);
}

BOOL CParamTestDlg::PeekMyMsg()
{
	Sleep(1);
	PeekMsg();
	if( !m_IsCarrying)
	{
		m_Progress.SetPos(0);
		m_OutInfo.SetTextColor(CLR_INTERRUPT);
		SetDlgItemText(IDC_STATICINFO, _F("测试中断!"));
		return TRUE;
	}
	return FALSE;
}

void CParamTestDlg::EnableOther()
{
	m_NameCombo.EnableWindow(TRUE);
	m_PeriodCombo.EnableWindow(TRUE);
	if(!m_nOptimizeParam)
	{
		GetDlgItem(IDC_TESTPARAM)->EnableWindow(TRUE);
		m_AimCombo.EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_OPARAM)->EnableWindow(TRUE);
	}
	GetDlgItem(IDC_ADDGP)->EnableWindow(TRUE);
	GetDlgItem(IDC_DELGP)->EnableWindow(TRUE);
	GetDlgItem(IDC_DELGPALL)->EnableWindow(TRUE);
	GetDlgItem(IDC_SHOWMORE)->EnableWindow(TRUE);
	int selIndex=m_List.GetNextItem(-1,LVNI_SELECTED);
	if(selIndex!=-1)
		GetDlgItem(IDC_SHOWGGDETAIL)->EnableWindow(TRUE);
	GetDlgItem(IDC_SHOWDETAIL)->EnableWindow(TRUE);
	GetDlgItem(IDOK)->EnableWindow(TRUE);
	SetDlgItemText(IDCANCEL, _F("关闭"));
}

void CParamTestDlg::DisableOther()
{
	m_NameCombo.EnableWindow(FALSE);
	m_PeriodCombo.EnableWindow(FALSE);
	m_AimCombo.EnableWindow(FALSE);
	if(!m_nOptimizeParam)
	{
		GetDlgItem(IDC_TESTPARAM)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_OPARAM)->EnableWindow(FALSE);
	}
	GetDlgItem(IDC_ADDGP)->EnableWindow(FALSE);
	GetDlgItem(IDC_DELGP)->EnableWindow(FALSE);
	GetDlgItem(IDC_DELGPALL)->EnableWindow(FALSE);
	GetDlgItem(IDC_SHOWMORE)->EnableWindow(FALSE);
	GetDlgItem(IDC_SHOWGGDETAIL)->EnableWindow(FALSE);
	GetDlgItem(IDC_SHOWDETAIL)->EnableWindow(FALSE);
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	SetDlgItemText(IDCANCEL, _F("中止测试"));
}

//同期涨幅
void CParamTestDlg::CalcFormula(float **OutData, long lReadNum)
{
	float   BuyPrice, SellPrice;	//BuyFlag表示已经买入股票的标志,BuyPrice表示买入股票的价格
	LPHISDAT pTmpHisData = m_pAnalyData;

	BuyPrice = pTmpHisData->Close;
	pTmpHisData += lReadNum-1;
	SellPrice = pTmpHisData->Close; 
	m_fMarkup = (SellPrice - BuyPrice) / BuyPrice;
}

//总收益最大化
void CParamTestDlg::CalcFormula0(float **OutData, long lReadNum, TINDEXINFO *pIndex)
{
	int		i;
	float	fBonus=0.0f, tmpfBonus=0.0f;	//收益,用百分数表示
	BOOL	bAllInvalid = TRUE;	//判断此次计算是否全为无效数据
	float   BuyPrice, SellPrice;	//BuyFlag表示已经买入股票的标志,BuyPrice表示买入股票的价格
	double	dBuyMoney = 0.0, dSellMoney = 0.0 ,dCurBuyCount=0.0;	//买入使用总资金,卖出使用总资金
	double	dBuyCount = 0.0, dSellCount = 0.0;	//买入量,卖出量
	LPHISDAT pTmpHisData = m_pAnalyData;
	int		nYLTimes = 0, nKSTimes = 0;
	float	fFeeRate;	//手续费率
	
	if (m_nDontNeedFee)
		fFeeRate = 0.0;
	else
		fFeeRate = getsxf(m_crCurrGPCode,m_crCurrSetCode);
	for (i = 0; i < lReadNum; i++)
	{
		if (fabs(OutData[ENTERLONG][i] - 1) < COMPPREC || fabs(OutData[ENTERSHORT][i] - 1) < COMPPREC)	//表示当前买进
		{
			if(pTmpHisData->Close>COMPPREC)
			{
				bAllInvalid = FALSE;
				BuyPrice=pTmpHisData->Close;
				dCurBuyCount=100000.0/BuyPrice;
				dBuyCount+=dCurBuyCount;
				dBuyMoney+=100000.0*(1+fFeeRate);
			}
			pTmpHisData++;
		}
		else
			if (fabs(OutData[EXITLONG][i] - 1) < COMPPREC || fabs(OutData[EXITSHORT][i] - 1) < COMPPREC)	//表示当前卖出
			{
				if(dBuyMoney>1.0&&pTmpHisData->Close>COMPPREC)
				{
					bAllInvalid = FALSE;
					SellPrice=pTmpHisData->Close;
					dSellCount=dBuyCount;
					dSellMoney=dSellCount*SellPrice*(1-fFeeRate);
					tmpfBonus=(dSellMoney-dBuyMoney)/dBuyMoney;
					if (tmpfBonus >= COMPPREC)
						nYLTimes++;
					else
						nKSTimes++;
					fBonus += tmpfBonus;
					dBuyCount = 0.0;
					dSellCount = 0.0;
					dBuyMoney = 0.0;
					dSellMoney = 0.0;
				}
				pTmpHisData++;
			}
			else
			{
				pTmpHisData++;
			}
	}
	if (!bAllInvalid)
	{
		//计算最后一次的收益
		pTmpHisData--;
		if (dBuyMoney>1.0&&pTmpHisData->Close>COMPPREC)	//无卖出点或无买入点
		{
			SellPrice = pTmpHisData->Close;
			dSellCount=dBuyCount;
			dSellMoney=dSellCount*SellPrice*(1-fFeeRate);
			tmpfBonus=(dSellMoney-dBuyMoney)/dBuyMoney;
			if (tmpfBonus >= COMPPREC) 
				nYLTimes++;
			else
				nKSTimes++;
			fBonus += tmpfBonus;
		}
		m_curBonus = fBonus;
		if(m_nOptimizeParam)
		{
			m_nYLTimes = nYLTimes;
			m_nKSTimes = nKSTimes;
			m_toTimes = m_nYLTimes + m_nKSTimes;
			m_fTotalBonus = fBonus;
		}
		else
		{
			if ( fBonus > m_fTotalBonus )		//保存当前最大收益及当时的换手次数和参数值,盈利交易和亏损交易次数
			{
				m_nYLTimes = nYLTimes;
				m_nKSTimes = nKSTimes;
				m_toTimes = m_nYLTimes + m_nKSTimes;
				m_fTotalBonus = fBonus;
				for ( i = 0; i < pIndex->nParaNum; i++ )
					m_fParaValue[i] = pIndex->aPara[i].nValue[m_nCalcPeriod];
			}
		}
	}
}

//平均收益最大化
void CParamTestDlg::CalcFormula1(float **OutData, long lReadNum, TINDEXINFO *pIndex)
{
	int		i;
	float	fBonus=0.0f, tmpfBonus=0.0f, ftBonus=0.0f;	//收益,用百分数表示
	BOOL	bAllInvalid = TRUE;	//判断此次计算是否全为无效数据
	float   BuyPrice, SellPrice;	//BuyFlag表示已经买入股票的标志,BuyPrice表示买入股票的价格
	double	dBuyMoney = 0.0, dSellMoney = 0.0 ,dCurBuyCount=0.0;	//买入使用总资金,卖出使用总资金
	double	dBuyCount = 0.0, dSellCount = 0.0;	//买入量,卖出量
	LPHISDAT pTmpHisData = m_pAnalyData;
	int		nYLTimes = 0, nKSTimes = 0;
	float	fFeeRate;	//手续费率
	
	if (m_nDontNeedFee)
		fFeeRate = 0.0;
	else
		fFeeRate = getsxf(m_crCurrGPCode,m_crCurrSetCode);	
	for (i = 0; i < lReadNum; i++)
	{
		if (fabs(OutData[ENTERLONG][i] - 1) < COMPPREC || fabs(OutData[ENTERSHORT][i] - 1) < COMPPREC)	//表示当前买进
		{
			if(pTmpHisData->Close>COMPPREC)
			{
				bAllInvalid = FALSE;
				BuyPrice=pTmpHisData->Close;
				dCurBuyCount=100000.0/BuyPrice;
				dBuyCount+=dCurBuyCount;
				dBuyMoney+=100000.0*(1+fFeeRate);
			}
			pTmpHisData++;
		}
		else
			if (fabs(OutData[EXITLONG][i] - 1) < COMPPREC || fabs(OutData[EXITSHORT][i] - 1) < COMPPREC)	//表示当前卖出
			{
				if(dBuyMoney>1.0&&pTmpHisData->Close>COMPPREC)
				{
					bAllInvalid = FALSE;
					SellPrice=pTmpHisData->Close;
					dSellCount=dBuyCount;
					dSellMoney=dSellCount*SellPrice*(1-fFeeRate);
					tmpfBonus=(dSellMoney-dBuyMoney)/dBuyMoney;
					ftBonus+=tmpfBonus;
					if (tmpfBonus >= COMPPREC) 
						nYLTimes++;
					else
						nKSTimes++;
					fBonus += tmpfBonus;
					dBuyCount = 0.0;
					dSellCount = 0.0;
					dBuyMoney = 0.0;
					dSellMoney = 0.0;
				}
				pTmpHisData++;
			}
			else
			{
				pTmpHisData++;
			}
	}
	if (!bAllInvalid)
	{
		//计算最后一次的收益
		pTmpHisData--;
		if (dBuyMoney>1.0&&pTmpHisData->Close>COMPPREC)	//无卖出点或无买入点
		{
			SellPrice = pTmpHisData->Close;
			dSellCount=dBuyCount;
			dSellMoney=dSellCount*SellPrice*(1-fFeeRate);
			tmpfBonus=(dSellMoney-dBuyMoney)/dBuyMoney;
			ftBonus+=tmpfBonus;
			if (tmpfBonus >= COMPPREC) 
				nYLTimes++;
			else
				nKSTimes++;
			fBonus += tmpfBonus;
		}
		if (nYLTimes+nKSTimes != 0)
			fBonus /= (nYLTimes+nKSTimes);
		m_curBonus = fBonus;
		if(m_nOptimizeParam)
		{
			m_fAverageBonus = fBonus;
		}
		else
		{
			if ( fBonus > m_fAverageBonus )		//保存当前最大收益及当时的换手次数和参数值,盈利交易和亏损交易次数
			{
				m_nYLTimes = nYLTimes;
				m_nKSTimes = nKSTimes;
				m_toTimes = m_nYLTimes + m_nKSTimes;
				m_fTotalBonus = ftBonus;
				m_fAverageBonus = fBonus;
				for ( i = 0; i < pIndex->nParaNum; i++ )
					m_fParaValue[i] = pIndex->aPara[i].nValue[m_nCalcPeriod];
			}
		}
	}
}

//扣除最大盈利后收益最大化
void CParamTestDlg::CalcFormula2(float **OutData, long lReadNum, TINDEXINFO *pIndex)
{
	int		i;
	float	fBonus=0.0f, tmpfBonus=0.0f, ftmpMaxBonus = 0.0f, ftBonus=0.0f;	//收益,用百分数表示, fMaxYL -- 最大盈利
	BOOL	bAllInvalid = TRUE;	//判断此次计算是否全为无效数据
	float   BuyPrice, SellPrice;	//BuyFlag表示已经买入股票的标志,BuyPrice表示买入股票的价格
	double	dBuyMoney = 0.0, dSellMoney = 0.0 ,dCurBuyCount=0.0;	//买入使用总资金,卖出使用总资金
	double	dBuyCount = 0.0, dSellCount = 0.0;	//买入量,卖出量
	LPHISDAT pTmpHisData = m_pAnalyData;
	int		nYLTimes = 0, nKSTimes = 0;
	float	fFeeRate;	//手续费率
	
	if (m_nDontNeedFee)
		fFeeRate = 0.0;
	else
		fFeeRate = getsxf(m_crCurrGPCode,m_crCurrSetCode);
	for (i = 0; i < lReadNum; i++)
	{
		if (fabs(OutData[ENTERLONG][i] - 1) < COMPPREC || fabs(OutData[ENTERSHORT][i] - 1) < COMPPREC)	//表示当前买进
		{
			if(pTmpHisData->Close>COMPPREC)
			{
				bAllInvalid = FALSE;
				BuyPrice=pTmpHisData->Close;
				dCurBuyCount=100000.0/BuyPrice;
				dBuyCount+=dCurBuyCount;
				dBuyMoney+=100000.0*(1+fFeeRate);
			}
			pTmpHisData++;
		}
		else
			if (fabs(OutData[EXITLONG][i] - 1) < COMPPREC || fabs(OutData[EXITSHORT][i] - 1) < COMPPREC)	//表示当前卖出
			{
				if(dBuyMoney>1.0&&pTmpHisData->Close>COMPPREC)
				{
					bAllInvalid = FALSE;
					SellPrice=pTmpHisData->Close;
					dSellCount=dBuyCount;
					dSellMoney=dSellCount*SellPrice*(1-fFeeRate);
					tmpfBonus=(dSellMoney-dBuyMoney)/dBuyMoney;
					ftBonus+=tmpfBonus;
					if (tmpfBonus >= COMPPREC) 
					{
						nYLTimes++;
						if (tmpfBonus > ftmpMaxBonus)
						{
							ftmpMaxBonus = tmpfBonus;
						}
					}
					else
						nKSTimes++;
					fBonus += tmpfBonus;
					dBuyCount = 0.0;
					dSellCount = 0.0;
					dBuyMoney = 0.0;
					dSellMoney = 0.0;
				}
				pTmpHisData++;
			}
			else
			{
				pTmpHisData++;
			}
	}
	if (!bAllInvalid)
	{
		//计算最后一次的收益
		pTmpHisData--;
		if (dBuyMoney>1.0&&pTmpHisData->Close>COMPPREC)	//无卖出点或无买入点
		{
			SellPrice = pTmpHisData->Close;
			dSellCount=dBuyCount;
			dSellMoney=dSellCount*SellPrice*(1-fFeeRate);
			tmpfBonus=(dSellMoney-dBuyMoney)/dBuyMoney;
			ftBonus+=tmpfBonus;
			if (tmpfBonus >= COMPPREC) 
			{
				nYLTimes++;
				if (tmpfBonus > ftmpMaxBonus)
				{
					ftmpMaxBonus = tmpfBonus;
				}
			}
			else
				nKSTimes++;
			fBonus += tmpfBonus;
		}
		fBonus -= ftmpMaxBonus;
		m_curBonus = fBonus;
		if(m_nOptimizeParam)
		{
			m_fDivHeadBonus = fBonus;
		}
		else
		{
			if ( fBonus > m_fDivHeadBonus )		//保存当前最大收益及当时的换手次数和参数值,盈利交易和亏损交易次数
			{
				m_nYLTimes = nYLTimes;
				m_nKSTimes = nKSTimes;
				m_toTimes = m_nYLTimes + m_nKSTimes;
				m_fTotalBonus = ftBonus;
				m_fDivHeadBonus = fBonus;
				for ( i = 0; i < pIndex->nParaNum; i++ )
					m_fParaValue[i] = pIndex->aPara[i].nValue[m_nCalcPeriod];
			}
		}
	}
}

//平均盈利最大化
void CParamTestDlg::CalcFormula3(float **OutData, long lReadNum, TINDEXINFO *pIndex)
{
	int		i;
	float	fBonus=0.0f, tmpfBonus=0.0f, ftBonus=0.0f;	//收益,用百分数表示
	BOOL	bAllInvalid = TRUE;	//判断此次计算是否全为无效数据
	float   BuyPrice, SellPrice;	//BuyFlag表示已经买入股票的标志,BuyPrice表示买入股票的价格
	double	dBuyMoney = 0.0, dSellMoney = 0.0 ,dCurBuyCount=0.0;	//买入使用总资金,卖出使用总资金
	double	dBuyCount = 0.0, dSellCount = 0.0;	//买入量,卖出量
	LPHISDAT pTmpHisData = m_pAnalyData;
	int		nYLTimes = 0, nKSTimes = 0;
	float	fFeeRate;	//手续费率
	
	if (m_nDontNeedFee)
		fFeeRate = 0.0;
	else
		fFeeRate = getsxf(m_crCurrGPCode,m_crCurrSetCode);
	for (i = 0; i < lReadNum; i++)
	{
		if (fabs(OutData[ENTERLONG][i] - 1) < COMPPREC || fabs(OutData[ENTERSHORT][i] - 1) < COMPPREC)	//表示当前买进
		{
			if(pTmpHisData->Close>COMPPREC)
			{
				bAllInvalid = FALSE;
				BuyPrice=pTmpHisData->Close;
				dCurBuyCount=100000.0/BuyPrice;
				dBuyCount+=dCurBuyCount;
				dBuyMoney+=100000.0*(1+fFeeRate);
			}
			pTmpHisData++;
		}
		else
			if (fabs(OutData[EXITLONG][i] - 1) < COMPPREC || fabs(OutData[EXITSHORT][i] - 1) < COMPPREC)	//表示当前卖出
			{
				if(dBuyMoney>1.0&&pTmpHisData->Close>COMPPREC)
				{
					bAllInvalid = FALSE;
					SellPrice=pTmpHisData->Close;
					dSellCount=dBuyCount;
					dSellMoney=dSellCount*SellPrice*(1-fFeeRate);
					tmpfBonus=(dSellMoney-dBuyMoney)/dBuyMoney;
					ftBonus+=tmpfBonus;
					if (tmpfBonus >= COMPPREC) 
					{
						nYLTimes++;
						fBonus += tmpfBonus;
					}
					else
						nKSTimes++;
					dBuyCount = 0.0;
					dSellCount = 0.0;
					dBuyMoney = 0.0;
					dSellMoney = 0.0;
				}
				pTmpHisData++;
			}
			else
			{
				pTmpHisData++;
			}
	}
	if (!bAllInvalid)
	{
		//计算最后一次的收益
		pTmpHisData--;
		if (dBuyMoney>1.0&&pTmpHisData->Close>COMPPREC)	//无卖出点或无买入点
		{
			SellPrice = pTmpHisData->Close;
			dSellCount=dBuyCount;
			dSellMoney=dSellCount*SellPrice*(1-fFeeRate);
			tmpfBonus=(dSellMoney-dBuyMoney)/dBuyMoney;
			ftBonus+=tmpfBonus;
			if (tmpfBonus >= COMPPREC) 
			{
				nYLTimes++;
				fBonus += tmpfBonus;
			}
			else
				nKSTimes++;
		}
		if (nYLTimes+nKSTimes != 0)
			fBonus /= (nYLTimes+nKSTimes);
		m_curBonus = fBonus;
		if(m_nOptimizeParam)
		{
			m_fAverageYL = fBonus;
		}
		else
		{
			if ( fBonus > m_fAverageYL )		//保存当前最大收益及当时的换手次数和参数值,盈利交易和亏损交易次数
			{
				m_nYLTimes = nYLTimes;
				m_nKSTimes = nKSTimes;
				m_toTimes = m_nYLTimes + m_nKSTimes;
				m_fTotalBonus = ftBonus;
				m_fAverageYL = fBonus;
				for ( i = 0; i < pIndex->nParaNum; i++ )
					m_fParaValue[i] = pIndex->aPara[i].nValue[m_nCalcPeriod];
			}
		}
	}
}

//平均亏损最小化
void CParamTestDlg::CalcFormula4(float **OutData, long lReadNum, TINDEXINFO *pIndex)
{
	int		i;
	float	fBonus=0.0f, tmpfBonus=0.0f, ftBonus=0.0f;	//收益,用百分数表示
	BOOL	bAllInvalid = TRUE;	//判断此次计算是否全为无效数据
	float   BuyPrice, SellPrice;	//BuyFlag表示已经买入股票的标志,BuyPrice表示买入股票的价格
	double	dBuyMoney = 0.0, dSellMoney = 0.0 ,dCurBuyCount=0.0;	//买入使用总资金,卖出使用总资金
	double	dBuyCount = 0.0, dSellCount = 0.0;	//买入量,卖出量
	LPHISDAT pTmpHisData = m_pAnalyData;
	int		nYLTimes = 0, nKSTimes = 0;
	float	fFeeRate;	//手续费率
	
	if (m_nDontNeedFee)
		fFeeRate = 0.0;
	else
		fFeeRate = getsxf(m_crCurrGPCode,m_crCurrSetCode);	
	for (i = 0; i < lReadNum; i++)
	{
		if (fabs(OutData[ENTERLONG][i] - 1) < COMPPREC || fabs(OutData[ENTERSHORT][i] - 1) < COMPPREC)	//表示当前买进
		{
			if(pTmpHisData->Close>COMPPREC)
			{
				bAllInvalid = FALSE;
				BuyPrice=pTmpHisData->Close;
				dCurBuyCount=100000.0/BuyPrice;
				dBuyCount+=dCurBuyCount;
				dBuyMoney+=100000.0*(1+fFeeRate);
			}
			pTmpHisData++;
		}
		else
			if (fabs(OutData[EXITLONG][i] - 1) < COMPPREC || fabs(OutData[EXITSHORT][i] - 1) < COMPPREC)	//表示当前卖出
			{
				if(dBuyMoney>1.0&&pTmpHisData->Close>COMPPREC)
				{
					bAllInvalid = FALSE;
					SellPrice=pTmpHisData->Close;
					dSellCount=dBuyCount;
					dSellMoney=dSellCount*SellPrice*(1-fFeeRate);
					tmpfBonus=(dSellMoney-dBuyMoney)/dBuyMoney;
					ftBonus+=tmpfBonus;
					if (tmpfBonus >= COMPPREC) 
						nYLTimes++;
					else
					{
						nKSTimes++;
						fBonus += tmpfBonus;
					}
					dBuyCount = 0.0;
					dSellCount = 0.0;
					dBuyMoney = 0.0;
					dSellMoney = 0.0;
				}
				pTmpHisData++;
			}
			else
			{
				pTmpHisData++;
			}
	}
	if (!bAllInvalid)
	{
		//计算最后一次的收益
		pTmpHisData--;
		if (dBuyMoney>1.0&&pTmpHisData->Close>COMPPREC)	//无卖出点或无买入点
		{
			SellPrice = pTmpHisData->Close;
			dSellCount=dBuyCount;
			dSellMoney=dSellCount*SellPrice*(1-fFeeRate);
			tmpfBonus=(dSellMoney-dBuyMoney)/dBuyMoney;
			ftBonus+=tmpfBonus;
			if (tmpfBonus >= COMPPREC) 
				nYLTimes++;
			else
			{
				nKSTimes++;
				fBonus += tmpfBonus;
			}
		}
		if(fBonus<COMPPREC) fBonus=ftBonus;
		if (nYLTimes+nKSTimes != 0)
			fBonus /= (nYLTimes+nKSTimes);
		if(fBonus>COMPPREC) m_curBonus = 0.0f;
		else m_curBonus = fBonus;
		if(m_nOptimizeParam)
		{
			if(fBonus>COMPPREC) m_fAverageKS = 0.0f;
			else m_fAverageKS = fBonus;
		}
		else
		{
			//由于fBonus是负数,因此平均亏损最小化反而要用">"号
			if ( fBonus > m_fAverageKS )		//保存当前最大收益及当时的换手次数和参数值,盈利交易和亏损交易次数
			{
				m_nYLTimes = nYLTimes;
				m_nKSTimes = nKSTimes;
				m_toTimes = m_nYLTimes + m_nKSTimes;
				m_fTotalBonus = ftBonus;
				if(fBonus>COMPPREC) m_fAverageKS = 0.0f;
				else m_fAverageKS = fBonus;
				for ( i = 0; i < pIndex->nParaNum; i++ )
					m_fParaValue[i] = pIndex->aPara[i].nValue[m_nCalcPeriod];
			}
		}
	}
}

//单次最大盈利最大化
void CParamTestDlg::CalcFormula5(float **OutData, long lReadNum, TINDEXINFO *pIndex)
{
	int		i;
	float	fBonus=0.0f, tmpfBonus=0.0f, ftBonus=0.0f;	//收益,用百分数表示
	BOOL	bAllInvalid = TRUE;	//判断此次计算是否全为无效数据
	float   BuyPrice, SellPrice;	//BuyFlag表示已经买入股票的标志,BuyPrice表示买入股票的价格
	double	dBuyMoney = 0.0, dSellMoney = 0.0 ,dCurBuyCount=0.0;	//买入使用总资金,卖出使用总资金
	double	dBuyCount = 0.0, dSellCount = 0.0;	//买入量,卖出量
	LPHISDAT pTmpHisData = m_pAnalyData;
	int		nYLTimes = 0, nKSTimes = 0;
	float	fFeeRate;	//手续费率
	
	if (m_nDontNeedFee)
		fFeeRate = 0.0;
	else
		fFeeRate = getsxf(m_crCurrGPCode,m_crCurrSetCode);	
	for (i = 0; i < lReadNum; i++)
	{
		if (fabs(OutData[ENTERLONG][i] - 1) < COMPPREC || fabs(OutData[ENTERSHORT][i] - 1) < COMPPREC)	//表示当前买进
		{
			if(pTmpHisData->Close>COMPPREC)
			{
				bAllInvalid = FALSE;
				BuyPrice=pTmpHisData->Close;
				dCurBuyCount=100000.0/BuyPrice;
				dBuyCount+=dCurBuyCount;
				dBuyMoney+=100000.0*(1+fFeeRate);
			}
			pTmpHisData++;
		}
		else
			if (fabs(OutData[EXITLONG][i] - 1) < COMPPREC || fabs(OutData[EXITSHORT][i] - 1) < COMPPREC)	//表示当前卖出
			{
				if(dBuyMoney>1.0&&pTmpHisData->Close>COMPPREC)
				{
					bAllInvalid = FALSE;
					SellPrice=pTmpHisData->Close;
					dSellCount=dBuyCount;
					dSellMoney=dSellCount*SellPrice*(1-fFeeRate);
					tmpfBonus=(dSellMoney-dBuyMoney)/dBuyMoney;
					ftBonus+=tmpfBonus;
					if (tmpfBonus >= COMPPREC)
					{
						nYLTimes++;
						if (tmpfBonus > fBonus)
						{
							fBonus = tmpfBonus;
						}
					}
					else
						nKSTimes++;
					dBuyCount = 0.0;
					dSellCount = 0.0;
					dBuyMoney = 0.0;
					dSellMoney = 0.0;
				}
				pTmpHisData++;
			}
			else
			{
				pTmpHisData++;
			}
	}
	if (!bAllInvalid)
	{
		//计算最后一次的收益
		pTmpHisData--;
		if (dBuyMoney>1.0&&pTmpHisData->Close>COMPPREC)	//无卖出点或无买入点
		{
			SellPrice = pTmpHisData->Close;
			dSellCount=dBuyCount;
			dSellMoney=dSellCount*SellPrice*(1-fFeeRate);
			tmpfBonus=(dSellMoney-dBuyMoney)/dBuyMoney;
			ftBonus+=tmpfBonus;
			if (tmpfBonus >= COMPPREC)
			{
				nYLTimes++;
				if (tmpfBonus > fBonus)
				{
					fBonus = tmpfBonus;
				}
			}
			else
				nKSTimes++;
		}
		m_curBonus = fBonus;
		if(m_nOptimizeParam)
		{
			m_fMaxBonus = fBonus;
		}
		else
		{
			if ( fBonus > m_fMaxBonus )		//保存当前最大收益及当时的换手次数和参数值,盈利交易和亏损交易次数
			{
				m_nYLTimes = nYLTimes;
				m_nKSTimes = nKSTimes;
				m_toTimes = m_nYLTimes + m_nKSTimes;
				m_fTotalBonus = ftBonus;
				m_fMaxBonus = fBonus;
				for ( i = 0; i < pIndex->nParaNum; i++ )
					m_fParaValue[i] = pIndex->aPara[i].nValue[m_nCalcPeriod];
			}
		}
	}
}

//单次最大亏损最小化
void CParamTestDlg::CalcFormula6(float **OutData, long lReadNum, TINDEXINFO *pIndex)
{
	int		i;
	float	fBonus=0.0f, tmpfBonus=0.0f, ftBonus=0.0f;	//收益,用百分数表示
	BOOL	bAllInvalid = TRUE;	//判断此次计算是否全为无效数据
	float   BuyPrice, SellPrice;	//BuyFlag表示已经买入股票的标志,BuyPrice表示买入股票的价格
	double	dBuyMoney = 0.0, dSellMoney = 0.0 ,dCurBuyCount=0.0;	//买入使用总资金,卖出使用总资金
	double	dBuyCount = 0.0, dSellCount = 0.0;	//买入量,卖出量
	LPHISDAT pTmpHisData = m_pAnalyData;
	int		nYLTimes = 0, nKSTimes = 0;
	float	fFeeRate;	//手续费率
	
	if (m_nDontNeedFee)
		fFeeRate = 0.0;
	else
		fFeeRate = getsxf(m_crCurrGPCode,m_crCurrSetCode);	
	for (i = 0; i < lReadNum; i++)
	{
		if (fabs(OutData[ENTERLONG][i] - 1) < COMPPREC || fabs(OutData[ENTERSHORT][i] - 1) < COMPPREC)	//表示当前买进
		{
			if(pTmpHisData->Close>COMPPREC)
			{
				bAllInvalid = FALSE;
				BuyPrice=pTmpHisData->Close;
				dCurBuyCount=100000.0/BuyPrice;
				dBuyCount+=dCurBuyCount;
				dBuyMoney+=100000.0*(1+fFeeRate);
			}
			pTmpHisData++;
		}
		else
			if (fabs(OutData[EXITLONG][i] - 1) < COMPPREC || fabs(OutData[EXITSHORT][i] - 1) < COMPPREC)	//表示当前卖出
			{
				if(dBuyMoney>1.0&&pTmpHisData->Close>COMPPREC)
				{
					bAllInvalid = FALSE;
					SellPrice=pTmpHisData->Close;
					dSellCount=dBuyCount;
					dSellMoney=dSellCount*SellPrice*(1-fFeeRate);
					tmpfBonus=(dSellMoney-dBuyMoney)/dBuyMoney;
					ftBonus+=tmpfBonus;
					if (tmpfBonus >= COMPPREC)
						nYLTimes++;
					else
					{
						nKSTimes++;
						if (tmpfBonus < fBonus)
						{
							fBonus = tmpfBonus;
						}
					}
					dBuyCount = 0.0;
					dSellCount = 0.0;
					dBuyMoney = 0.0;
					dSellMoney = 0.0;
				}
				pTmpHisData++;
			}
			else
			{
				pTmpHisData++;
			}
	}
	if (!bAllInvalid)
	{
		//计算最后一次的收益
		pTmpHisData--;
		if (dBuyMoney>1.0&&pTmpHisData->Close>COMPPREC)	//无卖出点或无买入点
		{
			SellPrice = pTmpHisData->Close;
			dSellCount=dBuyCount;
			dSellMoney=dSellCount*SellPrice*(1-fFeeRate);
			tmpfBonus=(dSellMoney-dBuyMoney)/dBuyMoney;
			ftBonus+=tmpfBonus;
			if (tmpfBonus >= COMPPREC)
				nYLTimes++;
			else
			{
				nKSTimes++;
				if (tmpfBonus < fBonus)
				{
					fBonus = tmpfBonus;
				}
			}
		}
		if(fBonus>COMPPREC) m_curBonus = 0.0f;
		else m_curBonus = fBonus;
		if(m_nOptimizeParam)
		{
			if(fBonus>COMPPREC) m_fMaxKS = 0.0f;
			else m_fMaxKS = fBonus;
		}
		else
		{
			if(fBonus<COMPPREC) fBonus=ftBonus;
			if ( fBonus > m_fMaxKS )		//保存当前最大收益及当时的换手次数和参数值,盈利交易和亏损交易次数
			{
				m_nYLTimes = nYLTimes;
				m_nKSTimes = nKSTimes;
				m_toTimes = m_nYLTimes + m_nKSTimes;
				m_fTotalBonus = ftBonus;
				if(fBonus>COMPPREC) m_fMaxKS = 0.0f;
				else m_fMaxKS = fBonus;
				for ( i = 0; i < pIndex->nParaNum; i++ )
					m_fParaValue[i] = pIndex->aPara[i].nValue[m_nCalcPeriod];
			}
		}
	}
}

//盈亏系数最大化
void CParamTestDlg::CalcFormula7(float **OutData, long lReadNum, TINDEXINFO *pIndex)
{
	int		i;
	float	fYLBonus=0.0f, fKSBonus = 0.0f, tmpfBonus = 0.0f, ftBonus=0.0f;	//收益,用百分数表示
	BOOL	bAllInvalid = TRUE;	//判断此次计算是否全为无效数据
	float   BuyPrice, SellPrice;	//BuyFlag表示已经买入股票的标志,BuyPrice表示买入股票的价格
	double	dBuyMoney = 0.0, dSellMoney = 0.0 ,dCurBuyCount=0.0;	//买入使用总资金,卖出使用总资金
	double	dBuyCount = 0.0, dSellCount = 0.0;	//买入量,卖出量
	LPHISDAT pTmpHisData = m_pAnalyData;
	int		nYLTimes = 0, nKSTimes = 0;
	float	fFeeRate;	//手续费率
	
	if (m_nDontNeedFee)
		fFeeRate = 0.0;
	else
		fFeeRate = getsxf(m_crCurrGPCode,m_crCurrSetCode);	
	for (i = 0; i < lReadNum; i++)
	{
		if (fabs(OutData[ENTERLONG][i] - 1) < COMPPREC || fabs(OutData[ENTERSHORT][i] - 1) < COMPPREC)	//表示当前买进
		{
			if(pTmpHisData->Close>COMPPREC)
			{
				bAllInvalid = FALSE;
				BuyPrice=pTmpHisData->Close;
				dCurBuyCount=100000.0/BuyPrice;
				dBuyCount+=dCurBuyCount;
				dBuyMoney+=100000.0*(1+fFeeRate);
			}
			pTmpHisData++;
		}
		else
			if (fabs(OutData[EXITLONG][i] - 1) < COMPPREC || fabs(OutData[EXITSHORT][i] - 1) < COMPPREC)	//表示当前卖出
			{
				if(dBuyMoney>1.0&&pTmpHisData->Close>COMPPREC)
				{
					bAllInvalid = FALSE;
					SellPrice=pTmpHisData->Close;
					dSellCount=dBuyCount;
					dSellMoney=dSellCount*SellPrice*(1-fFeeRate);
					tmpfBonus=(dSellMoney-dBuyMoney)/dBuyMoney;
					ftBonus+=tmpfBonus;
					if (tmpfBonus >= COMPPREC) 
					{
						nYLTimes++;
						fYLBonus += tmpfBonus;
					}
					else
					{
						nKSTimes++;
						fKSBonus += tmpfBonus;
					}
					dBuyCount = 0.0;
					dSellCount = 0.0;
					dBuyMoney = 0.0;
					dSellMoney = 0.0;
				}
				pTmpHisData++;
			}
			else
			{
				pTmpHisData++;
			}
	}
	if (!bAllInvalid)
	{
		//计算最后一次的收益
		pTmpHisData--;
		if (dBuyMoney>1.0&&pTmpHisData->Close>COMPPREC)	//无卖出点或无买入点
		{
			SellPrice = pTmpHisData->Close;
			dSellCount=dBuyCount;
			dSellMoney=dSellCount*SellPrice*(1-fFeeRate);
			tmpfBonus=(dSellMoney-dBuyMoney)/dBuyMoney;
			ftBonus+=tmpfBonus;
			if (tmpfBonus >= COMPPREC) 
			{
				nYLTimes++;
				fYLBonus += tmpfBonus;
			}
			else
			{
				nKSTimes++;
				fKSBonus += tmpfBonus;
			}
		}
		if (fabs(fYLBonus)+fabs(fKSBonus) < COMPPREC)
			tmpfBonus = 0.0f;
		else
			tmpfBonus = (fabs(fYLBonus)-fabs(fKSBonus))/(fabs(fYLBonus)+fabs(fKSBonus));
		m_curBonus = tmpfBonus;
		if(m_nOptimizeParam)
		{
			m_YKCoeff  = tmpfBonus;
		}
		else
		{
			if (tmpfBonus > m_YKCoeff)		//保存当前最大收益及当时的换手次数和参数值
			{
				m_nYLTimes = nYLTimes;
				m_nKSTimes = nKSTimes;
				m_toTimes = m_nYLTimes + m_nKSTimes;
				m_fTotalBonus = ftBonus;
				m_YKCoeff = tmpfBonus;
				for ( i = 0; i < pIndex->nParaNum; i++ )
					m_fParaValue[i] = pIndex->aPara[i].nValue[m_nCalcPeriod];
			}
		}
	}
}

void CParamTestDlg::CalcBonus(TINDEXINFO *pIndex,long ReadNum, int nType, long optIndex)
{
	float *pOutData[MAX_LINEOUT];

	for (int i=0; i<4;i++) 
		pOutData[i] = g_pCalc->_CallDTopViewCalc_13(ReadNum);
	if ( m_nTestType==EXP_TYPE && g_pCalc->_CallDTopViewCalc_5(EXP_TYPE, m_NameCombo.GetCurSel(), pOutData) == 0)	//计算成功
	{
		if(!m_nOptimizeParam)
		{
			CalcFormula(pOutData, ReadNum);
			switch (nType)
			{
			case 0://总收益最大化
				CalcFormula0(pOutData, ReadNum, pIndex);
				break;
			case 1://平均收益最大化
				CalcFormula1(pOutData, ReadNum, pIndex);
				break;
			case 2://扣除最大盈利后收益最大化
				CalcFormula2(pOutData, ReadNum, pIndex);
				break;
			case 3://平均盈利最大化
				CalcFormula3(pOutData, ReadNum, pIndex);
				break;
			case 4://平均亏损最小化
				CalcFormula4(pOutData, ReadNum, pIndex);
				break;
			case 5://单次最大盈利最大化
				CalcFormula5(pOutData, ReadNum, pIndex);
				break;
			case 6://单次最大亏损最小化
				CalcFormula6(pOutData, ReadNum, pIndex);
				break;
			case 7://盈亏系数最大化
				CalcFormula7(pOutData, ReadNum, pIndex);
				break;
			}
			m_pCache[optIndex] += m_curBonus;
		}
		else
		{
			CalcFormula(pOutData, ReadNum);
			CalcFormula0(pOutData, ReadNum, pIndex);
			CalcFormula1(pOutData, ReadNum, pIndex);
			CalcFormula2(pOutData, ReadNum, pIndex);
			CalcFormula3(pOutData, ReadNum, pIndex);
			CalcFormula4(pOutData, ReadNum, pIndex);
			CalcFormula5(pOutData, ReadNum, pIndex);
			CalcFormula6(pOutData, ReadNum, pIndex);
			CalcFormula7(pOutData, ReadNum, pIndex);
		}
	}
	if ( m_nTestType==TJ_TYPE && g_pCalc->_CallDTopViewCalc_5(TJ_TYPE, m_NameCombo.GetCurSel(), pOutData) == 0)	//计算成功
	{
		CalcFormula(pOutData, ReadNum);
		CalcTJForm(pOutData, ReadNum, nType, pIndex);
		if(!m_nOptimizeParam) m_pCache[optIndex] += m_curBonus;
	}
	for (i=0; i<4; i++) g_pCalc->_CallDTopViewCalc_14(pOutData[i]);
}

void CParamTestDlg::CalcTJForm(float **OutData, long lReadNum, int nType, TINDEXINFO *pIndex)
{
	int		i, j;
	float	fBonus[4], tmpfBonus[4], fEndBonus[8];	//收益,用百分数表示
	float   BuyPrice, SellPrice[4];	//BuyFlag表示已经买入股票的标志,BuyPrice表示买入股票的价格
	LPHISDAT pTmpHisData = m_pAnalyData;
	int		nTimes = 0;
	float   values[4];
	for(i=0;i<4;i++)
		fBonus[i]=0.0f;

	for (i = 0; i < lReadNum; i++)
	{
		if (fabs(OutData[0][i] - 1) < COMPPREC)	//表示当前买进
		{
			if(pTmpHisData->Close>COMPPREC)
			{
				BuyPrice=pTmpHisData->Close;
				nTimes++;
				int nValid=lReadNum-i-1;
				CalcValues(pTmpHisData,values,min(nValid,m_nTestPeriod));
				for(j=0;j<4;j++) 
				{
					SellPrice[j]=values[j];
					tmpfBonus[j] = (SellPrice[j]-BuyPrice)/BuyPrice;
					fBonus[j] += tmpfBonus[j];
				}
			}
		}
		pTmpHisData++;
	}
	for(j=0;j<4;j++)
	{
		if(nTimes>0)
		{
			fEndBonus[2*j]  = fBonus[j];
			fEndBonus[2*j+1] = fBonus[j]/(float)nTimes; 
		}
		else
		{
			fEndBonus[2*j]=0.0f;
			fEndBonus[2*j+1]=0.0f;
		}
	}
	if(m_nOptimizeParam)
	{
		m_toTimes = nTimes;
		for(j=0;j<8;j++)
		{
			m_fTJBonus[j]  = fEndBonus[j];
		}
	}
	else
	{
		m_curBonus = fEndBonus[nType];
		if ( fEndBonus[nType] > m_fMaxBonus && nTimes>0)		//保存当前最大收益及当时的换手次数和参数值,盈利交易和亏损交易次数
		{
			m_toTimes = nTimes;
			m_fMaxBonus = fEndBonus[nType];
			for ( i = 0; i < pIndex->nParaNum; i++ )
				m_fParaValue[i] = pIndex->aPara[i].nValue[m_nCalcPeriod];
		}
	}	
}

void CParamTestDlg::CalcValues(LPHISDAT pHisData,float *pValues,short nData)
{
	LPHISDAT tmpData=++pHisData;
	pValues[0]=tmpData[0].High;
	pValues[1]=tmpData[0].Low;
	pValues[2]=tmpData[0].Close*tmpData[0].fVolume;
	long lToVol=tmpData[0].fVolume;
	for(int i=1;i<nData;i++)
	{
		if(tmpData[i].High>pValues[0]) pValues[0]=tmpData[i].High;
		if(tmpData[i].Low<pValues[1])  pValues[1]=tmpData[i].Low;
		pValues[2]+=tmpData[i].Close*tmpData[i].fVolume;
		lToVol+=tmpData[i].fVolume;
	}
	pValues[3]=tmpData[nData-1].Close;
	if(lToVol<COMPPREC) pValues[2]=pValues[3];
	else pValues[2]/=(float)lToVol;
	
}

void CParamTestDlg::OnOparam() 
{
	if(m_nOptimizeParam)
	{
		m_nOptimizeParam=0;
		GetDlgItem(IDC_COMBO3)->EnableWindow(TRUE);
		GetDlgItem(IDC_SHOWMORE)->EnableWindow(FALSE);
		GetDlgItem(IDC_SHOWDETAIL)->EnableWindow(FALSE);
		for(int i=0;i<6;i++)
		{
			GetDlgItem(IDC_EDIT_PARAM1+i)->EnableWindow(FALSE);
		}
		FillLvc();
		UpdateData(FALSE);
	}
}

void CParamTestDlg::OnTestparam() 
{
	if(m_nOptimizeParam!=1)
	{
		m_nOptimizeParam=1;
		GetDlgItem(IDC_SHOWMORE)->EnableWindow(TRUE);
		int selIndex=m_List.GetNextItem(-1,LVNI_SELECTED);
		if(selIndex!=-1)
			GetDlgItem(IDC_SHOWGGDETAIL)->EnableWindow(TRUE);
		GetDlgItem(IDC_SHOWDETAIL)->EnableWindow(TRUE);
		for(int i=0;i<6;i++)
		{
			GetDlgItem(IDC_EDIT_PARAM1+i)->EnableWindow(TRUE);
		}
		GetDlgItem(IDC_COMBO3)->EnableWindow(FALSE);
		FillLvc();
		UpdateData(FALSE);
	}
}

void CParamTestDlg::OnShowmore() 
{
	CNewMenu showMore;
	showMore.CreatePopupMenu();
	if(m_nTestType==EXP_TYPE)
	{
		showMore.AppendMenu(0, IDC_AverageBonus, _F(syTitle[5]));
		showMore.AppendMenu(0, IDC_DivHeadBonus, _F(syTitle[6]));
		showMore.AppendMenu(0, IDC_AverageYL, _F(syTitle[7]));
		showMore.AppendMenu(0, IDC_AverageKS, _F(syTitle[8]));
		showMore.AppendMenu(0, IDC_MaxBonus, _F(syTitle[9]));
		showMore.AppendMenu(0, IDC_MaxKS, _F(syTitle[10]));
		showMore.AppendMenu(0, IDC_ToTimes, _F(syTitle[11]));
		showMore.AppendMenu(0, IDC_YKCoeff, _F(syTitle[12]));
		for(UINT i=IDC_AverageBonus;i<=IDC_YKCoeff;i++)
		{
			if(m_showhat[i-IDC_AverageBonus+5])
				showMore.CheckMenuItem(i,MF_BYCOMMAND|MF_CHECKED);
		}
	}
	if(m_nTestType==TJ_TYPE)
	{
		showMore.AppendMenu(0, IDC_TOLHIGH, _F(syTjTitle[3]));
		showMore.AppendMenu(0, IDC_AVERHIGH, _F(syTjTitle[4]));
		showMore.AppendMenu(0, IDC_TOLLOW, _F(syTjTitle[5]));
		showMore.AppendMenu(0, IDC_AVERLOW, _F(syTjTitle[6]));
		showMore.AppendMenu(0, IDC_TOLAVER, _F(syTjTitle[7]));
		showMore.AppendMenu(0, IDC_AVERAVER, _F(syTjTitle[8]));
		showMore.AppendMenu(0, IDC_TOLEND, _F(syTjTitle[9]));
		showMore.AppendMenu(0, IDC_AVEREND, _F(syTjTitle[10]));
		for(UINT i=IDC_TOLHIGH;i<=IDC_AVEREND;i++)
		{
			if(m_showhat[i-IDC_TOLHIGH+3])
				showMore.CheckMenuItem(i,MF_BYCOMMAND|MF_CHECKED);
		}
	}
	CRect showMoreRect;
	GetDlgItem(IDC_SHOWMORE)->GetWindowRect(&showMoreRect);
	showMore.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, showMoreRect.left, showMoreRect.bottom, this);
}

void CParamTestDlg::OnSelShowMore1(UINT nID)
{
	if(m_nTestType==EXP_TYPE)
	{
		m_showhat[nID-IDC_AverageBonus+5]=!m_showhat[nID-IDC_AverageBonus+5];
	}
	FillLvc();
	int selIndex=m_List.GetNextItem(-1,LVNI_SELECTED);
	if(selIndex!=-1)
		GetDlgItem(IDC_SHOWGGDETAIL)->EnableWindow(TRUE);
	GetDlgItem(IDC_SHOWDETAIL)->EnableWindow(TRUE);
}

void CParamTestDlg::OnSelShowMore2(UINT nID)
{
	if(m_nTestType==TJ_TYPE)
	{
		m_showhat[nID-IDC_TOLHIGH+3]=!m_showhat[nID-IDC_TOLHIGH+3];
	}
	FillLvc();
	GetDlgItem(IDC_SHOWDETAIL)->EnableWindow(FALSE);
}

void CParamTestDlg::OnColumnclickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if(!m_IsCarried||m_IsCarrying) return;
	UpdateData();
	WORD index=0;
	for (int i = 0; i < m_List.GetItemCount(); i++)
	{	
		index=HIWORD(m_List.GetItemData(i));
		m_List.SetItemData(i,MAKELPARAM(i,index));
	}
	nCurrSortType=pNMListView->iSubItem;
	if(nCurrSortType>0)
		m_List.SortItems(MyCompareProc,(LPARAM)&m_List);

	*pResult = 0;
}

static int CALLBACK MyCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	// lParamSort contains a pointer to the list view control.
	// The lParam of an item is just its index.
	CListCtrl* pListCtrl = (CListCtrl*) lParamSort;
	CString    strItem1 = pListCtrl->GetItemText(LOWORD(lParam1), nCurrSortType);
	CString    strItem2 = pListCtrl->GetItemText(LOWORD(lParam2), nCurrSortType);
	
	float fItem1, fItem2;
	fItem1 = atof(strItem1);
	fItem2 = atof(strItem2);
	if (fItem1 < fItem2) return 1;
	else
		if (fItem1 > fItem2) return -1;
		else return 0;
}

void CParamTestDlg::CalcHighLowEx(float &high, float &low, const int nPreBuy, const int nNowSell, const long lReadNum)
{
	LPHISDAT pTmpHisData = m_pAnalyData;
	high = pTmpHisData[nPreBuy].High;low = pTmpHisData[nPreBuy].Low;
	for(int i=nPreBuy;i<=nNowSell&&i<lReadNum&&TBE_MEANLESS(pTmpHisData[i].Close);i++)
	{
		if(pTmpHisData[i].High>high) high = pTmpHisData[i].High;
		if(pTmpHisData[i].Low<low) low = pTmpHisData[i].Low;
	}
}

void CParamTestDlg::CalcDetails(float **OutData, long lReadNum, short iGp, short calcType)
{
	float	fBonus=0.0f, tmpfBonus=0.0f;	//收益,用百分数表示
	BOOL	bAllInvalid = TRUE;	//判断此次计算是否全为无效数据
	float   BuyPrice, SellPrice;	//BuyFlag表示已经买入股票的标志,BuyPrice表示买入股票的价格
	double	dBuyMoney = 0.0, dSellMoney = 0.0 ,dCurBuyCount=0.0;	//买入使用总资金,卖出使用总资金
	int	dBuyCount = 0, dSellCount = 0;	//买入量,卖出量
	LPHISDAT pTmpHisData = m_pAnalyData;
	float	fFeeRate;	//手续费率
	testDetail tmp;
	
	if (m_nDontNeedFee)
		fFeeRate = 0.0;
	else
		fFeeRate = getsxf(m_crCurrGPCode,m_crCurrSetCode);
	
	int nPreBuy = 0;
	for (int i = 0; i < lReadNum; i++)
	{
		memset(&tmp, 0, sizeof(testDetail));
		if (fabs(OutData[ENTERLONG][i] - 1) < COMPPREC || fabs(OutData[ENTERSHORT][i] - 1) < COMPPREC)	//表示当前买进
		{
			if(pTmpHisData->Close>COMPPREC&&pTmpHisData->Close<9999999.9f)
			{
				nPreBuy = i; 
				bAllInvalid = FALSE;
				tmp.iGp=iGp;
				tmp.nTime=pTmpHisData->Time;
				tmp.iToDo=1;
				BuyPrice=pTmpHisData->Close;
				tmp.fPrice=pTmpHisData->Close;
				dCurBuyCount=(int)((int)(300000.0f/BuyPrice)/100)*100;
				tmp.lCount=dCurBuyCount;
				tmp.fValue=0.0f;
				dBuyCount+=dCurBuyCount;
				dBuyMoney+=dCurBuyCount*BuyPrice*(1+fFeeRate);
				if(calcType==CALC_SINGLE) 	m_aDetails.push_back(tmp);
				else if(calcType==CALC_ALL) 
				{
					m_sTestDetails.push_back(tmp);
				}
			}
			pTmpHisData++;
		}
		else
			if (fabs(OutData[EXITLONG][i] - 1) < COMPPREC || fabs(OutData[EXITSHORT][i] - 1) < COMPPREC)	//表示当前卖出
			{
				if(dBuyMoney>1.0&&pTmpHisData->Close>COMPPREC&&pTmpHisData->Close<9999999.9f)
				{
					bAllInvalid = FALSE;
					tmp.iGp=iGp;
					tmp.nTime=pTmpHisData->Time;
					tmp.iToDo=0;
					SellPrice=pTmpHisData->Close;
					tmp.fPrice=pTmpHisData->Close;
					dSellCount=dBuyCount;
					tmp.lCount=dBuyCount;
					dSellMoney=dSellCount*SellPrice*(1-fFeeRate);
					tmpfBonus=dSellMoney-dBuyMoney;
					tmp.fValue=tmpfBonus;
					tmp.fProfit=100.0f*tmpfBonus/dBuyMoney;

					float fHigh, fLow;
					fHigh = fLow = 0.0f;
					if(i>nPreBuy)
						CalcHighLowEx(fHigh, fLow, nPreBuy, i, lReadNum);
					tmp.fTopProfit = 100.0f*(dSellCount*fHigh*(1-fFeeRate)-dBuyMoney)/dBuyMoney;
					tmp.fButtonProfit = 100.0f*(dSellCount*fLow*(1-fFeeRate)-dBuyMoney)/dBuyMoney;
					
					if(calcType==CALC_SINGLE) 
						m_aDetails.push_back(tmp);
					else if(calcType==CALC_ALL) 
					{
						long lSellTime = tmp.nTime.year*10000+tmp.nTime.month*100+tmp.nTime.day;
						m_sTestDetails.push_back(tmp);
						m_allTestDetails[lSellTime] = tmp;
					}
					dBuyCount = 0;
					dSellCount = 0;
					dBuyMoney = 0.0;
					dSellMoney = 0.0;
				}
				pTmpHisData++;
			}
			else
			{
				pTmpHisData++;
			}
	}
	memset(&tmp, 0, sizeof(testDetail));
	if (!bAllInvalid)
	{
		//计算最后一次的收益
		pTmpHisData--;
		if (dBuyMoney>1.0&&pTmpHisData->Close>COMPPREC&&pTmpHisData->Close<9999999.9f)
		{
			tmp.iGp=iGp;
			tmp.nTime=pTmpHisData->Time;
			tmp.iToDo=-1;
			SellPrice = pTmpHisData->Close;
			tmp.fPrice=pTmpHisData->Close;
			dSellCount=dBuyCount;
			tmp.lCount=dBuyCount;
			dSellMoney=dSellCount*SellPrice*(1-fFeeRate);
			tmpfBonus=dSellMoney-dBuyMoney;
			tmp.fValue=tmpfBonus;
			tmp.fProfit=100.0f*tmpfBonus/dBuyMoney;
			
			float fHigh, fLow;
			fHigh = fLow = 0.0f;
			if(i>nPreBuy)
				CalcHighLowEx(fHigh, fLow, nPreBuy, i, lReadNum);
			tmp.fTopProfit = 100.0f*(dSellCount*fHigh*(1-fFeeRate)-dBuyMoney)/dBuyMoney;
			tmp.fButtonProfit = 100.0f*(dSellCount*fLow*(1-fFeeRate)-dBuyMoney)/dBuyMoney;

			if(calcType==CALC_SINGLE) 
				m_aDetails.push_back(tmp);
			else if(calcType==CALC_ALL)
			{
				long lSellTime = tmp.nTime.year*10000+tmp.nTime.month*100+tmp.nTime.day;
				m_sTestDetails.push_back(tmp);
				m_allTestDetails[lSellTime] = tmp;
			}
		}
	}	
}

void CParamTestDlg::CalcTjDetails(float **OutData, long lReadNum)
{
	LPHISDAT pTmpHisData = m_pAnalyData;
	testDetail tmp; 

	for (int i = 0; i < lReadNum; i++)
	{
		memset(&tmp, 0, sizeof(testDetail));
		if (fabs(OutData[0][i] - 1) < COMPPREC)	//表示当前买进
		{
			if(pTmpHisData->Close>COMPPREC)
			{
				tmp.iGp=-1;
				tmp.nTime=pTmpHisData->Time;
				tmp.iToDo=3;
				tmp.fPrice=pTmpHisData->Close;
				m_aDetails.push_back(tmp);
			}
		}
		pTmpHisData++;
	}
}

void CParamTestDlg::SingleDetail() 
{
	m_aDetails.clear();
	if(m_List.GetSelectedCount()<1||!m_IsCarried||m_IsCarrying) return;
	TINDEXINFO	*tmp, IndexBakup;
	NTime	time1, time2;
	if(m_nTestType==EXP_TYPE) tmp = g_pCalc->_CallDTopViewCalc_8(EXP_TYPE, m_NameCombo.GetCurSel());
	if(m_nTestType==TJ_TYPE) tmp = g_pCalc->_CallDTopViewCalc_8(TJ_TYPE, m_NameCombo.GetCurSel());
	if (tmp == NULL) return;
	memcpy(&IndexBakup, tmp, sizeof(TINDEXINFO));
	m_nCalcPeriod = m_PeriodCombo.GetCurSel();
	int iTestCount=m_List.GetNextItem(-1,LVNI_SELECTED);
	int index=HIWORD(m_List.GetItemData(iTestCount));
	if(index<0) return;
	if(m_nOptimizeParam)
	{
		tmp->aPara[0].nValue[m_nCalcPeriod] = atoi(m_sParam1);
		tmp->aPara[1].nValue[m_nCalcPeriod] = atoi(m_sParam2);
		tmp->aPara[2].nValue[m_nCalcPeriod] = atoi(m_sParam3);
		tmp->aPara[3].nValue[m_nCalcPeriod] = atoi(m_sParam4);
		tmp->aPara[4].nValue[m_nCalcPeriod] = atoi(m_sParam5);
		tmp->aPara[5].nValue[m_nCalcPeriod] = atoi(m_sParam6);
	}
	else
	{
		int nColumnCount = m_List.GetHeaderCtrl()->GetItemCount();
		CString sParam;
		if(m_nTestType==EXP_TYPE) sParam=m_List.GetItemText(iTestCount,4);
		if(m_nTestType==TJ_TYPE)  sParam=m_List.GetItemText(iTestCount,3);
		if(sParam=="") return;
		int num=tmp->nParaNum;
		CString sTmp;
		int iStart=0,iEnd;
		for(int i=0;i<num-1;i++)
		{
			iEnd=sParam.Find(",",iStart);
			int iCount=iEnd-iStart;
			sTmp=sParam.Mid(iStart,iCount);
			tmp->aPara[i].nValue[m_nCalcPeriod] = atoi(sTmp);
			iStart=++iEnd;
		}
		sTmp=sParam.Mid(iStart);
		tmp->aPara[i].nValue[m_nCalcPeriod] = atoi(sTmp);
	}

	m_pAnalyData = NULL;
	strcpy(m_crCurrGPCode, (*g_pStockIO)[index]->Code);
	m_crCurrSetCode = (*g_pStockIO)[index]->setcode;
	time1.year = m_BeginTime.GetYear();
	time1.month = m_BeginTime.GetMonth();
	time1.day = m_BeginTime.GetDay();
	time2.year = m_EndTime.GetYear();
	time2.month = m_EndTime.GetMonth();
	time2.day = m_EndTime.GetDay();
	long lReadNum = g_pCalc->_CallDTopViewCalc_4(m_crCurrGPCode,m_crCurrSetCode,CalcPeriodToTdxPeriod(m_nCalcPeriod), time1, time2,1); //全部除权

	float *pOutData[MAX_LINEOUT];
	for (int i=0; i<4;i++) 
		pOutData[i] = g_pCalc->_CallDTopViewCalc_13(lReadNum);
	if(m_nTestType==EXP_TYPE) g_pCalc->_CallDTopViewCalc_5(EXP_TYPE, m_NameCombo.GetCurSel(), pOutData);
	if(m_nTestType==TJ_TYPE) g_pCalc->_CallDTopViewCalc_5(TJ_TYPE, m_NameCombo.GetCurSel(), pOutData);
	m_pAnalyData = (LPHISDAT)g_pCalc->_CallDTopViewCalc_15();
	if(lReadNum <= 0 || m_pAnalyData == NULL)	//计算不成功
		return;
	if(m_nTestType==EXP_TYPE) CalcDetails(pOutData, lReadNum, index, CALC_SINGLE);
	if(m_nTestType==TJ_TYPE)  CalcTjDetails(pOutData, lReadNum);
	memcpy(&tmp->aPara, &IndexBakup.aPara, sizeof(TPARAINFO) * 6);
	char sTitle[128];
	sprintf(sTitle,"%s: %s",tmp->acCode,(*g_pStockIO)[index]->Name);
	m_pShowDetail = new CMyDetailDlg(this);
	m_pShowDetail->m_sName=sTitle;
	m_pShowDetail->m_testDetails=m_aDetails;
	m_pShowDetail->Create(IDD_DETAIL);
	m_pShowDetail->ShowWindow(SW_SHOW);
}

void CParamTestDlg::OnShowdetail()
{
	m_allTestDetails.clear();
	m_sTestDetails.clear();
	if(m_List.GetSelectedCount()<1||!m_IsCarried||m_IsCarrying) return;

	if(!CanProcesLocalData(this))
		return;	
	
	m_NameCombo.EnableWindow(FALSE);
	m_PeriodCombo.EnableWindow(FALSE);
	m_AimCombo.EnableWindow(FALSE);
	if(!m_nOptimizeParam)
		GetDlgItem(IDC_TESTPARAM)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_OPARAM)->EnableWindow(FALSE);
	GetDlgItem(IDC_ADDGP)->EnableWindow(FALSE);
	GetDlgItem(IDC_DELGP)->EnableWindow(FALSE);
	GetDlgItem(IDC_DELGPALL)->EnableWindow(FALSE);
	GetDlgItem(IDC_SHOWMORE)->EnableWindow(FALSE);
	GetDlgItem(IDC_SHOWGGDETAIL)->EnableWindow(FALSE);
	GetDlgItem(IDC_SHOWDETAIL)->EnableWindow(FALSE);
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	SetDlgItemText(IDCANCEL, _F("中止测试"));

	m_Progress.SetPos(0);
	m_OutInfo.SetTextColor(CLR_NOWTESTING);
	m_OutInfo.SetWindowText(_F("正在进行测试..."));
	m_IsCarried = FALSE;
	m_IsCarrying = TRUE;
	DisableOther();
	//在Disable其它之后，需设一控件为焦点,否则ESC不起作用
	GetDlgItem(IDCANCEL)->SetFocus();

	TINDEXINFO	*tmp, IndexBakup;
	NTime	time1, time2;
	if(m_nTestType==EXP_TYPE) tmp = g_pCalc->_CallDTopViewCalc_8(EXP_TYPE, m_NameCombo.GetCurSel());
	if (tmp == NULL) return;
	memcpy(&IndexBakup, tmp, sizeof(TINDEXINFO));
	m_nCalcPeriod = m_PeriodCombo.GetCurSel();
	if(m_nOptimizeParam)
	{
		tmp->aPara[0].nValue[m_nCalcPeriod] = atoi(m_sParam1);
		tmp->aPara[1].nValue[m_nCalcPeriod] = atoi(m_sParam2);
		tmp->aPara[2].nValue[m_nCalcPeriod] = atoi(m_sParam3);
		tmp->aPara[3].nValue[m_nCalcPeriod] = atoi(m_sParam4);
		tmp->aPara[4].nValue[m_nCalcPeriod] = atoi(m_sParam5);
		tmp->aPara[5].nValue[m_nCalcPeriod] = atoi(m_sParam6);
	}
	else return;
	int nTestGp = m_List.GetItemCount()-1;
	m_Progress.SetRange32(0, nTestGp);
	m_Progress.SetStep(1);
	m_Progress.SetPos(0);
	for(int i=0; i<nTestGp; i++)
	{
		if(PeekMyMsg())
		{
			EnableOther();
			g_bDonotClose = FALSE;
			m_IsCarried = TRUE;
			return;
		}

		int index=HIWORD(m_List.GetItemData(i));
		if(index<0) return;
		m_pAnalyData = NULL;
		strcpy(m_crCurrGPCode, (*g_pStockIO)[index]->Code);
		m_crCurrSetCode = (*g_pStockIO)[index]->setcode;
		time1.year = m_BeginTime.GetYear();
		time1.month = m_BeginTime.GetMonth();
		time1.day = m_BeginTime.GetDay();
		time2.year = m_EndTime.GetYear();
		time2.month = m_EndTime.GetMonth();
		time2.day = m_EndTime.GetDay();
		long lReadNum = g_pCalc->_CallDTopViewCalc_4(m_crCurrGPCode,m_crCurrSetCode,CalcPeriodToTdxPeriod(m_nCalcPeriod), time1, time2,1); //全部除权
		if(lReadNum <= 0) continue;

		float *pOutData[MAX_LINEOUT];	
		for (int j=0; j<4;j++) 
			pOutData[j] = g_pCalc->_CallDTopViewCalc_13(lReadNum);
		if(m_nTestType==EXP_TYPE) g_pCalc->_CallDTopViewCalc_5(EXP_TYPE, m_NameCombo.GetCurSel(), pOutData);
		m_pAnalyData = (LPHISDAT)g_pCalc->_CallDTopViewCalc_15();
		if(m_pAnalyData == NULL)	//计算不成功
			continue;
		CalcDetails(pOutData, lReadNum, index, CALC_ALL);
		m_Progress.StepIt();
	}
	memcpy(&tmp->aPara, &IndexBakup.aPara, sizeof(TPARAINFO) * 6);
	if(!m_allTestDetails.empty()&&!m_sTestDetails.empty())
	{
		CAllTestMapDlg AllTestMap;
		AllTestMap.m_aTestData=m_allTestDetails;
		AllTestMap.m_sTestData=m_sTestDetails;
		AllTestMap.DoModal();
	}

	m_OutInfo.SetWindowText(_F("测试完毕！"));
	m_Progress.SetPos(0);
	EnableOther();
	m_IsCarried = TRUE;
	m_IsCarrying = FALSE;
	g_bDonotClose = FALSE;
}

void CParamTestDlg::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) 
{	
	if(!m_IsCarried||m_IsCarrying) return;
	int selIndex=m_List.GetNextItem(-1,LVNI_SELECTED);
	CString zhpj=m_List.GetItemText(selIndex, 0);
	if (selIndex!=-1&&zhpj!="综合平均")
	{
		SingleDetail();
	}	
	*pResult = 0;
}

void CParamTestDlg::OnDelgpall() 
{
	for(int i = m_nAddedGPNum - 1;i >= 0;i--)
	{
		m_nAddedGPIndex[i] = -1;
	}
	m_nAddedGPNum=0;
	FillGPReport();
	GetDlgItem(IDC_SHOWGGDETAIL)->EnableWindow(FALSE);
	GetDlgItem(IDC_SHOWDETAIL)->EnableWindow(FALSE); 
}

void CParamTestDlg::OnShowggdetail() 
{
	if(!m_IsCarried||m_IsCarrying) return;
	int selIndex=m_List.GetNextItem(-1,LVNI_SELECTED);
	CString zhpj=m_List.GetItemText(selIndex, 0);
	if (selIndex!=-1&&zhpj!="综合平均")
	{
		SingleDetail();
	}
}

#include "PasswordDlg.h"
void CParamTestDlg::OnModgs() 
{
	m_nCalcPeriod = m_PeriodCombo.GetCurSel();	
	
	TINDEXINFO* pTmpIndex = g_pCalc->_CallDTopViewCalc_8(m_nTestType, m_NameCombo.GetCurSel());
	if (pTmpIndex != NULL)
	{
		BOOL bOK = TRUE;
		if (pTmpIndex->lpszPassword != NULL && strlen(pTmpIndex->lpszPassword) != 0)
		{
			CPasswordDlg PasswordDlg;
			PasswordDlg.m_strName = pTmpIndex->acCode;
			PasswordDlg.m_bFomulaCompareBin = TRUE;
			memcpy(PasswordDlg.m_lpszPassword,pTmpIndex->lpszPassword,PASSWORD_LEN);
			if (PasswordDlg.DoModal() != IDOK)
				bOK = FALSE;
		}
		if (bOK) 
			g_pCalc->_CallDTopViewCalc_16(m_nTestType, m_NameCombo.GetCurSel(), CalcPeriodToTdxPeriod(m_nCalcPeriod));
	}
}
