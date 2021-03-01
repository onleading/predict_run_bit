// QJReport.cpp: implementation of the QJReport class.
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "tdxw.h"
#include "QJReport.h"
#include "ProgressWnd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

QJReport::QJReport():Grid(2,TRUE)
{
	memset(fData,0,sizeof(struct QJData_Struct)*MAXGPNUM*11);

	pZsAnalyData = NULL;
	ZsAnalyDataNum = 0;
	//m_lQJEndDate值和m_lQJBgnDate值初始都等于g_lQJEndDate和g_lQJBgnDate
	if(g_lQJEndDate==0)
		g_lQJEndDate = g_lOpenRq;
	m_lQJEndDate	= g_lQJEndDate;
	if(g_lQJBgnDate==0)
	{
		CTime theTime(m_lQJEndDate/10000,(m_lQJEndDate%10000)/100,(m_lQJEndDate%10000)%100,0,0,0); 
		CTimeSpan timeSpan(15,0,0,0); 	//15天
		CTime theTime2 = theTime-timeSpan;
		g_lQJBgnDate = theTime2.GetYear()*10000+theTime2.GetMonth()*100+theTime2.GetDay();
	}
	m_lQJBgnDate = g_lQJBgnDate;

	CurQJType = g_nCurQJType;//缺省为涨跌幅度

	SetPartMode(FALSE);
}

QJReport::~QJReport()
{
	TDEL(pZsAnalyData);

	g_lQJEndDate = m_lQJEndDate;
	g_lQJBgnDate = m_lQJBgnDate;
}

void QJReport::SetDomain(int nBlockFlag,int WhichType)
{
	Grid::SetDomain(nBlockFlag,WhichType);

	//设置窗口标题
	m_strTitle="";
	if(nBlockFlag == -1)				
		m_strTitle = GPType[m_WhichGPType];
	else
		m_strTitle = GetBlockCaption(nBlockFlag);
	m_strTitle = _F(m_strTitle);

	(m_pView->GetParent())->SetWindowText (_F("报表分析-")+m_strTitle);

	InitReport(); //调用报表初始化，开始重画报表
}

void QJReport::InsertSwitch(int nSCType)
{
	pSwitchBar->ClearAllLabel();

	m_bFixedGrid = FALSE;
	int iNo=0;
	CString tmpStr;
	pSwitchBar->m_bhasFLBtn=TRUE;
	pSwitchBar->InsertSwitchLabel(iNo++,_F("分类"),WM_COMMAND,IDR_LONELY_MENU,3,TRUE);
	for(int i=0;i<g_CustomGridCat.size();i++)
	{
		if(g_CustomGridCat[i].FlagType==DSOPT_ITEM) 
			continue;
		if(g_CustomGridCat[i].wParam==ID_RIGHT_TYPE20 && (g_pLcIdxMng==NULL || g_pLcIdxMng->GetLcNum()==0))
			continue;		
		tmpStr=g_CustomGridCat[i].lpTitle;
		pSwitchBar->InsertSwitchLabel(iNo++,_F(tmpStr),g_CustomGridCat[i].msg,g_CustomGridCat[i].wParam,g_CustomGridCat[i].lParam,g_CustomGridCat[i].bMenu);
	}
}

void QJReport::InitReport()
{
	nTotalCol = 13-m_nFixedLeadCol;
    nCurRow = nStartRow = nStartCol = 0;
	nCurSortIndex = 2;	//都按第一栏排序升序
	bAscendSort = TRUE;

	DrawFlag = DRAW_ALL;	
	m_pView->Invalidate(FALSE);
	RefreshScrollBar();	//重新组织滚动条
	//在得到数据之前，也要刷新，否则由于对话框的原因，会乱屏
	GetData();  //得到数据
	//如果是区间分析，因为nCurSortIndex为2，故需要Sort();
	Sort();		//缺省为序号排序
	DrawFlag = DRAW_ALL;
	m_pView->Invalidate(FALSE);
	RefreshScrollBar();	//重新组织滚动条
}

void QJReport::GetData()
{
	RefreshAllHq();

	int i,j,tempInt,nReadDateNum;
	struct AnalyData*	pAnalyData = NULL;
	long				nBgnData,nEndData;
	long				nCurrDate;	//当前日期值
	double              fSHSumAmount = 0.0,fSZSumAmount = 0.0;
	CurrStockData		Hq;
	int					m_nTodayNum;

	//得到指数的一些信息,以便上一交易日，下一交易日有用
	ZsAnalyDataNum = g_pStockIO->GetAnalyDataNum(g_pStockIO->Get_MemStkInfo(SH_DPCODE,SH),PER_DAY);
	TDEL(pZsAnalyData);
	pZsAnalyData = new AnalyData[ZsAnalyDataNum];
	ZsAnalyDataNum = g_pStockIO->ReadAnalyDataAndCurrHQ(g_pStockIO->Get_MemStkInfo(SH_DPCODE,SH),pZsAnalyData,ZsAnalyDataNum,PER_DAY,&Hq,m_nTodayNum);

	memset(fData,0,MAXGPNUM*11*sizeof(struct QJData_Struct)); 

	nReadDateNum = g_pStockIO->GetAnalyDataNum(g_pStockIO->Get_MemStkInfo(SH_DPCODE,SH),PER_DAY);
	pAnalyData = new AnalyData[nReadDateNum];
	nReadDateNum = g_pStockIO->ReadAnalyDataAndCurrHQ(g_pStockIO->Get_MemStkInfo(SH_DPCODE,SH),pAnalyData,nReadDateNum,PER_DAY,&Hq,m_nTodayNum);
	//现在nReadDateNum是实际可以读出的(指数)天数,计算出起始数据个数和终至数据个数
	for(nBgnData = 0;nBgnData < nReadDateNum && pAnalyData[nBgnData].Time.Date < m_lQJBgnDate;nBgnData++);
	for(nEndData = 0;nEndData < nReadDateNum && pAnalyData[nEndData].Time.Date < m_lQJEndDate;nEndData++);
	if(nBgnData >= nReadDateNum || nEndData >= nReadDateNum) fSHSumAmount = MEANLESS_DATA;
	else
	{
		for(nCurrDate = nBgnData; nCurrDate <= nEndData; nCurrDate++)
			fSHSumAmount += pAnalyData[nCurrDate].Amount;
	}
	TDEL(pAnalyData);
	nReadDateNum = g_pStockIO->GetAnalyDataNum(g_pStockIO->Get_MemStkInfo(SZ_DPCODE,SZ),PER_DAY);
	pAnalyData = new AnalyData[nReadDateNum];
	nReadDateNum = g_pStockIO->ReadAnalyDataAndCurrHQ(g_pStockIO->Get_MemStkInfo(SZ_DPCODE,SZ),pAnalyData,nReadDateNum,PER_DAY,&Hq,m_nTodayNum);
	for(nBgnData = 0;nBgnData < nReadDateNum && pAnalyData[nBgnData].Time.Date < m_lQJBgnDate;nBgnData++);
	for(nEndData = 0;nEndData < nReadDateNum && pAnalyData[nEndData].Time.Date < m_lQJEndDate;nEndData++);
	if(nBgnData >= nReadDateNum || nEndData >= nReadDateNum) fSZSumAmount = MEANLESS_DATA;
	else
	{
		for(nCurrDate = nBgnData; nCurrDate <= nEndData; nCurrDate++)
			fSZSumAmount+=pAnalyData[nCurrDate].Amount;
	}
	TDEL(pAnalyData);

	//设定模态进度条对话框
	CProgressWnd wndProgress(m_pView,_F("正在计算，请稍等..."),TRUE);
	wndProgress.GoModal();
	wndProgress.SetRange(0,nTotalRow);
	wndProgress.SetStep(3);
	g_bDonotClose = TRUE;
	for(i = 0; i < nTotalRow; i++)
	{
		if(i%3 == 0)
		{
			//进度条处理
			wndProgress.SetText((*g_pStockIO)[lpnUsedStkIndex[i]]->Name);
			wndProgress.StepIt();
		}
		if(i%10 == 0)
		{
			wndProgress.PeekAndPump();
			if(wndProgress.Cancelled())
			{
				TDX_MessageBox(m_pView->m_hWnd,"计算被取消,报表数据不完整！",MB_OK|MB_ICONINFORMATION);
				g_bDonotClose = FALSE;
				break;
			}
		}
		tempInt = lpnUsedStkIndex[i];	//得到i的序号
		if(tempInt>=DS_FROM) continue;
		MemStkInfo *tmpInfo=(*g_pStockIO)[tempInt];
		if(!tmpInfo) continue;
				
		nReadDateNum = g_pStockIO->GetAnalyDataNum(tmpInfo,PER_DAY);
		TDEL(pAnalyData);
		pAnalyData = new AnalyData[nReadDateNum];
		int nTmpDateNum = g_pStockIO->ReadAnalyDataFromTime(tmpInfo,pAnalyData,nReadDateNum,m_lQJBgnDate,m_lQJEndDate,PER_DAY);
		nReadDateNum = g_pStockIO->ReadAnalyDataFromTime(tmpInfo,pAnalyData,nTmpDateNum+1+5,0,m_lQJEndDate,PER_DAY);				//多读一天，＋5
		if(nReadDateNum > 0  && g_bHisViewTQ)
			g_pStockIO->TQData(tmpInfo,pAnalyData,0,nReadDateNum-1,nReadDateNum,1,PER_DAY);	//1为前除权
		for(nBgnData = 0;nBgnData < nReadDateNum && pAnalyData[nBgnData].Time.Date < m_lQJBgnDate;nBgnData++);	//20040907iwld 01号数据错误
		if(nBgnData>0) --nBgnData;			//找到前一天的
		for(nEndData = 0;nEndData < nReadDateNum && pAnalyData[nEndData].Time.Date < m_lQJEndDate;nEndData++);

		if(nBgnData >= nReadDateNum) nBgnData = nReadDateNum - 1;
		if(nEndData >= nReadDateNum) nEndData = nReadDateNum - 1;
		if(nReadDateNum <= 0 || nTmpDateNum<1)
		{
			fData[tempInt][0].f1 = fData[tempInt][0].f2 = MEANLESS_DATA;
			fData[tempInt][1].f1 = fData[tempInt][1].f2 = MEANLESS_DATA;
			fData[tempInt][2].f1 = fData[tempInt][2].f2 = MEANLESS_DATA;
			fData[tempInt][3].f1 = fData[tempInt][3].f2 = MEANLESS_DATA;
			fData[tempInt][4].f1 = fData[tempInt][4].f2 = MEANLESS_DATA;
			fData[tempInt][5].f1 = fData[tempInt][5].f2 = MEANLESS_DATA;
			fData[tempInt][6].f1 = fData[tempInt][6].f2 = MEANLESS_DATA;
			fData[tempInt][7].f1 = fData[tempInt][7].f2 = MEANLESS_DATA;
			fData[tempInt][8].f1 = fData[tempInt][8].f2 = MEANLESS_DATA;
			fData[tempInt][9].f1 = fData[tempInt][9].f2 = MEANLESS_DATA;
			fData[tempInt][10].f1 = fData[tempInt][10].f2 = MEANLESS_DATA;
			fData[tempInt][11].f1 = fData[tempInt][11].f2 = MEANLESS_DATA;
			fData[tempInt][12].f1 = fData[tempInt][12].f2 = MEANLESS_DATA;
		}
		else
		{
			//前收盘
			if(nReadDateNum>nTmpDateNum)
				fData[tempInt][0].f1 = pAnalyData[nBgnData].Close;
			else if(pAnalyData[nBgnData].Close<COMPPREC)
				fData[tempInt][0].f1 = pAnalyData[min(nEndData,nBgnData+1)].Open;
			else 
				fData[tempInt][0].f1 = pAnalyData[nBgnData].Open;
			//期中最高
			fData[tempInt][1].f1 = QJMax(pAnalyData,nBgnData+1,nEndData);
			//期中最低
			fData[tempInt][2].f1 = QJMin(pAnalyData,nBgnData+1,nEndData);
			//收盘
			if(pAnalyData[nEndData].Close<COMPPREC) //如果当天停牌，前一天
				fData[tempInt][3].f1 = pAnalyData[max(0,nEndData-1)].Close;
			else
				fData[tempInt][3].f1 = pAnalyData[nEndData].Close;
			//涨跌/幅度
			if(fData[tempInt][3].f1==MEANLESS_DATA || fData[tempInt][0].f1==MEANLESS_DATA)
				fData[tempInt][4].f1 = MEANLESS_DATA;
			else
				fData[tempInt][4].f1 = fData[tempInt][3].f1 - fData[tempInt][0].f1;
			if(fData[tempInt][0].f1 < COMPPREC || fabs(fData[tempInt][3].f1 - fData[tempInt][0].f1)<COMPPREC || fData[tempInt][0].f1==MEANLESS_DATA || fData[tempInt][3].f1==MEANLESS_DATA )
				fData[tempInt][4].f2 = MEANLESS_DATA;
			else
				fData[tempInt][4].f2 = 100.0*(fData[tempInt][3].f1 - fData[tempInt][0].f1) / fData[tempInt][0].f1;
			//振动/幅度
			if(fData[tempInt][1].f1==MEANLESS_DATA || fData[tempInt][2].f1==MEANLESS_DATA ||fData[tempInt][0].f1==MEANLESS_DATA)	
				fData[tempInt][5].f1 = MEANLESS_DATA;
			else
				fData[tempInt][5].f1 = fData[tempInt][1].f1 - fData[tempInt][2].f1;
			if(fData[tempInt][0].f1 < COMPPREC || fabs(fData[tempInt][1].f1 - fData[tempInt][2].f1) < COMPPREC || fData[tempInt][1].f1 == MEANLESS_DATA || fData[tempInt][2].f1 == MEANLESS_DATA || fData[tempInt][0].f1 == MEANLESS_DATA)
				fData[tempInt][5].f2 = MEANLESS_DATA;
			else
				fData[tempInt][5].f2 = 100.0*(fData[tempInt][1].f1 - fData[tempInt][2].f1)/fData[tempInt][0].f1;
			fData[tempInt][5].f2Type = FLOAT2_VALUE;
			//成交总量
			fData[tempInt][6].f1 = 0.00;
			for(j = nBgnData+1; j <= nEndData; j++)
				if(pAnalyData[j].Volume != MEANLESS_DATA)
					fData[tempInt][6].f1 += pAnalyData[j].Volume;
			fData[tempInt][6].f1Type = VOLUME_VALUE;
			//成交金额
			fData[tempInt][7].f1 = 0.00;
			for(j = nBgnData+1; j <= nEndData; j++)
				if(pAnalyData[j].Amount != MEANLESS_DATA)
					fData[tempInt][7].f1 += pAnalyData[j].Amount;
			fData[tempInt][7].f1Type = AMOUNT_VALUE;
			//市场比例
			if(tmpInfo->setcode == SZ)
			{
				if( fSZSumAmount < COMPPREC || fabs(fSHSumAmount)<COMPPREC || fSZSumAmount == MEANLESS_DATA || fData[tempInt][7].f1 == MEANLESS_DATA) 
					fData[tempInt][8].f1 = MEANLESS_DATA;
				else	fData[tempInt][8].f1 = 100.0*(fData[tempInt][7].f1 / fSZSumAmount);
			}
			else
			{
				if( fSHSumAmount < COMPPREC || fabs(fSHSumAmount)<COMPPREC || fSHSumAmount == MEANLESS_DATA || fData[tempInt][7].f1 == MEANLESS_DATA )
					fData[tempInt][8].f1 = MEANLESS_DATA;
				else	fData[tempInt][8].f1 = 100.0*(fData[tempInt][7].f1 / fSHSumAmount);
			}
			//换手率%
			if(tmpInfo->ActiveCapital*10000 > 1)
				fData[tempInt][9].f1 = CalcHSL(tmpInfo,fData[tempInt][6].f1,tmpInfo->ActiveCapital,PER_DAY);
			else
				fData[tempInt][9].f1 = 0.0;
			//天量变幅度
			float tmpvolume1 = pAnalyData[nBgnData+1].Volume;
			float tmpvolume2 = pAnalyData[nEndData].Volume;
			//周量变幅度
			for(int k=1;k<5;k++)
			{
				if(nBgnData+1-k>=0) tmpvolume1 += pAnalyData[nBgnData+1-k].Volume;
				if(nEndData-k>=0) tmpvolume2 += pAnalyData[nEndData-k].Volume;
			}
			//量变幅度
			if(tmpvolume1 < COMPPREC || tmpvolume2 < COMPPREC || fabs(tmpvolume2-tmpvolume1)<COMPPREC || tmpvolume2==MEANLESS_DATA || tmpvolume1==MEANLESS_DATA)
				fData[tempInt][10].f1 = MEANLESS_DATA;
			else
				fData[tempInt][10].f1 = 100.0*(tmpvolume2-tmpvolume1) / tmpvolume1;
		}
	}
	g_bDonotClose = FALSE;
	TDEL(pAnalyData);
}

float QJReport::fGetSortValue(short xh)
{
	if(xh>=DS_FROM || nCurSortIndex == 1 || nCurSortIndex == 0 ) return 0.0; //不处理nCurSortIndex = 1的情况
	int tmptype = GetColIndex(nCurSortIndex)-2;
	if(tmptype != 4 && tmptype != 5) 
		return fData[xh][tmptype].f1;
	else		//幅差等应按百分比排序
		return fData[xh][tmptype].f2;
}

//排序函数
void QJReport::Sort()
{
	int i;
	BOOL bOtherSort = FALSE;
	if(nCurSortIndex == 1)		//按证券名称排序
	{
		for (i = 0;i < nTotalRow;i++) 
		{
			strcpy(lpSort2[i].StrValue, (*g_pStockIO)[lpnUsedStkIndex[i]]->Name); //取排序值
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
		if(nCurSortIndex == 0)	//按证券代码排序(实则按序号排序)
		{
			for (i = 0;i < nTotalRow;i++)
			{
				lpSort[i].fValue = lpnUsedStkIndex[i];
				lpSort[i].nIndex = lpnUsedStkIndex[i];
				lpSort[i].tag = 0;
			}
		}
		else					//其它排序
		{
			for (i = 0;i < nTotalRow;i++)
			{
				 lpSort[i].fValue = fGetSortValue(lpnUsedStkIndex[i]);
				 lpSort[i].nIndex = lpnUsedStkIndex[i];
				 lpSort[i].tag = 0;
			}
			bOtherSort = TRUE;
		}
		qsort((void *) lpSort, nTotalRow, sizeof(struct sort_struc), sort_func );
		if (bAscendSort == FALSE)
			for (i = 0;i < nTotalRow;i++) lpnUsedStkIndex[i] = lpSort[i].nIndex;
		else	
			for (i = 0;i < nTotalRow;i++) lpnUsedStkIndex[i] = lpSort[nTotalRow-1-i].nIndex;

		//如果是其它类型的排序，总是将MEANLESS_DATA丢在最后，不参与排序
		if(bOtherSort)
		{
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
	return;
}

void QJReport::OnSize(UINT nType, int cx, int cy)
{
	SetRect(CRect(0,0,cx,cy));
}


//计算方格左边位置,Width为返回方格宽度,nCol表示列顺序中绝对序号
int QJReport::GetCellLeft(int nCol,int & nWidth) 
{   
	int i=0,xtmp=nCellHeadWidth;
	if(m_nFixedLeadCol<1) i=nStartCol;	//没有固定的派头列
	while(i < nCol)
	{   
		xtmp+=(int)(lpnQJItemWidth[GetColIndex(i)]*FontSize.cx/2);
	    if (i == m_nFixedLeadCol-1) 
			i = nStartCol+m_nFixedLeadCol;
		else		
			i++;
	}
	nWidth = (int)(lpnQJItemWidth[GetColIndex(nCol)]*FontSize.cx/2);
	return xtmp;
}


//画标题
void QJReport::DrawTitle(CDC *pDC)
{
	int i,j,xtmp,width;
	//写标题区
	CString tempStr1;
	int pos=8;
	tempStr1.Format("区间分析报表--%s",QJTypeStr[CurQJType]);
	CSize size = g_d.DisplayText(pDC,pos,2,GridColor.Grid_Up_Color,0,_F(tempStr1));
	pos += size.cx+5;
	if(m_pWriteFile)	m_WriteStr+=_F(tempStr1);
	size = g_d.DisplayText(pDC,pos,2,VipColor.TXTCOLOR,0,_F(" 市场:"));
	pos += size.cx+5;
	if(m_pWriteFile)	m_WriteStr+=_F(" 市场:");
	size = g_d.DisplayText(pDC,pos,2,GridColor.Grid_Volume_Color,0,"%s",m_strTitle);
	pos += size.cx+5;
	if(m_pWriteFile)	m_WriteStr+=m_strTitle;
	size = g_d.DisplayText(pDC,pos,2,VipColor.TXTCOLOR,0,_F(" 区间:"));
	pos += size.cx+5;
	if(m_pWriteFile)	m_WriteStr+=_F(" 区间:");

	CString tmpDateStr1=GetTdxDateWeekStr(m_lQJBgnDate);
	CString tmpDateStr2=GetTdxDateWeekStr(m_lQJEndDate);
	tempStr1.Format("%s - %s",tmpDateStr1,tmpDateStr2);
	size = g_d.DisplayText(pDC,pos,2,GridColor.Grid_Volume_Color,0,_F(tempStr1));
	pos += size.cx+10;
	if(m_pWriteFile)	m_WriteStr+=_F(tempStr1);
	if(m_pWriteFile)	
	{
		m_pWriteFile->WriteString(m_WriteStr+(CString)"\n");
		m_WriteStr = "";
	}
	size = g_d.DisplayText(pDC,pos,2,VipColor.TXTCOLOR,0,_F("点右键进行设置"));
	pos += size.cx+5;		

	//开始画
	int tempTitlePos = nSigTitleHeight+6;
	xtmp = nCellHeadWidth;
	for(i = j = 0;j < nTotalCol+m_nFixedLeadCol-1 && xtmp < m_DRect.right;i++)
	{
  		if (i>=m_nFixedLeadCol) 
			j = i+nStartCol;
		else	  
			j = i;
		width = (int)(lpnQJItemWidth[GetColIndex(j)]*FontSize.cx/2);
		if((nCurSortIndex != ((i>=m_nFixedLeadCol)?(nStartCol+i):i)) || (nCurSortIndex == 0) )
		{
			if(i<m_nFixedLeadCol)
				g_d.DisplayText(pDC,xtmp+10,tempTitlePos,GridColor.GridTitleTextColor,0,_F(lpszQJItemName[GetColIndex(j)]));
			else
				g_d.DisplayText(pDC,xtmp,tempTitlePos,GridColor.GridTitleTextColor,width-3,_F(lpszQJItemName[GetColIndex(j)]));
		}
		else
		{	
			g_d.DisplayText(pDC,xtmp,tempTitlePos,GridColor.GridTitleTextColor,width-FontSize.cx/2-3,_F(lpszQJItemName[GetColIndex(j)]));
			g_d.DisplayText(pDC,xtmp,tempTitlePos,GridColor.GridTitleArrowColor,width,(bAscendSort==TRUE)?_F("↓"):_F("↑"));
		}
		xtmp+=width;
		if(m_pWriteFile) 
			m_WriteStr+=_F(lpszQJItemName[GetColIndex(j)])+(CString)"\t";
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

void QJReport::GetSomeSize(CDC *pDC)
{
	FontSize = pDC->GetTextExtent("试");
	FontSize.cx+=2;
	nSigTitleHeight = FontSize.cy+6;
	nSigCellHeight = FontSize.cy+4;
	nTitleHeight = 2*nSigTitleHeight+4; //得到标题栏高度
	nCellHeight =  1.5*nSigCellHeight;	//得到方格的高度
	nCellHeadWidth = 2.2*FontSize.cx;
	if(g_GH.NoXHCol) 
		nCellHeadWidth=0;
}

void QJReport::DrawReport(CDC *pDC)
{
	GetDrawRect(m_DRect);
	if(m_DRect.Height() < 40) return;
	int i,j;

	CREATE_GRIDMEMDC
	GetSomeSize(&memDC);

	//画栏目行,在其中初化始一些变量:列数nColNum
	g_d.Bar(&memDC,0,0,m_DRect.Width(),nTitleHeight-nSigTitleHeight-4,g_GUI.MixedColor(VipColor.AXISCOLOR,25,VipColor.BACKCOLOR));
	DrawTitle(&memDC);
	//调节RowNum和ColNum
	AdjustRowColNum();
	//开始画线
	int tempWidth,tempLeft;
	tempLeft = GetCellLeft(m_nFixedLeadCol+nEndCol,tempWidth);

	g_d.DrawLine(&memDC,0,nTitleHeight-nSigTitleHeight-5,m_DRect.Width(),
				nTitleHeight-nSigTitleHeight-5,VipColor.AXISCOLOR);
	for(i = 0;i <= nRowNum;i++)	//画行线
	{
		g_d.DrawLine(&memDC,0,nTitleHeight+i*nCellHeight-1,
				(tempColFlag==TRUE)?(tempLeft+tempWidth):(m_DRect.Width()),
				nTitleHeight+i*nCellHeight-1);
	}
	int xtmp = nCellHeadWidth ;
	for(j = 0;j < m_nFixedLeadCol+nColNum;j++) //画列线
	{			
		g_d.DrawLine(&memDC,xtmp-1,nSigTitleHeight,xtmp-1,
			(tempRowFlag==TRUE)?(nRowNum*nCellHeight+nTitleHeight):m_DRect.Height());
		if(j < m_nFixedLeadCol)		
			xtmp+=(int)(lpnQJItemWidth[GetColIndex(j)]*FontSize.cx/2);
		else			
			xtmp+=(int)(lpnQJItemWidth[GetColIndex(j+nStartCol)]*FontSize.cx/2);
	}
	g_d.DrawLine(&memDC,xtmp-1,nSigTitleHeight,xtmp-1,(tempRowFlag==TRUE)?(nRowNum*nCellHeight+nTitleHeight):m_DRect.Height());//紧跟上个循环

	//在方格中写内容
	MemStkInfo *tempStkInfo;
	CurrStockData CurrStockDatap;
	if(m_bWriteAllHQ) //导出全部时
	{
		for(j = 0;j < nTotalRow;j++)
		{
			tempStkInfo = (*g_pStockIO)[lpnUsedStkIndex[j]];
			g_pStockIO->ReadCurrStockData (tempStkInfo,&CurrStockDatap);
			for(i=0;i<tempColNum+m_nFixedLeadCol;i++)
			{
				if(i<m_nFixedLeadCol)
					DrawItem(&memDC,j,i,2,tempStkInfo,&CurrStockDatap);
				else
					DrawItem(&memDC,j,i+nStartCol,GetXSFlag(tempStkInfo),tempStkInfo,&CurrStockDatap);
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
			tempStkInfo = (*g_pStockIO)[lpnUsedStkIndex[nStartRow+j]];
			g_pStockIO->ReadCurrStockData (tempStkInfo,&CurrStockDatap);
			for(i=0;i<tempColNum+m_nFixedLeadCol;i++)
			{
				if(i<m_nFixedLeadCol)
					DrawItem(&memDC,j,i,2,tempStkInfo,&CurrStockDatap);
				else
					DrawItem(&memDC,j,i+nStartCol,GetXSFlag(tempStkInfo),tempStkInfo,&CurrStockDatap);
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

void QJReport::DrawItem(CDC *pDC,int row,int col,int xsflag,MemStkInfo * scodetmp,CurrStockData * hqp)
{
	int tempInt = lpnUsedStkIndex[nStartRow+row];
	int tempCol = GetColIndex(col);	//根据索引来设置栏目
	if(tempInt>=DS_FROM && tempCol!=0 && tempCol!=1) 
		return;
	switch(tempCol)
	{
	case 0://股票代码
		DrawCellText(pDC,row,col,0,scodetmp->Code,GridColor.Grid_CodeName_Color,TRUE);
		if(m_pWriteFile)	
		{
			if(m_bWriteExcel)
				m_WriteStr+=(CString)"=\""+scodetmp->Code+(CString)"\"";
			else
				m_WriteStr+=scodetmp->Code;
		}
		break;
	case 1://股票名称
		DrawCellText(pDC,row,col,0,scodetmp->Name,GetBlockGPColor(scodetmp,GridColor.Grid_CodeName_Color,m_WhichGPType,m_nBlockFlag),TRUE,TRUE,scodetmp);
		if(m_pWriteFile)	m_WriteStr+=scodetmp->Name;
		break;
	case 2:
	case 3:
	case 4:
	case 5:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
		if(fData[tempInt][tempCol-2].f1 != MEANLESS_DATA)
		{
			if(fData[tempInt][tempCol-2].f1Type == FLOAT_VALUE)
			{
				DrawCellText(pDC,row,col,0,fData[tempInt][tempCol-2].f1,xsflag,GridColor.Grid_Level_Color);
				if(m_pWriteFile)	m_WriteStr+=FloatToStr(fData[tempInt][tempCol-2].f1);
			}
			else if(fData[tempInt][tempCol-2].f1Type == VOLUME_VALUE)
			{
				DrawCellText( pDC, row, col,0, GridColor.Grid_Volume_Color,MakeVol(fData[tempInt][tempCol-2].f1));
				if(m_pWriteFile)	m_WriteStr+=MakeVol(fData[tempInt][tempCol-2].f1);
			}
			else
			{
				DrawCellText( pDC, row, col,0, GridColor.Grid_Other_Color,MakeJE(fData[tempInt][tempCol-2].f1));
				if(m_pWriteFile)	m_WriteStr+=MakeJE(fData[tempInt][tempCol-2].f1);
			}
		}
		else
			DrawCellText(pDC,row,col,0,MEANLESS_STR,GridColor.Grid_Level_Color);
		break;
	default:
		if(fData[tempInt][tempCol-2].f1 != MEANLESS_DATA)
		{
			//两行时,第一行不导出
			if(fData[tempInt][tempCol-2].f1Type == FLOAT_VALUE)
				DrawCellText(pDC,row,col,1,fData[tempInt][tempCol-2].f1,xsflag,GridColor.Grid_Level_Color);
			else if(fData[tempInt][tempCol-2].f1Type == VOLUME_VALUE)
				DrawCellText( pDC, row, col,1, GridColor.Grid_Volume_Color,MakeVol(fData[tempInt][tempCol-2].f1));
			else
				DrawCellText( pDC, row, col,1, VipColor.TXTCOLOR,MakeJE(fData[tempInt][tempCol-2].f1));
		}
		if(fData[tempInt][tempCol-2].f2 != MEANLESS_DATA)
		{
			if(fData[tempInt][tempCol-2].f2Type == FLOAT_VALUE)
			{
				DrawCellText(pDC,row,col,2,fData[tempInt][tempCol-2].f2,0.0,2,1);
				if(m_pWriteFile)	m_WriteStr+=FloatToStr(fData[tempInt][tempCol-2].f2);
			}
			else if(fData[tempInt][tempCol-2].f2Type == FLOAT2_VALUE)
			{
				DrawCellText(pDC,row,col,2,fData[tempInt][tempCol-2].f2,2,VipColor.TXTCOLOR,1);
				if(m_pWriteFile)	m_WriteStr+=FloatToStr(fData[tempInt][tempCol-2].f2);
			}
			else if(fData[tempInt][tempCol-2].f2Type == VOLUME_VALUE)
			{
				DrawCellText( pDC, row, col,2, GridColor.Grid_Volume_Color,MakeVol(fData[tempInt][tempCol-2].f2));
				if(m_pWriteFile)	m_WriteStr+=MakeVol(fData[tempInt][tempCol-2].f2);
			}
			else
			{
				DrawCellText( pDC, row, col,2, VipColor.TXTCOLOR,MakeJE(fData[tempInt][tempCol-2].f2));
				if(m_pWriteFile)	m_WriteStr+=MakeJE(fData[tempInt][tempCol-2].f2);
			}
		}
		break;
	}
	if(m_pWriteFile)
		m_WriteStr+="\t";
}

//当鼠标点中一行时
void QJReport::OnClick(CDC *pDC,CPoint point,BOOL IsRight)
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
		CNewMenu menu;
		menu.LoadMenu (IDR_RIGHTMENU_HIS);
//		menu.LoadToolBar(IDR_MAINFRAME); //使用工具栏的图标
		CNewMenu *pPopup=(CNewMenu *)menu.GetSubMenu(2); //历史分析右键菜单
		pPopup->SetDefaultItem(ID_RIGHT_OPEN);
		(pDC->GetWindow())->ClientToScreen (&point);
		pPopup->TrackPopupMenu (TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,AfxGetMainWnd());
	}
}

void QJReport::OnLButtonDown(UINT nFlags, CPoint point) 
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
		m_rgn.CreateRectRgn (m_Rect.left+Left,m_Rect.top+nTitleHeight-nSigCellHeight,m_Rect.left+Left+Width,m_Rect.top+nTitleHeight);
		if(m_rgn.PtInRegion (point))
		{
			if( (nCurSortIndex != i && i < m_nFixedLeadCol) || (nCurSortIndex != nStartCol+i && i > m_nFixedLeadCol-1) )
			{
				if(i != 0)
				{
					bAscendSort = TRUE;
					if(i > m_nFixedLeadCol-1)	
						nCurSortIndex = nStartCol + i;
					else	
						nCurSortIndex = i;
				}
				else
				{
					bAscendSort = FALSE;
					nCurSortIndex = 0;
				}
				Sort();
				ToSomeRow(0);
				DrawFlag = DRAW_ALL;
				DrawReport(&dc);
			}
			else if(i != 0)
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

//求区间最高值的函数
float QJReport::QJMax(struct AnalyData* pAnalyData,short nBgnData,short nEndData)
{
	float fTemp = 0.0;
	for(int i = nBgnData; i <= nEndData; i++)
		if(fTemp < pAnalyData[i].High && pAnalyData[i].High != MEANLESS_DATA)
			fTemp = pAnalyData[i].High;
	if(fabs(fTemp) < COMPPREC) fTemp = MEANLESS_DATA;
	return fTemp;
}
//求区间最高值所在天数的函数
long QJReport::QJMaxDate(struct AnalyData* pAnalyData,short nBgnNum,short nEndNum)
{
	float	fTemp = 0.0;
	long	lMaxDate = pAnalyData[nBgnNum].Time.Date;
	for(int i = nBgnNum; i <= nEndNum; i++)
		if(fTemp<pAnalyData[i].High && pAnalyData[i].High != MEANLESS_DATA)
		{
			fTemp    = pAnalyData[i].High;
			lMaxDate = pAnalyData[i].Time.Date;
		}
	if(fabs(fTemp) < COMPPREC) lMaxDate = MEANLESS_DATA;
	return lMaxDate;
}
//求区间最低值的函数
float QJReport::QJMin(struct AnalyData* pAnalyData,short nBgnNum,short nEndNum)
{
	float	fTemp = MEANLESS_DATA;	//fTemp为一价位不可能超过的大值
	for(int i = nBgnNum; i <= nEndNum; i++)
		if(fTemp > pAnalyData[i].Low && pAnalyData[i].Low != MEANLESS_DATA)
			fTemp = pAnalyData[i].Low;
	return fTemp;
}
//求区间最低值所在天数的函数
long QJReport::QJMinDate(struct AnalyData* pAnalyData,short nBgnNum,short nEndNum)
{
	float	fTemp = MEANLESS_DATA;//fTemp为一大值
	long	lMinDate = pAnalyData[nBgnNum].Time.Date;
	for(int i = nBgnNum; i <= nEndNum; i++)
		if(fTemp>pAnalyData[i].Low && pAnalyData[i].Low != MEANLESS_DATA)
		{
			fTemp    = pAnalyData[i].Low;
			lMinDate = pAnalyData[i].Time.Date;
		}
	return lMinDate;
}

int QJReport::GetColIndex(int col)
{
	int *naTmp;
	if(CurQJType == 0)
		naTmp = naPriceExtent;	//涨跌幅度
	else if(CurQJType == 1)
		naTmp = naPopPoint;		//换手率排名
	else if(CurQJType == 2)
		naTmp = naVolumeExtent;	//量变幅度
	else 
		naTmp = naDiffExtent;	//震荡幅度
	return *(naTmp+col);
}

//设定分析类型
void QJReport::SetQJType(int type)
{
	if(type < 0 || type > 3) return;
	CurQJType = type;

	nCurSortIndex = 2;		//都按第一栏排序,升序
	bAscendSort = TRUE;
	Sort();
	ToSomeRow(0);
	ToSomeCol(0);
	DrawFlag = DRAW_ALL;
	m_pView->Invalidate(FALSE);
}

//设置起讫日期
void  QJReport::SetDateSection(CTime date1,CTime date2)	
{
	long lDate = TimeToLong(date1);	//重新调整起始日期
	if(pZsAnalyData && ZsAnalyDataNum>0)
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
		InitReport();
	}
}

//使这个报表睡眠
void QJReport::Sleep()
{
	pSwitchBar->SetWindowPos(0,0,0,0,0,SWP_SHOWWINDOW);
}

//唤醒这个已经激活的报表
void QJReport::WakeUp()
{
	(m_pView->GetParent())->SetWindowText (_F("报表分析-"+m_strTitle));
	DrawFlag = DRAW_ALL;
	m_pView->Invalidate(FALSE);
	RefreshScrollBar();
}

void QJReport::OnCtrlLeft()
{
	if(nStartCol == 0) return;

	int xtmp,width;
	xtmp = nCellHeadWidth+(int)(lpnQJItemWidth[0]*FontSize.cx/2)+(int)(lpnQJItemWidth[1]*FontSize.cx/2);
	for(int i = nStartCol;i >= 0&&xtmp < m_DRect.Width();i--)
	{
		width = (int)(lpnQJItemWidth[GetColIndex(i+2)]*FontSize.cx/2);
		xtmp += width;
	}
	if(xtmp > m_DRect.Width()) nStartCol = i+1;
	else if(xtmp == m_DRect.Width()) nStartCol = i;
	else nStartCol = 0;

	DrawFlag = DRAW_NOREADDATA;
	m_pView->Invalidate(FALSE);
}

int  QJReport::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_PAINT:
		m_pView->Invalidate(FALSE);
		return 1;	
	}
	return 0;
}
