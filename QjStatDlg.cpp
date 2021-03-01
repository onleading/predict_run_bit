// QjStatDlg.cpp : implementation file
#include "stdafx.h"
#include "tdxw.h"
#include "QjStatDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQjStatDlg dialog


CQjStatDlg::CQjStatDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQjStatDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CQjStatDlg)
	m_strAmount = _T("");
	m_strClose = _T("");
	m_strHsl = _T("");
	m_strMax = _T("");
	m_strMin = _T("");
	m_strOpen = _T("");
	m_strTime1 = _T("");
	m_strValue = _T("");
	m_strVolume = _T("");
	m_strJunJa = _T(""); 
	m_strZhangFu = _T("");
	m_strZhenFu = _T("");
	m_strDataNum = _T("");
	m_time1 = 0;
	m_time2 = 0;
	//}}AFX_DATA_INIT
	m_bPhaseEnable = FALSE;
	////
	m_pStkInfo = NULL;
	m_pAnalyData = NULL;
	m_nPeriod = PER_DAY;
	m_nItemNum = m_nFirstPos = m_nEndPos = 0;
	m_bZs = m_bQz = FALSE;
}


void CQjStatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQjStatDlg)
	DDX_Text(pDX, IDC_AMOUNT, m_strAmount);
	DDX_Text(pDX, IDC_CLOSE, m_strClose);
	DDX_Text(pDX, IDC_HSL, m_strHsl);
	DDX_Text(pDX, IDC_MAX, m_strMax);
	DDX_Text(pDX, IDC_MIN, m_strMin);
	DDX_Text(pDX, IDC_OPEN, m_strOpen);
	DDX_Text(pDX, IDC_TIME1, m_strTime1);
	DDX_Text(pDX, IDC_VALUE, m_strValue);
	DDX_Text(pDX, IDC_VOLUME, m_strVolume);
	DDX_Text(pDX, IDC_AVERAGE, m_strJunJa);
	DDX_Text(pDX, IDC_ZANGFU, m_strZhangFu);
	DDX_Text(pDX, IDC_ZHENFU, m_strZhenFu);
	DDX_Text(pDX, IDC_DATANUM, m_strDataNum);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_time1);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER2, m_time2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQjStatDlg, CDialog)
	//{{AFX_MSG_MAP(CQjStatDlg)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER1, OnDatetimechangeDatetimepicker)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER2, OnDatetimechangeDatetimepicker)
	ON_BN_CLICKED(IDC_RMREPORT, OnBoardsort)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQjStatDlg message handlers

void CQjStatDlg::SetDataAndPeriod(MemStkInfo *pStkInfo,struct AnalyData *pData,int ItemNum,int nFirstPos,int nEndPos,BYTE Period)
{
	m_pStkInfo = pStkInfo;
	m_nPeriod = Period;
	m_pAnalyData = pData;
	m_nItemNum = ItemNum;
	m_nFirstPos = nFirstPos;
	m_nEndPos = nEndPos;

	if(m_nPeriod==PER_DAY || m_nPeriod==PER_WEEK || m_nPeriod==PER_MONTH || m_nPeriod==PER_DAYN || m_nPeriod==PER_SEASON || m_nPeriod==PER_YEAR)
		m_bPhaseEnable = TRUE;
	else
		m_bPhaseEnable = FALSE;
}

float CQjStatDlg::GetOneHsl(long lDate,float tmpVolume,int vol_factor)
{
	float fRealLTGB = 0,fRealZGB = 0;
	g_pStockIO->ReadLTGB_ZGB_OneDate(m_pStkInfo,lDate, &fRealLTGB, &fRealZGB, m_pStkInfo->ActiveCapital, m_pStkInfo->J_zgb);
	float tmpHsl = 0;
	if(!m_bZs && fRealLTGB > 1)
		tmpHsl = CalcHSL(m_pStkInfo,tmpVolume*vol_factor,fRealLTGB,m_nPeriod);
	return tmpHsl;
}
	
void CQjStatDlg::GetData(int FirstPos,int nEndPos)
{
	int itmp = GetStockType(m_pStkInfo->setcode,m_pStkInfo->Code);
	m_bZs = testzs(m_pStkInfo);
	m_bQz = ((itmp==CODE_SHQZ) || (itmp==CODE_SZQZ));

	ANALYDATA		Analy;
	float			tmpFrontClose = COMPPREC; //前收盘
	memcpy(&Analy,m_pAnalyData+FirstPos,
		sizeof(ANALYDATA));
	if(FirstPos >= 0)
		tmpFrontClose = m_pAnalyData[FirstPos-1].Close;
	if(tmpFrontClose <= COMPPREC)
		tmpFrontClose = Analy.Open;
	int datanum = 1;
	int vol_factor = 1;
	float tmpVolume=1.0*Analy.Volume;
	float tmpHsl = GetOneHsl(Analy.Time.Date,tmpVolume,vol_factor);
	if(abs(nEndPos-FirstPos) > 40 && (m_nPeriod == PER_DAY || m_nPeriod == PER_WEEK || m_nPeriod == PER_MONTH || m_nPeriod == PER_DAYN|| m_nPeriod == PER_SEASON|| m_nPeriod == PER_YEAR))
	{
		vol_factor = 1000; //如果累计的量太大，会打破long,故用vol_factor
		tmpVolume = tmpVolume*1.0/vol_factor;
		tmpHsl = GetOneHsl(Analy.Time.Date,tmpVolume,vol_factor);
	}
	for ( int i = FirstPos+1;i <= nEndPos; ++i )
	{
		if ( Analy.High < m_pAnalyData[i].High )
			Analy.High = m_pAnalyData[i].High;
		if ( Analy.Low  > m_pAnalyData[i].Low )
			Analy.Low  = m_pAnalyData[i].Low;
		Analy.Amount += m_pAnalyData[i].Amount;
		tmpVolume += m_pAnalyData[i].Volume*1.0/vol_factor;
		tmpHsl += GetOneHsl(m_pAnalyData[i].Time.Date,m_pAnalyData[i].Volume,1);
		datanum++;
	}
	Analy.Close = m_pAnalyData[i-1].Close;
	switch ( m_nPeriod )
	{
	case PER_MIN1:	
	case PER_MIN5:	
	case PER_MIN15:
	case PER_MIN30:
	case PER_HOUR:	
	case PER_MINN:	
		_snprintf(m_Info.time1,20,"%2u/%02u/%02u:%02u",
			(m_pAnalyData+FirstPos)->Time.Daye.Day/100,
			(m_pAnalyData+FirstPos)->Time.Daye.Day%100,
			(m_pAnalyData+FirstPos)->Time.Daye.Minute/60,
			(m_pAnalyData+FirstPos)->Time.Daye.Minute%60);
		_snprintf(m_Info.value,20,"%2u/%02u/%02u:%02u",
			(m_pAnalyData+nEndPos)->Time.Daye.Day/100,
			(m_pAnalyData+nEndPos)->Time.Daye.Day%100,
			(m_pAnalyData+nEndPos)->Time.Daye.Minute/60,
			(m_pAnalyData+nEndPos)->Time.Daye.Minute%60);
		break;
	case PER_DAY:
	case PER_WEEK:
	case PER_MONTH:
	case PER_DAYN:	
	case PER_SEASON:
	case PER_YEAR:
		_snprintf(m_Info.time1,20,"%u/%02u/%02u",
			(m_pAnalyData+FirstPos)->Time.Date/10000,
			((m_pAnalyData+FirstPos)->Time.Date%10000)/100,
			(m_pAnalyData+FirstPos)->Time.Date%100);
		_snprintf(m_Info.value,20,"%u/%02u/%02u",
			(m_pAnalyData+nEndPos)->Time.Date/10000,
			((m_pAnalyData+nEndPos)->Time.Date%10000)/100,
			(m_pAnalyData+nEndPos)->Time.Date%100);
		break;
	default:
		break;
	}
	////////////////////////////////////////////////////////////////////
	m_Info.datanum = datanum;
	_snprintf(m_Info.name,20,"%s",m_pStkInfo->Name);
	if(GetXSFlag(m_pStkInfo)!=3)
	{
		_snprintf(m_Info.open,20,"%9.2f",Analy.Open);
		_snprintf(m_Info.max,20,"%9.2f",Analy.High);
		_snprintf(m_Info.close,20,"%9.2f",Analy.Close);
		_snprintf(m_Info.min,20,"%9.2f",Analy.Low);
		if(tmpVolume>0)
			_snprintf(m_Info.junja,20,"%9.2f",(Analy.Amount/tmpVolume)/vol_factor);
		else
			m_Info.junja[0] = 0;
	}
	else
	{
		_snprintf(m_Info.open,20,"%9.3f",Analy.Open);
		_snprintf(m_Info.max,20,"%9.3f",Analy.High);
		_snprintf(m_Info.close,20,"%9.3f",Analy.Close);
		_snprintf(m_Info.min,20,"%9.3f",Analy.Low);
		if(tmpVolume>0)
		{
			if(m_bQz)
			{
				if(m_nPeriod==PER_DAY)
					_snprintf(m_Info.junja,20,"%9.3f",(Analy.Amount/(tmpVolume*m_pStkInfo->Unit))/vol_factor);
				else
					m_Info.junja[0] = 0;
			}
			else
				_snprintf(m_Info.junja,20,"%9.3f",(Analy.Amount/tmpVolume)/vol_factor);
		}
		else
			m_Info.junja[0] = 0;
	}
	
	if(m_bZs)	m_Info.junja[0] = 0; //指数不能统计均价

	strcpy(m_Info.amount,MakeJE(Analy.Amount));
	if(!m_bZs)
	{
		if(need_justcjl(m_pStkInfo->setcode,m_pStkInfo->Code))
			_snprintf(m_Info.volume,20,"%.f",(tmpVolume/m_pStkInfo->Unit)*vol_factor);
		else
			_snprintf(m_Info.volume,20,"%.f",tmpVolume*vol_factor);
	}
	else
	{
		if(m_nPeriod==PER_DAY||m_nPeriod==PER_WEEK||m_nPeriod==PER_MONTH||m_nPeriod==PER_DAYN||m_nPeriod==PER_SEASON||m_nPeriod==PER_YEAR)
			_snprintf(m_Info.volume,20,"%u万",(long)((tmpVolume/10000)*vol_factor));
		else
			m_Info.volume[0]=0;
	}
	_snprintf(m_Info.hsl,20,"%6.2f%%",tmpHsl);
	if(tmpFrontClose>COMPPREC)
	{
		if(GetXSFlag(m_pStkInfo)!=3)
			_snprintf(m_Info.zhangdie,20,"%6.2f",(Analy.Close-tmpFrontClose));
		else
			_snprintf(m_Info.zhangdie,20,"%6.3f",(Analy.Close-tmpFrontClose));
		_snprintf(m_Info.zhangfu,20,"%6.2f%%",100*(Analy.Close-tmpFrontClose)/tmpFrontClose);
	}
	else
	{
		m_Info.zhangdie[0]=0;
		m_Info.zhangfu[0]=0;
	}
	if(Analy.Low>COMPPREC)
		_snprintf(m_Info.zhenfu,20,"%6.2f%%",100*(Analy.High-Analy.Low)/Analy.Low);		//Analy.Close -> Analy.Low
	else
		m_Info.zhenfu[0]=0;
	////////////////////////////////////////////////////////////////////
	if(m_bPhaseEnable)
	{
		GetDlgItem(IDC_DATETIMEPICKER1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DATETIMEPICKER2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_TIME1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_VALUE)->ShowWindow(SW_HIDE);
		
//		m_time1 = LongToTime((m_pAnalyData+FirstPos)->Time.Date);
//		m_time2 = LongToTime((m_pAnalyData+nEndPos)->Time.Date);
	}
	else
	{
		GetDlgItem(IDC_DATETIMEPICKER1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DATETIMEPICKER2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TIME1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_VALUE)->ShowWindow(SW_SHOW);
	}
}

void CQjStatDlg::ShowIt()
{
	SetWindowText(m_pStkInfo->Name+(CString)(_F(" 区间统计")));
	m_strTime1=m_Info.time1;
	m_strValue=m_Info.value;
	m_strOpen=m_Info.open;
	m_strMax=m_Info.max;
	m_strMin=m_Info.min;
	m_strClose=m_Info.close;
	m_strVolume=m_Info.volume;
	m_strJunJa=m_Info.junja;
	m_strAmount=m_Info.amount;
	m_strHsl=m_Info.hsl;
	AllTrim(m_Info.zhangdie);
	AllTrim(m_Info.zhangfu);
	if(m_Info.zhangdie[0])
		m_strZhangFu.Format("%s(%s)",m_Info.zhangdie,m_Info.zhangfu);
	else
		m_strZhangFu="";
	m_strZhenFu=m_Info.zhenfu;
	m_strDataNum=IntToStr(m_Info.datanum)+_F("个");

	GetDlgItem(IDC_PHASESORT)->EnableWindow(m_bPhaseEnable);
	GetDlgItem(IDC_RMREPORT)->EnableWindow(m_bPhaseEnable);

	UpdateData(FALSE);
}

BOOL CQjStatDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetDateCtrlRange(GetDlgItem(IDC_DATETIMEPICKER1));
	SetDateCtrlRange(GetDlgItem(IDC_DATETIMEPICKER2));
	
	if(m_pStkInfo && m_pAnalyData && m_nItemNum>1)
	{
		GetData(m_nFirstPos,m_nEndPos);
		if(m_bPhaseEnable) //第一次应该将m_time1等赋值
		{			
			m_time1 = LongToTime((m_pAnalyData+m_nFirstPos)->Time.Date);
			m_time2 = LongToTime((m_pAnalyData+m_nEndPos)->Time.Date);
		}
		ShowIt();
	}
	
	return TRUE;
}


void CQjStatDlg::OnDatetimechangeDatetimepicker(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData(TRUE);
	
	if(m_pStkInfo && m_pAnalyData && m_nItemNum>1)
	{
		long t1=TimeToLong(m_time1),t2=TimeToLong(m_time2);
		int nFirstPos = -1,nEndPos = -1;
		for(int i=0; i < m_nItemNum;i++)
		{
			if(nFirstPos==-1 && m_pAnalyData[i].Time.Date>=t1)
				nFirstPos = i;
			if(nFirstPos!=-1 && m_pAnalyData[i].Time.Date==t2)
			{
				nEndPos = i;
				break;
			}
			else if(nFirstPos!=-1 && m_pAnalyData[i].Time.Date>t2)
			{
				if(i>0)
				{
					nEndPos = i-1;
					break;
				}
				else 
				{
					nEndPos = nFirstPos;
					break;
				}
			}
		}
		if(nFirstPos==-1)
		{
			m_time1=LongToTime(m_pAnalyData[0].Time.Date);
			nFirstPos = 0;
			UpdateData(FALSE);
		}
		if(nEndPos==-1)
		{
			m_time2=LongToTime(m_pAnalyData[m_nItemNum-1].Time.Date);
			nEndPos = m_nItemNum-1;
			UpdateData(FALSE);
		}
		if(nFirstPos > nEndPos || t1>t2)
		{
			m_time1=LongToTime(m_pAnalyData[nEndPos].Time.Date);
			m_time2=m_time1;
			nFirstPos = nEndPos;
			UpdateData(FALSE);
		}
		m_nFirstPos = nFirstPos;
		m_nEndPos = nEndPos;

		GetData(m_nFirstPos,m_nEndPos);
		ShowIt();
	}

	*pResult = 0;
}

void CQjStatDlg::OnBoardsort() 
{
	UpdateData(TRUE);
	CDialog::OnOK();
}
