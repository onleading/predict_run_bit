// RMReport.cpp: implementation of the RMReport class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tdxw.h"
#include "RMReport.h"
#include "GridView.h"
#include "comview2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction	
//////////////////////////////////////////////////////////////////////
RMReport::RMReport(LPCOMINFO pComInfo):Grid(1,FALSE,pComInfo)
{
	memset(fData,0,TOTAL_BLOCKNUM*100*sizeof(float));
	pZsAnalyData = NULL;
	ZsAnalyDataNum = 0;
	m_lQJBgnDate = m_lQJEndDate = g_lOpenRq;
	m_bTodayRM = FALSE;
	SetPartMode(TRUE);

	m_bJSZJFlag = (pComInfo->nUnitStyle==RMREPORT_ZJ);
	m_lpnWidth = lpnRMItemWidth_All;
}

RMReport::~RMReport()
{
	TDELA(pZsAnalyData);
	g_GH.nCurBlockFlag=CurBlockFlag;
}

#define BlockFL_Num		7
char *	BlockFLStr[]	= {"所有板块","自定义板块","地区板块","行业板块","概念板块","组合板块","证监会行业板块"};
UINT	BlockFLMsgID[]	= {ID_ALLBLOCKFX,ID_SELFBLOCKFX,ID_DYBLOCKFX,ID_HYBLOCKFX,ID_CONCEPTFX,ID_ZHBBLOCKFX,ID_ZJHHYBLOCKFX};

void RMReport::InitReport(int flag)
{
	int nHqNum = sizeof(nColToXh_Hq)/sizeof(int);
	int nZjNum = sizeof(nColToXh_Zj)/sizeof(int);
	nTotalCol = (m_bJSZJFlag?nZjNum:nHqNum)-m_nFixedLeadCol;	//不包括块板名称
    nCurRow = nStartRow = nStartCol = 0;

	nCurSortIndex = 2;
	if(m_bJSZJFlag)
		nCurSortIndex = 1;
	bAscendSort = TRUE;
	
	if(flag>=0 && flag<BlockFL_Num)
		CurBlockFlag = flag;
	else
		CurBlockFlag = 0;

	CurBlockFlag = 6;

	if(CurBlockFlag==BlockFL_Num-1)
		m_lpnWidth = lpnRMItemWidth_Long_All;
	else
		m_lpnWidth = lpnRMItemWidth_All;

	m_pColToXh = m_bJSZJFlag?nColToXh_Zj:nColToXh_Hq;

	g_bSepEsc = TRUE;
	AfxGetMainWnd()->SetWindowText (_F((CString)"板块指数-"+BlockFLStr[CurBlockFlag]));

	Start_Calc();
}

void RMReport::Start_Calc()
{
	int i;
	switch(CurBlockFlag)
	{
	case 1:
		nTotalRow = g_nBlockNum;
		for(i = 0;i < g_nBlockNum;i++)
		{
			strcpy(m_BlockInf[i].strBlockName,g_BlockInfo[i].Name);
			m_BlockInf[i].nBlockNo = i+2;
		}
		strcpy(m_BlockInf[nTotalRow].strBlockName,_F("自选股"));
		m_BlockInf[nTotalRow].nBlockNo = 0;
		++nTotalRow;
		strcpy(m_BlockInf[nTotalRow].strBlockName,_F("股票池"));
		m_BlockInf[nTotalRow].nBlockNo = 1;
		++nTotalRow;
		break;
	case 2:
		nTotalRow = TOTAL_DY_NUM;
		for(i = 0;i < TOTAL_DY_NUM;i++)
		{
			strcpy(m_BlockInf[i].strBlockName,_F(g_cwAddrTopic[i]));
			m_BlockInf[i].nBlockNo = i+DY_START;
		}
		break;
	case 3:
		nTotalRow = TOTAL_HY_NUM;
		for(i = 0;i < TOTAL_HY_NUM;i++)
		{
			strcpy(m_BlockInf[i].strBlockName,_F(g_cwHYstr[i]));
			m_BlockInf[i].nBlockNo = i+HY_START;
		}
		break;
	case 4:
		nTotalRow = g_ConceptBlockNum;
		for(i = 0;i < g_ConceptBlockNum;i++)
		{
			strcpy(m_BlockInf[i].strBlockName,_F(g_ConceptBlock[i].strBlockName));
			m_BlockInf[i].nBlockNo = i+CON_START;
		}
		break;
	case 5:
		nTotalRow = g_ZHBBlockNum;
		for(i = 0;i < g_ZHBBlockNum;i++)
		{
			strcpy(m_BlockInf[i].strBlockName,_F(g_ZHBBlock[i].zhmc));
			m_BlockInf[i].nBlockNo = i+ZHB_START;
		}
		break;
	case 6:
		nTotalRow = g_pConstMng->GetZJHHYBlockNum();
		for(i = 0;i < nTotalRow;i++)
		{
			char *strHYName = g_pConstMng->GetHYFromCommandIDXH(i);
			strcpy(m_BlockInf[i].strBlockName,_F(strHYName));
			m_BlockInf[i].nBlockNo = i+ZJHHY_START;
		}
		break;
	default:
		nTotalRow = g_nBlockNum;
		for(i = 0;i < nTotalRow;i++)
		{
			strcpy(m_BlockInf[i].strBlockName,g_BlockInfo[i].Name);
			m_BlockInf[i].nBlockNo = i+2;
		}
		//
		strcpy(m_BlockInf[nTotalRow].strBlockName,_F("自选股"));
		m_BlockInf[nTotalRow].nBlockNo = 0;
		++nTotalRow;
		strcpy(m_BlockInf[nTotalRow].strBlockName,_F("股票池"));
		m_BlockInf[nTotalRow].nBlockNo = 1;
		++nTotalRow;
		for(i = 0;i < TOTAL_DY_NUM;i++)
		{
			strcpy(m_BlockInf[nTotalRow+i].strBlockName,_F(g_cwAddrTopic[i]));
			m_BlockInf[nTotalRow+i].nBlockNo = i+DY_START;
		}
		nTotalRow+=TOTAL_DY_NUM;
		for(i = 0;i < TOTAL_HY_NUM;i++)
		{
			strcpy(m_BlockInf[nTotalRow+i].strBlockName,_F(g_cwHYstr[i]));
			m_BlockInf[nTotalRow+i].nBlockNo = i+HY_START;
		}
		nTotalRow+=TOTAL_HY_NUM;
		for(i = 0;i < g_ConceptBlockNum;i++)
		{
			strcpy(m_BlockInf[nTotalRow+i].strBlockName,_F(g_ConceptBlock[i].strBlockName));
			m_BlockInf[nTotalRow+i].nBlockNo = i+CON_START;
		}
		nTotalRow+=g_ConceptBlockNum;
		for(i = 0;i < g_ZHBBlockNum;i++)
		{
			strcpy(m_BlockInf[nTotalRow+i].strBlockName,_F(g_ZHBBlock[i].zhmc));
			m_BlockInf[nTotalRow+i].nBlockNo = i+ZHB_START;
		}
		nTotalRow+=g_ZHBBlockNum;
		break;
	}
	for(i = 0;i < nTotalRow;i++)
		lpnUsedStkIndex[i] = i;

	DrawFlag = DRAW_ALL;	
	m_pView->Invalidate(FALSE);
	RefreshScrollBar();	//重新组织滚动条

//	RefreshAllHq(60);
	//在得到数据之前，也要刷新，否则由于对话框的原因，会乱屏
	GetData();		//得到数据
	Sort();			//缺省为序号排序
	DrawFlag = DRAW_ALL;
	m_pView->Invalidate(FALSE);
	RefreshScrollBar();	//重新组织滚动条
}

void  RMReport::Com2Switch1()
{
	if(m_nInCom2&&theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView2)))
	{
		nCurRow = 0;
		OpenBlockHQ();
	}
}

//设置起讫日期
void  RMReport::SetDateSection(CTime date1,CTime date2)	
{
	if(!CanProcesLocalData(m_pView))
		return;
	long lDate = TimeToLong(date1);	//重新调整起始日期
	if(lDate>=g_lOpenRq)
	{
		m_lQJBgnDate = m_lQJEndDate = g_lOpenRq;
		InitReport(CurBlockFlag);
	}
	else if(pZsAnalyData && ZsAnalyDataNum>0)
	{
		if(pZsAnalyData[0].Time.Date > lDate)
			m_lQJBgnDate = pZsAnalyData[0].Time.Date;
		else
		{
			int pos = 0;
			while(pZsAnalyData[pos].Time.Date < lDate && pos < ZsAnalyDataNum)
				pos++;
			if(pos >= ZsAnalyDataNum) pos = ZsAnalyDataNum - 1;
			m_lQJBgnDate   = pZsAnalyData[pos].Time.Date;
		}
		m_lQJEndDate = TimeToLong(date2);
		if(m_lQJEndDate < m_lQJBgnDate) m_lQJEndDate = m_lQJBgnDate;
		InitReport(CurBlockFlag);
	}
}

struct QJCalcData
{
	DWORD dwSumVolume;
	float fSumAmount;
	float fSumZf;
};

void RMReport::GetStockQJInfo(MemStkInfo *tmpInfo, DWORD &dwSumVolume, float &fSumAmount, float &fSumZf, char CacheFlag)
{
	int iNo=g_pStockIO->GetIndex(tmpInfo->Code,tmpInfo->setcode);
	static map<long, QJCalcData, less<long> > QJCalcCache;

	if(CacheFlag==1)		//晴空
		QJCalcCache.clear();

	map<long, QJCalcData, less<long> >::iterator iter = QJCalcCache.find(iNo);
	if(iter!=QJCalcCache.end())
	{
		QJCalcData qcd = (*iter).second;
		dwSumVolume = qcd.dwSumVolume;
		fSumAmount = qcd.fSumAmount;
		fSumZf = qcd.fSumZf;
		return;
	}

	long nBgnData, nEndData;
	long nReadDateNum = g_pStockIO->GetAnalyDataNum(tmpInfo,PER_DAY);
	AnalyData *pAnalyData = new AnalyData[nReadDateNum+20];
	memset(pAnalyData, 0, (nReadDateNum+20)*sizeof(AnalyData));
	int nTmpDateNum = g_pStockIO->ReadAnalyDataFromTime(tmpInfo,pAnalyData,nReadDateNum,m_lQJBgnDate,m_lQJEndDate,PER_DAY);
	nReadDateNum = g_pStockIO->ReadAnalyDataFromTime(tmpInfo,pAnalyData,nTmpDateNum+1,0,m_lQJEndDate,PER_DAY);				//多读一天
	if(nReadDateNum > 0)
		g_pStockIO->TQData(tmpInfo,pAnalyData,0,nReadDateNum-1,nReadDateNum,1,PER_DAY);	//1为前除权
	for(nBgnData = 0;nBgnData < nReadDateNum && pAnalyData[nBgnData].Time.Date < m_lQJBgnDate;nBgnData++);	//20040907iwld 01号数据错误
	if(nBgnData>0) --nBgnData;			//找到前一天的
	for(nEndData = 0;nEndData < nReadDateNum && pAnalyData[nEndData].Time.Date < m_lQJEndDate;nEndData++);
	
	if(nBgnData >= nReadDateNum) nBgnData = nReadDateNum - 1;
	if(nEndData >= nReadDateNum) nEndData = nReadDateNum - 1;

	dwSumVolume = 0; fSumAmount = fSumZf = 0.0f;
	for(int i=nBgnData+1;i<=nEndData;i++)
	{
		dwSumVolume += pAnalyData[i].Volume;
		fSumAmount += pAnalyData[i].Amount;
	}
	dwSumVolume /= 100;
	if(pAnalyData[nBgnData].Close>COMPPREC) 
		fSumZf = (pAnalyData[nEndData].Close-pAnalyData[nBgnData].Close)/pAnalyData[nBgnData].Close;

	TDELA(pAnalyData);
	
	QJCalcData qcd={0};
	qcd.dwSumVolume = dwSumVolume;
	qcd.fSumAmount = fSumAmount;
	qcd.fSumZf = fSumZf;
	QJCalcCache[iNo] = qcd;
}

#include "ProgressWnd.h"
//!!!
void RMReport::GetData()
{
	CurrStockData		Hq={0};
	int					m_nTodayNum=0;
	//得到指数的一些信息,以便上一交易日，下一交易日有用
	ZsAnalyDataNum = g_pStockIO->GetAnalyDataNum(g_pStockIO->Get_MemStkInfo(SH_DPCODE,SH),PER_DAY);
	TDELA(pZsAnalyData);
	pZsAnalyData = new AnalyData[ZsAnalyDataNum];
	ZsAnalyDataNum = g_pStockIO->ReadAnalyDataAndCurrHQ(g_pStockIO->Get_MemStkInfo(SH_DPCODE,SH),pZsAnalyData,ZsAnalyDataNum,PER_DAY,&Hq,m_nTodayNum);

	m_bTodayRM = (m_lQJBgnDate==g_lOpenRq&&m_lQJEndDate==g_lOpenRq);

	short indexbuff[MAXGPNUM];
	int blockgpNum,i,j;
	MemStkInfo *tmpInfo;
	CurrStockData CurrHQ;
	float	ftmp=0.00;
	short   TruepStockNum=0;
	double   AriHike=0.00;
	float	sLTGB=0.00;
	double	AddActiveCapital=0.00;
	double	AddYearYield=0.00; //累加的年净收益
	double	AddSz=0.00; //累加的总市值

	g_bDonotClose = TRUE;
	char nCacheFlag = 1;
	
	for(i = 0;i < nTotalRow;i++)
	{
		if(m_BlockInf[i].nBlockNo<DY_START)
			blockgpNum = FiltBlockCode(m_BlockInf[i].nBlockNo,indexbuff);
		else
			blockgpNum = GetGpIndex(m_BlockInf[i].nBlockNo,-1,indexbuff);
		if(blockgpNum<1)
		{
			for(j = 0;j < 100;j++) fData[i][j] = MEANLESS_DATA;
			continue;
		}
		for(j = 0;j < 100;j++) fData[i][j] = 0.0;

		AriHike = 0.0;
		TruepStockNum  = 0;
		AddActiveCapital = 0.0;
		AddYearYield = 0.0;
		AddSz = 0.0;
		float max_value = -0.2f,tmpzf;
		int	  max_value_index = -1;
	
		for(j = 0;j < blockgpNum;j++)
		{
			tmpInfo = (*g_pStockIO)[indexbuff[j]];
			g_pStockIO->ReadCurrStockData(tmpInfo,&CurrHQ);
			TICKSTAT its=tmpInfo->its;
			if(m_bJSZJFlag&&its.Minute==0)
				ReadDataTickStat(tmpInfo, &its);

			if(m_bTodayRM)
			{
				if (CurrHQ.Now<COMPPREC || CurrHQ.Close <= COMPPREC) 	
					continue;
				fData[i][2] += CurrHQ.Volume;	//成交量
				fData[i][3] += CurrHQ.Amount;	//成交金额
				tmpzf = (CurrHQ.Now - CurrHQ.Close)/CurrHQ.Close;
				if(tmpzf > 0.101f) tmpzf = 0.101f; //限定在10.1%以内,避免新股的涨幅过高对热门板块判断的影响
				if(m_bJSZJFlag&&its.Minute>0)
				{
					fData[i][9] += its.fAmount[1][0];
					fData[i][10] += its.fAmount[1][1];
					fData[i][11] += its.fAmount[2][0];
					fData[i][12] += its.fAmount[2][1];
					fData[i][13] += (its.fAmount[0][0]-its.fAmount[1][0]-its.fAmount[2][0]);
					fData[i][14] += (its.fAmount[0][1]-its.fAmount[1][1]-its.fAmount[2][1]);
				}
			}
			else 
			{
				DWORD dwSumVolume = 0;
				float fSumAmount = 0.0f;
				float fSumZf = 0.0f;
				GetStockQJInfo(tmpInfo, dwSumVolume, fSumAmount, fSumZf, nCacheFlag);
				nCacheFlag = 0;
				fData[i][2] += dwSumVolume;		//成交量
				fData[i][3] += fSumAmount;		//成交金额
				tmpzf = fSumZf;
			}
			if(tmpzf>COMPPREC)
				fData[i][19]+=1;
			AriHike += tmpzf;
			if(tmpzf >= max_value) 
			{
				max_value = tmpzf;
				if(tmpzf > COMPPREC)
					max_value_index = indexbuff[j];
			}
			sLTGB = tmpInfo->ActiveCapital;
			AddActiveCapital += sLTGB;		//总流通股本
			ftmp = CurrHQ.Now;
			if(ftmp<COMPPREC) ftmp=CurrHQ.Close;
			if(ftmp>COMPPREC && tmpInfo->J_HalfYearFlag)
			{
				AddSz+=ftmp*tmpInfo->J_zgb*10000.0;
				AddYearYield+=tmpInfo->J_jly*10000.0*12/tmpInfo->J_HalfYearFlag;
			}
			fData[i][1] += tmpzf * sLTGB;//权涨幅
			TruepStockNum ++;
		}
		int nBlockNo = m_BlockInf[i].nBlockNo-ZJHHY_START;
		char *Code = g_pLcIdxMng->GetIdxStkCode(nBlockNo);
		MemStkInfo *pIdxInfo = g_pStockIO->Get_MemStkInfo(Code, SZ);
		if(!pIdxInfo) continue;
		CURRDATA iData = {0};
		g_pStockIO->ReadCurrStockData(pIdxInfo, &iData);
		fData[i][0] = iData.Now;
		if(iData.Close>COMPPREC) 
			fData[i][1] = 100*(iData.Now-iData.Close)/iData.Close;
		fData[i][2] = iData.Volume;		//成交量
		fData[i][3] = iData.Amount;		//成交金额
		
		fData[i][7] =  max_value_index;
		fData[i][8] =  blockgpNum;
		/*
		fData[i][0] =  AriHike / TruepStockNum * 100;
		if(AddActiveCapital > 1)
			fData[i][1] /= AddActiveCapital;
		else
			fData[i][1] = 0.00;
		fData[i][1] =  fData[i][1]*100;
		*/
		if(AddActiveCapital > 1)
			fData[i][5] =  fData[i][2] / 10000 / AddActiveCapital * 100 * 100;
		else
			fData[i][5] = 0.00;
		if(AddYearYield > COMPPREC)
			fData[i][6] =  AddSz/AddYearYield;
		else
			fData[i][6] =  0.00;
	}
	double AddAmount = 0.0;
	for (i = 0;i < nTotalRow;i++)
	{
		if(fData[i][3]==MEANLESS_DATA) continue;
		AddAmount += fData[i][3];
	}
	for (i = 0;i < nTotalRow;i++)
	{
		if(fData[i][3]==MEANLESS_DATA) continue;
		if(AddAmount > COMPPREC)
			fData[i][4] = fData[i][3] / AddAmount * 100; //最后计算市场比例
		else
			fData[i][4] = 0.00;
	}
	g_bDonotClose = FALSE;
}

float RMReport::fGetSortValue(short xh)
{
	if(m_pColToXh[nCurSortIndex]==0) return 0.0; //不处理nCurSortIndex = 0的情况
	if(m_pColToXh[nCurSortIndex]==9)
	{
		if(fData[xh][8] != MEANLESS_DATA && fData[xh][8] > 2.00)
			return (fData[xh][19]*100/fData[xh][8]*10000+fData[xh][8]);
		return 0.0;
	}
	return fData[xh][m_pColToXh[nCurSortIndex]-1];
}

//排序函数
void RMReport::Sort()
{
	int i;
	if(m_pColToXh[nCurSortIndex] == 0)		//按板块名称排序
	{
		for (i = 0;i < nTotalRow;i++) 
		{
			strcpy(lpSort2[i].StrValue, m_BlockInf[lpnUsedStkIndex[i]].strBlockName); //取排序值
			lpSort2[i].nIndex = lpnUsedStkIndex[i];
			lpSort2[i].tag = 0;
		}
		qsort((void *) lpSort2, nTotalRow, sizeof(struct sort_struc2), sort_func2 );
		if (bAscendSort == FALSE)
			for ( i=0;i < nTotalRow;i++) lpnUsedStkIndex[i] = lpSort2[i].nIndex;
		else	
			for ( i=0;i < nTotalRow;i++) lpnUsedStkIndex[i] = lpSort2[nTotalRow-1-i].nIndex;
	}
	else
	{
		for (i = 0;i < nTotalRow;i++)
		{
			 lpSort3[i].fValue = fGetSortValue(lpnUsedStkIndex[i]);
			 lpSort3[i].nIndex = lpnUsedStkIndex[i];
			 lpSort3[i].nItemNum = fData[i][8];
			 lpSort3[i].tag = 0;
		}
		qsort((void *) lpSort3, nTotalRow, sizeof(struct sort_struc3), sort_func );
		if (bAscendSort == FALSE)
			for (i = 0;i < nTotalRow;i++) lpnUsedStkIndex[i] = lpSort3[i].nIndex;
		else	
			for (i = 0;i < nTotalRow;i++) lpnUsedStkIndex[i] = lpSort3[nTotalRow-1-i].nIndex;
		for(i = 0;i < nTotalRow && fGetSortValue(lpnUsedStkIndex[i]) != MEANLESS_DATA;i++);
		if(i < nTotalRow)
		{
			int startpos,endpos,tempIndex;
			startpos = i;
			for(i = startpos+1;i < nTotalRow && fGetSortValue(lpnUsedStkIndex[i]) == MEANLESS_DATA;i++);
			endpos = i - 1;
			for(i = endpos + 1;i < nTotalRow;i++)
			{
				tempIndex = lpnUsedStkIndex[i];
				lpnUsedStkIndex[i] = lpnUsedStkIndex[startpos+i-endpos-1];
				lpnUsedStkIndex[startpos+i-endpos-1] = tempIndex;
			}
		}
	}
}

void RMReport::OnSize(UINT nType, int cx, int cy)
{
	SetRect(CRect(0,0,cx,cy));
}

//计算方格左边位置,Width为返回方格宽度,nCol表示列顺序中绝对序号
int RMReport::GetCellLeft(int nCol,int & nWidth) 
{   
	int i=0,xtmp=nCellHeadWidth ;
	if(m_nFixedLeadCol<1) i=nStartCol;	//没有固定的派头列
	while(i < nCol)
	{   
		xtmp+=(int)(m_lpnWidth[m_pColToXh[i]]*FontSize.cx/2);
	    if (i == m_nFixedLeadCol-1) 
			i = nStartCol+m_nFixedLeadCol;
		else		
			i++;
	}
	nWidth = (int)(m_lpnWidth[m_pColToXh[nCol]]*FontSize.cx/2);
	return xtmp;
}

//画标题
void RMReport::DrawTitle(CDC *pDC)
{
	int i,j,xtmp,width;
	if(nSigTitleHeight>0&&nTitleHeight>0)
	{
		//写标题区
		CString tempStr1;
		int pos=8;
		tempStr1.Format("热门板块分析--%s",BlockFLStr[CurBlockFlag]);
		CSize size = g_d.DisplayText(pDC,pos,2,GridColor.Grid_Up_Color,0,_F(tempStr1));
		pos += size.cx+5;
		if(m_pWriteFile)	m_WriteStr+=_F(tempStr1);
		
		if(m_lQJBgnDate==g_lOpenRq&&m_lQJEndDate==g_lOpenRq)
			tempStr1.Format("今日:%s",GetTdxDateWeekStr(g_lOpenRq));
		else if(m_lQJBgnDate==m_lQJEndDate)
			tempStr1.Format("%s",GetTdxDateWeekStr(m_lQJEndDate));
		else
		{
			size = g_d.DisplayText(pDC,pos,2,VipColor.TXTCOLOR,0,_F(" 区间:"));
			pos += size.cx+5;
			if(m_pWriteFile)	m_WriteStr+=_F(" 区间:");
			CString tmpDateStr1=GetTdxDateWeekStr(m_lQJBgnDate);
			CString tmpDateStr2=GetTdxDateWeekStr(m_lQJEndDate);
			tempStr1.Format("%s 至 %s",tmpDateStr1,tmpDateStr2);
		}
		
		size = g_d.DisplayText(pDC,pos,2,GridColor.Grid_Volume_Color,0,_F(tempStr1));
		pos += size.cx+10;
		if(m_pWriteFile)	m_WriteStr+=_F(tempStr1);
		
		if(m_pColToXh[nCurSortIndex] != 0)
		{
			tempStr1.Format(" %s %s",lpszRMItemName_All[m_pColToXh[nCurSortIndex]],bAscendSort?"降序":"升序");
			size = g_d.DisplayText(pDC,pos,2,GridColor.Grid_Volume_Color,0,_F(tempStr1));
			pos += size.cx+10;
			if(m_pWriteFile)	
				m_WriteStr+=(CString)"  "+_F(tempStr1);
		}
		size = g_d.DisplayText(pDC,pos,2,VipColor.TXTCOLOR,0,_F("点右键进行设置"));
		pos += size.cx+5;	
		if(m_pWriteFile)	
		{
			m_pWriteFile->WriteString(m_WriteStr+(CString)"\n");
			m_WriteStr = "";
		}
	}

	CRect TRect(0,0,m_Rect.Width(),m_Rect.Height());
	TRect.bottom = TRect.top+nTitleHeight;
	g_d.Draw3DBar2(pDC, TRect, GridColor.GridLineColor/*RGB(128,128,128)*/, FALSE);
//	int tempTitlePos = nSigTitleHeight+6;
	int tempTitlePos = 0;
	xtmp = nCellHeadWidth;
	for(i = j = 0;j < nTotalCol+m_nFixedLeadCol-1 && xtmp < m_DRect.right;i++) //对于热门板块分析,为nTotalCol
	{
  		if (i>=m_nFixedLeadCol) 
			j = i+nStartCol;
		else	  
			j = i;
		width = (int)(m_lpnWidth[m_pColToXh[j]]*FontSize.cx/2);
		if( (nCurSortIndex != ((i>=m_nFixedLeadCol)?(nStartCol+i):i)) || (m_pColToXh[nCurSortIndex] == 0) )
			g_d.DisplayText(pDC,(j==0)?(xtmp+2):xtmp,tempTitlePos,GridColor.GridTitleTextColor,(j==0)?0:(width-3),_F(lpszRMItemName_All[m_pColToXh[j]]));
		else
		{	
			g_d.DisplayText(pDC,xtmp,tempTitlePos,GridColor.GridTitleTextColor,width-FontSize.cx/2-3,_F(lpszRMItemName_All[m_pColToXh[j]]));
			g_d.DisplayText(pDC,xtmp,tempTitlePos,GridColor.GridTitleArrowColor,width,(bAscendSort==TRUE)?_F("↓"):_F("↑"));
		}
		xtmp+=width;
		if(m_pWriteFile) 
			m_WriteStr+=_F(lpszRMItemName_All[m_pColToXh[j]])+(CString)"\t";
	}
	if(m_pWriteFile)	
	{
		m_pWriteFile->WriteString(m_WriteStr+"\n");
		m_WriteStr = "";
	}
	if(xtmp > m_DRect.right)
		nColNum = j-nStartCol-m_nFixedLeadCol;	//顺便计算列数
	else 
		nColNum = j-nStartCol-m_nFixedLeadCol+1;
	if(nColNum < 0) nColNum = 0;
}

void RMReport::GetSomeSize(CDC *pDC)
{
	FontSize = pDC->GetTextExtent("试");
	nSigTitleHeight = 0;
//	nSigTitleHeight = FontSize.cy+6;
	nSigCellHeight = 1.382f*FontSize.cy+6;
	nTitleHeight = FontSize.cy+4; //得到标题栏高度
//	nTitleHeight = 2*nSigTitleHeight+4; //得到标题栏高度
	nCellHeight =  nSigCellHeight;	//得到方格的高度
	nCellHeadWidth = 2.2*FontSize.cx;
	if(g_GH.NoXHCol) 
		nCellHeadWidth=0;
}

//主画图函数,有两种画法DRAW_PUTBITMAP和DRAW_ALL
void RMReport::DrawReport(CDC *pDC)
{
	GetDrawRect(m_DRect, -1);
	if(m_DRect.Height() < 40) return;
	int i,j;

	CREATE_GRIDMEMDC
	GetSomeSize(&memDC);

	//画栏目行,在其中初化始一些变量:列数nColNum
	if(nSigTitleHeight>0&&nTitleHeight>0)
		g_d.Bar(&memDC,0,0,m_DRect.Width(),nTitleHeight-nSigTitleHeight-3,g_GUI.MixedColor(VipColor.AXISCOLOR,25,VipColor.BACKCOLOR));
	DrawTitle(&memDC);
	//调节RowNum和ColNum
	AdjustRowColNum();	

	//开始画线
//	int tempWidth,tempLeft;
//	tempLeft = GetCellLeft(m_nFixedLeadCol+nEndCol,tempWidth);

	/*
	for(i = 0;i <= nRowNum;i++)	//画行线
	{
		if(i%2==0) continue;
		g_d.Bar(&memDC,0,nTitleHeight+i*nCellHeight,m_DRect.right,nTitleHeight+(i+1)*nCellHeight,GridColor.Grid2GridColor);
	}
	*/

	if(m_bWriteAllHQ) //导出全部时
	{
		for(j = 0;j < nTotalRow;j++)
		{
			for(i=0;i<tempColNum+m_nFixedLeadCol;i++)
			{
				if(i<m_nFixedLeadCol)
					DrawItem(&memDC,j,i);
				else
					DrawItem(&memDC,j,i+nStartCol);
			}
			if(m_pWriteFile)
			{
				m_pWriteFile->WriteString(m_WriteStr+"\n");
				m_WriteStr = "";
			}
		}
	}
	else
	{
		for(j = 0;j < tempRowNum;j++)
		{
			for(i=0;i<tempColNum+m_nFixedLeadCol;i++)
			{
				if(i<m_nFixedLeadCol)
					DrawItem(&memDC,j,i);
				else
					DrawItem(&memDC,j,i+nStartCol);
			}
			if(m_pWriteFile)
			{
				m_pWriteFile->WriteString(m_WriteStr+"\n");
				m_WriteStr = "";
			}
		}
	}
	//写左边序号内容
	for(i=0;nCellHeadWidth>3 && i<tempRowNum;i++)
		g_d.DisplayText(&memDC,0,nTitleHeight+i*nCellHeight+(nCellHeight-FontSize.cy)/2,VipColor.TXTCOLOR,nCellHeadWidth-3,"%4d",nStartRow+i+1);

	//从内存DC拷贝到屏幕DC
	pDC->BitBlt (m_DRect.left,m_DRect.top,m_DRect.Width(),m_DRect.Height(),&memDC,0,0,SRCCOPY);
	
	DrawHighLine(pDC);//画指示线

	DELETE_GRIDMEMDC

	DrawFlag = DRAW_NOREADDATA;
}

void RMReport::DrawItem(CDC *pDC,int row,int col)
{
	int tempInt = lpnUsedStkIndex[nStartRow+row];
	switch(m_pColToXh[col])
	{
	case 0://板块名称
		DrawCellText(pDC,row,col,0,m_BlockInf[tempInt].strBlockName,GridColor.Grid_CodeName_Color,TRUE);
		if(m_pWriteFile)	m_WriteStr+=m_BlockInf[tempInt].strBlockName;
		break;
	case 3://成交量
		if(fData[tempInt][2] != MEANLESS_DATA)
		{
			DrawCellText( pDC, row, col,0, GridColor.Grid_Volume_Color,MakeJE(fData[tempInt][2]));
			if(m_pWriteFile)	m_WriteStr+=MakeJE(fData[tempInt][2]);
		}
		else
			DrawCellText(pDC,row,col,0,MEANLESS_STR,GridColor.Grid_Level_Color);
		break;
	case 4://成交金额
		if(fData[tempInt][3] != MEANLESS_DATA)
		{
			DrawCellText( pDC, row, col,0, GridColor.Grid_Other_Color,MakeJE(fData[tempInt][3]));
			if(m_pWriteFile)	m_WriteStr+=MakeJE(fData[tempInt][3]);
		}
		else
			DrawCellText(pDC,row,col,0,MEANLESS_STR,GridColor.Grid_Level_Color);
		break;
	case 8:
		{
			if(fData[tempInt][7] != MEANLESS_DATA && fData[tempInt][8] != MEANLESS_DATA && fData[tempInt][7] >= COMPPREC && fData[tempInt][8] > COMPPREC)
			{
				int tmpNo = (int)fData[tempInt][7];
				MemStkInfo *tmpInfo = (*g_pStockIO)[tmpNo];
				if(tmpInfo)
				{
					DrawCellText( pDC, row, col,0,tmpInfo->Name,GetBlockGPColor(tmpInfo,GridColor.Grid_CodeName_Color),TRUE,TRUE,tmpInfo);
					if(m_pWriteFile)	m_WriteStr+=tmpInfo->Name;
					break;
				}
			}
			DrawCellText(pDC,row,col,0,MEANLESS_STR,GridColor.Grid_Level_Color,TRUE);
		}
		break;
	case 9:
		{
			if(fData[tempInt][8] != MEANLESS_DATA && fData[tempInt][8] >= COMPPREC)
			{
				int tmpZfNo = (int)fData[tempInt][19];
				int tmpNo = (int)fData[tempInt][8];
				char tmpStr[100];
				sprintf(tmpStr,"%d/%d",tmpZfNo,tmpNo);
				DrawCellText( pDC, row, col,0, VipColor.TXTCOLOR,tmpStr);
				if(m_pWriteFile)	m_WriteStr+=tmpStr;
				break;
			}
			DrawCellText(pDC,row,col,0,MEANLESS_STR,GridColor.Grid_Level_Color);
		}
		break;
	case 1:
	case 2:
		if(fData[tempInt][m_pColToXh[col]-1] != MEANLESS_DATA)
		{
			DrawCellText(pDC,row,col,0,fData[tempInt][m_pColToXh[col]-1],0.0,2);
			if(m_pWriteFile)	m_WriteStr+=FloatToStr(fData[tempInt][m_pColToXh[col]]);
		}
		else
			DrawCellText(pDC,row,col,0,MEANLESS_STR,GridColor.Grid_Level_Color);
		break;
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
		if(fData[tempInt][m_pColToXh[col]-1] != MEANLESS_DATA)
		{
			COLORREF cf = (m_pColToXh[col]%2==0)?VipColor.KPCOLOR:VipColor.KNCOLOR;
			DrawCellText(pDC,row,col,0,cf,MakeJE(fData[tempInt][m_pColToXh[col]-1]));
			if(m_pWriteFile)	m_WriteStr+=FloatToStr(fData[tempInt][m_pColToXh[col]-1]);
		}
		else
			DrawCellText(pDC,row,col,0,MEANLESS_STR,GridColor.Grid_Level_Color);		
		break;
	default:
		if(fData[tempInt][m_pColToXh[col]-1] != MEANLESS_DATA)
		{
			DrawCellText(pDC,row,col,0,fData[tempInt][m_pColToXh[col]-1],2,VipColor.TXTCOLOR);
			if(m_pWriteFile)	m_WriteStr+=FloatToStr(fData[tempInt][m_pColToXh[col]-1]);
		}
		else
			DrawCellText(pDC,row,col,0,MEANLESS_STR,GridColor.Grid_Level_Color);		
		break;
	}
	if(m_pWriteFile)
		m_WriteStr+="\t";
}

//当鼠标点中一行时
void RMReport::OnClick(CDC *pDC,CPoint point,BOOL IsRight)
{
	int temp;
	if(point.y > m_Rect.top + nTitleHeight)
	{
		temp = (point.y-m_Rect.top-nTitleHeight) / nCellHeight ;
		temp = max(0,temp);
		temp = min(temp,nRowNum-1);
		DrawHighLine(pDC);
		nCurRow = temp+nStartRow;
		DrawHighLine(pDC);
	}
	if(IsRight) //如果按右键
	{
		return;
		CNewMenu menu;
		menu.LoadMenu (IDR_RIGHTMENU_HIS);
//		menu.LoadToolBar(IDR_MAINFRAME);
		CNewMenu *pPopup=(CNewMenu *)menu.GetSubMenu(3); //历史分析右键菜单
		pPopup->SetDefaultItem(ID_OPENBLOCKHQ);
		(pDC->GetWindow())->ClientToScreen (&point);
		pPopup->TrackPopupMenu (TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,m_pView/*AfxGetMainWnd()*/);
	}
}

void RMReport::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int Width,Left;
	CClientDC dc(m_pView);

	OnClick (&dc,point,FALSE);
	if(point.y > m_Rect.top+nTitleHeight) return;
	//选某列
	for(int i = 0;i < m_nFixedLeadCol + (nEndCol - nStartCol + 1);i++)   // 包括最后一列
	{
		CRgn m_rgn;
		Left = GetCellLeft((i<m_nFixedLeadCol)?i:(i+nStartCol),Width);
		m_rgn.CreateRectRgn (m_Rect.left+Left,m_Rect.top+nTitleHeight-nCellHeight,m_Rect.left+Left+Width,m_Rect.top+nTitleHeight);
		if(m_rgn.PtInRegion (point))
		{
			if( (nCurSortIndex != i && i < m_nFixedLeadCol) || (nCurSortIndex != nStartCol+i && i > m_nFixedLeadCol-1) )
			{
				bAscendSort = TRUE;
				if(i > 0)	nCurSortIndex = nStartCol + i;
				else		nCurSortIndex = i;
				Sort();
				ToSomeRow(0);
				DrawFlag = DRAW_ALL;
				DrawReport(&dc);
			}
			else
			{
				bAscendSort = !bAscendSort;
				Sort();
				ToSomeRow(0);
				DrawFlag = DRAW_ALL;
				DrawReport(&dc);
			}
		}
	}
}

void RMReport::Sleep()
{
	g_bSepEsc = FALSE;
	pSwitchBar->SetWindowPos(0,0,0,0,0,SWP_SHOWWINDOW);
}

void RMReport::WakeUp()
{
	g_bSepEsc = TRUE;
	(m_pView->GetParent())->SetWindowText (_F((CString)"报表分析-"+BlockFLStr[CurBlockFlag]));
	DrawFlag = DRAW_ALL;
	m_pView->Invalidate(FALSE);
	RefreshScrollBar();
}

void RMReport::OnCtrlLeft()
{
	if(nStartCol == 0) return;

	int xtmp,width;
	xtmp = nCellHeadWidth+(int)(m_lpnWidth[m_pColToXh[0]]*FontSize.cx/2)+(int)(m_lpnWidth[m_pColToXh[1]]*FontSize.cx/2);
	for(int i = nStartCol;i >= 0&&xtmp < m_DRect.Width();i--)
	{
		width = (int)(m_lpnWidth[m_pColToXh[i+2]]*FontSize.cx/2);
		xtmp += width;
	}
	if(xtmp > m_DRect.Width()) nStartCol = i+1;
	else if(xtmp == m_DRect.Width()) nStartCol = i;
	else nStartCol = 0;
	DrawFlag = DRAW_NOREADDATA;
	m_pView->Invalidate(FALSE);
}

void RMReport::OpenBlockHQ()
{
	if(nTotalRow > 0)
	{
		int blockno = m_BlockInf[lpnUsedStkIndex[nCurRow]].nBlockNo;
		if(blockno < DY_START) blockno-=2;

		g_bOldGridDomain=FALSE;

		if(m_pView)
		{
			if(blockno==-2)
				m_pView->SendMessage(UM_EXOPENBLOCKHQ, -1,TYPE_ZXG);
			else if(blockno==-1)
				m_pView->SendMessage(UM_EXOPENBLOCKHQ, -1,TYPE_TJG);
			else
				m_pView->SendMessage(UM_EXOPENBLOCKHQ, blockno,-1);
		}
		g_bOldGridDomain=TRUE;
	}
}

int  RMReport::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_PAINT:
		m_pView->Invalidate(FALSE);
		return 1;
	}
	return 0;
}

void RMReport::InsertSwitch(int nSCType)
{
	m_bFixedGrid = FALSE;
	pSwitchBar->ClearAllLabel();
	for(int i=0;i<BlockFL_Num;i++)
		pSwitchBar->InsertSwitchLabel(i,_F(BlockFLStr[i]),WM_COMMAND,BlockFLMsgID[i]);
}

void RMReport::SetCurLabel(int n)
{
	pSwitchBar->SetCurLabel(n);
}
