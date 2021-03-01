// QRReport.cpp: implementation of the QRReport class.
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "tdxw.h"
#include "QRReport.h"
#include "ProgressWnd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

QRReport::QRReport():Grid(2,TRUE)
{
	memset(fBaseData,0,12*sizeof(float));
	memset(fData,0,MAXGPNUM*12*sizeof(float));
	m_lNowDate		= g_lOpenRq;
	OldBaseGPNo = BaseGPNo = -1;
	SetPartMode(FALSE);
}

void QRReport::SetDomain(int nBlockFlag,int WhichType)
{
	Grid::SetDomain(nBlockFlag,WhichType);

	if(!BeBlock(WhichType))
		BaseGPNo = g_pStockIO->GetIndex(SH_DPCODE);
	else
		BaseGPNo = -1;
	OldBaseGPNo = -1;

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

void QRReport::InsertSwitch(int nSCType)
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

void QRReport::SetBase(int GPNo) //将GPNo设为基准股票
{
	OldBaseGPNo = BaseGPNo;
	BaseGPNo = GPNo;

	BaseCalc(); //基准处理函数
	DrawFlag = DRAW_ALL;	
	m_pView->Invalidate(FALSE);
}

void QRReport::SetItBase()	//将当前选中的股票设为基准
{
	int index = lpnUsedStkIndex[nCurRow];
	SetBase(index);
}

void QRReport::InitReport()
{
	int i;
	
	CString tempStr;
	//得到栏目的字串
	for(i = 0;i < 5;i++)
	{
		tempStr.Format("%d日强",QRParam[i]);
		strcpy(lpszQRItemName[3+i],tempStr);  //从第三个开始
	}
	nTotalCol = 14-m_nFixedLeadCol;
    nCurRow = nStartRow = nStartCol = 0;
	nCurSortIndex = 0;
	bAscendSort = FALSE;

	DrawFlag = DRAW_ALL;	
	m_pView->Invalidate(FALSE);
	RefreshScrollBar();	//重新组织滚动条
	//在得到数据之前，也要刷新，否则由于对话框的原因，会乱屏
	GetData();  //得到数据
//	Sort();		//缺省为序号排序

	BaseCalc(); //基准处理函数
	DrawFlag = DRAW_ALL;	
	m_pView->Invalidate(FALSE);
	RefreshScrollBar();	//重新组织滚动条
}

//基准处理函数
void QRReport::BaseCalc()
{
	int i,j;

	//如果上一次有基准，则将fData数组还原
	if(OldBaseGPNo != -1)
	{
		for(i = 0;i < nTotalRow;i++)
		for(j = 0;j < 7;j++) //仅对强度栏目处理
		{
			if(fBaseData[j] != MEANLESS_DATA && fData[lpnUsedStkIndex[i]][j] != MEANLESS_DATA) //如果两者都有意义
				fData[lpnUsedStkIndex[i]][j] += fBaseData[j];
		}
	}

	if(BaseGPNo != -1)
	{
		BOOL bFind = FALSE;
		for(i = 0;i < nTotalRow;i++)	//在当前股票中寻找基准股
		{
			if(lpnUsedStkIndex[i] == BaseGPNo)
			{
				bFind = TRUE;break;
			}
		}
		if(bFind) //如果在当前股票中有此股票,此直接拷贝
		{
			for(j = 0;j < 12;j++)
				fBaseData[j] = fData[lpnUsedStkIndex[i]][j];
		}
		else	//如果没有，则从调用GetSingleGPData从文件中读取
			GetSingleGPData(BaseGPNo,&fBaseData);

		for(i = 0;i < nTotalRow;i++)
		for(j = 0;j < 7;j++) //仅对强度栏目处理
		{
			if(fBaseData[j] != MEANLESS_DATA && fData[lpnUsedStkIndex[i]][j] != MEANLESS_DATA) //如果两者都有意义
				fData[lpnUsedStkIndex[i]][j] -= fBaseData[j];
		}
	}
}

void QRReport::GetSingleGPData(int GPNo,float (*Data)[12])
{
	int nReadDateNum,nDataPtr;
	long	lLastYear;
	CurrStockData		Hq;
	int					m_nTodayNum;

	nReadDateNum = g_pStockIO->GetAnalyDataNum((*g_pStockIO)[GPNo],PER_DAY);
	struct AnalyData *pAnalyData = new AnalyData[nReadDateNum];
	memset(pAnalyData,0,nReadDateNum*sizeof(AnalyData));
	nReadDateNum = g_pStockIO->ReadAnalyDataAndCurrHQ((*g_pStockIO)[GPNo],pAnalyData,nReadDateNum,PER_DAY,&Hq,m_nTodayNum);
	if(nReadDateNum > 0  && g_bHisViewTQ)
		g_pStockIO->TQData((*g_pStockIO)[GPNo],pAnalyData,0,nReadDateNum-1,nReadDateNum,1,PER_DAY);	//1为前除权

	if(nReadDateNum > 1 &&
	   fabs(pAnalyData[nReadDateNum-1].Close-MEANLESS_DATA)>COMPPREC &&
	   fabs(pAnalyData[nReadDateNum-2].Close)>COMPPREC &&
	   fabs(pAnalyData[nReadDateNum-2].Close-MEANLESS_DATA)>COMPPREC )
			(*Data)[0] = 100.0*(pAnalyData[nReadDateNum-1].Close-pAnalyData[nReadDateNum-2].Close)/pAnalyData[nReadDateNum-2].Close;		//今日强度
	else	(*Data)[0] = MEANLESS_DATA;
	if(nReadDateNum > QRParam[0] &&
	   fabs(pAnalyData[nReadDateNum-1].Close-MEANLESS_DATA)>COMPPREC &&
	   fabs(pAnalyData[nReadDateNum-QRParam[0]-1].Close)>COMPPREC &&
	   fabs(pAnalyData[nReadDateNum-QRParam[0]-1].Close-MEANLESS_DATA)>COMPPREC )
			(*Data)[1] = 100.0*(pAnalyData[nReadDateNum-1].Close-pAnalyData[nReadDateNum-QRParam[0]-1].Close)/pAnalyData[nReadDateNum-QRParam[0]-1].Close;		//Para1日强度
	else	(*Data)[1] = MEANLESS_DATA;
	if(nReadDateNum > QRParam[1] &&
	   fabs(pAnalyData[nReadDateNum-1].Close-MEANLESS_DATA)>COMPPREC &&
	   fabs(pAnalyData[nReadDateNum-QRParam[1]-1].Close)>COMPPREC &&
	   fabs(pAnalyData[nReadDateNum-QRParam[1]-1].Close-MEANLESS_DATA)>COMPPREC )
			(*Data)[2] = 100.0*(pAnalyData[nReadDateNum-1].Close-pAnalyData[nReadDateNum-QRParam[1]-1].Close)/pAnalyData[nReadDateNum-QRParam[1]-1].Close;		//Para2日强度
	else	(*Data)[2] =MEANLESS_DATA;
	if(nReadDateNum > QRParam[2] &&
	   fabs(pAnalyData[nReadDateNum-1].Close-MEANLESS_DATA)>COMPPREC &&
	   fabs(pAnalyData[nReadDateNum-QRParam[2]-1].Close)>COMPPREC &&
	   fabs(pAnalyData[nReadDateNum-QRParam[2]-1].Close-MEANLESS_DATA)>COMPPREC )
			(*Data)[3] = 100.0*(pAnalyData[nReadDateNum-1].Close-pAnalyData[nReadDateNum-QRParam[2]-1].Close)/pAnalyData[nReadDateNum-QRParam[2]-1].Close;		//Para3日强度
	else	(*Data)[3] =MEANLESS_DATA;
	if(nReadDateNum > QRParam[3] &&
	   fabs(pAnalyData[nReadDateNum-1].Close-MEANLESS_DATA)>COMPPREC &&
	   fabs(pAnalyData[nReadDateNum-QRParam[3]-1].Close)>COMPPREC &&
	   fabs(pAnalyData[nReadDateNum-QRParam[3]-1].Close-MEANLESS_DATA)>COMPPREC )
			(*Data)[4] = 100.0*(pAnalyData[nReadDateNum-1].Close-pAnalyData[nReadDateNum-QRParam[3]-1].Close)/pAnalyData[nReadDateNum-QRParam[3]-1].Close;		//Para4日强度
	else	(*Data)[4] =MEANLESS_DATA;
	if(nReadDateNum > QRParam[4] &&
	   fabs(pAnalyData[nReadDateNum-1].Close-MEANLESS_DATA)>COMPPREC &&
	   fabs(pAnalyData[nReadDateNum-QRParam[4]-1].Close)>COMPPREC &&
	   fabs(pAnalyData[nReadDateNum-QRParam[4]-1].Close-MEANLESS_DATA)>COMPPREC )
			(*Data)[5] = 100.0*(pAnalyData[nReadDateNum-1].Close-pAnalyData[nReadDateNum-QRParam[4]-1].Close)/pAnalyData[nReadDateNum-QRParam[4]-1].Close;		//Para5日强度
	else	(*Data)[5] =MEANLESS_DATA;
	
	if(nReadDateNum > 0)
	{
		lLastYear = (pAnalyData[nReadDateNum-1].Time.Date/10000-1)*10000+pAnalyData[nReadDateNum-1].Time.Date%10000;
		nDataPtr  = 0;
		while(pAnalyData[nDataPtr].Time.Date<lLastYear&&nDataPtr<nReadDateNum)	nDataPtr++;	//查找距今最近的上一年的数据
		if(nDataPtr > 0 &&	//Modified 2001.6.2 nDataPtr必须大于0
		   nDataPtr<nReadDateNum &&
		   fabs(pAnalyData[nReadDateNum-1].Close-MEANLESS_DATA)>COMPPREC &&
		   fabs(pAnalyData[nDataPtr-1].Close)>COMPPREC &&
		   fabs(pAnalyData[nDataPtr-1].Close-MEANLESS_DATA)>COMPPREC )
				(*Data)[6] = 100.0*(pAnalyData[nReadDateNum-1].Close-pAnalyData[nDataPtr-1].Close)/pAnalyData[nDataPtr-1].Close;		//年线强度
		else	(*Data)[6] =MEANLESS_DATA;
		(*Data)[7] = pAnalyData[nReadDateNum-1].Open;			//开盘
		(*Data)[8] = pAnalyData[nReadDateNum-1].High;			//最高
		(*Data)[9] = pAnalyData[nReadDateNum-1].Low;				//最低
		(*Data)[10] = pAnalyData[nReadDateNum-1].Close;			//收盘
		(*Data)[11] = pAnalyData[nReadDateNum-1].Amount;			//成交金额
	}
	else
	{
		(*Data)[6] =  MEANLESS_DATA;

		(*Data)[7] =  MEANLESS_DATA;
		(*Data)[8] =  MEANLESS_DATA;
		(*Data)[9] =  MEANLESS_DATA;
		(*Data)[10] = MEANLESS_DATA;
		(*Data)[11] = MEANLESS_DATA;
	}
	TDEL(pAnalyData);
}

void QRReport::GetData()
{
	RefreshAllHq();

	int i,tempInt;	//nDataPtr是当前日期在读出数组中的序号

	memset(fBaseData,0,12*sizeof(float));
	memset(fData,0,MAXGPNUM*12*sizeof(float));

	//设定模态进度条对话框
	CProgressWnd wndProgress(m_pView,_F("正在计算，请稍等..."),TRUE);
	wndProgress.GoModal();
	wndProgress.SetRange(0,nTotalRow);
	wndProgress.SetStep(3);
	g_bDonotClose = TRUE;
	float (*tempData)[12];	//二级数组指针
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
		tempData = &fData[tempInt];	
		
		GetSingleGPData(tempInt,tempData);	//数组指针
	}
	g_bDonotClose = FALSE;
}

float QRReport::fGetSortValue(short xh)
{
	if(xh>=DS_FROM || nCurSortIndex == 1 || nCurSortIndex == 0 ) return 0.0; //不处理nCurSortIndex = 1的情况
	return fData[xh][nCurSortIndex - m_nFixedLeadCol];
}

//排序函数
void QRReport::Sort()
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

void QRReport::OnSize(UINT nType, int cx, int cy)
{
	SetRect(CRect(0,0,cx,cy));
}

//计算方格左边位置,Width为返回方格宽度,nCol表示列顺序中绝对序号
int QRReport::GetCellLeft(int nCol,int & nWidth) 
{   
	int i=0,xtmp=nCellHeadWidth ;
	if(m_nFixedLeadCol<1) i=nStartCol;	//没有固定的派头列
	while(i < nCol)
	{   
		xtmp+=(int)(lpnQRItemWidth[i]*FontSize.cx/2);
	    if (i == m_nFixedLeadCol-1) 
			i = nStartCol+m_nFixedLeadCol;
		else		
			i++;
	}
	nWidth = (int)(lpnQRItemWidth[nCol]*FontSize.cx/2);
	return xtmp;
}

//画标题
void QRReport::DrawTitle(CDC *pDC)
{
	int i,j,xtmp,width;
	//写标题区
	CString tempStr1;
	int pos=8;
	tempStr1 = "强弱分析报表";
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

	tempStr1.Format("%s",GetTdxDateWeekStr(m_lNowDate));
	size = g_d.DisplayText(pDC,pos,2,GridColor.Grid_Volume_Color,0,_F(tempStr1));
	pos += size.cx+5;
	if(m_pWriteFile)	m_WriteStr+=_F(tempStr1);
	size = g_d.DisplayText(pDC,pos,2,VipColor.TXTCOLOR,0,_F(" 基准:"));
	pos += size.cx+5;
	if(m_pWriteFile)	m_WriteStr+=_F(" 基准:");

	if(BaseGPNo != -1)
	{
		MemStkInfo *tempStkInfo = (*g_pStockIO)[BaseGPNo];
		tempStr1 = tempStkInfo->Name;
	}
	else	tempStr1 = "无";
	size = g_d.DisplayText(pDC,pos,2,GridColor.Grid_Volume_Color,0,tempStr1);
	pos += size.cx+10;
	if(m_pWriteFile)	m_WriteStr+=tempStr1;
	if(m_pWriteFile)	
	{
		m_pWriteFile->WriteString(m_WriteStr+(CString)"\n");
		m_WriteStr = "";
	}
	size = g_d.DisplayText(pDC,pos,2,VipColor.TXTCOLOR,0,_F("点右键进行设置"));
	pos += size.cx+5;	
	
	//开始画
	int tempTitlePos = 2*nSigTitleHeight+6;
	xtmp = nCellHeadWidth;
	for(i = j = 0;j < nTotalCol+m_nFixedLeadCol-1 && xtmp < m_DRect.right;i++)
	{
  		if (i>=m_nFixedLeadCol) 
			j = i+nStartCol;
		else	  
			j = i;
		width = (int)(lpnQRItemWidth[j]*FontSize.cx/2);
		if((nCurSortIndex != ((i>=m_nFixedLeadCol)?(nStartCol+i):i)) || (nCurSortIndex == 0) )
		{
			if(i<m_nFixedLeadCol)
				g_d.DisplayText(pDC,xtmp+10,tempTitlePos,GridColor.GridTitleTextColor,0,_F(lpszQRItemName[j]));
			else
				g_d.DisplayText(pDC,xtmp,tempTitlePos,GridColor.GridTitleTextColor,width-3,_F(lpszQRItemName[j]));
		}
		else
		{	
			g_d.DisplayText(pDC,xtmp,tempTitlePos,GridColor.GridTitleTextColor,width-FontSize.cx/2-3,_F(lpszQRItemName[j]));
			g_d.DisplayText(pDC,xtmp,tempTitlePos,GridColor.GridTitleArrowColor,width,(bAscendSort==TRUE)?_F("↓"):_F("↑"));
		}
		xtmp+=width;
		if(m_pWriteFile) 
			m_WriteStr+=_F(lpszQRItemName[j])+(CString)"\t";
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

void QRReport::GetSomeSize(CDC *pDC)
{
	FontSize = pDC->GetTextExtent("试");
	FontSize.cx+=2;
	nSigTitleHeight = FontSize.cy+6;
	nSigCellHeight = FontSize.cy+7;
	nTitleHeight = 3*nSigTitleHeight+4; //得到标题栏高度
	nCellHeight =  nSigCellHeight;	//得到方格的高度
	nCellHeadWidth = 2.2*FontSize.cx;
	if(g_GH.NoXHCol) 
		nCellHeadWidth=0;
}

//主画图函数,有两种画法DRAW_NOREADDATA和DRAW_ALL
void QRReport::DrawReport(CDC *pDC)
{
	GetDrawRect(m_DRect);
	if(m_DRect.Height() < 40) return;
	int i,j;

	CREATE_GRIDMEMDC
	GetSomeSize(&memDC);

	//画栏目行,在其中初化始一些变量:列数nColNum
	g_d.Bar(&memDC,0,0,m_DRect.Width(),nCellHeight-1,g_GUI.MixedColor(VipColor.AXISCOLOR,25,VipColor.BACKCOLOR));
	DrawTitle(&memDC);
	//调节RowNum和ColNum
	AdjustRowColNum();
	//开始画线
	int tempWidth,tempLeft;
	tempLeft = GetCellLeft(m_nFixedLeadCol+nEndCol,tempWidth);

	g_d.DrawLine(&memDC,0,nTitleHeight-nSigTitleHeight-3,m_DRect.Width(),
				nTitleHeight-nSigTitleHeight-3,GridColor.GridLineColor);

	//画分隔线(标题与基准股票栏之间)
	g_d.DrawLine(&memDC,0,nCellHeight-2,m_DRect.Width(),
				nCellHeight-2,VipColor.AXISCOLOR);

	for(i = 0;i <= nRowNum;i++)	//画行线
	{
		g_d.DrawLine(&memDC,0,nTitleHeight+i*nCellHeight-1,
				(tempColFlag==TRUE)?(tempLeft+tempWidth):(m_DRect.Width()),
				nTitleHeight+i*nCellHeight-1);
	}

	int xtmp = nCellHeadWidth ;
	for(j = 0;j < m_nFixedLeadCol+nColNum;j++) //画列线
	{			
		g_d.DrawLine(&memDC,xtmp-1,nTitleHeight-nSigTitleHeight-2,xtmp-1,
			(tempRowFlag==TRUE)?(nRowNum*nCellHeight+nTitleHeight):m_DRect.Height());
		if(j < m_nFixedLeadCol)	
			xtmp+=(int)(lpnQRItemWidth[j]*FontSize.cx/2);
		else		
			xtmp+=(int)(lpnQRItemWidth[j+nStartCol]*FontSize.cx/2);
	}
	g_d.DrawLine(&memDC,xtmp-1,nTitleHeight-nSigTitleHeight-2,xtmp-1,(tempRowFlag==TRUE)?(nRowNum*nCellHeight+nTitleHeight):m_DRect.Height());//紧跟上个循环,画到顶上

	//在方格中写内容
	MemStkInfo *tempStkInfo;
	CurrStockData CurrStockDatap;
	if(BaseGPNo != -1)	//如果有基准
	{
		//在基准行写基准数据
		tempStkInfo = (*g_pStockIO)[BaseGPNo];
		g_pStockIO->ReadCurrStockData (tempStkInfo,&CurrStockDatap);
		for(i=0;i<tempColNum+m_nFixedLeadCol;i++)
		{
			if(i<m_nFixedLeadCol)
				DrawItem(&memDC,-1,i,2,tempStkInfo,&CurrStockDatap);	//row在此情况下设为-1
			else
				DrawItem(&memDC,-1,i+nStartCol,GetXSFlag(tempStkInfo),tempStkInfo,&CurrStockDatap);
		}
		if(m_pWriteFile)
		{
			m_pWriteFile->WriteString(m_WriteStr+"\n");
			m_WriteStr = "";
		}
	}
	if(!m_bWriteAllHQ) //导出全部时
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
	else
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
	//写左边序号内容
	for(i=0;nCellHeadWidth>3 && i<tempRowNum;i++)
		g_d.DisplayText(&memDC,0,nTitleHeight+i*nCellHeight+(nCellHeight-FontSize.cy)/2,VipColor.TXTCOLOR,nCellHeadWidth-3,"%4d",nStartRow+i+1);
	//从内存DC拷贝到屏幕DC
	pDC->BitBlt (m_DRect.left,m_DRect.top,m_DRect.Width(),m_DRect.Height(),&memDC,0,0,SRCCOPY);
	
	DrawHighLine(pDC);//画指示线

	DELETE_GRIDMEMDC

	DrawFlag = DRAW_NOREADDATA;
}

void QRReport::DrawItem(CDC *pDC,int row,int col,int xsflag,MemStkInfo * scodetmp,CurrStockData * hqp)
{
	float tempData[12];	//临时数组
	int	i;
	if(row != -1)	//如果不是基准行
	{
		int tempInt = lpnUsedStkIndex[nStartRow+row];
		if(tempInt>=DS_FROM && col!=0 && col!=1) 
			return;
		for(i = 0;i < 12;i++)
			tempData[i] = fData[tempInt][i];
	}
	else
	{
		for(i = 0;i < 12;i++)
			tempData[i] = fBaseData[i];
	}
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
	case 9: //开盘
		if(tempData[7] != MEANLESS_DATA)
		{
			DrawCellText(pDC,row,col,0,tempData[7], xsflag,GridColor.Grid_Level_Color);
			if(m_pWriteFile)	m_WriteStr+=FloatToStr(tempData[7]);
		}
		else
			DrawCellText(pDC,row,col,0,MEANLESS_STR,GridColor.Grid_Level_Color);		
		break;
	case 10:  //最高
		if(tempData[8] != MEANLESS_DATA)
		{
			DrawCellText(pDC,row,col,0,tempData[8], xsflag,GridColor.Grid_Level_Color);
			if(m_pWriteFile)	m_WriteStr+=FloatToStr(tempData[8]);
		}
		else
			DrawCellText(pDC,row,col,0,MEANLESS_STR,GridColor.Grid_Level_Color);
		break;
	case 11:  //最低
		if(tempData[9] != MEANLESS_DATA)
		{
			DrawCellText(pDC,row,col,0,tempData[9], xsflag,GridColor.Grid_Level_Color);
			if(m_pWriteFile)	m_WriteStr+=FloatToStr(tempData[9]);
		}
		else
			DrawCellText(pDC,row,col,0,MEANLESS_STR,GridColor.Grid_Level_Color);
		break;
	case 12:  //收盘
		if(tempData[10] != MEANLESS_DATA)
		{
			DrawCellText(pDC,row,col,0,tempData[10], xsflag,GridColor.Grid_Level_Color);
			if(m_pWriteFile)	m_WriteStr+=FloatToStr(tempData[10]);
		}
		else
			DrawCellText(pDC,row,col,0,MEANLESS_STR,GridColor.Grid_Level_Color);
		break;
	case 13:  //成交金额
		if( tempData[11] != MEANLESS_DATA )
		{
			DrawCellText( pDC, row, col,0, GridColor.Grid_Other_Color,MakeJE(tempData[11]));
			if(m_pWriteFile)	m_WriteStr+=MakeJE(tempData[11]);
		}
		else
			DrawCellText(pDC,row,col,0,MEANLESS_STR,GridColor.Grid_Level_Color);
		break;
	default:  
		if(tempData[col-m_nFixedLeadCol] != MEANLESS_DATA) //指标排序栏目
		{
			DrawCellText(pDC,row,col,0,tempData[col-m_nFixedLeadCol],0.0,2);
			if(m_pWriteFile)	m_WriteStr+=FloatToStr(tempData[col-m_nFixedLeadCol]);
		}
		else
			DrawCellText(pDC,row,col,0,MEANLESS_STR,GridColor.Grid_Level_Color);
		break;
	}
	if(m_pWriteFile)
		m_WriteStr+="\t";
}

//当鼠标点中一行时
void QRReport::OnClick(CDC *pDC,CPoint point,BOOL IsRight)
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
		CNewMenu *pPopup=(CNewMenu *)menu.GetSubMenu(1); //历史分析右键菜单
		pPopup->SetDefaultItem(ID_RIGHT_OPEN);
		(pDC->GetWindow())->ClientToScreen (&point);
		pPopup->TrackPopupMenu (TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,AfxGetMainWnd());
	}
}

void QRReport::OnLButtonDown(UINT nFlags, CPoint point) 
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

void QRReport::Sleep()
{
	pSwitchBar->SetWindowPos(0,0,0,0,0,SWP_SHOWWINDOW);
}

void QRReport::WakeUp()
{
	(m_pView->GetParent())->SetWindowText (_F("报表分析-"+m_strTitle));
	DrawFlag = DRAW_ALL;
	m_pView->Invalidate(FALSE);
	RefreshScrollBar();
}

void QRReport::OnCtrlLeft()
{
	if(nStartCol == 0) return;

	int xtmp,width;
	xtmp = nCellHeadWidth+(int)(lpnQRItemWidth[0]*FontSize.cx/2)+(int)(lpnQRItemWidth[1]*FontSize.cx/2);
	for(int i = nStartCol;i >= 0&&xtmp < m_DRect.Width();i--)
	{
		width = (int)(lpnQRItemWidth[i+m_nFixedLeadCol]*FontSize.cx/2);
		xtmp += width;
	}
	if(xtmp > m_DRect.Width()) nStartCol = i+1;
	else if(xtmp == m_DRect.Width()) nStartCol = i;
	else nStartCol = 0;
	DrawFlag = DRAW_NOREADDATA;
	m_pView->Invalidate(FALSE);
}

int  QRReport::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_PAINT:
		m_pView->Invalidate(FALSE);
		return 1;	
	}
	return 0;
}
