// ZBReport.cpp: implementation of the ZBReport class.
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "tdxw.h"
#include "ZBReport.h"
#include "ProgressWnd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

ZBReport::ZBReport():Grid(2,TRUE)
{
	memset(fData,0,sizeof(float)*MAXGPNUM*32);

	pZsAnalyData = NULL;
	ZsAnalyDataNum = 0;

	m_lNowDate	= g_lOpenRq;

	//缺省IndexSortInfo等数据
	nCurSortIndex = g_nZBReportSortIndex;
	bAscendSort = TRUE;
	IndexSortNum = 1;
	IndexSortInfo[0].Index=g_pCalc->_CallDTopViewCalc_9(ZB_TYPE,g_strLastZBSort);
	if(IndexSortInfo[0].Index<0)
		IndexSortInfo[0].Index=21; //缺省为RSI
	IndexSortInfo[0].Cycle = PER_DAY;
	TINDEXINFO *tmpInfo=(*g_pCalc)[ZB_TYPE]+IndexSortInfo[0].Index;
	for(int i=0;i<MAX_PARAMNUM;i++)
		IndexSortInfo[0].Param[i] = tmpInfo->aPara[i].nValue[PER_DAY];

	SetPartMode(FALSE);
}

ZBReport::~ZBReport()
{
	TDEL(pZsAnalyData);
	g_nZBReportSortIndex = nCurSortIndex;
	//将本次使用的指标存盘
	if(IndexSortInfo[0].Index>=0)
	{
		TINDEXINFO *tmpInfo=(*g_pCalc)[ZB_TYPE]+IndexSortInfo[0].Index;
		if(tmpInfo) strcpy(g_strLastZBSort,tmpInfo->acCode);
	}
}

void ZBReport::SetDomain(int nBlockFlag,int WhichType)
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

void ZBReport::InsertSwitch(int nSCType)
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

//Flag 为1,表示后交易日，为2,表示前交易日
void ZBReport::SetDate(int Flag)
{
	if(!pZsAnalyData || ZsAnalyDataNum < 1) 
	{
		TDX_MessageBox(m_pView->m_hWnd,"没有可用的数据,请下载数据!",MB_OK|MB_ICONINFORMATION);
		return;
	}
	if((pZsAnalyData[0].Time.Date==m_lNowDate&&Flag==2) ||
	   (pZsAnalyData[ZsAnalyDataNum-1].Time.Date==m_lNowDate&&Flag==1) 	
	  )
	  return;
	int pos = 0;
	while(pZsAnalyData[pos].Time.Date < m_lNowDate && pos < ZsAnalyDataNum)
		pos++;
	if(pos >= ZsAnalyDataNum) 
		pos = ZsAnalyDataNum - 1;
	if(Flag == 2)
		pos = max(0,pos-1);
	else
		pos = min(ZsAnalyDataNum-1,pos+1);
	m_lNowDate = pZsAnalyData[pos].Time.Date;
	InitReport();
}

void ZBReport::SetDate(int Flag,CTime date) 
{
	if(Flag != 0) 
	{
		SetDate(Flag);
		return;
	}
	long lDate = TimeToLong(date);
	if(pZsAnalyData && ZsAnalyDataNum>0)
	{
		if(pZsAnalyData[0].Time.Date > lDate)
			m_lNowDate = pZsAnalyData[0].Time.Date;
		else
		{
			int pos = 0;
			while(pZsAnalyData[pos].Time.Date < lDate && pos < ZsAnalyDataNum)
				pos++;
			if(pos >= ZsAnalyDataNum) pos = ZsAnalyDataNum - 1;
			m_lNowDate = pZsAnalyData[pos].Time.Date;
		}
		if(m_lNowDate!=lDate)
		{
			TDX_MessageBox(m_pView->m_hWnd,"本地无此日的数据(可能没有下载数据或此日非交易日)!",MB_OK|MB_ICONINFORMATION);
			return;
		}
		InitReport();
	}
	else
	{
		TDX_MessageBox(m_pView->m_hWnd,"没有可用的数据,请下载数据!",MB_OK|MB_ICONINFORMATION);
		return;
	}
}

void ZBReport::InitReport()
{
	int i,j;

	//得到指标栏目的字串
	TINDEXINFO *pIndex;
	CString tempStr,tempStr2;
	nTotalCol = 10-m_nFixedLeadCol;
	for(j = 0;j < IndexSortNum;j++)
	{
		pIndex = g_pCalc->_CallDTopViewCalc_8(ZB_TYPE,IndexSortInfo[j].Index);
		if(!pIndex) break;
		for(i = 0;i < pIndex->nLineNum;i++)
		{
			tempStr = pIndex->aLine[i].acLineName;
			strcpy(lpszZBItemName[m_nFixedLeadCol+nTotalCol],"  "+tempStr);
			nTotalCol++;		//栏目数加1
		}
	}
    nCurRow = nStartRow = nStartCol = 0;

	if(nCurSortIndex>=nTotalCol+m_nFixedLeadCol)
	{
		nCurSortIndex=m_nFixedLeadCol;
		bAscendSort=TRUE;
	}
	DrawFlag = DRAW_ALL;	
	m_pView->Invalidate(FALSE);
	RefreshScrollBar();	//重新组织滚动条
	//在得到数据之前，也要刷新，否则由于对话框的原因，会乱屏
	GetData();  //得到数据
	Sort();		//缺省为序号排序
	
	DrawFlag = DRAW_ALL;
	m_pView->Invalidate(FALSE);
	RefreshScrollBar();	//重新组织滚动条
}

void ZBReport::GetData()
{
	RefreshAllHq();
	if(CanLevel2() && g_bSupportDDE)
		RefreshAllStatHq(120);

	int i,j,n,k,tempInt,nDataPtr,nLineDataPtr,nReadDateNum;	//nDataPtr是当前日期在读出数组中的序号
	BOOL                bDataExist; //bDataExist是当前股票是否有该日期的数据,为1表示有数据
	CurrStockData		Hq;
	int					m_nTodayNum;
	TINDEXINFO		   *pIndex = NULL;
	struct AnalyData*	pAnalyData = NULL;

	//得到指数的一些信息,以便上一交易日，下一交易日有用
	ZsAnalyDataNum = g_pStockIO->GetAnalyDataNum(g_pStockIO->Get_MemStkInfo(SH_DPCODE,SH),PER_DAY);
	TDEL(pZsAnalyData);
	pZsAnalyData = new AnalyData[ZsAnalyDataNum];
	ZsAnalyDataNum = g_pStockIO->ReadAnalyDataAndCurrHQ(g_pStockIO->Get_MemStkInfo(SH_DPCODE,SH),pZsAnalyData,ZsAnalyDataNum,PER_DAY,&Hq,m_nTodayNum);

	memset(fData,0,MAXGPNUM*32*sizeof(float));

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
		bDataExist	 = FALSE;
		nReadDateNum = g_pStockIO->GetAnalyDataNum((*g_pStockIO)[lpnUsedStkIndex[i]],PER_DAY);
		TDEL(pAnalyData);
		pAnalyData = new AnalyData[nReadDateNum];
		nReadDateNum = g_pStockIO->ReadAnalyDataAndCurrHQ((*g_pStockIO)[lpnUsedStkIndex[i]],pAnalyData,nReadDateNum,PER_DAY,&Hq,m_nTodayNum);
		if(nReadDateNum > 0 && g_bHisViewTQ)
			g_pStockIO->TQData((*g_pStockIO)[lpnUsedStkIndex[i]],pAnalyData,0,nReadDateNum-1,nReadDateNum,1,PER_DAY);	//1为前除权

		nDataPtr = 0;
		if(nReadDateNum > 0)
		{
			while(pAnalyData[nDataPtr].Time.Date <= m_lNowDate && nDataPtr < nReadDateNum)
			{
				if(pAnalyData[nDataPtr].Time.Date == m_lNowDate) 
				{							//寻找这一天的历史数据
					bDataExist=TRUE;
					break;
				}
				nDataPtr ++;
			}
		}
		if(!bDataExist)					//如果数据没有读到
		{
			fData[tempInt][0] = MEANLESS_DATA;
			fData[tempInt][1] = MEANLESS_DATA;
			fData[tempInt][2] = MEANLESS_DATA;
			fData[tempInt][3] = MEANLESS_DATA;
			fData[tempInt][4] = MEANLESS_DATA;
			fData[tempInt][5] = MEANLESS_DATA;
			fData[tempInt][6] = MEANLESS_DATA;
			fData[tempInt][7] = MEANLESS_DATA;
			for(nLineDataPtr = 0; nLineDataPtr < nTotalCol-8; nLineDataPtr++)//读取指标数据
				fData[tempInt][nLineDataPtr+8] = MEANLESS_DATA;
		}
		else
		{
			MemStkInfo *tmpInfo=(*g_pStockIO)[tempInt];
			if(!tmpInfo) continue;
			float fzdf = MEANLESS_DATA;
			if(nDataPtr>0&&pAnalyData[nDataPtr-1].Close>COMPPREC)	//计算涨跌幅
				fzdf = 100.0f*(pAnalyData[nDataPtr].Close-pAnalyData[nDataPtr-1].Close)/pAnalyData[nDataPtr-1].Close;
	
			fData[tempInt][0] = fzdf;								//涨跌幅
			fData[tempInt][1] = pAnalyData[nDataPtr].Open;			//开盘
			fData[tempInt][2] = pAnalyData[nDataPtr].High;			//最高
			fData[tempInt][3] = pAnalyData[nDataPtr].Low;			//最低
			fData[tempInt][4] = pAnalyData[nDataPtr].Close;			//收盘
			fData[tempInt][5] = pAnalyData[nDataPtr].Volume;		//成交数量
			fData[tempInt][6] = pAnalyData[nDataPtr].Amount;		//成交金额
			//计算换手率%
			if( fabs(tmpInfo->ActiveCapital*10000) > COMPPREC )
				fData[tempInt][7] = CalcHSL(tmpInfo,fData[tempInt][5],tmpInfo->ActiveCapital,PER_DAY);
			else
				fData[tempInt][7] = MEANLESS_DATA;
			k = 0;	//记数
			HISDAT	*	pHisData;
			pHisData = new HISDAT[nReadDateNum];
			memset(pHisData,0,nReadDateNum*sizeof(HISDAT));
			//将原始数据转成Calc分析数据
			RawData2CalcData((*g_pStockIO)[tempInt]->Code,(*g_pStockIO)[tempInt]->setcode,PER_DAY,pAnalyData,pHisData,nReadDateNum,g_NoTime);
			try
			{
				int nRealNum = g_pCalc->_CallDTopViewCalc_3((*g_pStockIO)[tempInt]->Code,(*g_pStockIO)[tempInt]->setcode,PER_DAY,pHisData,nReadDateNum);
				float *pOutData[MAX_LINEOUT];
				for(j=0; j<MAX_LINEOUT;j++) 
					pOutData[j] = g_pCalc->_CallDTopViewCalc_13(nRealNum);
				for(n = 0;n < IndexSortNum;n++)
				{
					pIndex = g_pCalc->_CallDTopViewCalc_8(ZB_TYPE,IndexSortInfo[n].Index);
					if(!pIndex) break;
					if(g_pCalc->_CallDTopViewCalc_5(ZB_TYPE, IndexSortInfo[n].Index, pOutData) != -1)
					{
						for(nLineDataPtr=0; nLineDataPtr<pIndex->nLineNum; nLineDataPtr++)//读取指标数据
						{
							if(TBE_MEANLESS(pOutData[nLineDataPtr][nDataPtr]) == 0)
								fData[tempInt][k+8] = MEANLESS_DATA;
							else
								fData[tempInt][k+8] = pOutData[nLineDataPtr][nDataPtr];
							k++;
						}
					}
				}
				for (j=0; j<MAX_LINEOUT; j++) 
					g_pCalc->_CallDTopViewCalc_14(pOutData[j]);
			}
			catch(...)
			{

			}
			delete pHisData;
		}
	}
	g_bDonotClose = FALSE;
	TDEL(pAnalyData);
}

float ZBReport::fGetSortValue(short xh)
{
	if(xh>=DS_FROM || nCurSortIndex == 1 || nCurSortIndex == 0 ) return 0.0; //不处理nCurSortIndex = 1的情况
	return fData[xh][nCurSortIndex - m_nFixedLeadCol];
}

//排序函数
void ZBReport::Sort()
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
			bAscendSort=FALSE;
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

void ZBReport::OnSize(UINT nType, int cx, int cy)
{
	SetRect(CRect(0,0,cx,cy));
}

//计算方格左边位置,Width为返回方格宽度,nCol表示列顺序中绝对序号
int ZBReport::GetCellLeft(int nCol,int & nWidth) 
{   
	int i=0,xtmp=nCellHeadWidth ;
	if(m_nFixedLeadCol<1) i=nStartCol;	//没有固定的派头列
	while(i < nCol)
	{   
		xtmp+=(int)(lpnZBItemWidth[i]*FontSize.cx/2);
	    if (i == m_nFixedLeadCol-1) 
			i = nStartCol+m_nFixedLeadCol;
		else		
			i++;
	}
	nWidth = (int)(lpnZBItemWidth[nCol]*FontSize.cx/2);
	return xtmp;
}

//画标题
void ZBReport::DrawTitle(CDC *pDC)
{
	int i,j,xtmp,width;
	//写标题区
	CString tempStr1,tempStr2;
	int pos=8;
	tempStr1 = "历史行情报表";
	CSize size = g_d.DisplayText(pDC,pos,2,GridColor.Grid_Up_Color,0,_F(tempStr1));
	pos += size.cx+5;
	if(m_pWriteFile)	m_WriteStr+=_F(tempStr1);
	size = g_d.DisplayText(pDC,pos,2,VipColor.TXTCOLOR,0,_F(" 市场:"));
	pos += size.cx+5;
	if(m_pWriteFile)	m_WriteStr+=_F(" 市场:");
	size = g_d.DisplayText(pDC,pos,2,GridColor.Grid_Volume_Color,0,"%s",m_strTitle);
	pos += size.cx+5;
	if(m_pWriteFile)	m_WriteStr+=m_strTitle;
	size = g_d.DisplayText(pDC,pos,2,VipColor.TXTCOLOR,0,_F(" 日期:"));
	pos += size.cx+5;
	if(m_pWriteFile)	m_WriteStr+=_F(" 日期:");
	if(m_lNowDate == -1)
		tempStr1 = "未知";
	else
		tempStr1.Format("%s",GetTdxDateWeekStr(m_lNowDate));
	size = g_d.DisplayText(pDC,pos,2,GridColor.Grid_Volume_Color,0,_F(tempStr1));
	pos += size.cx+5;
	if(m_pWriteFile)	m_WriteStr+=_F(tempStr1);
	size = g_d.DisplayText(pDC,pos,2,VipColor.TXTCOLOR,0,_F(" 指标:"));
	pos += size.cx+5;
	if(m_pWriteFile)	m_WriteStr+=_F(" 指标:");

	tempStr1 = "";
	TINDEXINFO *pIndex;
	for(j = 0;j < IndexSortNum;j++)
	{
		pIndex = g_pCalc->_CallDTopViewCalc_8(ZB_TYPE,IndexSortInfo[j].Index);
		if(!pIndex) break;
		tempStr2.Format("%s_%s",pIndex->acName,pIndex->acCode);
		if(pIndex->nParaNum > 0)
		{
			tempStr2 += "(";
			for(i = 0;i < pIndex->nParaNum - 1;i++)
				tempStr2 += IntToStr(IndexSortInfo[j].Param[i]) + ",";
			tempStr2 += IntToStr(IndexSortInfo[j].Param[pIndex->nParaNum-1]) + ")  ";
		}
		tempStr1 += tempStr2+" ";
	}
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
		width = (int)(lpnZBItemWidth[j]*FontSize.cx/2);
		if((nCurSortIndex != ((i>=m_nFixedLeadCol)?(nStartCol+i):i)) || (nCurSortIndex == 0) )
		{
			if(i<m_nFixedLeadCol)
				g_d.DisplayText(pDC,xtmp+10,tempTitlePos,GridColor.GridTitleTextColor,0,_F(lpszZBItemName[j]));
			else
				g_d.DisplayText(pDC,xtmp,tempTitlePos,GridColor.GridTitleTextColor,width-3,_F(lpszZBItemName[j]));
		}
		else
		{	
			g_d.DisplayText(pDC,xtmp,tempTitlePos,GridColor.GridTitleTextColor,width-FontSize.cx/2-3,_F(lpszZBItemName[j]));
			g_d.DisplayText(pDC,xtmp,tempTitlePos,GridColor.GridTitleArrowColor,width,(bAscendSort==TRUE)?_F("↓"):_F("↑"));
		}
		xtmp+=width;
		if(m_pWriteFile) 
			m_WriteStr+=_F(lpszZBItemName[j])+(CString)"\t";
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

void ZBReport::GetSomeSize(CDC *pDC)
{
	FontSize = pDC->GetTextExtent("试");
	FontSize.cx+=2;
	nSigTitleHeight = FontSize.cy+6;
	nSigCellHeight = FontSize.cy+7;
	nTitleHeight = 2*nSigTitleHeight+4; //得到标题栏高度
	nCellHeight =  nSigCellHeight;	//得到方格的高度
	nCellHeadWidth = 2.2*FontSize.cx;
	if(g_GH.NoXHCol) 
		nCellHeadWidth=0;
}

void ZBReport::DrawReport(CDC *pDC)
{
	GetDrawRect(m_DRect);
	if(m_DRect.Height() < 40) return;
	int i,j;

	CREATE_GRIDMEMDC
	GetSomeSize(&memDC);
	
	//画栏目行,在其中初化始一些变量:列数nColNum
	g_d.Bar(&memDC,0,0,m_DRect.Width(),nTitleHeight-nSigTitleHeight-3,g_GUI.MixedColor(VipColor.AXISCOLOR,25,VipColor.BACKCOLOR));
	DrawTitle(&memDC);
	//调节RowNum和ColNum
	AdjustRowColNum();	
	//开始画线
	int tempWidth,tempLeft;
	tempLeft = GetCellLeft(m_nFixedLeadCol+nEndCol,tempWidth);
	g_d.DrawLine(&memDC,0,nTitleHeight-nSigTitleHeight-4,m_DRect.Width(),
				nTitleHeight-nSigTitleHeight-4,VipColor.AXISCOLOR);
	for(i = 0;i <= nRowNum;i++)	//画行线
	{
		g_d.DrawLine(&memDC,0,nTitleHeight+i*nCellHeight-1,
				(tempColFlag==TRUE)?(tempLeft+tempWidth):(m_DRect.Width()),
				nTitleHeight+i*nCellHeight-1);
	}
	int xtmp=nCellHeadWidth ;
	for(j=0;j<m_nFixedLeadCol+nColNum;j++) //画列线
	{			
		g_d.DrawLine(&memDC,xtmp-1,nCellHeight,xtmp-1,(tempRowFlag==TRUE)?(nRowNum*nCellHeight+nTitleHeight):m_DRect.Height());//紧跟上个循环
		if(j < m_nFixedLeadCol)	xtmp+=(int)(lpnZBItemWidth[j]*FontSize.cx/2);
		else					xtmp+=(int)(lpnZBItemWidth[j+nStartCol]*FontSize.cx/2);
	}
	g_d.DrawLine(&memDC,xtmp-1,nCellHeight,xtmp-1,(tempRowFlag==TRUE)?(nRowNum*nCellHeight+nTitleHeight):m_DRect.Height());//紧跟上个循环,画到顶上

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

void ZBReport::DrawItem(CDC *pDC,int row,int col,int xsflag,MemStkInfo * scodetmp,CurrStockData * hqp)
{
	int tempInt = lpnUsedStkIndex[nStartRow+row];
	if(tempInt>=DS_FROM && col!=0 && col!=1) 
		return;
	switch(col)
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
	case 2:		//涨跌幅
		if(fData[tempInt][0] != MEANLESS_DATA)
		{
			DrawCellText(pDC,row,col,0,fData[tempInt][0], 2,GetItemColor(fData[tempInt][0],0.0));
			if(m_pWriteFile)	m_WriteStr+=FloatToStr(fData[tempInt][0]);
		}
		else
			DrawCellText(pDC,row,col,0,MEANLESS_STR,GridColor.Grid_Level_Color);
		break;
	case 3: //开盘
		if(fData[tempInt][1] != MEANLESS_DATA)
		{
			DrawCellText(pDC,row,col,0,fData[tempInt][1], xsflag,GridColor.Grid_Level_Color);
			if(m_pWriteFile)	m_WriteStr+=FloatToStr(fData[tempInt][1]);
		}
		else
			DrawCellText(pDC,row,col,0,MEANLESS_STR,GridColor.Grid_Level_Color);
		break;
	case 4:  //最高
		if(fData[tempInt][2] != MEANLESS_DATA)
		{
			DrawCellText(pDC,row,col,0,fData[tempInt][2], xsflag,GridColor.Grid_Level_Color);
			if(m_pWriteFile)	m_WriteStr+=FloatToStr(fData[tempInt][2]);
		}
		else
			DrawCellText(pDC,row,col,0,MEANLESS_STR,GridColor.Grid_Level_Color);
		break;
	case 5:  //最低
		if(fData[tempInt][3] != MEANLESS_DATA)
		{
			DrawCellText(pDC,row,col,0,fData[tempInt][3], xsflag,GridColor.Grid_Level_Color);
			if(m_pWriteFile)	m_WriteStr+=FloatToStr(fData[tempInt][3]);
		}
		else
			DrawCellText(pDC,row,col,0,MEANLESS_STR,GridColor.Grid_Level_Color);
		break;
	case 6:  //收盘
		if(fData[tempInt][4] != MEANLESS_DATA)
		{
			DrawCellText(pDC,row,col,0,fData[tempInt][4], xsflag,GridColor.Grid_Level_Color);
			if(m_pWriteFile)	m_WriteStr+=FloatToStr(fData[tempInt][4]);
		}
		else
			DrawCellText(pDC,row,col,0,MEANLESS_STR,GridColor.Grid_Level_Color);
		break;
	case 7:  //成交数量
		if( !testzs(scodetmp) && fData[tempInt][5] != MEANLESS_DATA)
		{
			DrawCellText( pDC, row, col,0, GridColor.Grid_Volume_Color,MakeVol(fData[tempInt][5]));
			if(m_pWriteFile)	m_WriteStr+=MakeVol(fData[tempInt][5]);
		}
		else
			DrawCellText(pDC,row,col,0,MEANLESS_STR,GridColor.Grid_Level_Color);
		break;
	case 8:  //成交金额
		if( fData[tempInt][6] != MEANLESS_DATA )
		{
			DrawCellText( pDC, row, col,0, GridColor.Grid_Other_Color,MakeJE(fData[tempInt][6]));
			if(m_pWriteFile)	m_WriteStr+=MakeJE(fData[tempInt][6]);
		}
		else
			DrawCellText(pDC,row,col,0,MEANLESS_STR,GridColor.Grid_Level_Color);
		break;
	case 9:   //换手率
		if( fData[tempInt][7] != MEANLESS_DATA )
		{
			DrawCellText(pDC,row,col,0,fData[tempInt][7],xsflag,GridColor.Grid_Level_Color);
			if(m_pWriteFile)	m_WriteStr+=FloatToStr(fData[tempInt][7]);
		}
		else
			DrawCellText(pDC,row,col,0,MEANLESS_STR,GridColor.Grid_Level_Color);		
		break;
	default:  
		if(fData[tempInt][col-m_nFixedLeadCol] != MEANLESS_DATA) //指标排序栏目
		{
			DrawCellText(pDC,row,col,0,fData[tempInt][col-m_nFixedLeadCol],xsflag,VipColor.TXTCOLOR);
			if(m_pWriteFile)	m_WriteStr+=FloatToStr(fData[tempInt][col-m_nFixedLeadCol]);
		}
		else
			DrawCellText(pDC,row,col,0,MEANLESS_STR,GridColor.Grid_Level_Color);
		break;
	}
	if(m_pWriteFile)
		m_WriteStr+="\t";
}

//当鼠标点中一行时
void ZBReport::OnClick(CDC *pDC,CPoint point,BOOL IsRight)
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
		CNewMenu *pPopup=(CNewMenu *)menu.GetSubMenu(0); //历史分析右键菜单
		pPopup->SetDefaultItem(ID_RIGHT_OPEN);
		(pDC->GetWindow())->ClientToScreen (&point);
		pPopup->TrackPopupMenu (TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,AfxGetMainWnd());
	}
}

void ZBReport::OnLButtonDown(UINT nFlags, CPoint point) 
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
		m_rgn.CreateRectRgn (m_Rect.left+Left,m_Rect.top+nTitleHeight-nSigTitleHeight,m_Rect.left+Left+Width,m_Rect.top+nTitleHeight);
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

//使这个报表睡眠
void ZBReport::Sleep()
{
	pSwitchBar->SetWindowPos(0,0,0,0,0,SWP_SHOWWINDOW);
}

//唤醒这个已经激活的报表
void ZBReport::WakeUp()
{
	(m_pView->GetParent())->SetWindowText (_F("报表分析-"+m_strTitle));
	DrawFlag = DRAW_ALL;
	m_pView->Invalidate(FALSE);
	RefreshScrollBar();
}

void ZBReport::OnCtrlLeft()
{
	if(nStartCol == 0) return;

	int xtmp,width;
	xtmp = nCellHeadWidth+(int)(lpnZBItemWidth[0]*FontSize.cx/2)+(int)(lpnZBItemWidth[1]*FontSize.cx/2);
	for(int i = nStartCol;i >= 0&&xtmp < m_DRect.Width();i--)
	{
		width = (int)(lpnZBItemWidth[i+m_nFixedLeadCol]*FontSize.cx/2);
		xtmp += width;
	}
	if(xtmp > m_DRect.Width()) nStartCol = i+1;
	else if(xtmp == m_DRect.Width()) nStartCol = i;
	else nStartCol = 0;

	DrawFlag = DRAW_NOREADDATA;
	m_pView->Invalidate(FALSE);
}

int  ZBReport::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_PAINT:
		m_pView->Invalidate(FALSE);
		return 1;	
	}
	return 0;
}
