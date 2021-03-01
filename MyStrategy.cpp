// MyStrategy.cpp: implementation of the MyStrategy class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "tdxw.h"
#include "MyStrategy.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const long g_nCancel=-888888;
typedef void (MyStrategy::*proc)(void); 

CalcLine::Serialize(CArchive &ar)
{
	if(ar.IsStoring())
	{
		ar<<lineType;
		ar<<lineIndex;
		ar<<lineValue;
	}
	else
	{
		ar>>lineType;
		ar>>lineIndex;
		ar>>lineValue;
	}
}

SingleCon::Serialize(CArchive &ar)            
{
	int nOtherLine;
	if(ar.IsStoring())
	{
		ar<<mainType;
		ar<<indexNo;
		ar<<procIndex;
		mainLine.Serialize(ar);
		nOtherLine=otherLines.size();
		ar<<nOtherLine;
		ar<<endDate;		//不save日期
		for(int i=0;i<nOtherLine;i++)
		{
			otherLines[i].Serialize(ar);
		}
		ar<<sList;
	}
	else
	{
		ar>>mainType;
		ar>>indexNo;
		ar>>procIndex;
		mainLine.Serialize(ar);
		ar>>nOtherLine;
		ar>>endDate;	//不load日期
	//	endDate = g_lOpenRq;		//用当前日期
		for(int i=0;i<nOtherLine;i++)
		{
			CalcLine lineToLoad;
			lineToLoad.Serialize(ar);
			otherLines.push_back(lineToLoad);
		}
		ar>>sList;
	}
}										

static proc UConProc[]= {&MyStrategy::CalcEqual, &MyStrategy::CalcGreater,&MyStrategy::CalcLess, NULL, NULL, NULL,
&MyStrategy::CalcUpCross, &MyStrategy::CalcDnCross, &MyStrategy::CalcUp, &MyStrategy::CalcDn,&MyStrategy::CalcTurnUp, &MyStrategy::CalcTurnDn};
static proc KSProc[]={&MyStrategy::CalcMaxKS, &MyStrategy::CalcTopKS};
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
MyStrategy::MyStrategy()
{
	m_nConGap=0;
	m_nEndGap=0;
	m_nCalcPeriod=DAY_ST;
	m_nCon=0;
	for(int i=0;i<MAX_TYPE;i++)
		m_bHasType[i] = 0;
	m_bSort=FALSE;
	m_pRecentData=NULL;
	m_pCurGpData=NULL;
	m_fDataLines=NULL;
	m_aTransData=NULL;
	m_transData=NULL;
	m_finalOut=NULL;

	m_lMyReadNum = 0;
	m_lReadNum = 0;
	m_lNeedNum = 1000;
	m_nDataNum = 0;
	m_bTQ = TRUE;
}

MyStrategy::~MyStrategy()
{
	
}

void MyStrategy::SetTQFlag(BOOL bTQ)
{
	m_bTQ = bTQ;
}

///////////////////////////////////////////////////////////////////////
CTime MyStrategy::ParseCalcDate()
{
	int nPlans=m_curPlans.myCons.size();
	long nearTime=-1;
	CTime endTime;
	for(int i=0;i<nPlans;i++)
	{
		if(m_curPlans.myCons[i].endDate>nearTime)
			nearTime=max(nearTime, m_curPlans.myCons[i].endDate);
	}
	if(nearTime>-1)
	{
		int endyear=nearTime/10000;
		int endmonth=(nearTime-10000*endyear)/100;
		int endday=nearTime-10000*endyear-100*endmonth;
		return CTime(endyear, endmonth, endday, 0, 0, 0);
	}
	return CTime();
}

void MyStrategy::InputCalcDataTime(NTime &time1,NTime &time2, int nPeriod)
{
	memcpy(&m_startTime, &time1, sizeof(NTime));
	memcpy(&m_endTime, &time2, sizeof(NTime));
	m_nCalcPeriod = nPeriod;
}

BOOL MyStrategy::EqualGreaterDate(NTime &validDate, NTime &endDate)
{
	long lValidDate = 10000*validDate.year+100*validDate.month+validDate.day;
	long lEndDate = 10000*endDate.year+100*endDate.month+endDate.day;
	return lValidDate>lEndDate;
}

BOOL MyStrategy::EqualLessDate(NTime &validDate, NTime &endDate)
{
	long lValidDate = 10000*validDate.year+100*validDate.month+validDate.day;
	long lEndDate = 10000*endDate.year+100*endDate.month+endDate.day;
	return lValidDate<=lEndDate;
}

void MyStrategy::CalcEndGap()
{
	long lReadNum = g_pCalc->_CallDTopViewCalc_4(SH_DPCODE,SH,CalcPeriodToTdxPeriod(m_nCalcPeriod), ASK_ALL,m_bTQ?1:0);
	LPHISDAT pAnalyData = (LPHISDAT)g_pCalc->_CallDTopViewCalc_15();
	if (pAnalyData == NULL||lReadNum <= 0) return;
	for(int i=0;i<lReadNum&&EqualGreaterDate(pAnalyData[lReadNum-1-i].Time, m_endTime);i++);
	if(i<lReadNum) m_nEndGap=i;
	else m_nEndGap=0;
}

void MyStrategy::CalcConGap(long nearTime)
{
	int endyear=nearTime/10000;
	int endmonth=(nearTime-10000*endyear)/100;
	int endday=nearTime-10000*endyear-100*endmonth;
    NTime endTime;
	memset(&endTime, 0, sizeof(NTime));
	endTime.year=endyear;endTime.month=endmonth;endTime.day=endday;
	int i, gap=0;
	for(i=0;i<m_lMyReadNum;i++,gap++)
	{
		if(EqualLessDate(m_pAnalyData[m_lMyReadNum-1-i].Time, endTime))
		{
			m_nConGap=gap;
			return;
		}
	}
}

void MyStrategy::SetCurGpIndex(int curGpIndex)
{
	m_curGPIndex=curGpIndex;
}

void MyStrategy::ParseType()
{
	ASSERT(m_nCon==m_curPlans.myCons.size());
	for(int i=0;i<m_nCon;i++)
		m_bHasType[m_curPlans.myCons[i].mainType] = TRUE;
}

BOOL MyStrategy::GetMyAnalData()
{
	m_pCurGpInfo=(*g_pStockIO)[m_curGPIndex];
	if(!m_pCurGpInfo) return FALSE;
	TSET_NULL(m_fDataLines, 0, 2*m_lNeedNum);
	memset(m_aTransData, 0, m_nCon*m_lNeedNum*sizeof(long));
	memset(m_transData, 0, m_lNeedNum*sizeof(long));
	memset(m_finalOut, 0, m_lNeedNum*sizeof(long));
///////////////////////////////////////////////////////////历史数据
	if(m_bHasType[ZB_SEL]||m_bHasType[TJ_SEL]||m_bHasType[CWHQ_SEL]||m_bHasType[KL_TYPE]||m_bHasType[STYLE_SEL])
	{
		TCHAR crCurGpCode[10];
		short crCurSetCode;
		strcpy(crCurGpCode, m_pCurGpInfo->Code);
		crCurSetCode = m_pCurGpInfo->setcode;
		m_lMyReadNum = g_pCalc->_CallDTopViewCalc_4(crCurGpCode,crCurSetCode, CalcPeriodToTdxPeriod(m_nCalcPeriod), m_startTime, m_endTime,m_bTQ?1:0);
		m_lMyReadNum = min(m_lReadNum,m_lMyReadNum);
		m_pAnalyData = (LPHISDAT)g_pCalc->_CallDTopViewCalc_15();
		if (m_pAnalyData == NULL||m_lMyReadNum < MAX_NEED_DATA) return FALSE;
	}
///////////////////////////////////////////////////////////基本信息行情数据
	if(m_bHasType[CWHQ_SEL]&&m_bHasType[STYLE_SEL])
	{
		int		nTodayDataNum;
		TDEL(m_pCurGpData);
		TDEL(m_pRecentData);
		m_pCurGpData = new CurrStockData;
		m_pRecentData = new AnalyData[m_lMyReadNum+m_nEndGap];
		memset(m_pCurGpData, 0, sizeof(CurrStockData));
		memset(m_pRecentData, 0, (m_lMyReadNum+m_nEndGap)*sizeof(AnalyData));
		m_nDataNum = g_pStockIO->ReadAnalyDataAndCurrHQ(m_pCurGpInfo,m_pRecentData,m_lMyReadNum+m_nEndGap,PER_DAY,m_pCurGpData,nTodayDataNum,GetMulNum(PER_DAY),1);
		ASSERT(m_lReadNum>=m_lMyReadNum);
		g_pStockIO->TQData(m_pCurGpInfo,m_pRecentData,0,m_nDataNum-1,m_nDataNum,1,PER_DAY);
	}
	if(m_bHasType[CWHQ_SEL]&&!m_bHasType[STYLE_SEL])
	{
		TDEL(m_pCurGpData);
		m_pCurGpData = new CurrStockData;
		memset(m_pCurGpData, 0, sizeof(CurrStockData));
		g_pStockIO->ReadCurrStockData(m_pCurGpInfo, m_pCurGpData);
	}

	return TRUE;
}

void MyStrategy::InitAllData()
{
	m_lReadNum = g_pCalc->_CallDTopViewCalc_4(SH_DPCODE,SH,CalcPeriodToTdxPeriod(m_nCalcPeriod), m_startTime, m_endTime,m_bTQ?1:0)+500;
	m_lNeedNum = max(m_lReadNum,1000);
	m_fDataLines = new float[2*m_lNeedNum];
	TSET_NULL(m_fDataLines, 0, 2*m_lNeedNum);
	m_nCon=m_curPlans.myCons.size();
   	m_aTransData = new long[m_nCon*m_lNeedNum];
	memset(m_aTransData, 0, m_nCon*m_lNeedNum*sizeof(long));
	m_transData = new long[m_lNeedNum];
	memset(m_transData, 0, m_lNeedNum*sizeof(long));
	m_finalOut = new long[m_lNeedNum];
	memset(m_finalOut, 0, m_lNeedNum*sizeof(long));
}

void MyStrategy::UnInitAllData()
{
	if(m_pCurGpData) 
	{
		delete m_pCurGpData;m_pCurGpData=NULL;
	}
	if(m_pRecentData) 
	{
		delete []m_pRecentData;m_pRecentData=NULL;
	}
	delete []m_fDataLines;m_fDataLines=NULL;
	delete []m_aTransData;m_aTransData=NULL;
	delete []m_transData;m_transData=NULL;
	delete []m_finalOut;m_finalOut=NULL;

	if(!m_iSort.empty())
	{
		ASSERT(m_iSort.size()==m_aMapSort.size());
		for(int i=0;i<m_aMapSort.size();i++)
			m_aMapSort[i].clear();
	//	m_aMapSort.clear();
	}
}

void MyStrategy::PutCurTransDataToCalc(int nCurCon)
{
	memcpy(m_transData, &m_aTransData[nCurCon*m_lReadNum], m_lReadNum*sizeof(long));
	if(m_bSort) 
	{
		for(int i=0;i<m_iSort.size();i++)
		{
			if(nCurCon==m_iSort[i])
			{
				m_mapSort = &(m_aMapSort[i]);
				break;
			}
		}
	}
}

void MyStrategy::PutCurTransDataBack(int nCurCon)
{
	memcpy(&m_aTransData[nCurCon*m_lReadNum]+m_nConGap, m_transData, (m_lMyReadNum-m_nConGap)*sizeof(long));
}

BOOL MyStrategy::InputCurConData(SingleCon curUCon)
{	
	int i;
	float *pOutData[MAX_LINEOUT];
	if(curUCon.mainType==ZB_SEL||curUCon.mainType==TJ_SEL||curUCon.mainType==KL_SEL)
	{
		TINDEXINFO	*tmp;
		tmp = g_pCalc->_CallDTopViewCalc_8(curUCon.mainType, curUCon.indexNo);
		if (tmp == NULL) return FALSE;
		for (i=0; i<MAX_LINEOUT;i++) pOutData[i] = g_pCalc->_CallDTopViewCalc_13(m_lMyReadNum);
		if ( 0 != g_pCalc->_CallDTopViewCalc_5(curUCon.mainType, curUCon.indexNo, pOutData))	//计算不成功
		{
			for (i=0; i<MAX_LINEOUT;i++) g_pCalc->_CallDTopViewCalc_14(pOutData[i]);
			return FALSE;
		}
	}
	float *aTmpData;
	////填充主线
	aTmpData=m_fDataLines;
	switch(curUCon.mainLine.lineType)
	{
	case K_LINE:
		if(m_pAnalyData==NULL) return FALSE;
		if(curUCon.mainLine.lineIndex==0)
		{
			for(i=m_lMyReadNum-1;i>=m_lMyReadNum-m_nConGap-MAX_NEED_DATA;i--)
			{
				if(m_pAnalyData[i].High<COMPPREC) TSET_MEANLESS(aTmpData[i]);
				else aTmpData[i]=m_pAnalyData[i].High;
			}
		}
		if(curUCon.mainLine.lineIndex==1)
		{
			for(i=m_lMyReadNum-1;i>=m_lMyReadNum-m_nConGap-MAX_NEED_DATA;i--)
			{
				if(m_pAnalyData[i].Open<COMPPREC) TSET_MEANLESS(aTmpData[i]);
				else aTmpData[i]=m_pAnalyData[i].Open;
			}
		}
		if(curUCon.mainLine.lineIndex==2)
		{
			for(i=m_lMyReadNum-1;i>=m_lMyReadNum-m_nConGap-MAX_NEED_DATA;i--)
			{
				if(m_pAnalyData[i].Low<COMPPREC) TSET_MEANLESS(aTmpData[i]);
				else aTmpData[i]=m_pAnalyData[i].Low;
			}
		}
		if(curUCon.mainLine.lineIndex==3)
		{
			for(i=m_lMyReadNum-1;i>=m_lMyReadNum-m_nConGap-MAX_NEED_DATA;i--)
			{
				if(m_pAnalyData[i].Close<COMPPREC) TSET_MEANLESS(aTmpData[i]);
				else aTmpData[i]=m_pAnalyData[i].Close;
			}
		}
		break;
	case CALC_LINE:
		memcpy(aTmpData+m_lMyReadNum-m_nConGap-MAX_NEED_DATA, pOutData[curUCon.mainLine.lineIndex]+m_lMyReadNum-m_nConGap-MAX_NEED_DATA, (MAX_NEED_DATA+m_nConGap)*sizeof(float));
		break;
	case STATIC_LINE:
		if(m_pCurGpInfo==NULL||m_pCurGpData==NULL) return FALSE;
		float tmpValue, ftmp;
		TSET_MEANLESS(tmpValue);TSET_MEANLESS(ftmp);
		int ii;
		switch(curUCon.indexNo)
		{
		case C_ZGB:		tmpValue = m_pCurGpInfo->J_zgb;break;
		case C_LTB:		tmpValue = m_pCurGpInfo->ActiveCapital;break;
			
		case C_BG:		tmpValue = m_pCurGpInfo->J_bg;break;
		case C_HG:		tmpValue = m_pCurGpInfo->J_hg;break;
			
		case C_ZSZ:		tmpValue = (m_pCurGpInfo->J_zgb-m_pCurGpInfo->J_hg)*m_pCurGpData->Now;break;
		case C_LTSZ:	tmpValue = m_pCurGpInfo->ActiveCapital*m_pCurGpData->Now;break;
		case C_ZZC:		tmpValue = m_pCurGpInfo->J_zzc;break;
		case C_LDZC:	tmpValue = m_pCurGpInfo->J_ldzc;break;
			
		case C_wxzc:	tmpValue = m_pCurGpInfo->J_wxzc;break;	//无形资产
		case C_cqtz:	tmpValue = m_pCurGpInfo->J_cqtz;break;	//长期投资
		case C_ldfz:	tmpValue = m_pCurGpInfo->J_ldfz;break;	//流动负债
		case C_cqfz:	tmpValue = m_pCurGpInfo->J_cqfz;break;	//长期负债
		case C_zbgjj:	tmpValue = m_pCurGpInfo->J_zbgjj;break;	//资本公积金
		case C_yszk:	tmpValue = m_pCurGpInfo->J_yszk;break;	//应收帐款
		case C_yyly:	tmpValue = m_pCurGpInfo->J_yyly;break;	//营业利益
		case C_tzsy:	tmpValue = m_pCurGpInfo->J_tzsy;break;	//投资收益
		case C_jyxjl:	tmpValue = m_pCurGpInfo->J_jyxjl;break;	//经营现金净流量
		case C_zxjl:	tmpValue = m_pCurGpInfo->J_zxjl;break;	//总现金净流量
		case C_ch:		tmpValue = m_pCurGpInfo->J_ch;break; //存货
		case C_jly:		tmpValue = m_pCurGpInfo->J_jly;break;	//净利益
		case C_wfply:	tmpValue = m_pCurGpInfo->J_wfply;break;	//未分配利益
			
		case C_JZC:		tmpValue = m_pCurGpInfo->J_jzc;break;
		case C_MGJZC:	
			if(m_pCurGpInfo->J_zgb > 1 && fabs(m_pCurGpInfo->J_jzc)>COMPPREC)
				tmpValue = m_pCurGpInfo->J_jzc/m_pCurGpInfo->J_zgb;
			else
				tmpValue = m_pCurGpInfo->J_tzmgjz;
			break;
		case C_GDQYB:	
			if( !testzs(m_pCurGpInfo) && m_pCurGpInfo->J_zzc > 1)
			{
				tmpValue = m_pCurGpInfo->J_jzc*100.0/m_pCurGpInfo->J_zzc;
				break;
			}
			TSET_MEANLESS(tmpValue);
			break;
		case C_ZBGJJ:  tmpValue = m_pCurGpInfo->J_zbgjj;break;
		case C_MGGJJ:  
			if(m_pCurGpInfo->J_zgb > 1)
				tmpValue = m_pCurGpInfo->J_zbgjj/m_pCurGpInfo->J_zgb;
			else
				TSET_MEANLESS(tmpValue);
			break;
		case C_MGWFP: //每股未分配
			if(m_pCurGpInfo->J_zgb > 1)
				tmpValue = m_pCurGpInfo->J_wfply/m_pCurGpInfo->J_zgb;
			else
				TSET_MEANLESS(tmpValue);
			break;
		case C_MGSY:   
			if(m_pCurGpInfo->J_zgb > 1)
				tmpValue = m_pCurGpInfo->J_jly/m_pCurGpInfo->J_zgb;
			else
				TSET_MEANLESS(tmpValue);
			break;
		case C_ZYSY:	tmpValue = m_pCurGpInfo->J_zysy;break;
		case C_LYZE:	tmpValue = m_pCurGpInfo->J_lyze;break;
		case C_SHLY:	tmpValue = m_pCurGpInfo->J_shly;break;
		case C_JYL:		
			if( !testzs(m_pCurGpInfo) && m_pCurGpInfo->J_jzc > 1 )
			{
				tmpValue = m_pCurGpInfo->J_jly/m_pCurGpInfo->J_jzc*100.0;
				break;
			}
			TSET_MEANLESS(tmpValue);
			break;
		case C_NOW:
			if(m_nConGap>0||m_nEndGap>0) tmpValue = m_pAnalyData[m_lMyReadNum-1-m_nConGap].Close;
			else tmpValue = m_pCurGpData->Now;
			if(tmpValue < COMPPREC) TSET_MEANLESS(tmpValue); //停板不算
			break;
			
		case C_MAX:
			if(m_nConGap>0||m_nEndGap>0) tmpValue = m_pAnalyData[m_lMyReadNum-1-m_nConGap].High;
			else tmpValue = m_pCurGpData->Max;
			if(tmpValue < COMPPREC) TSET_MEANLESS(tmpValue); //停板不算
			break;
			
		case C_MIN:
			if(m_nConGap>0||m_nEndGap>0) tmpValue = m_pAnalyData[m_lMyReadNum-1-m_nConGap].Low;
			else tmpValue = m_pCurGpData->Min;
			if(tmpValue < COMPPREC) TSET_MEANLESS(tmpValue); //停板不算
			break;
			
		case C_OPEN:
			if(m_nConGap>0||m_nEndGap>0) tmpValue = m_pAnalyData[m_lMyReadNum-1-m_nConGap].Open;
			else tmpValue = m_pCurGpData->Open;
			if(tmpValue < COMPPREC) TSET_MEANLESS(tmpValue); //停板不算
			break;
			
		case C_CLOSE:
			if(m_nConGap>0||m_nEndGap>0) tmpValue = m_pAnalyData[m_lMyReadNum-1-m_nConGap].Close;
			else tmpValue = m_pCurGpData->Close;
			if(tmpValue < COMPPREC) TSET_MEANLESS(tmpValue); //停板不算
			break;
			
		case C_VOLUME:
			if(m_nConGap>0||m_nEndGap>0) tmpValue = m_pAnalyData[m_lMyReadNum-1-m_nConGap].fVolume;
			else tmpValue = m_pCurGpData->Volume;
			if(tmpValue < COMPPREC) TSET_MEANLESS(tmpValue); //停板不算
			break;
			
		case C_AMOUNT:
			if(m_nConGap>0||m_nEndGap>0) tmpValue = m_pAnalyData[m_lMyReadNum-1-m_nConGap].Amount;
			else tmpValue = m_pCurGpData->Amount;
			if(tmpValue < COMPPREC) TSET_MEANLESS(tmpValue); //停板不算
			break;
			
		case C_ZAF:
			if(m_nConGap>0||m_nEndGap>0)
			{
				if(m_pAnalyData[m_lMyReadNum-2-m_nConGap].Close>COMPPREC)
				{
					if(m_pAnalyData[m_lMyReadNum-1-m_nConGap].Close<COMPPREC) ftmp = 0.0;
					else ftmp = m_pAnalyData[m_lMyReadNum-1-m_nConGap].Close - m_pAnalyData[m_lMyReadNum-2-m_nConGap].Close;
					tmpValue = ftmp/m_pAnalyData[m_lMyReadNum-1-m_nConGap].Close*(float)100.0;
					break;
				}
				else TSET_MEANLESS(tmpValue); break; 
			}
			else
			{
				if (m_pCurGpData->Close>COMPPREC)
				{	
					if ( m_pCurGpData->Now < COMPPREC )	ftmp = 0.0;
					else					ftmp = m_pCurGpData->Now - m_pCurGpData->Close;
					tmpValue = ftmp/m_pCurGpData->Close*(float)100.0;
					break;
				}
				else TSET_MEANLESS(tmpValue); break;
			}
			
		case C_ZEF:
			if(m_nConGap>0||m_nEndGap>0)
			{
				if(m_pAnalyData[m_lMyReadNum-1-m_nConGap].Close>COMPPREC)
				{
					if( m_pAnalyData[m_lMyReadNum-1-m_nConGap].High>COMPPREC && m_pAnalyData[m_lMyReadNum-1-m_nConGap].Low>COMPPREC)
						ftmp = m_pAnalyData[m_lMyReadNum-1-m_nConGap].High-m_pAnalyData[m_lMyReadNum-1-m_nConGap].Low;
					else 
						ftmp = 0.0;
					tmpValue = ftmp/m_pAnalyData[m_lMyReadNum-1-m_nConGap].Close*100.0;
					break;
				}
				else TSET_MEANLESS(tmpValue); break;
			}
			else
			{
				if (m_pCurGpData->Close>COMPPREC)
				{
					if ( m_pCurGpData->Max > COMPPREC && m_pCurGpData->Min > COMPPREC)
						ftmp = m_pCurGpData->Max - m_pCurGpData->Min;
					else
						ftmp = 0.0;
					tmpValue = ftmp/m_pCurGpData->Close*100.0;
					break;
				}
				else TSET_MEANLESS(tmpValue); break;
			}
		case C_SYL:													//市盈率
			if( !testzs(m_pCurGpInfo) && m_pCurGpInfo->J_zgb > 1)
			{
				ftmp = (m_pCurGpInfo->J_jly*12/m_pCurGpInfo->J_HalfYearFlag)/m_pCurGpInfo->J_zgb;
				if(ftmp > COMPPREC && m_pCurGpData->Now > COMPPREC && m_pAnalyData[m_lMyReadNum-1-m_nConGap].Close > COMPPREC)
				{
					if(m_nConGap>0||m_nEndGap>0) tmpValue = m_pAnalyData[m_lMyReadNum-1-m_nConGap].Close/ftmp;
					else tmpValue = m_pCurGpData->Now/ftmp;
					break;
				}
			}
			else TSET_MEANLESS(tmpValue); break;			
		case C_HSL:													
			if( !testzs(m_pCurGpInfo) && m_pCurGpInfo->ActiveCapital > 1 )
			{
				if(m_nConGap>0||m_nEndGap>0) tmpValue = 1.0*m_pAnalyData[m_lMyReadNum-1-m_nConGap].fVolume*m_pCurGpInfo->Unit/(m_pCurGpInfo->ActiveCapital*100.0);
				else tmpValue = 1.0*m_pCurGpData->Volume*m_pCurGpInfo->Unit/(m_pCurGpInfo->ActiveCapital*100.0); //先除10000再乘100
				break;
			}
			else TSET_MEANLESS(tmpValue); break;			
		case C_LIANGB:												//量比
			if((m_nConGap>0||m_nEndGap>0)&&m_lMyReadNum>5)
			{
				long tov=0;
				for(int iv=0;iv<5;iv++) tov+=m_pAnalyData[m_lMyReadNum-1-m_nConGap-iv].fVolume;
				tmpValue = 5.0f*(float)m_pAnalyData[m_lMyReadNum-1-m_nConGap].fVolume/(float)tov;
				break;
			}
			else
			{
				if( !testzs(m_pCurGpInfo) )
				{
					short fz[8];
					InitStkTime(m_pCurGpInfo->setcode,fz);
					ii = GetTdxMinute(fz);
					if (ii > 0 && m_pCurGpInfo->VolBase > 0l)
					{
						tmpValue = (float)m_pCurGpData->Volume/ii/m_pCurGpInfo->VolBase;
						break;
					}
				}
				else TSET_MEANLESS(tmpValue); break;
			}			
		case C_WTB:		
			if( !testzs(m_pCurGpInfo) )
			{
				long wl = 0, wlc = 0;
				for (ii = 0;ii < 3;ii++)
				{   
					wlc = wlc+(long)m_pCurGpData->Buyv[ii]-(long)m_pCurGpData->Sellv[ii];
					wl = wl +(long)m_pCurGpData->Buyv[ii]+(long)m_pCurGpData->Sellv[ii];
				}
				if (wl != 0l)
				{
					tmpValue = 100.0*wlc/wl;
					break;
				}
			}
			else TSET_MEANLESS(tmpValue); break;
		default:
			break;
		}
		if(TBE_MEANLESS(tmpValue)==0) 
			break;
		for(i=m_lMyReadNum-1;i>=m_lMyReadNum-MAX_NEED_DATA;i--)
			aTmpData[i]=tmpValue;
		break;
	case DS_LINE:
		short   nTrendSet;
		short   nSpan=5;
		short	bStyle;
		short   nFirstLoc=-1;
		short   nSecendLoc;
		short   nThirdLoc;
		short   i;
		int nNo = m_lMyReadNum-1-m_nConGap;
		struct AnalyData *pAnalyData = m_pRecentData;
		if(m_lMyReadNum<3*nSpan||nNo<3*nSpan-1) break;
		CLE_CONTABLE *tmpTable;	
		int nLine=curUCon.indexNo;
		int nType=nLine/100;
		int nIndex=nLine%100;
		switch(nType)
		{
		case 0:
			tmpTable = g_pCleKLine1;
			break;
		case 1:
			tmpTable = g_pCleKLine2;
			break;
		case 2:
			tmpTable = g_pCleKLine3;
			break;
		case 3:
			tmpTable = g_pCleKLine4;
			break;
		default:
			tmpTable = NULL;
			break;
		}
		if(tmpTable==NULL) break;
		int nSet=tmpTable[nIndex].Status;
		if(isTrendK_Style(m_pCurGpInfo,pAnalyData,m_nDataNum,nSpan,nNo,nSet)!=0) nTrendSet = TRUE;
		else	nTrendSet = FALSE;
		bStyle = FALSE;
		if(nTrendSet==TRUE)
		{
			switch(tmpTable[nIndex].MsgNo)
			{
			case KLINE_1001://低档多条并列阳线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COORDUP,2,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1002://低档多条并列阴线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COORDDOWN,2,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1003://低档横盘中剑形线
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,0,KLINEO_SMALL,KLINEO_DVLSHADOW,1,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1004://低档横盘中弓形线
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,0,KLINEO_SMALL,KLINEO_UVLSHADOW,1,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1005://低档横盘中下跳大阳线
				nSecendLoc = nSpan;
				nFirstLoc  = nNo;
				do
				{	nFirstLoc = E_OddK_Style(pAnalyData,nSecendLoc,nFirstLoc,KLINEO_UP,KLINEO_BIG,0,1,1);
				if(nFirstLoc>0&&nFirstLoc<nNo&&
					OddK_Size  (pAnalyData,nFirstLoc-1)==KLINEO_SMALL&&
					OddK_UpDown(pAnalyData,nFirstLoc-1)==KLINEO_DOWN &&
					pAnalyData[nFirstLoc].Close<pAnalyData[nFirstLoc-1].Open )
				{ bStyle=TRUE; break; }
				nSecendLoc--;
				nFirstLoc --;
				}
				while(nFirstLoc!=-1&&nSecendLoc>0);
				break;
			case KLINE_1006://低档横盘中上跳大阴线
				nSecendLoc = nSpan;
				nFirstLoc  = nNo;
				do
				{	nFirstLoc = E_OddK_Style(pAnalyData,nSecendLoc,nFirstLoc,KLINEO_DOWN,KLINEO_BIG,0,1,1);
				if(nFirstLoc>0&&nFirstLoc<nNo&&
					OddK_Size  (pAnalyData,nFirstLoc-1)==KLINEO_SMALL&&
					OddK_UpDown(pAnalyData,nFirstLoc-1)==KLINEO_UP   &&
					pAnalyData[nFirstLoc].Close>pAnalyData[nFirstLoc-1].Open )
				{ bStyle=TRUE; break; }
				nSecendLoc--;
				nFirstLoc --;
				}
				while(nFirstLoc!=-1&&nSecendLoc>0);
				break;
			case KLINE_1007://低档横盘末期弓形线
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,0,KLINEO_SMALL,KLINEO_UVLSHADOW,1,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1008://低档横盘末期剑形线
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,0,KLINEO_SMALL,KLINEO_DVLSHADOW,1,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1009://低档横盘末期大阳突破
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_BIG,0,1,1);
				if(nFirstLoc==nNo)  { bStyle=TRUE; break; }
				break;
			case KLINE_1010://小幅盘落后大阳向上
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_BIG,0,1,1);
				if(nFirstLoc==nNo&&E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,3,0)==nNo-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1011://低档横盘末期红三兵
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_SMALL,0,3,1);
				if(nFirstLoc==nNo)  { bStyle=TRUE; break; }
				break;
			case KLINE_1012://低档急跌三大阴
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,3,1);
				if(nFirstLoc>1&&OddK_Size(pAnalyData,nFirstLoc-2)!=KLINEO_SMALL&&
					OddK_Size(pAnalyData,nFirstLoc-1)!=KLINEO_SMALL&&
					OddK_Size(pAnalyData,nFirstLoc)!=KLINEO_SMALL)
				{ bStyle=TRUE; break; }
				break;
			case KLINE_1013://三空阴线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,2,1);
				if(nFirstLoc>0&&OddK_Size(pAnalyData,nFirstLoc)==KLINEO_BIG&&
					OddK_Size(pAnalyData,nFirstLoc-1)==KLINEO_BIG&&
					OddK_Size(pAnalyData,nFirstLoc-2)==KLINEO_BIG)
				{ bStyle=TRUE; break; }
				break;
			case KLINE_1014://低档急跌后下跳十字星
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
				if(nFirstLoc!=-1&&OddK_UpDown(pAnalyData,nFirstLoc)==KLINEO_LEVEL&&
					OddK_Shadow(pAnalyData,nFirstLoc)!=KLINEO_NOHEADFOOT&&
					OddK_Shadow(pAnalyData,nFirstLoc)!=KLINEO_NOHEAD    &&
					OddK_Shadow(pAnalyData,nFirstLoc)!=KLINEO_NOFOOT    )
				{ bStyle=TRUE; break; }
				break;
			case KLINE_1015://低档急跌后下跳剑形线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
				if(nFirstLoc!=-1&&OddK_Size(pAnalyData,nFirstLoc)==KLINEO_SMALL&&
					OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_DVLSHADOW)
				{ bStyle=TRUE; break; }
				break;
			case KLINE_1016://低档急跌后下跳弓形线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
				if(nFirstLoc!=-1&&OddK_Size(pAnalyData,nFirstLoc)==KLINEO_SMALL&&
					OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_UVLSHADOW)
				{ bStyle=TRUE; break; }
				break;
			case KLINE_1017://低档急跌后尽头阴线(有下长影)
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
				if(nFirstLoc!=-1&&pAnalyData[nFirstLoc].Close>pAnalyData[nFirstLoc-1].Low)  { bStyle=TRUE; break; }
				break;
			case KLINE_1018://低档急跌后独阳线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FAKECUTIN,1,1);
				if(nFirstLoc!=-1&&OddK_Size(pAnalyData,nFirstLoc)==KLINEO_BIG)  { bStyle=TRUE; break; }
				break;
			case KLINE_1019://低档急跌后独阴线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,1,1);
				if(nFirstLoc!=-1&&OddK_Size(pAnalyData,nFirstLoc)==KLINEO_BIG)  { bStyle=TRUE; break; }
				break;
			case KLINE_1020://低档急跌后阳包阴
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CUD,1,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1021://低档急跌后最后包容阴线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CDU,1,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1022://低档急跌后阴孕阳
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDU,1,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1023://低档急跌后阴孕阴
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDD,1,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1024://低档急跌后阴孕十字星
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDL,1,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1025://低档急跌后阴孕弓形线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDD,1,1);
				while(nFirstLoc>=0)
				{   if(OddK_Size(pAnalyData,nFirstLoc)==KLINEO_SMALL&&
				OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_UVLSHADOW)
				{	bStyle=TRUE;	break;	}
				else	nFirstLoc = E_TwinK_Style(pAnalyData,nSpan-(nNo-nFirstLoc),nFirstLoc-1,KLINET_GDD,1,1);
				}
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDU,1,1);
				while(nFirstLoc>=0)
				{   if(OddK_Size(pAnalyData,nFirstLoc)==KLINEO_SMALL&&
				OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_UVLSHADOW)
				{	bStyle=TRUE;	break;	}
				else	nFirstLoc = E_TwinK_Style(pAnalyData,nSpan-(nNo-nFirstLoc),nFirstLoc-1,KLINET_GDU,1,1);
				}
				break;
			case KLINE_1026://低档急跌后阴孕剑形线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDD,1,1);
				while(nFirstLoc>=0)
				{   if(OddK_Size(pAnalyData,nFirstLoc)==KLINEO_SMALL&&
				OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_DVLSHADOW)
				{	bStyle=TRUE;	break;	}
				else	nFirstLoc = E_TwinK_Style(pAnalyData,nSpan-(nNo-nFirstLoc),nFirstLoc-1,KLINET_GDD,1,1);
				}
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDU,1,1);
				while(nFirstLoc>=0)
				{   if(OddK_Size(pAnalyData,nFirstLoc)==KLINEO_SMALL&&
				OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_DVLSHADOW)
				{	bStyle=TRUE;	break;	}
				else	nFirstLoc = E_TwinK_Style(pAnalyData,nSpan-(nNo-nFirstLoc),nFirstLoc-1,KLINET_GDU,1,1);
				}
				break;
			case KLINE_1027://低档急跌后切入线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CUTIN,1,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1028://低档急跌后阴夹阳
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_NDU,1,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1029://低档急跌后上拉线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_DRAGUP,1,1);
				if(nFirstLoc>0&&OddK_Size(pAnalyData,nFirstLoc-1)==KLINEO_BIG)  { bStyle=TRUE; break; }
				break;
			case KLINE_1030://低档急跌后连拉剑形线
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_SMALL,KLINEO_DVLSHADOW,2,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1031://低档二次狙击性阳线
				nSecendLoc = E_TwinK_Style(pAnalyData,3*nSpan,nNo,KLINET_ASCENT,2,1);
				if(nSecendLoc!=-1)
				{	nFirstLoc = E_TwinK_Style(pAnalyData,3*nSpan,nSecendLoc+2,KLINET_SLIDE,1,0);
				if(nFirstLoc!=-1&&nFirstLoc==nSecendLoc+2)
				{	nThirdLoc = E_TwinK_Style(pAnalyData,3*nSpan+nSecendLoc-nNo,nSecendLoc-3,KLINET_SLIDE,1,0);
				if(nThirdLoc!=-1&&nThirdLoc<nSecendLoc)
				{ bStyle=TRUE; break; }
				}
				}
				break;
			case KLINE_1032://上档盘整后大阳突破
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,1,1);
				if(nFirstLoc!=-1&&OddK_Size(pAnalyData,nFirstLoc)==KLINEO_BIG)  { bStyle=TRUE; break; }
				break;
			case KLINE_1033://跳高一字线后不补缺
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,1,1);
				if(nFirstLoc!=-1&&E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_LEVEL,KLINEO_SMALL,KLINEO_NOHEADFOOT,1,1)==nFirstLoc)  { bStyle=TRUE; break; }
				break;
			case KLINE_1034://二次高开阳线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,2,1);
				if(nFirstLoc!=-1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1035://跳空攀援线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,1,1);
				if(nFirstLoc!=-1&&E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,1,1)==nFirstLoc+1)  { bStyle=TRUE; break; }
				break;
			case KLINE_1036://渐大三连阳
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,0,0,3,1);
				if(nFirstLoc>2&&
					(pAnalyData[nFirstLoc].Close-pAnalyData[nFirstLoc].Open)>(pAnalyData[nFirstLoc-1].Close-pAnalyData[nFirstLoc-1].Open+COMPPREC) &&
					(pAnalyData[nFirstLoc-1].Close-pAnalyData[nFirstLoc-1].Open)>(pAnalyData[nFirstLoc-2].Close-pAnalyData[nFirstLoc-2].Open+COMPPREC))
				{ bStyle=TRUE; break; }
				break;
			case KLINE_1037://上升中途跳高十字星
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,1,1);
				if(nFirstLoc!=-1&&OddK_UpDown(pAnalyData,nFirstLoc)==KLINEO_LEVEL&&
					OddK_Shadow(pAnalyData,nFirstLoc)!=KLINEO_NOHEADFOOT&&
					OddK_Shadow(pAnalyData,nFirstLoc)!=KLINEO_NOHEAD    &&
					OddK_Shadow(pAnalyData,nFirstLoc)!=KLINEO_NOFOOT    )
				{ bStyle=TRUE; break; }
				break;
			case KLINE_1038://上升中途跳高剑形线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,1,1);
				if(nFirstLoc!=-1&&OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_DVLSHADOW&&
					OddK_Size(pAnalyData,nFirstLoc)==KLINEO_SMALL)
				{ bStyle=TRUE; break; }
				break;
			case KLINE_1039://上升中途跳高弓形线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,1,1);
				if(nFirstLoc!=-1&&OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_UVLSHADOW&&
					OddK_Size(pAnalyData,nFirstLoc)==KLINEO_SMALL)
				{ bStyle=TRUE; break; }
				break;
			case KLINE_1040://上升中途小压迫线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CRACK,1,1);
				if(nFirstLoc!=-1&&OddK_Size(pAnalyData,nFirstLoc)==KLINEO_SMALL)  { bStyle=TRUE; break; }
				break;
			case KLINE_1041://上升中途伪阴线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FEIGNDOWN,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1042://上升中途阳包阴
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CUD,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1043://上升中途归顺线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SURRENDER,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1044://上升中途上拉线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_DRAGUP,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1045://上升中途阳孕阴
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GUD,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1046://上升中途切入线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CUTIN,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1047://上升中途阳孕阳
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GUU,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1048://上升中途并阳线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COORDUP,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1049://上升中途滑行线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1050://上升三连击
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,2,1);
				if(nFirstLoc>0&&nFirstLoc<nNo&&
					OddK_UpDown(pAnalyData,nFirstLoc+1)==KLINEO_DOWN&&
					pAnalyData[nFirstLoc-2].Open>pAnalyData[nFirstLoc+1].Close)
				{ nFirstLoc++; bStyle=TRUE; break; }
				break;
			case KLINE_1051://上升三法
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,2,1);
				if(nFirstLoc>2&&nFirstLoc<nNo&&
					OddK_UpDown(pAnalyData,nFirstLoc-3)==KLINEO_UP&&
					OddK_UpDown(pAnalyData,nFirstLoc+1)==KLINEO_UP&&
					pAnalyData[nFirstLoc-2].Open<pAnalyData[nFirstLoc+1].Close)
				{ nFirstLoc++; bStyle=TRUE; break; }
				break;
			case KLINE_1052://上升跳空三法
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,2,1);
				if(nFirstLoc!=-1&&E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_DRAGDOWN,1,1)==nFirstLoc+1)
				{ nFirstLoc++; bStyle=TRUE; break; }
				break;
			case KLINE_1053://超越覆盖线或压迫线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,1,1);
				while(nFirstLoc>3)
				{	if(E_TwinK_Style(pAnalyData,nSpan-nNo+nFirstLoc,nFirstLoc-1,KLINET_SLIDE,2,1)==nFirstLoc-2&&
				pAnalyData[nFirstLoc].Close>pAnalyData[nFirstLoc-4].Open) { bStyle=TRUE; break; }
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan-nNo+nFirstLoc,nFirstLoc-1,KLINET_ASCENT,1,1);
				}
				break;
			case KLINE_1054://上升中途 受阻红三兵
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,0,0,3,1);
				if(nFirstLoc!=-1&&
					(pAnalyData[nFirstLoc].Close-pAnalyData[nFirstLoc].Open)<(pAnalyData[nFirstLoc-1].Close-pAnalyData[nFirstLoc-1].Open) &&
					(pAnalyData[nFirstLoc-1].Close-pAnalyData[nFirstLoc-1].Open)<(pAnalyData[nFirstLoc-2].Close-pAnalyData[nFirstLoc-2].Open))
				{ bStyle=TRUE; break; }
				break;
			case KLINE_1055://上升中途 思考红三兵
				nFirstLoc = nNo;
				do
				{   if(nFirstLoc>1&&nFirstLoc>nNo-nSpan)
				{	if(OddK_UpDown(pAnalyData,nFirstLoc  )==KLINEO_UP &&
				OddK_UpDown(pAnalyData,nFirstLoc-1)==KLINEO_UP &&
				OddK_UpDown(pAnalyData,nFirstLoc-2)==KLINEO_UP &&
				OddK_Size(pAnalyData,nFirstLoc  )==KLINEO_SMALL&&
				OddK_Size(pAnalyData,nFirstLoc-1)==KLINEO_BIG  &&
				OddK_Size(pAnalyData,nFirstLoc-2)==KLINEO_SMALL)
				{	bStyle=TRUE; break;
				}
				}
				nFirstLoc--;
				}
				while(nFirstLoc>1&&nFirstLoc>nNo-nSpan);
				break;
			case KLINE_1056://上升中途阴夹阳
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_NDU,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1057://上升中途阳包阳
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CUU,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1058://上升中途伪阳线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FEIGNUP,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1059://高档多条并列阳线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COORDUP,2,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1060://高档多根并列阴线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COORDDOWN,2,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1061://反击滑行线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,1,1);
				if(nFirstLoc>=0&&nFirstLoc+1==E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_BIG,0,1,1) &&
					pAnalyData[nFirstLoc].Close>pAnalyData[nFirstLoc-2].Open) { bStyle=TRUE; break; }
				break;
			case KLINE_1062://高档横盘中上跳大阴线
				nSecendLoc = nSpan;
				nFirstLoc  = nNo;
				do
				{	nFirstLoc = E_OddK_Style(pAnalyData,nSecendLoc,nFirstLoc,KLINEO_DOWN,KLINEO_BIG,0,1,1);
				if(nFirstLoc>0&&nFirstLoc<nNo&&
					OddK_Size  (pAnalyData,nFirstLoc-1)==KLINEO_SMALL&&
					OddK_UpDown(pAnalyData,nFirstLoc-1)==KLINEO_UP   &&
					pAnalyData[nFirstLoc].Close>pAnalyData[nFirstLoc-1].Open )
				{ bStyle=TRUE; break; }
				nSecendLoc--;
				nFirstLoc --;
				}
				while(nFirstLoc!=-1&&nSecendLoc>0);
				break;
			case KLINE_1063://高档横盘中弓形线
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,0,KLINEO_SMALL,KLINEO_UVLSHADOW,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1064://高档横盘中剑形线
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,0,KLINEO_SMALL,KLINEO_DVLSHADOW,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1065://滑行线接滑行线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,1,1);
				if(nFirstLoc>3&&E_TwinK_Style(pAnalyData,nSpan,nFirstLoc-2,KLINET_SLIDE,1,1)==nFirstLoc-3&&
					OddK_UpDown(pAnalyData,nFirstLoc-2)==KLINEO_UP)
				{ bStyle=TRUE; break; }
				break;
			case KLINE_1066://见顶三鸦
				nSecendLoc = nSpan;
				nFirstLoc  = nNo;
				do
				{	nFirstLoc = E_TwinK_Style(pAnalyData,nSecendLoc,nFirstLoc,KLINET_FEIGNDOWN,1,1);
				if(nFirstLoc!=-1&&E_TwinK_Style(pAnalyData,nSecendLoc,nFirstLoc+1,KLINET_SLIDE,1,1)==nFirstLoc+1)
				{ bStyle=TRUE; break; }
				nSecendLoc--;
				nFirstLoc --;
				}
				while(nFirstLoc!=-1&&nSecendLoc>0);
				break;
			case KLINE_1067://高档盘整末期剑形线
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,0,KLINEO_SMALL,KLINEO_DVLSHADOW,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1068://高档盘整末期大阴向下
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_BIG,0,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1069://高档盘整末期黑三兵
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,2,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1070://小幅盘升后大阴向下
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_BIG,0,1,1);
				if(nFirstLoc>5&&pAnalyData[nFirstLoc].Close<MIN(pAnalyData[nFirstLoc-6].Close,pAnalyData[nFirstLoc-6].Open)) { bStyle=TRUE; break; }
				break;
			case KLINE_1071://暴跌三鸦
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,2,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1072://炮楼顶
				nFirstLoc  = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,2,1);
				nSecendLoc = E_TwinK_Style(pAnalyData,nSpan,nFirstLoc,KLINET_ASCENT,2,1);
				if(nFirstLoc>10&&nSecendLoc<nFirstLoc&&nSecendLoc>nFirstLoc-10) { bStyle=TRUE; break; }
				break;
			case KLINE_1073://高档急升三大阳
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,3,1);
				if(nFirstLoc>1&&OddK_Size(pAnalyData,nFirstLoc-2)!=KLINEO_SMALL&&
					OddK_Size(pAnalyData,nFirstLoc-1)!=KLINEO_SMALL&&
					OddK_Size(pAnalyData,nFirstLoc)!=KLINEO_SMALL)
				{ bStyle=TRUE; break; }
				break;
			case KLINE_1074://三空阳线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,3,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1075://高档弃十字星
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPUP,1,1);
				if(nFirstLoc!=-1&&E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1)==nFirstLoc+2&&
					OddK_Size(pAnalyData,nFirstLoc+1)==KLINEO_SMALL&&
					OddK_Shadow(pAnalyData,nFirstLoc+1)!=KLINEO_NOHEAD&&
					OddK_Shadow(pAnalyData,nFirstLoc+1)!=KLINEO_NOFOOT)
				{ bStyle=TRUE; break; }
				break;
			case KLINE_1076://高档弃弓形线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ABANDON,1,1);
				if(nFirstLoc!=-1&&nFirstLoc<nNo)
				{	if(OddK_UpDown(pAnalyData,nFirstLoc)==KLINEO_UP       &&
				OddK_Size  (pAnalyData,nFirstLoc)==KLINEO_SMALL    &&
				OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_UVLSHADOW&&
				pAnalyData[nFirstLoc+1].Close<pAnalyData[nFirstLoc].Close)
				{ bStyle=TRUE; break; }
				}
				break;
			case KLINE_1077://高档弃吊劲线
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_SMALL,KLINEO_DVLSHADOW,1,0);
				if(nFirstLoc>0&&nFirstLoc<nNo)
				{	if(OddK_UpDown(pAnalyData,nFirstLoc-1)==KLINEO_UP  &&
				OddK_UpDown(pAnalyData,nFirstLoc+1)==KLINEO_DOWN&&
				pAnalyData[nFirstLoc-1].Close<pAnalyData[nFirstLoc].Close&&
				pAnalyData[nFirstLoc+1].Open <pAnalyData[nFirstLoc].Close)
				{ bStyle=TRUE; break; }
				}
				break;
			case KLINE_1078://高档尽头阳线
				nSecendLoc = nSpan;
				nFirstLoc  = nNo;
				do
				{	nFirstLoc = E_OddK_Style(pAnalyData,nSecendLoc,nFirstLoc,KLINEO_UP,0,0,1,1);
				if(nFirstLoc!=-1&&nFirstLoc<nNo&&
					OddK_Size  (pAnalyData,nFirstLoc  )!=KLINEO_SMALL&&
					OddK_UpDown(pAnalyData,nFirstLoc+1)==KLINEO_UP   &&
					pAnalyData[nFirstLoc+1].Open >pAnalyData[nFirstLoc].Close&&
					pAnalyData[nFirstLoc+1].Close<pAnalyData[nFirstLoc].High )
				{ nFirstLoc++; bStyle=TRUE; break; }
				nSecendLoc--;
				nFirstLoc --;
				}
				while(nFirstLoc!=-1&&nSecendLoc>0);
				break;
			case KLINE_1079://绝顶独阴线
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_BIG,0,1,1);
				while(nFirstLoc>0)
				{   if(nFirstLoc<nNo&&OddK_UpDown(pAnalyData,nFirstLoc-1)==KLINEO_UP&&
				OddK_UpDown(pAnalyData,nFirstLoc+1)==KLINEO_UP&&
				pAnalyData[nFirstLoc].Open>pAnalyData[nFirstLoc-1].Close&&
				pAnalyData[nFirstLoc].Close>pAnalyData[nFirstLoc-1].Close-0.2*(pAnalyData[nFirstLoc-1].Close-pAnalyData[nFirstLoc-1].Open)&&
				pAnalyData[nFirstLoc].Close>pAnalyData[nFirstLoc+1].Close)
				{	nFirstLoc++;
				bStyle=TRUE;
				break;
				}
				else
					nFirstLoc = E_OddK_Style(pAnalyData,nSpan-(nNo-nFirstLoc),nFirstLoc-1,KLINEO_DOWN,KLINEO_BIG,0,1,1);
				}
				break;
			case KLINE_1080://绝顶独阳线
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_UP,KLINEO_BIG,0,1,1);
				while(nFirstLoc>0)
				{   if(nFirstLoc<nNo&&OddK_UpDown(pAnalyData,nFirstLoc-1)==KLINEO_UP&&
				pAnalyData[nFirstLoc].Open>MAX(pAnalyData[nFirstLoc-1].Open,pAnalyData[nFirstLoc-1].Close)&&
				pAnalyData[nFirstLoc].Open>MAX(pAnalyData[nFirstLoc+1].Open,pAnalyData[nFirstLoc+1].Close))
				{	nFirstLoc++;
				bStyle=TRUE;
				break;
				}
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan-(nNo-nFirstLoc),nFirstLoc-1,KLINEO_UP,KLINEO_BIG,0,1,1);
				}
				break;
			case KLINE_1081://高档跳空滑行线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,1,1);
				while(nFirstLoc>1)
				{   if(pAnalyData[nFirstLoc-1].Close>MAX(pAnalyData[nFirstLoc-2].Open,pAnalyData[nFirstLoc-2].Close))
				{	bStyle=TRUE;	break;	}
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan-(nNo-nFirstLoc),nFirstLoc-1,KLINET_SLIDE,1,1);
				}
				break;
			case KLINE_1082://高档阴包阳
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CDU,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1083://高档最后包容阳线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CUD,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1084://高档阴包阴
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CDD,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1085://高档阴包剑形线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CDD,1,1);
				if(nFirstLoc>0&&OddK_Size(pAnalyData,nFirstLoc-1)==KLINEO_SMALL&&OddK_Shadow(pAnalyData,nFirstLoc-1)==KLINEO_DVLSHADOW) { bStyle=TRUE; break; }
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CDU,1,1);
				if(nFirstLoc>0&&OddK_Size(pAnalyData,nFirstLoc-1)==KLINEO_SMALL&&OddK_Shadow(pAnalyData,nFirstLoc-1)==KLINEO_DVLSHADOW) { bStyle=TRUE; break; }
				break;
			case KLINE_1086://高档阳孕阴
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GUD,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1087://高档阳孕阳
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GUU,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1088://高档阳孕十字星
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GUL,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1089://高档阳孕吊颈线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GUD,1,1);
				if(nFirstLoc!=-1&&OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_DVLSHADOW)
				{ bStyle=TRUE; break; }
				break;
			case KLINE_1090://高档覆盖线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COVER,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1091://高档阳夹阴
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_NDU,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1092://高档下拖线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_DRAGDOWN,1,1);
				if(nFirstLoc!=-1) { bStyle=TRUE; break; }
				break;
			case KLINE_1093://高档二次狙击性阴线
				nSecendLoc = E_TwinK_Style(pAnalyData,3*nSpan,nNo,KLINET_SLIDE,2,1);
				if(nSecendLoc!=-1)
				{	nFirstLoc = E_TwinK_Style(pAnalyData,3*nSpan,nSecendLoc+2,KLINET_ASCENT,1,0);
				if(nFirstLoc!=-1&&nFirstLoc==nSecendLoc+2)
				{	nThirdLoc = E_TwinK_Style(pAnalyData,3*nSpan+nSecendLoc-nNo,nSecendLoc-3,KLINET_ASCENT,1,0);
				if(nThirdLoc!=-1&&nThirdLoc<nSecendLoc)
				{ bStyle=TRUE; break; }
				}
				}
				break;
			case KLINE_1094://下档盘整后大阴突破
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
				if(nFirstLoc>0&&E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,KLINEO_BIG,0,1,0)<nFirstLoc-1)
				{ bStyle=TRUE; break; }
				break;
			case KLINE_1095://跳水一字线后不补缺
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
				if(nFirstLoc!=-1&&
					OddK_UpDown(pAnalyData,nFirstLoc)==KLINEO_LEVEL&&
					OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_NOHEADFOOT)
				{ bStyle=TRUE; break; }
				break;
			case KLINE_1096://二次跳空阴线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,2,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1097://跳空滑行线
				nSpan = nSpan;
				nSecendLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
				while(nSecendLoc>1&&nSecendLoc<nNo)
				{   if(pAnalyData[nSecendLoc-1].Open <pAnalyData[nSecendLoc-2].Open &&
				pAnalyData[nSecendLoc-1].Open >pAnalyData[nSecendLoc-2].Close&&
				pAnalyData[nSecendLoc-1].Close<pAnalyData[nSecendLoc-2].Close&&
				pAnalyData[nSecendLoc+1].Open <pAnalyData[nSecendLoc  ].Open &&
				pAnalyData[nSecendLoc+1].Open >pAnalyData[nSecendLoc  ].Close&&
				pAnalyData[nSecendLoc+1].Close<pAnalyData[nSecendLoc  ].Close)
				{	nFirstLoc=nSecendLoc+1; bStyle=TRUE; break;	}
				nSecendLoc = E_TwinK_Style(pAnalyData,nSpan-(nNo-nSecendLoc),nSecendLoc-1,KLINET_JMPDOWN,1,1);
				}
				break;
			case KLINE_1098://渐大三连阴
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,0,0,3,1);
				if(nFirstLoc!=-1&&nFirstLoc>1)
				{   if(pAnalyData[nFirstLoc].Open-pAnalyData[nFirstLoc].Close>pAnalyData[nFirstLoc-1].Open-pAnalyData[nFirstLoc-1].Close&&
				pAnalyData[nFirstLoc-1].Open-pAnalyData[nFirstLoc-1].Close>pAnalyData[nFirstLoc-2].Open-pAnalyData[nFirstLoc-2].Close)
				{ bStyle=TRUE; break; }
				}
				break;
			case KLINE_1099://下跌中途跳水十字星
				nFirstLoc = nNo;
				do
				{	if(nFirstLoc>1&&
					MIN(pAnalyData[nFirstLoc-2].Close,pAnalyData[nFirstLoc-2].Open)>MAX(pAnalyData[nFirstLoc-1].Close,pAnalyData[nFirstLoc-1].Open) &&
					MIN(pAnalyData[nFirstLoc-1].Close,pAnalyData[nFirstLoc-1].Open)>MAX(pAnalyData[nFirstLoc  ].Close,pAnalyData[nFirstLoc  ].Open) &&
					OddK_Shadow(pAnalyData,nFirstLoc  )!=KLINEO_NOHEADFOOT&&
					OddK_Shadow(pAnalyData,nFirstLoc  )!=KLINEO_NOHEAD    &&
					OddK_Shadow(pAnalyData,nFirstLoc  )!=KLINEO_NOFOOT    &&
					OddK_Shadow(pAnalyData,nFirstLoc-1)!=KLINEO_NOHEADFOOT&&
					OddK_Shadow(pAnalyData,nFirstLoc-1)!=KLINEO_NOHEAD    &&
					OddK_Shadow(pAnalyData,nFirstLoc-1)!=KLINEO_NOFOOT    &&
					OddK_Size  (pAnalyData,nFirstLoc  )==KLINEO_SMALL	 &&
					OddK_Size  (pAnalyData,nFirstLoc-1)==KLINEO_SMALL	 )
				{	bStyle=TRUE;
				break;
				}
				nFirstLoc--;
				}
				while(nFirstLoc>1&&nFirstLoc>nNo-nSpan);
				break;
			case KLINE_1100://下跌中途跳水弓形线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
				if(nFirstLoc!=-1&&OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_ULSHADOW)
				{ bStyle=TRUE; break; }
				break;
			case KLINE_1101://下跌中途跳水剑形线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
				if(nFirstLoc!=-1&&OddK_Shadow(pAnalyData,nFirstLoc)==KLINEO_DVLSHADOW)
				{ bStyle=TRUE; break; }
				break;
			case KLINE_1102://下跌中途小奉承线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FLATTER,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1103://下跌中途伪阳线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FEIGNUP,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1104://下跌中途阴包阳
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CDU,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1105://下跌中途反叛线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_REBELL,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1106://下跌中途下拖线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_DRAGDOWN,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1107://下跌中途阴包阳
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDU,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1108://下跌中途覆盖线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COVER,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1109://下跌中途阴孕阴
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_GDD,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1110://下跌中途并阳线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COORDUP,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1111://下跌中途并阴线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_COORDDOWN,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1112://下跌中途攀援线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,1,1);
				if(nFirstLoc==nNo)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1113://下降三连击
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_SLIDE,2,1);
				if(nFirstLoc!=-1&&nFirstLoc<nNo&&
					OddK_UpDown(pAnalyData,nFirstLoc+1)==KLINEO_UP&&
					pAnalyData[nFirstLoc-2].Open<pAnalyData[nFirstLoc+1].Close)
				{ nFirstLoc++; bStyle=TRUE; break; }
				break;
			case KLINE_1114://下降三法
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_ASCENT,2,1);
				if(nFirstLoc>2&&nFirstLoc<nNo&&
					OddK_UpDown(pAnalyData,nFirstLoc-3)==KLINEO_DOWN&&
					OddK_UpDown(pAnalyData,nFirstLoc+1)==KLINEO_DOWN&&
					pAnalyData[nFirstLoc-2].Open>pAnalyData[nFirstLoc+1].Close)
				{ nFirstLoc++; bStyle=TRUE; break; }
				break;
			case KLINE_1115://下降跳空三法
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_JMPDOWN,1,1);
				if(nFirstLoc!=-1&&nFirstLoc<nNo&&
					E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_DRAGUP,1,1)==nFirstLoc+1)
				{ bStyle=TRUE; break; }
				break;
			case KLINE_1116://超越切入线或奉承线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CUTIN,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FLATTER,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1117://下跌中途受阻黑三兵
				nFirstLoc = E_OddK_Style(pAnalyData,nSpan,nNo,KLINEO_DOWN,0,0,3,1);
				if(nFirstLoc!=-1)
				{   if(pAnalyData[nFirstLoc].Open-pAnalyData[nFirstLoc].Close<pAnalyData[nFirstLoc-1].Open-pAnalyData[nFirstLoc-1].Close&&
				pAnalyData[nFirstLoc-1].Open-pAnalyData[nFirstLoc-1].Close<pAnalyData[nFirstLoc-2].Open-pAnalyData[nFirstLoc-2].Close)
				{ bStyle=TRUE; break; }
				}
				break;
			case KLINE_1118://下跌中途思考黑三兵
				nFirstLoc = nNo;
				do
				{   if(nFirstLoc>1&&nFirstLoc>nNo-nSpan)
				{	if(OddK_UpDown(pAnalyData,nFirstLoc  )==KLINEO_DOWN &&
				OddK_UpDown(pAnalyData,nFirstLoc-1)==KLINEO_DOWN &&
				OddK_UpDown(pAnalyData,nFirstLoc-2)==KLINEO_DOWN &&
				OddK_Size(pAnalyData,nFirstLoc  )==KLINEO_SMALL  &&
				OddK_Size(pAnalyData,nFirstLoc-1)==KLINEO_BIG    &&
				OddK_Size(pAnalyData,nFirstLoc-2)==KLINEO_SMALL  )
				{	bStyle=TRUE; break;
				}
				}
				nFirstLoc--;
				}
				while(nFirstLoc>1&&nFirstLoc>nNo-nSpan);
				break;
			case KLINE_1119://下跌中途阳夹阴
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_NUD,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1120://下跌中途阴包阴
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_CDD,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			case KLINE_1121://下跌中途伪阴线
				nFirstLoc = E_TwinK_Style(pAnalyData,nSpan,nNo,KLINET_FEIGNDOWN,1,1);
				if(nFirstLoc!=-1)	{ bStyle=TRUE; break; }
				break;
			default:
				break;
			}
		}
		if(bStyle&&nFirstLoc==nNo) bStyle=TRUE;
		else bStyle=FALSE;
		for(i=m_lMyReadNum-1;i>=m_lMyReadNum-MAX_NEED_DATA;i--)
		{
			aTmpData[i]=(float)bStyle;
		}
		break;
	}
		
	////填充其他线
	aTmpData=&m_fDataLines[m_lReadNum];
	int nOtherLines=curUCon.otherLines.size();
	for(int j=0;j<nOtherLines;j++)                    
	{
		switch(curUCon.otherLines[j].lineType)
		{
		case K_LINE:
			if(curUCon.otherLines[j].lineIndex==0)
			{
				for(i=m_lMyReadNum-1;i>=m_lMyReadNum-MAX_NEED_DATA;i--)
				{
					if(m_pAnalyData[i].High<COMPPREC) TSET_MEANLESS(aTmpData[i]);
					else aTmpData[i]=m_pAnalyData[i].High;
				}
			}
			if(curUCon.otherLines[j].lineIndex==1)
			{
				for(i=m_lMyReadNum-1;i>=m_lMyReadNum-MAX_NEED_DATA;i--)
				{
					if(m_pAnalyData[i].Open<COMPPREC) TSET_MEANLESS(aTmpData[i]);
					else aTmpData[i]=m_pAnalyData[i].Open;
				}
			}
			if(curUCon.otherLines[j].lineIndex==2)
			{
				for(i=m_lMyReadNum-1;i>=m_lMyReadNum-MAX_NEED_DATA;i--)
				{
					if(m_pAnalyData[i].Low<COMPPREC) TSET_MEANLESS(aTmpData[i]);
					else aTmpData[i]=m_pAnalyData[i].Low;
				}
			}
			if(curUCon.otherLines[j].lineIndex==3)
			{
				for(i=m_lMyReadNum-1;i>=m_lMyReadNum-MAX_NEED_DATA;i--)
				{
					if(m_pAnalyData[i].Close<COMPPREC) TSET_MEANLESS(aTmpData[i]);
					else aTmpData[i]=m_pAnalyData[i].Close;
				}
			}
			break;
		case CALC_LINE:
			memcpy(aTmpData+m_lMyReadNum-m_nConGap-MAX_NEED_DATA, pOutData[curUCon.otherLines[0].lineIndex]+m_lMyReadNum-m_nConGap-MAX_NEED_DATA, (MAX_NEED_DATA+m_nConGap)*sizeof(float));
			break;
		case NUM_LINE:
			for(i=m_lMyReadNum-1;i>=m_lMyReadNum-MAX_NEED_DATA;i--)
			{
				aTmpData[i]=curUCon.otherLines[j].lineValue;
			}
			break;
		default:
			break;
		}
	}
	for (i=0; i<MAX_LINEOUT;i++) g_pCalc->_CallDTopViewCalc_14(pOutData[i]);
	return TRUE;
}

void MyStrategy::AddSortCon(int iSort)
{
	map<float, int, greater_equal<float> > sortMap;
	sortMap.clear();
	m_aMapSort.push_back(sortMap);
	m_iSort.push_back(iSort);
	m_bSort=TRUE;
}

void MyStrategy::DelSortCon(int nToDel)
{
	ASSERT(m_iSort.size()==m_aMapSort.size());
	vector<int>::iterator iterIndex=m_iSort.begin();
	vector<map<float, int, greater_equal<float> > >::iterator iterMap=m_aMapSort.begin();
	for(int i=0;i<m_iSort.size();i++,iterIndex++,iterMap++)
	{
		int index=(*iterIndex);
		if(index==nToDel)                                     
		{
			m_iSort.erase(iterIndex);
			m_aMapSort.erase(iterMap);
			for(;i<m_iSort.size();i++)
				m_iSort[i]--;
			break;
		}
		if(index>nToDel)
		{
			for(;i<m_iSort.size();i++)
				m_iSort[i]--;
			break;
		}
	}
	if(m_iSort.empty()) m_bSort=FALSE; 
}

void MyStrategy::AddSingleCon(SingleCon &conToAdd)
{
	m_curPlans.myCons.push_back(conToAdd);
}

void MyStrategy::SetIsAndRatio(BOOL bIsAndRatio)
{
	m_curPlans.IsAndRatio = bIsAndRatio;
}

void MyStrategy::DelSingleCon(UINT nToDel)
{
	if(nToDel>m_curPlans.myCons.size()||nToDel<0) return;
	vector<SingleCon>::iterator toDel=m_curPlans.myCons.begin();
	for(int i=0;i<nToDel;i++,toDel++);
	m_curPlans.myCons.erase(toDel);
	if(m_bSort&&!m_iSort.empty())
		DelSortCon(nToDel);
}

BOOL MyStrategy::GetConsInfo()
{
	return m_curPlans.myCons.size()-m_iSort.size();
}

BOOL MyStrategy::HasSortCon()
{
	return m_bSort;
}

CString MyStrategy::GetPlanScript()
{
	return m_curPlans.sDescript;
}

BOOL MyStrategy::ParsCurCon(int nCurCon)
{
	int nType=m_curPlans.myCons[nCurCon].mainType;
	int nIndex=m_curPlans.myCons[nCurCon].indexNo;
	TINDEXINFO	*pIndex = g_pCalc->_CallDTopViewCalc_8(nType, nIndex);
	if(!pIndex) return FALSE;
	if(nType==TJ_TYPE)                   //条件选股不使用任何判断函数
	{
		if(m_curPlans.myCons[nCurCon].procIndex==-1)
			return TRUE;
		else 
			return FALSE;
	}
	if(nType==ZB_TYPE)                   //指标选股判断函数索引值须合理
	{
		if(m_curPlans.myCons[nCurCon].procIndex>-1&&m_curPlans.myCons[nCurCon].procIndex<sizeof(UConProc)/sizeof(proc))
			return TRUE;
		else 
			return FALSE; 
	}
	if(m_curPlans.myCons[nCurCon].mainLine.lineType==K_LINE)   //可主图叠加
	{
		return pIndex->nType==OnlyMain||pIndex->nType==KOverlay;
	}
	if(m_curPlans.myCons[nCurCon].mainLine.lineType==NUM_LINE) 
		return FALSE;
	if(m_curPlans.myCons[nCurCon].procIndex>2&&m_curPlans.myCons[nCurCon].procIndex<6) return m_bSort; 
	return TRUE;
}

void MyStrategy::RePlaceDate(CString &sList, long nDate)
{
	int nDateSpace = sList.Find(' ', 0);
	if(nDateSpace>=0)
	{
		CString sDate = sList.Left(nDateSpace+1);
		CTime time1(nDate/10000, (nDate/100)%100, nDate%100, 0, 0, 0);
		CString tempstr = time1.Format("%y-%m-%d ");
		sList.Replace(sDate, tempstr);
	}
}

void MyStrategy::SLPlan(CArchive &ar, BOOL bUseNewTime)
{
	int nMainStra;
	if(ar.IsStoring())
	{
		nMainStra=m_curPlans.myCons.size();
		ar<<nMainStra;
		for(int i=0;i<nMainStra;i++)
		{
			if(bUseNewTime)
				m_curPlans.myCons[i].endDate = -1;	//用当前日期
			m_curPlans.myCons[i].Serialize(ar);
		}
		ar<<m_curPlans.sDescript;
	}
	else
	{
		ar>>nMainStra;
		for(int i=0;i<nMainStra;i++)
		{
			SingleCon conToLoad;
			conToLoad.Serialize(ar);
			if(conToLoad.endDate<0)
				conToLoad.endDate=g_lOpenRq;
			RePlaceDate(conToLoad.sList, conToLoad.endDate);	//描述信息用当前信息
			m_curPlans.myCons.push_back(conToLoad);
		}
		ar>>m_curPlans.sDescript;
	}
}

int MyStrategy::GetListStrings(vector<CString> &aListStrings)
{
	int nCons=m_curPlans.myCons.size();
	for(int i=0;i<m_curPlans.myCons.size();i++)
	{
		aListStrings.push_back(m_curPlans.myCons[i].sList);
	}
	return nCons;
}

BOOL MyStrategy::CompCurCon(CString &tmpStr)
{
	int nCons=m_curPlans.myCons.size();
	for(int i=0;i<nCons;i++)
	{
		if(m_curPlans.myCons[i].sList==tmpStr)
			return TRUE;
	}
	return FALSE;
}

BOOL MyStrategy::IsSort()
{
	int nCons=m_curPlans.myCons.size();
	for(int i=0;i<nCons;i++)
	{
		int iProc=m_curPlans.myCons[i].procIndex;
		if(iProc>2&&iProc<6)
		{
			AddSortCon(i);
		}
	}
	return m_bSort;
}

void MyStrategy::PutPlanScript(CString &planScript)
{
	m_curPlans.sDescript=planScript;
}
///////////////////////////////////////////////////////////////
//条件策略函数
void MyStrategy::CalcCurCon(SingleCon curCon)
{
	int i;
	InputCurConData(curCon);
	if(curCon.mainType==ZB_SEL||curCon.mainType==CWHQ_SEL)
	{
		if(UConProc[curCon.procIndex])
			(this->*(UConProc[curCon.procIndex]))();
		else
		{
			for(i=m_lMyReadNum-1;i>=m_lMyReadNum-MAX_NEED_DATA;i--)
				m_transData[i]=g_nCancel;
			if(TBE_MEANLESS(m_fDataLines[m_lMyReadNum-1-m_nConGap])==0) 
				return;
			float sortIndex=m_fDataLines[m_lMyReadNum-1-m_nConGap];
			(*m_mapSort)[sortIndex]=m_curGPIndex;
		}
	}
	else if(curCon.mainType==TJ_SEL||curCon.mainType==KL_SEL||curCon.mainType==STYLE_SEL)
	{
		for(i=m_lMyReadNum-1;i>=m_lMyReadNum-MAX_NEED_DATA;i--)
			m_transData[i]=(long)m_fDataLines[i];
	}
}

BOOL MyStrategy::CalcMyPlan()
{   
	int nCon=m_curPlans.myCons.size();
	if(nCon<1) return FALSE;
	for(int i=0;i<nCon;i++)
	{
		CalcConGap(m_curPlans.myCons[i].endDate);
		PutCurTransDataToCalc(i);
		CalcCurCon(m_curPlans.myCons[i]);
		PutCurTransDataBack(i);
	}
	return CalcFinalOut(!m_curPlans.IsAndRatio);
}

int MyStrategy::CalcSortCon(short i, SingleCon &curCon, long TotalStockNum, short *sortSelStk)
{
	int nProc=curCon.procIndex, j;
	long nValue=curCon.otherLines[0].lineValue;
	int nInclude=(nProc==3?1:nValue);
	map<float, int, greater_equal<float> >::iterator iter;
	switch(nProc)
	{
	case 3:                                           //等于
		iter = m_aMapSort[i].begin();
		for(j=0;j<nValue-1&&iter!=m_aMapSort[i].end();j++,iter++);
		if(iter==m_aMapSort[i].end()) break;
		sortSelStk[0]=(*iter).second;
		break;
	case 4:											  //前XX名
		iter = m_aMapSort[i].begin();
		for(j=0;j<nInclude&&iter!=m_aMapSort[i].end();j++,iter++)
			sortSelStk[j]=(*iter).second;
		break;
	case 5:											  //后XX名
		iter = m_aMapSort[i].end();
		iter--;
		for(j=0;j<nInclude&&iter!=m_aMapSort[i].begin();j++,iter--)
			sortSelStk[j]=(*iter).second;
		if(j<nInclude&&iter==m_aMapSort[i].begin())
			sortSelStk[j]=(*iter).second;
		break;
	default:
		break;
	}
	return nInclude;
}

int MyStrategy::FiltSortRes(int IsAndRadio, int TotalStockNum, int SuccessNum, short *SelectedStk)
{
	int i ,j, k;
	short *resBefore=new short[TotalStockNum];
	memcpy(resBefore, SelectedStk, TotalStockNum*sizeof(short));
	short *sortSelStk=new short[TotalStockNum];
	int nMap=m_aMapSort.size();
	for(i=0;i<nMap;i++)
	{
		memset(sortSelStk, -1, TotalStockNum*sizeof(short));
		int iCon=m_iSort[i];
		int nSel=CalcSortCon(i, m_curPlans.myCons[iCon], TotalStockNum, sortSelStk);
		if(IsAndRadio)
		{
			for(j=0;j<nSel;j++)
			{
				for(k=0;k<SuccessNum;k++)
				{
					if(sortSelStk[j]>-1&&resBefore[k]>-1&&sortSelStk[j]==resBefore[k]) break;
				}
				if(k==SuccessNum)
					sortSelStk[j]=-1;
			}
		}
		else
		{
			int nRadio=nSel;
			for(j=0;j<TotalStockNum;j++)
			{
				if(resBefore[j]==-1) continue;
				for(k=0;k<nRadio;k++)
				{
					if(resBefore[j]==sortSelStk[k]) break;
				}
				if(k==nRadio)
					sortSelStk[nSel++]=resBefore[j];
			}
		}
		memcpy(resBefore, sortSelStk, TotalStockNum*sizeof(short));
/*		vector<short> resAfterSort(sortSelStk, sortSelStk+TotalStockNum);
		vector<short> res(2*TotalStockNum, -1);
		if(IsAndRadio)
			set_intersection(resBefore.begin(), resBefore.end(), 
			resAfterSort.begin(), resAfterSort.end(), res.begin());
		else
			set_union(resBefore.begin(), resBefore.end(), 
			resAfterSort.begin(), resAfterSort.end(), res.begin());
		res.erase(remove(res.begin(), res.end(), -1), res.end());
		resBefore=res;	*/
	}
/*	int nSuccessNum=resBefore.size();
	memset(SelectedStk, -1, TotalStockNum*sizeof(short));
	for(i=0;i<nSuccessNum;i++)
		SelectedStk[i]=resBefore[i];	*/
	int nSuccessNum=0;
	memset(SelectedStk, -1, TotalStockNum*sizeof(short));
	for(i=0;i<TotalStockNum;i++)
	{
		if(resBefore[i]>-1)
			SelectedStk[nSuccessNum++]=resBefore[i];
	}
	delete []resBefore;resBefore=NULL;
	delete []sortSelStk;sortSelStk=NULL;
	return nSuccessNum;
}
///////////////////////////////////////////////////////////////
//以下为策略测试函数
void MyStrategy::TransWithSign()
{
	PutCurTransDataToCalc(m_curSignStra.pairConIndex);
	if(m_curSignStra.bBuy)
	{
		if(!m_curSignStra.typeDo) CalcContiDayNoBuy(m_curSignStra.nCancelData);
		else if(m_curSignStra.typeDo) CalcContiNoBuy();
	}
	PutCurTransDataBack(m_curSignStra.pairConIndex);
}

void MyStrategy::TransWithKS()
{
	(this->*(KSProc[m_curCalcKs.procIndex]))();
}

void MyStrategy::CalcTestStrategy()
{
	m_curPlans=m_curTestStra.myPlans;
	CalcMyPlan();
	int nSignStra=m_curTestStra.sighStra.size();
	for(int i=0;i<nSignStra;i++)
	{
		m_curSignStra=m_curTestStra.sighStra[i];
		TransWithSign();
	}
	int nKs=m_curTestStra.ks.size();
	for(i=0;i<nKs;i++)
	{
		m_curCalcKs=m_curTestStra.ks[i];
		TransWithKS();
	}
}
///////////////////////////////////////////////////////////////////////
//各判断函数
void MyStrategy::CalcEqual()
{
	for(int i=m_lMyReadNum-1;i>=m_lMyReadNum-m_nConGap-MAX_NEED_DATA;i--)
	{
		if(!TBE_MEANLESS(m_fDataLines[i])||!TBE_MEANLESS(m_fDataLines[m_lReadNum+i])) 
			m_transData[i]=0;
		else if(fabs(m_fDataLines[i]-m_fDataLines[m_lReadNum+i])<COMPPREC)
			m_transData[i]=1;
		else
			m_transData[i]=0;
	}
}

void MyStrategy::CalcGreater()
{
	for(int i=m_lMyReadNum-1;i>=m_lMyReadNum-m_nConGap-MAX_NEED_DATA;i--)
	{
		if(!TBE_MEANLESS(m_fDataLines[i])||!TBE_MEANLESS(m_fDataLines[m_lReadNum+i])) 
			m_transData[i]=0;
		else if(m_fDataLines[i]>m_fDataLines[m_lReadNum+i])
			m_transData[i]=1;
		else
			m_transData[i]=0;
	}
}

void MyStrategy::CalcLess()
{
	for(int i=m_lMyReadNum-1;i>=m_lMyReadNum-m_nConGap-MAX_NEED_DATA;i--)
	{
		if(!TBE_MEANLESS(m_fDataLines[i])||!TBE_MEANLESS(m_fDataLines[m_lReadNum+i])) 
			m_transData[i]=0;
		else if(m_fDataLines[i]<m_fDataLines[m_lReadNum+i])
			m_transData[i]=1;
		else
			m_transData[i]=0;
	}
}

void MyStrategy::CalcUp()
{
	for(int i=m_lMyReadNum-1;i>=m_lMyReadNum-m_nConGap-MAX_NEED_DATA;i--)
	{
		if(!TBE_MEANLESS(m_fDataLines[i-1])||!TBE_MEANLESS(m_fDataLines[m_lReadNum+i])) 
			m_transData[i]=0;
		else if(m_fDataLines[i-1]<m_fDataLines[m_lReadNum+i])
			m_transData[i]=1;
		else
			m_transData[i]=0;
	}
}

void MyStrategy::CalcDn()
{
	for(int i=m_lMyReadNum-1;i>=m_lMyReadNum-MAX_NEED_DATA;i--)
	{
		if(!TBE_MEANLESS(m_fDataLines[i-1])||!TBE_MEANLESS(m_fDataLines[m_lReadNum+i])) 
			m_transData[i]=0;
		if(m_fDataLines[i-1]>m_fDataLines[m_lReadNum+i])
			m_transData[i]=1;
		else
			m_transData[i]=0;
	}
}

void MyStrategy::CalcUpCross()
{
	for(int i=m_lMyReadNum-1;i>=m_lMyReadNum-m_nConGap-MAX_NEED_DATA;i--)
	{
		if(!TBE_MEANLESS(m_fDataLines[i-1])||!TBE_MEANLESS(m_fDataLines[m_lReadNum+i-1])||
			!TBE_MEANLESS(m_fDataLines[i])||!TBE_MEANLESS(m_fDataLines[m_lReadNum+i])) 
			m_transData[i]=0;
		else if(m_fDataLines[i-1]<m_fDataLines[m_lReadNum+i-1]&&m_fDataLines[i]>m_fDataLines[m_lReadNum+i])
			m_transData[i]=1;
		else
			m_transData[i]=0;
	}
}

void MyStrategy::CalcDnCross()
{
	for(int i=m_lMyReadNum-1;i>=m_lMyReadNum-m_nConGap-MAX_NEED_DATA;i--)
	{
		if(!TBE_MEANLESS(m_fDataLines[i-1])||!TBE_MEANLESS(m_fDataLines[m_lReadNum+i-1])||
			!TBE_MEANLESS(m_fDataLines[i])||!TBE_MEANLESS(m_fDataLines[m_lReadNum+i])) 
			m_transData[i]=0;
		else if(m_fDataLines[i-1]>m_fDataLines[m_lReadNum+i-1]&&m_fDataLines[i]<m_fDataLines[m_lReadNum+i])
			m_transData[i]=1;
		else
			m_transData[i]=0;
	}
}

void MyStrategy::CalcTurnUp()
{
	for(int i=m_lMyReadNum-1;i>=m_lMyReadNum-m_nConGap-MAX_NEED_DATA;i--)
	{
		if(!TBE_MEANLESS(m_fDataLines[i-1])||!TBE_MEANLESS(m_fDataLines[m_lReadNum+i-2])||
			!TBE_MEANLESS(m_fDataLines[i-1])||!TBE_MEANLESS(m_fDataLines[m_lReadNum+i])) 
			m_transData[i]=0;
		else if(m_fDataLines[i-1]<m_fDataLines[m_lReadNum+i-2]&&m_fDataLines[i-1]<m_fDataLines[m_lReadNum+i])
			m_transData[i]=1;
		else
			m_transData[i]=0;
	}
}

void MyStrategy::CalcTurnDn()
{
	for(int i=m_lMyReadNum-1;i>=m_lMyReadNum-m_nConGap-MAX_NEED_DATA;i--)
	{
		if(!TBE_MEANLESS(m_fDataLines[i-1])||!TBE_MEANLESS(m_fDataLines[m_lReadNum+i-2])||
			!TBE_MEANLESS(m_fDataLines[i-1])||!TBE_MEANLESS(m_fDataLines[m_lReadNum+i])) 
			m_transData[i]=0;
		else if(m_fDataLines[i-1]>m_fDataLines[m_lReadNum+i-2]&&m_fDataLines[i-1]>m_fDataLines[m_lReadNum+i])
			m_transData[i]=1;
		else
			m_transData[i]=0;
	}
}

void MyStrategy::CalcMaxKS()
{
	int iBegin=-1,iEnd=0,i;
	short df=m_curCalcKs.nBreadth;
	for(i=0;i<m_lMyReadNum;i++)
	{
		if(m_finalOut[i]=1) 
		{
			iBegin=i;
			break;
		}
	}
	if(iBegin==-1) return;                //找首个买点
	iEnd=iBegin+1;                        //重置卖点
	while(iEnd<m_lMyReadNum)
	{
		for(i=iBegin+1;i<m_lMyReadNum;i++,iEnd++)           //找下个卖点
		{
			if(m_finalOut[i]=-1)
				break;
		}                                                 
		for(i=iBegin;i<iEnd;i++)                           //由对应买卖点确定止损
		{ 
			if(m_pAnalyData[i-1].Close>(100-df)*m_pAnalyData[iBegin].Close/100&&
				m_pAnalyData[i].Close<(100-df)*m_pAnalyData[iBegin].Close/100)
			{
				m_finalOut[i]=-1;
				break;
			}
		}                                     
		for(i=iEnd+1;i<m_lMyReadNum;i++)           //寻找下个买点
		{
			if(m_finalOut[i]=1)
			{
				iBegin=i;
				break;
			}
		}
		iEnd=iBegin+1;                                     //重置卖点
	}
}

void MyStrategy::CalcTopKS()
{
	int iBegin=-1,iEnd=0,i;
	short df=m_curCalcKs.nBreadth;
	int nData=m_curCalcKs.nData+1;
	for(i=0;i<m_lMyReadNum;i++)
	{
		if(m_finalOut[i]=1) 
		{
			iBegin=i;
			break;
		}
	}
	if(iBegin==-1) return;
	iEnd=iBegin+1;
	while(iEnd<m_lMyReadNum)
	{
		for(i=iBegin+1;i<m_lMyReadNum;i++,iEnd++)
		{
			if(m_finalOut[i]=-1)
				break;
		}
		for(i=iBegin+nData;i<iEnd;i++)
		{
			if((100-df)*m_pAnalyData[i].Close/100<m_pAnalyData[i-nData].Close)
			{
				m_finalOut[i]=-1;
				break;
			}
		}
		for(i=iEnd+1;i<m_lMyReadNum;i++)
		{
			if(m_finalOut[i]=1)
			{
				iBegin=i;
				break;
			}
		}
		iEnd=iBegin+1;
	}
}

void MyStrategy::CalcContiNoBuy()
{
	int iBegin=0,iEnd=0,i;
	while(iEnd<m_lMyReadNum)
	{
		for(;iBegin<m_lMyReadNum;iBegin++)
		{
			if(m_transData[iBegin]=1)
			{
				for(iEnd=iBegin+1;m_transData[iEnd]==1;iEnd++); 
				if(iEnd-iBegin>1)
				{
					for(i=iBegin+1;i<iEnd;i++)
						m_transData[i]=0;
				}
			}
		}
		iBegin=iEnd;
	}
}

void MyStrategy::CalcContiDayNoBuy(int cancelData)
{
	int iBegin=0,iEnd=0,i;
	while(iEnd<m_lMyReadNum)
	{
		for(;iBegin<m_lMyReadNum;iBegin++)
		{
			if(m_transData[iBegin]=1)
			{
				for(iEnd=iBegin+1;iEnd-iBegin<=cancelData;iEnd++); 
				{
					for(i=iBegin+1;i<iEnd;i++)
						m_transData[i]=0;
				}
			}
		}
		iBegin=iEnd;
	}
}

BOOL MyStrategy::CalcFinalOut(int IsAndRadio)
{
	memcpy(m_finalOut, m_aTransData, m_lReadNum*sizeof(long));
	for(int i=0;i<m_nCon;i++)
	{
		for(int j=m_lMyReadNum-1;j>=m_lMyReadNum-MAX_NEED_DATA;j--)
		{
			if(m_finalOut[j]==g_nCancel)
			{
				if(m_aTransData[i*m_lReadNum+j]==1) m_finalOut[j]=1;
				else if(m_aTransData[i*m_lReadNum+j]==0) m_finalOut[j]=0;
				else if(m_aTransData[i*m_lReadNum+j]==-1) m_finalOut[j]=-1;
				else if(m_aTransData[i*m_lReadNum+j]==g_nCancel) m_finalOut[j]=g_nCancel;
			}
			if(m_finalOut[j]==0)
			{
				if(m_aTransData[i*m_lReadNum+j]==1) m_finalOut[j]=IsAndRadio?0:1;
				else if(m_aTransData[i*m_lReadNum+j]==0) m_finalOut[j]=0;
				else if(m_aTransData[i*m_lReadNum+j]==-1) m_finalOut[j]=IsAndRadio?0:-1;
				else if(m_aTransData[i*m_lReadNum+j]==g_nCancel) m_finalOut[j]=0;
			}
			if(m_finalOut[j]==1)
			{
				if(m_aTransData[i*m_lReadNum+j]==1) m_finalOut[j]=1;
				else if(m_aTransData[i*m_lReadNum+j]==0) m_finalOut[j]=IsAndRadio?0:1;
				else if(m_aTransData[i*m_lReadNum+j]==-1) m_finalOut[j]=0;
				else if(m_aTransData[i*m_lReadNum+j]==g_nCancel) m_finalOut[j]=1;
			}
			if(m_finalOut[j]==-1)
			{
				if(m_aTransData[i*m_lReadNum+j]==1) m_finalOut[j]=0;
				else if(m_aTransData[i*m_lReadNum+j]==0) m_finalOut[j]=IsAndRadio?0:-1;
				else if(m_aTransData[i*m_lReadNum+j]==-1) m_finalOut[j]=-1;
				else if(m_aTransData[i*m_lReadNum+j]==g_nCancel) m_finalOut[j]=-1;
			}
		}
	}
	return(m_finalOut[m_lMyReadNum-1]==1);
}
//////////////////////////////////////////////////////////////////////////////////
//K线定势组合辅助函数

//判断一段K线中是否存在指定的单根K线形态
//时间段为pAnalyData[nNo-nSpan+1<=(...)<=nNo]
//nMode==1为连续统计,nMode==0为不判断连续统计
//若存在nKLineStyle形态的K线形态返回序号
//否则返回-1
short MyStrategy::E_OddK_Style(struct AnalyData *pAnalyData,short nSpan,short nNo,short nUpDown,short nSize,short nShadow,short nWeight,short nMode)
{	
	short   cCount=0;
	short   nFirstLoc;
	if(nSpan<0||nNo<0) return -1;
	for(short i=nNo; i>MAX(nNo-nSpan,0); i--)
	{	if((nUpDown==0||OddK_UpDown(pAnalyData,i)==nUpDown) &&
		   (nSize  ==0||OddK_Size  (pAnalyData,i)==nSize  ) &&
		   (nShadow==0||OddK_Shadow(pAnalyData,i)==nShadow) )
		{	cCount++;
			if(cCount==1) 		nFirstLoc=i;    //确定第一个符合的形态的位置
			if(cCount>=nWeight) break;			//已完成查找
		}
		else
		{	if(nMode==1)  cCount=0;		//连续统计
		}
	}
	if(cCount>=nWeight) return nFirstLoc;
	return -1;
}

//判断一段K线中是否存在指定的nWeight根K线形态
//时间段为pAnalyData[nNo-nSpan+1<=(...)<=nNo]
//nMode==1为连续统计,nMode==0为不判断连续统计
//若存在nKStyle形态的K线形态返回序号
//否则返回-1
short MyStrategy::E_TwinK_Style(struct AnalyData *pAnalyData,short nSpan,short nNo,short nKStyle,short nWeight,short nMode)
{	
	short   cCount=0;
	short   nFirstLoc;
	for(short i=nNo; i>MAX(nNo-nSpan,0); i--)
	{	if(isTwinK_Style(pAnalyData,i,nKStyle)==nKStyle)
		{
			cCount++;
			if(cCount==1) 		nFirstLoc=i;    //确定第一个符合的形态的位置
			if(cCount>=nWeight) break;			//已完成查找
		}
		else
		{	if(nMode==1)  cCount=0;		//连续统计
		}
	}
	if(cCount>=nWeight) return nFirstLoc;
	return -1;
}

//取单K线阳线,阴线,平盘线
//平盘线为绝对平盘
short MyStrategy::OddK_UpDown(struct AnalyData *pAnalyData,short nNo)
{
	if(fabs(pAnalyData[nNo].Close-pAnalyData[nNo].Open)<0.00001) return KLINEO_LEVEL;
	if(pAnalyData[nNo].Close>pAnalyData[nNo].Open) return KLINEO_UP;
	return KLINEO_DOWN;
}

//取单K线大小形态(大,中,小)
//以1/5为界
short MyStrategy::OddK_Size(struct AnalyData *pAnalyData,short nNo)
{
	float   fRate;
	fRate = fabs(pAnalyData[nNo].Close-pAnalyData[nNo].Open)/pAnalyData[nNo].Close;
	if(fRate>0.08 ) return KLINEO_BIG;
	if(fRate<0.02 ) return KLINEO_SMALL;
	return KLINEO_NORMAL;
}

//取单K线影线形态(秃头,光脚,秃头光脚...)
//秃头光脚  无上下影线
//光头      无上影线
//光脚      无下影线
short MyStrategy::OddK_Shadow(struct AnalyData *pAnalyData,short nNo)
{
	float fHeadRate;
	float fFootRate;
	fHeadRate = (pAnalyData[nNo].High-MAX(pAnalyData[nNo].Close,pAnalyData[nNo].Open))/fabs(pAnalyData[nNo].Close-pAnalyData[nNo].Open);
	fFootRate = (MIN(pAnalyData[nNo].Close,pAnalyData[nNo].Open)-pAnalyData[nNo].Low)/fabs(pAnalyData[nNo].Close-pAnalyData[nNo].Open);
	if(fHeadRate<COMPPREC&&fFootRate<COMPPREC) return KLINEO_NOHEADFOOT;
	if(fHeadRate<COMPPREC&&fFootRate>COMPPREC) return KLINEO_NOHEAD;
	if(fHeadRate>COMPPREC&&fFootRate<COMPPREC) return KLINEO_NOFOOT;
	if(fabs(fHeadRate-fFootRate)<COMPPREC  ) return KLINEO_ELSHADOW;
	if(fHeadRate>3.0&&fHeadRate/fFootRate>3.0) return KLINEO_UVLSHADOW;
	if(fFootRate>3.0&&fFootRate/fHeadRate>3.0) return KLINEO_DVLSHADOW;
	if(fHeadRate/fFootRate>1.0&&fFootRate>0.1) return KLINEO_ULSHADOW;
	return KLINEO_DLSHADOW;
}

//参考依据:
//1.长短		长度比
//2.涨幅		幅度比
//3.涨速		速度比,水平度
//4.价位		价位比
//5.符号		符号比
//6.密度		密度

//判断pAnalyData[0,...,nDataNum-1]中序号为nNo,跨度为nSpan的K线区段的形态
//其中移动平均高低档运算以6*nSpan计算,
short MyStrategy::isTrendK_Style(MemStkInfo *tmpInfo,struct AnalyData *pAnalyData,short nDataNum,short nSpan,short nNo,short nKStyle)
{	
	short    naLoc[60];     //段末位置
	short	 naLen[60];     //段长
	float    faSlope[60];	//斜率(速度)
	float    faWeight[60];  //段权重(涨幅和,相对本段)
	float    faDiff[60];  	//段涨幅
	float    fPriceLv;		//价位比
	short    nNodeNum;
	short    nNearNode;
	short    nLastNear;
	short    i,j;
	if(nDataNum<6*nSpan) return 0;

	HISDAT *tmpHisData = new HISDAT[nDataNum];
	RawData2CalcData(tmpInfo->Code,tmpInfo->setcode,PER_DAY,pAnalyData,tmpHisData,nDataNum,g_NoTime); //转成TCALC所认识的数据
	int 	nRealNum = g_pCalc->_CallDTopViewCalc_3(tmpInfo->Code,tmpInfo->setcode,PER_DAY,tmpHisData,nDataNum);
	delete tmpHisData;
	if(nRealNum < 0)  return 0;
	float *pOutData[MAX_LINEOUT];
	for(j=0; j<MAX_LINEOUT;j++) 
		pOutData[j] = g_pCalc->_CallDTopViewCalc_13(nRealNum);
	int IndexNo = g_pCalc->_CallDTopViewCalc_9(ZB_TYPE,"ZXNH");
	if(IndexNo < 0)
	{
		for (j=0; j<MAX_LINEOUT; j++) g_pCalc->_CallDTopViewCalc_14(pOutData[j]);
		return 0;
	}
	g_pCalc->_CallDTopViewCalc_5(ZB_TYPE, IndexNo, pOutData);

	nLastNear	= nDataNum-1;
	nNearNode	= nDataNum-2;
	nNodeNum	= 0;
	while(nNearNode>=0)
	{	while(nNearNode>=0)
		{	if(nNearNode==0) break;
			if(fabs(pOutData[0][nNearNode])<0.05) { nNearNode--; continue; }
			if(nLastNear-nNearNode<2) { nNearNode--; continue; }
			break;
		}
		if(nNearNode>=0)
		{
			naLoc[nNodeNum]     = nNearNode;
			naLen[nNodeNum]		= nLastNear-nNearNode+1;
			faDiff[nNodeNum]	= pAnalyData[nLastNear].Close-pAnalyData[nNearNode].Close;
			faSlope[nNodeNum]	= faDiff[nNodeNum]/(naLen[nNodeNum]*pAnalyData[nNearNode].Close);
			faWeight[nNodeNum]  = 0.0;
			for(i=nNearNode; i<=nLastNear; i++)
				faWeight[nNodeNum] += (pAnalyData[i].Close-pAnalyData[nNearNode].Close)/pAnalyData[nLastNear].Close;
			nNodeNum++;
			nLastNear = nNearNode;
			nNearNode --;
		}
	}
	for (j=0; j<MAX_LINEOUT; j++) g_pCalc->_CallDTopViewCalc_14(pOutData[j]);

	float	fMax;
	float	fMin;
	fMax = pAnalyData[0].Close;
	fMin = pAnalyData[0].Close;
	for(i=1; i<nDataNum; i++)
	{   if(fMax<pAnalyData[i].Close) fMax = pAnalyData[i].Close;
		if(fMin>pAnalyData[i].Close) fMin = pAnalyData[i].Close;
	}
	fPriceLv = 0.0;
	for(i=nDataNum-5; i<nDataNum; i++)	fPriceLv += pAnalyData[i].Close;
	fPriceLv = fPriceLv/5.0;
	fPriceLv = (fPriceLv-fMin)/(fMax-fMin);

	float HIGHPRICERATE	=	 0.75f;			//高档
	float LOWPRICERATE	=	 0.25f;			//低档
	float UPLIMIT		=	 COMPPREC;		//上升途中(幅度)
	float DOWNLIMIT		=	-COMPPREC;		//下跌途中(幅度)
	float UPLIMITW		=	 COMPPREC;		//上升途中(权重)
	float DOWNLIMITW	=	-COMPPREC;		//下跌途中(权重)
	float DULLRATE		=	 0.15*nDataNum;	//横盘比数
	float DULLLEN		=    0.90*nSpan;	//横盘跨距
	float FASTUPLIMIT	=	 0.03f;			//急升(幅度)
	float FASTDOWNLIMIT	=	-0.03f;			//急跌(幅度)
	float SLOWUPLIMIT	=	 0.025f;		//缓升(幅度)
	float SLOWDOWNLIMIT	=	-0.025f;		//缓跌(幅度)
	float FASTUPLIMITW	=	 0.03f;			//急升(权重)
	float FASTDOWNLIMITW=	-0.03f;			//急跌(权重)
	float SLOWUPLIMITW	=	 0.025f;		//缓升(权重)
	float SLOWDOWNLIMITW=	-0.025f;		//缓跌(权重)
	switch(nKStyle)
	{
	case KLINETL_HIGH:
	//高档(*),短期均价相对长期最高最低中心价而言,
		if(fPriceLv>HIGHPRICERATE)	return nKStyle;
		break;
	case KLINETL_LOW:
	//低档(*),短期均价相对长期最高最低中心价而言,
		if(fPriceLv<LOWPRICERATE)	return nKStyle;
		break;
	case KLINETL_UP:
	//上升途中(*),短期均价相对长期最高最低中心价而言,
		if(fPriceLv>HIGHPRICERATE)	return 0;
		if(fPriceLv<LOWPRICERATE )	return 0;
		if((faSlope[0]>UPLIMIT||faWeight[0]>UPLIMITW)&&(naLen[0]>DULLLEN||nNodeNum<DULLRATE))
			return nKStyle;
		break;
	case KLINETL_DOWN:
	//下跌途中(*),短期均价相对长期最高最低中心价而言
		if(fPriceLv>HIGHPRICERATE)	return 0;
		if(fPriceLv<LOWPRICERATE )	return 0;
		if((faSlope[0]<DOWNLIMIT||faWeight[0]<DOWNLIMITW)&&(naLen[0]>DULLLEN||nNodeNum<DULLRATE))
			return nKStyle;
		break;
	case KLINETL_TOSS:
	case KLINETL_DULL:
	//横盘(*),对大区段而言
		if(nNodeNum>DULLRATE)	return nKStyle;
		if(faWeight[0]<UPLIMITW&&faWeight[0]>DOWNLIMITW) return nKStyle;
		break;
	case KLINETL_HIGHTOSS:
	//高档震荡(*)
	case KLINETL_HIGHDULL:
	//高档横盘(*)
		if(fPriceLv<HIGHPRICERATE)	return 0;
		if(nNodeNum>DULLRATE)	return nKStyle;
		if(faWeight[0]<UPLIMITW&&faWeight[0]>DOWNLIMITW) return nKStyle;
		break;
	case KLINETL_LOWTOSS:
	//低档震荡(*)
	case KLINETL_LOWDULL:
	//低档横盘(*)
		if(fPriceLv>LOWPRICERATE)	return 0;
		if(nNodeNum>DULLRATE)	return nKStyle;
		if(faWeight[0]<UPLIMITW&&faWeight[0]>DOWNLIMITW) return nKStyle;
		break;
	case KLINETL_FASTUP:
	//急升(*)
		if(faSlope[0]>FASTUPLIMIT)   return nKStyle;
		break;
	case KLINETL_FASTDOWN:
	//急跌(*)
		if(faSlope[0]<FASTDOWNLIMIT) return nKStyle;
		break;
	case KLINETL_SLOWUP:
	//缓升(*)
		if(faSlope[0]>SLOWUPLIMIT&&faWeight[0]>SLOWUPLIMITW) return 0;
		if(faSlope [0]>UPLIMIT)		return nKStyle;
		if(faWeight[0]>UPLIMITW)	return nKStyle;
		break;
	case KLINETL_SLOWDOWN:
	//缓跌(*)
		if(faSlope[0]<SLOWDOWNLIMIT&&faWeight[0]<SLOWDOWNLIMITW) return 0;
		if(faSlope [0]<DOWNLIMIT)		return nKStyle;
		if(faWeight[0]<DOWNLIMITW)		return nKStyle;
		break;
	case KLINETL_HIGHFASTUP:
	//高档急升
		if(fPriceLv<HIGHPRICERATE)		return 0;
		if(faSlope[0]>FASTUPLIMIT)   	return nKStyle;
		break;
	case KLINETL_HIGHFASTDOWN:
	//高档急跌
		if(fPriceLv<HIGHPRICERATE)		return 0;
		if(faSlope[0]<FASTDOWNLIMIT) 	return nKStyle;
		break;
	case KLINETL_HIGHSLOWUP:
	//高档缓升
		if(fPriceLv<HIGHPRICERATE) return 0;
		if(faSlope[0]<SLOWUPLIMIT&&faSlope[0]>UPLIMIT)			return nKStyle;
		if(faWeight[0]<SLOWUPLIMITW&&faWeight[0]>UPLIMITW)		return nKStyle;
		break;
	case KLINETL_HIGHSLOWDOWN:
	//高档缓跌
		if(fPriceLv<HIGHPRICERATE)		return 0;
		if(faSlope[0]>SLOWDOWNLIMIT&&faSlope[0]<DOWNLIMIT)		return nKStyle;
		if(faWeight[0]>SLOWDOWNLIMITW&&faWeight[0]<DOWNLIMITW)	return nKStyle;
		break;
	case KLINETL_LOWFASTUP:
	//低档急升
		if(fPriceLv>LOWPRICERATE)		return 0;
		if(faSlope[0]>FASTUPLIMIT)   	return nKStyle;
		break;
	case KLINETL_LOWFASTDOWN:
	//低档急跌
		if(fPriceLv>LOWPRICERATE)		return 0;
		if(faSlope[0]<FASTDOWNLIMIT) 	return nKStyle;
		break;
	case KLINETL_LOWSLOWUP:
	//低档缓升
		if(fPriceLv>LOWPRICERATE)		return 0;
		if(faSlope[0]<SLOWUPLIMIT&&faSlope[0]>UPLIMIT)			return nKStyle;
		if(faWeight[0]<SLOWUPLIMITW&&faWeight[0]>UPLIMITW)		return nKStyle;
		break;
	case KLINETL_LOWSLOWDOWN:
	//低档缓跌
		if(fPriceLv>LOWPRICERATE)		return 0;
		if(faSlope[0]>SLOWDOWNLIMIT&&faSlope[0]<DOWNLIMIT)		return nKStyle;
		if(faWeight[0]>SLOWDOWNLIMITW&&faWeight[0]<DOWNLIMITW)	return nKStyle;
		break;

	case KLINETL_HIGHTOSS1:
	//高档震荡初期
		break;
	case KLINETL_HIGHTOSS2:
	//高档震荡中期
		break;
	case KLINETL_HIGHTOSS3:
	//高档震荡末期
		break;
	case KLINETL_LOWTOSS1:
	//低档震荡初期
		break;
	case KLINETL_LOWTOSS2:
	//低档震荡中期
		break;
	case KLINETL_LOWTOSS3:
	//低档震荡末期
		break;
	case KLINETL_HIGHDULL1:
	//高档横盘初期
		break;
	case KLINETL_HIGHDULL2:
	//高档横盘中期
		break;
	case KLINETL_HIGHDULL3:
	//高档横盘末期
		break;
	case KLINETL_LOWDULL1:
	//低档横盘初期
		break;
	case KLINETL_LOWDULL2:
	//低档横盘中期
		break;
	case KLINETL_LOWDULL3:
	//低档横盘末期
		break;
	}
	return 0;
}

//判断双根K线形态
//是否pAnalyData中序号为nNo的K线片段和其前一K线片段具有nKLineStyle形态
//不具有时返回0,否则返回nKStyle
short MyStrategy::isTwinK_Style(struct AnalyData *pAnalyData,short nNo,short nKStyle)
{
	short   nPrevUpDown,nThisUpDown;
	short   nPrevSize ,nThisSize ;
	float   fPrevOpen ,fThisOpen ;
	float   fPrevClose,fThisClose;
	if(nNo<1)  return 0;
	nPrevUpDown = OddK_UpDown(pAnalyData,nNo-1);
	nThisUpDown = OddK_UpDown(pAnalyData,nNo  );
	nPrevSize   = OddK_Size (pAnalyData,nNo-1);
	nThisSize   = OddK_Size (pAnalyData,nNo  );
	fPrevOpen   = pAnalyData[nNo-1].Open;
	fThisOpen   = pAnalyData[nNo].Open;
	fPrevClose  = pAnalyData[nNo-1].Close;
	fThisClose  = pAnalyData[nNo].Close;
	switch(nKStyle)
	{
	case KLINET_ASCENT:
	//攀援线
		if(nPrevUpDown==KLINEO_UP&&nThisUpDown==KLINEO_UP&&FloatComp(fThisOpen,fPrevClose)<0&&
		   FloatComp(fThisOpen,fPrevOpen)>0&&FloatComp(fThisClose,fPrevClose)>0)
			return nKStyle;
		break;
	case KLINET_SLIDE:
	//滑行线
		if(nPrevUpDown==KLINEO_DOWN&&nThisUpDown==KLINEO_DOWN&&FloatComp(fThisOpen,fPrevClose)>0&&
		   FloatComp(fThisOpen,fPrevOpen)<0&&FloatComp(fThisClose,fPrevClose)<0)
			return nKStyle;
		break;
	case KLINET_JMPUP:
	//跳高线
		if(nPrevUpDown!=KLINEO_DOWN&&nThisUpDown!=KLINEO_DOWN&&
		   FloatComp(fThisOpen,fPrevClose)>0)
			return nKStyle;
		break;
	case KLINET_JMPDOWN:
	//跳水线
		if(nPrevUpDown!=KLINEO_UP&&nThisUpDown!=KLINEO_UP&&
		   FloatComp(fThisOpen,fPrevClose)<0)
			return nKStyle;
		break;
	case KLINET_CRACK:
	//压迫线
		if(nPrevUpDown==KLINEO_UP&&nThisUpDown==KLINEO_DOWN&&
		   FloatComp(fThisOpen,fPrevOpen)>0)
			return nKStyle;
		break;
	case KLINET_FLATTER:
	//奉承线
		if(nPrevUpDown==KLINEO_DOWN&&nThisUpDown==KLINEO_UP&&
		   nPrevSize==KLINEO_BIG&&nThisUpDown!=KLINEO_SMALL&&
		   FloatComp(fThisClose,fPrevClose)<0)
			return nKStyle;
		break;
	case KLINET_ABANDON:
	//弃子线
		if(nPrevUpDown==KLINEO_UP&&nThisUpDown==KLINEO_UP&&
		   FloatComp(fThisOpen,fPrevClose)>0&&nThisSize==KLINEO_SMALL)
			return nKStyle;
		if(nPrevUpDown==KLINEO_DOWN&&nThisUpDown==KLINEO_DOWN&&
		   FloatComp(fThisOpen,fPrevClose)<0&&nThisSize==KLINEO_SMALL)
			return nKStyle;
		if(nPrevUpDown==KLINEO_UP&&nThisUpDown==KLINEO_DOWN&&
		   nPrevSize==KLINEO_BIG&&nThisUpDown==KLINEO_SMALL&&
		   FloatComp(fThisOpen,fPrevClose)>0)
			return nKStyle;
		if(nPrevUpDown==KLINEO_DOWN&&nThisUpDown==KLINEO_UP&&
		   nPrevSize==KLINEO_BIG&&nThisUpDown==KLINEO_SMALL&&
		   FloatComp(fThisOpen,fPrevClose)<0)
			return nKStyle;
		break;
	case KLINET_COVER:
	//覆盖线
		if(nPrevUpDown==KLINEO_UP&&nThisUpDown==KLINEO_DOWN&&
		   nPrevSize==KLINEO_BIG&&nThisUpDown!=KLINEO_SMALL&&
		   FloatComp(fThisOpen,fPrevClose)>0&&FloatComp(fThisClose,fPrevOpen)>0&&FloatComp(fThisClose,fPrevClose)<0)
			return nKStyle;
		break;
	case KLINET_CUTIN:
	//切入线
		if(nPrevUpDown==KLINEO_DOWN&&nThisUpDown==KLINEO_UP&&
		   nPrevSize==KLINEO_BIG&&nThisUpDown!=KLINEO_SMALL&&
		   FloatComp(fThisClose,fPrevClose)>0&&FloatComp(fThisClose,fPrevOpen)<0&&FloatComp(fThisOpen,fPrevClose)<0)
			return nKStyle;
		break;
	case KLINET_FAKECOVER:
	//类覆盖线
		if(nPrevUpDown==KLINEO_UP&&nThisUpDown==KLINEO_DOWN&&
		   FloatComp(fThisOpen,fPrevClose)>0&&FloatComp(fThisOpen,fPrevOpen)<0)
			return nKStyle;
		break;
	case KLINET_FAKECUTIN:
	//类切入线
		if(nPrevUpDown==KLINEO_DOWN&&nThisUpDown==KLINEO_UP&&
		   FloatComp(fThisClose,fPrevClose)>0&&FloatComp(fThisClose,fPrevOpen)<0&&FloatComp(fThisOpen,fPrevClose)<0)
			return nKStyle;
		break;
	case KLINET_COORD:
	//并列线(*)
		if(isTwinK_Style(pAnalyData,nNo,KLINET_COORDUP  )==KLINET_COORDUP  ) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_COORDDOWN)==KLINET_COORDDOWN) return nKStyle;
		break;
	case KLINET_COORDUP:
	//并列阳线
		if(nPrevUpDown==KLINEO_UP&&nThisUpDown==KLINEO_UP     &&
		   fabs(fPrevOpen -fThisOpen )<MIN(fPrevClose-fPrevOpen,fThisClose-fThisOpen)*0.6 &&
		   fabs(fPrevClose-fThisClose)<MIN(fPrevClose-fPrevOpen,fThisClose-fThisOpen)*0.6 &&
		   (fPrevClose-fPrevOpen )/(fThisClose-fThisOpen)<1.3 &&
		   (fPrevClose-fPrevOpen )/(fThisClose-fThisOpen)>0.7 )
				return nKStyle;
		break;
	case KLINET_COORDDOWN:
	//并列阴线
		if(nPrevUpDown==KLINEO_DOWN&&nThisUpDown==KLINEO_DOWN&&
		   fabs(fPrevOpen -fThisOpen )<MIN(fPrevOpen-fPrevClose,fThisOpen-fThisClose)*0.6 &&
		   fabs(fPrevClose-fThisClose)<MIN(fPrevOpen-fPrevClose,fThisOpen-fThisClose)*0.6 &&
		   (fPrevOpen-fPrevClose)/(fThisOpen-fThisClose)<1.3 &&
		   (fPrevOpen-fPrevClose)/(fThisOpen-fThisClose)>0.7 )
			return nKStyle;
		break;
	case KLINET_CONTAIN:
	//包容线(*)
		if(isTwinK_Style(pAnalyData,nNo,KLINET_CUU)==KLINET_CUU) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_CUD)==KLINET_CUD) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_CUL)==KLINET_CUL) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_CDU)==KLINET_CDU) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_CDD)==KLINET_CDD) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_CDL)==KLINET_CDL) return nKStyle;
		break;
	case KLINET_CUU:
	//阳包阳
		if(MAX(fPrevOpen,fPrevClose)-MAX(fThisOpen,fThisClose)>-COMPPREC) break;
		if(MIN(fPrevOpen,fPrevClose)-MIN(fThisOpen,fThisClose)< COMPPREC) break;
		if(nThisUpDown!=KLINEO_UP) break;
		if(nPrevUpDown!=KLINEO_UP) break;
		return nKStyle;
	case KLINET_CUD:
	//阳包阴
		if(MAX(fPrevOpen,fPrevClose)-MAX(fThisOpen,fThisClose)>-COMPPREC) break;
		if(MIN(fPrevOpen,fPrevClose)-MIN(fThisOpen,fThisClose)< COMPPREC) break;
		if(nThisUpDown!=KLINEO_UP  ) break;
		if(nPrevUpDown!=KLINEO_DOWN) break;
		return nKStyle;
	case KLINET_CUL:
	//阳包星
		if(MAX(fPrevOpen,fPrevClose)-MAX(fThisOpen,fThisClose)>-COMPPREC) break;
		if(MIN(fPrevOpen,fPrevClose)-MIN(fThisOpen,fThisClose)< COMPPREC) break;
		if(nThisUpDown!=KLINEO_UP   ) break;
		if(nPrevUpDown!=KLINEO_LEVEL) break;
		if(OddK_Shadow(pAnalyData,nNo-1)==KLINEO_NOHEADFOOT)	break;
		if(OddK_Shadow(pAnalyData,nNo-1)==KLINEO_NOHEAD    )	break;
		if(OddK_Shadow(pAnalyData,nNo-1)==KLINEO_NOFOOT    )	break;
		return nKStyle;
	case KLINET_CDU:
	//阴包阳
		if(MAX(fPrevOpen,fPrevClose)-MAX(fThisOpen,fThisClose)>-COMPPREC) break;
		if(MIN(fPrevOpen,fPrevClose)-MIN(fThisOpen,fThisClose)< COMPPREC) break;
		if(nThisUpDown!=KLINEO_DOWN) break;
		if(nPrevUpDown!=KLINEO_UP  ) break;
		return nKStyle;
	case KLINET_CDD:
	//阴包阴
		if(MAX(fPrevOpen,fPrevClose)-MAX(fThisOpen,fThisClose)>-COMPPREC) break;
		if(MIN(fPrevOpen,fPrevClose)-MIN(fThisOpen,fThisClose)< COMPPREC) break;
		if(nThisUpDown!=KLINEO_DOWN) break;
		if(nPrevUpDown!=KLINEO_DOWN) break;
		return nKStyle;
	case KLINET_CDL:
	//阴包星
		if(MAX(fPrevOpen,fPrevClose)-MAX(fThisOpen,fThisClose)>-COMPPREC) break;
		if(MIN(fPrevOpen,fPrevClose)-MIN(fThisOpen,fThisClose)< COMPPREC) break;
		if(nThisUpDown!=KLINEO_DOWN ) break;
		if(nPrevUpDown!=KLINEO_LEVEL) break;
		if(OddK_Shadow(pAnalyData,nNo-1)==KLINEO_NOHEADFOOT)	break;
		if(OddK_Shadow(pAnalyData,nNo-1)==KLINEO_NOHEAD    )	break;
		if(OddK_Shadow(pAnalyData,nNo-1)==KLINEO_NOFOOT    )	break;
		return nKStyle;
	case KLINET_GRAVID:
	//孕妊线(*)
		if(isTwinK_Style(pAnalyData,nNo,KLINET_GUU)==KLINET_GUU) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_GUD)==KLINET_GUD) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_GUL)==KLINET_GUL) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_GDU)==KLINET_GDU) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_GDD)==KLINET_GDD) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_GDL)==KLINET_GDL) return nKStyle;
		break;
	case KLINET_GUU:
	//阳孕阳
		if(MAX(fPrevOpen,fPrevClose)-MAX(fThisOpen,fThisClose)< COMPPREC) break;
		if(MIN(fPrevOpen,fPrevClose)-MIN(fThisOpen,fThisClose)>-COMPPREC) break;
		if(nPrevUpDown!=KLINEO_UP) break;
		if(nThisUpDown!=KLINEO_UP) break;
		return nKStyle;
	case KLINET_GUD:
	//阳孕阴
		if(MAX(fPrevOpen,fPrevClose)-MAX(fThisOpen,fThisClose)< COMPPREC) break;
		if(MIN(fPrevOpen,fPrevClose)-MIN(fThisOpen,fThisClose)>-COMPPREC) break;
		if(nPrevUpDown!=KLINEO_UP  ) break;
		if(nThisUpDown!=KLINEO_DOWN) break;
		return nKStyle;
	case KLINET_GUL:
	//阳孕星
		if(MAX(fPrevOpen,fPrevClose)-MAX(fThisOpen,fThisClose)< COMPPREC) break;
		if(MIN(fPrevOpen,fPrevClose)-MIN(fThisOpen,fThisClose)>-COMPPREC) break;
		if(nPrevUpDown!=KLINEO_UP   ) break;
		if(nThisUpDown!=KLINEO_LEVEL) break;
		if(OddK_Shadow(pAnalyData,nNo)==KLINEO_NOHEADFOOT)	break;
		if(OddK_Shadow(pAnalyData,nNo)==KLINEO_NOHEAD    )	break;
		if(OddK_Shadow(pAnalyData,nNo)==KLINEO_NOFOOT    )	break;
		return nKStyle;
	case KLINET_GDU:
	//阴孕阳
		if(MAX(fPrevOpen,fPrevClose)-MAX(fThisOpen,fThisClose)< COMPPREC) break;
		if(MIN(fPrevOpen,fPrevClose)-MIN(fThisOpen,fThisClose)>-COMPPREC) break;
		if(nPrevUpDown!=KLINEO_DOWN) break;
		if(nThisUpDown!=KLINEO_UP  ) break;
		return nKStyle;
	case KLINET_GDD:
	//阴孕阴
		if(MAX(fPrevOpen,fPrevClose)-MAX(fThisOpen,fThisClose)< COMPPREC) break;
		if(MIN(fPrevOpen,fPrevClose)-MIN(fThisOpen,fThisClose)>-COMPPREC) break;
		if(nPrevUpDown!=KLINEO_DOWN) break;
		if(nThisUpDown!=KLINEO_DOWN) break;
		return nKStyle;
	case KLINET_GDL:
	//阴孕星
		if(MAX(fPrevOpen,fPrevClose)-MAX(fThisOpen,fThisClose)< COMPPREC) break;
		if(MIN(fPrevOpen,fPrevClose)-MIN(fThisOpen,fThisClose)>-COMPPREC) break;
		if(nPrevUpDown!=KLINEO_DOWN ) break;
		if(nThisUpDown!=KLINEO_LEVEL) break;
		if(OddK_Shadow(pAnalyData,nNo)==KLINEO_NOHEADFOOT)  break;
		if(OddK_Shadow(pAnalyData,nNo)==KLINEO_NOHEAD    )	break;
		if(OddK_Shadow(pAnalyData,nNo)==KLINEO_NOFOOT    )	break;
		return nKStyle;
	case KLINET_DRAGUP:
	//上拉线
		if(nPrevUpDown==KLINEO_DOWN&&nThisUpDown==KLINEO_UP&&FloatComp(fThisClose,fPrevOpen)>0&&
		   FloatComp(fThisOpen,fPrevClose)>0&&FloatComp(fThisOpen,fPrevOpen)<0)
			return nKStyle;
		break;
	case KLINET_DRAGDOWN:
	//下拖线
		if(nPrevUpDown==KLINEO_UP&&nThisUpDown==KLINEO_DOWN&&
		   FloatComp(fThisOpen,fPrevOpen)>0&&FloatComp(fThisOpen,fPrevClose)<0)
			return nKStyle;
		break;
	case KLINET_SURRENDER:
	//归顺线
		if(nPrevUpDown==KLINEO_DOWN&&nThisUpDown==KLINEO_UP&&
		   FloatComp(fThisOpen,fPrevOpen)==0)
			return nKStyle;
		break;
	case KLINET_REBELL:
	//反叛线
		if(nPrevUpDown==KLINEO_UP&&nThisUpDown==KLINEO_DOWN&&
		   FloatComp(fThisOpen,fPrevOpen)==0)
			return nKStyle;
		break;
	case KLINET_FEIGN:
	//伪K线(*)
		if(isTwinK_Style(pAnalyData,nNo,KLINET_FEIGNUP  )==KLINET_FEIGNUP  ) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_FEIGNDOWN)==KLINET_FEIGNDOWN) return nKStyle;
		break;
	case KLINET_FEIGNUP:
	//伪阳线
		if(nPrevUpDown==KLINEO_UP&&nThisUpDown==KLINEO_UP&&
		   FloatComp(fThisClose,fPrevClose)<0&&FloatComp(fThisOpen,fPrevOpen)<0)
			return nKStyle;
		break;
	case KLINET_FEIGNDOWN:
	//伪阴线
		if(nPrevUpDown==KLINEO_DOWN&&nThisUpDown==KLINEO_DOWN&&
		   FloatComp(fThisClose,fPrevClose)>0&&FloatComp(fThisOpen,fPrevOpen)>0)
			return nKStyle;
		break;
	case KLINET_NIP:
	//中夹线(*)
		if(isTwinK_Style(pAnalyData,nNo,KLINET_NDU)==KLINET_NDU) return nKStyle;
		if(isTwinK_Style(pAnalyData,nNo,KLINET_NUD)==KLINET_NUD) return nKStyle;
		break;
	case KLINET_NDU:
	//阴夹阳
		if(nNo<2) return 0;
		if(nPrevUpDown!=KLINEO_UP  ) return 0;
		if(nThisUpDown!=KLINEO_DOWN) return 0;
		if(OddK_UpDown(pAnalyData,nNo-2)!=KLINEO_DOWN) 	 return 0;
		if(OddK_Size  (pAnalyData,nNo-2)==KLINEO_SMALL)  return 0;
		if(nPrevSize==KLINEO_SMALL)  					 return 0;
		if(nThisSize==KLINEO_SMALL)  					 return 0;
		if(pAnalyData[nNo-2].Close>fPrevClose) return 0;
		if(pAnalyData[nNo-2].Close>fPrevOpen)  return nKStyle;
		break;
	case KLINET_NUD:
	//阳夹阴
		if(nNo<2) return 0;
		if(nPrevUpDown!=KLINEO_DOWN) return 0;
		if(nThisUpDown!=KLINEO_UP  ) return 0;
		if(OddK_UpDown(pAnalyData,nNo-2)!=KLINEO_UP) 	 return 0;
		if(OddK_Size  (pAnalyData,nNo-2)==KLINEO_SMALL)  return 0;
		if(nPrevSize==KLINEO_SMALL)  					 return 0;
		if(nThisSize==KLINEO_SMALL)  					 return 0;
		if(pAnalyData[nNo-2].Close>fPrevOpen) return 0;
		if(pAnalyData[nNo-2].Close>fPrevClose)  return nKStyle;
		break;
	}
	return 0;
}






