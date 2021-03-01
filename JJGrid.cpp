// JJGrid.cpp: implementation of the JJGrid class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tdxw.h"
#include "JJGrid.h"
#include "FundHoldStockDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

JJGrid::JJGrid():Grid(2,TRUE)
{
	nCurFundFlag = 0;
	m_pFundHoldStock = NULL;
	m_pFundBase = NULL;
	nFHSNum = 0;
	nFundNum = 0;
	nFHSTotalNum = 0;
	bLoadOk = FALSE;
	SetPartMode(FALSE);
	m_vtHoldStock.clear();
	lpszItemName = NULL;
	lpfWidth = NULL;
}

JJGrid::~JJGrid()
{
	m_vtHoldStock.clear();
}

BOOL JJGrid::InitGrid(int nflag)
{	
	if(!g_bFundDataAlwaysSync && !g_bDebugMode)
	{
		g_nDataSyncType=DATASYNC_JJ;
		AfxGetMainWnd()->SendMessage(WM_COMMAND,ID_DATASYNC);
	}
	GetFundBaseData();	
	nCurRow = nStartRow = nStartCol = 0;
	memset(lpnColType,0,sizeof(int)*BASECOL);
	if(nflag>=0)
	{
		if (nflag>=OPENFUND && nflag<=HOLDSTOCK)
			nCurFundFlag = nflag;		
		else
			nCurFundFlag = OPENFUND;
	}
	if (HOLDSTOCK == nCurFundFlag)
	{
		nTotalCol = STOCKCOL-m_nFixedLeadCol;
		(m_pView->GetParent())->SetWindowText (_F("基金分析平台-基金重仓股票"));
	}
	else if(HBFUND != nCurFundFlag)
	{
		nTotalCol = BASECOL-m_nFixedLeadCol;
		lpnColType[1] = lpnColType[10] = lpnColType[12] = lpnColType[13] = lpnColType[14] = 1;
		if(OPENFUND == nCurFundFlag)
			(m_pView->GetParent())->SetWindowText (_F("基金分析平台-开放式基金"));
		else
			(m_pView->GetParent())->SetWindowText (_F("基金分析平台-封闭式基金"));
	}
	else
	{
		nTotalCol = HBCOL-m_nFixedLeadCol;
		lpnColType[1] = lpnColType[9] = lpnColType[10] = 1;
		(m_pView->GetParent())->SetWindowText (_F("基金分析平台-货币型基金"));
	}
	
	if(CLOSEDFUND == nCurFundFlag)
	{
		lpszItemName = lpszJJItemName;
		lpfWidth = lpnCJJItemWidth;
	}
	else if(HBFUND == nCurFundFlag)
	{
		lpszItemName = lpszhbItemName;
		lpfWidth = lpnhbItemWidth;
	}
	else if(HOLDSTOCK == nCurFundFlag)
	{
		lpszItemName = lpszJJStockName;
		lpfWidth = lpnJJStockWidth;
	}
	else
	{
		lpszItemName = lpszJJItemName;
		lpfWidth = lpnOJJItemWidth;
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
	
	return TRUE;
}

BOOL JJGrid::GetData()
{	
	int ntmpNum = 0;

	nFHSNum = g_nHoldNum;
	m_pFundHoldStock = g_pFundHoldStock;
	nFundNum = g_nFundNum;
	m_pFundBase = g_pFundBase;
	InsertStockInfo();
	
	if (OPENFUND == nCurFundFlag)
	{
		for(int i=0;i<nFundNum;i++)
		{
			if ('0' == m_pFundBase[i].chType) 
				lpnUsedStkIndex[ntmpNum++] = i;
		}
		nTotalRow = ntmpNum;
	}
	else if (CLOSEDFUND == nCurFundFlag)
	{
		for(int i=0;i<nFundNum;i++)
		{
			if ('1' == m_pFundBase[i].chType) 
				lpnUsedStkIndex[ntmpNum++] = i;
		}
		nTotalRow = ntmpNum;
	}
	else if(HBFUND == nCurFundFlag)
	{
		for(int i=0;i<nFundNum;i++)
		{
			if ('2' == m_pFundBase[i].chType) 
				lpnUsedStkIndex[ntmpNum++] = i;
		}
		nTotalRow = ntmpNum;
	}
	else if (HOLDSTOCK == nCurFundFlag)
	{
		for(int i=0;i<nFHSTotalNum;i++)
		{
			lpnUsedStkIndex[i] = i;
		}
		nTotalRow = nFHSTotalNum;
	}

	return TRUE;
}

void JJGrid::Sort()
{
	int i;
	BOOL bOtherSort = FALSE;
	if(lpnColType[nCurSortIndex])		//按证券名称排序
	{
		for (i = 0;i < nTotalRow;i++) 
		{
			strcpy(lpSort2[i].StrValue, GetSortString(lpnUsedStkIndex[i])); //取排序值
			lpSort2[i].nIndex = lpnUsedStkIndex[i];
			lpSort2[i].tag = 0;
		}
		qsort((void *) lpSort2, nTotalRow, sizeof(struct sort_struc2), sort_func2 );
		if (bAscendSort == FALSE)
			for ( i=0;i < nTotalRow;i++) lpnUsedStkIndex[i] = lpSort2[i].nIndex;
			else	
				for ( i=0;i < nTotalRow;i++) lpnUsedStkIndex[i] = lpSort2[nTotalRow-1-i].nIndex;
	}
	else if(!lpnColType[nCurSortIndex])	
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
		else 				//其它排序
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

void JJGrid::DrawGrid(CDC* pDC)
{
	GetDrawRect(m_DRect);
	if(m_DRect.Height() < 40) return;
	int i,j;

	if(!lpfWidth) return;

	CREATE_GRIDMEMDC
	GetSomeSize(&memDC);

	//画栏目行,在其中初始化一些变量:列数nColNum
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
	
	int xtmp = nCellHeadWidth ;
	for(j = 0;j < m_nFixedLeadCol+nColNum;j++) //画列线
	{			
		g_d.DrawLine(&memDC,xtmp-1,nCellHeight,xtmp-1,
			(tempRowFlag==TRUE)?(nRowNum*nCellHeight+nTitleHeight):m_DRect.Height());
		if(j < m_nFixedLeadCol)	
			xtmp+=(int)(lpfWidth[j]*FontSize.cx/2);
		else		
			xtmp+=(int)(lpfWidth[j+nStartCol]*FontSize.cx/2);
	}
	g_d.DrawLine(&memDC,xtmp-1,nCellHeight,xtmp-1,(tempRowFlag==TRUE)?(nRowNum*nCellHeight+nTitleHeight):m_DRect.Height());//紧跟上个循环,画到顶上

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

void JJGrid::OnSize(UINT nType, int cx, int cy)
{
	SetRect(CRect(0,0,cx,cy));
}

void JJGrid::InsertSwitch(int nSCType)
{
	pSwitchBar->ClearAllLabel();
	
	m_bFixedGrid = FALSE;
	pSwitchBar->InsertSwitchLabel(0,_F("开放式基金"),WM_COMMAND,ID_OPENFUND);
	pSwitchBar->InsertSwitchLabel(1,_F("封闭式基金"),WM_COMMAND,ID_CLOSEDFUND);
	pSwitchBar->InsertSwitchLabel(2,_F("货币型基金"),WM_COMMAND,ID_HBFUND);
	pSwitchBar->InsertSwitchLabel(3,_F("基金重仓股票"),WM_COMMAND,ID_FUNDHOLDSTOCK);
}

int JJGrid::ProcessMsg(UINT message, WPARAM wParam /* = 0 */, LPARAM lParam /* = 0 */)
{
	switch(message)
	{
	case WM_PAINT:
		m_pView->Invalidate(FALSE);
		return 1;
	case WM_KEYDOWN:
		{
			if (wParam == VK_TAB)
			{
				int nIndex = (nCurFundFlag+1)%4;
				InitGrid(nIndex);
				SetCurLabel(nIndex);
			}
		}
	case WM_COMMAND:
		{
			if(wParam == ID_RIGHT_OPEN)
			{
				if(HOLDSTOCK == nCurFundFlag)
				{
					short setcode;
					char* lpszSymbol = GetStockCode(setcode);
					if(NULL == lpszSymbol)	return 0;
					OpenComView(0,lpszSymbol,setcode);
				}
				else 
				{
					char * lpszCode = m_pFundBase[lpnUsedStkIndex[nCurRow]].szSymbol;
					MemStkInfo *tempStkInfo = NULL;
					tempStkInfo = g_pStockIO->Get_MemStkInfo(lpszCode);
					if(NULL == tempStkInfo)	return 0;
					OpenComView(0,tempStkInfo->Code,tempStkInfo->setcode);
				}				
			}
			if(wParam == ID_TOPSTOCK)
			{
				float fMax;
				char * lpszCode = GetTopStock(m_pFundBase[lpnUsedStkIndex[nCurRow]].szSymbol,fMax);
				if(!lpszCode) return 0;
				MemStkInfo *pStkInfo = g_pStockIO->Get_MemStkInfo(lpszCode);
				if(!pStkInfo) return 0;
				OpenComView(0,pStkInfo->Code,pStkInfo->setcode);
			}
			if (wParam == ID_TOP10STOCK && m_pFundBase)
			{
				CFundHoldStockDlg dlg;
				dlg.m_nType = nCurFundFlag;
				dlg.m_fTotalNet = m_pFundBase[lpnUsedStkIndex[nCurRow]].fTotalNet;
				strcpy(dlg.m_lpszSymbol,m_pFundBase[lpnUsedStkIndex[nCurRow]].szSymbol);
				dlg.m_strTitle.Format("%s主要持有下列股票",m_pFundBase[lpnUsedStkIndex[nCurRow]].szName);
				dlg.DoModal();
			}
			if (wParam == ID_TOPFUND && m_pFundBase)
			{
				CFundHoldStockDlg dlg;
				dlg.m_nType = nCurFundFlag;
				dlg.m_fTotalNet = m_pFundBase[lpnUsedStkIndex[nCurRow]].fTotalNet;
				strcpy(dlg.m_lpszSymbol,m_vtHoldStock[lpnUsedStkIndex[nCurRow]].szSymbol);
				dlg.m_strTitle.Format("%s主要被下列基金所持有",m_vtHoldStock[lpnUsedStkIndex[nCurRow]].szSymbol);
				dlg.DoModal();
			}
		}
	}
	return 0;
}

void JJGrid::SetCurLabel(int n)
{
	pSwitchBar->SetCurLabel(n);
}

//当鼠标点中一行时
void JJGrid::OnClick(CDC *pDC,CPoint point,BOOL IsRight)
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
		menu.LoadMenu (IDR_RIGHTMENU_JJ);
//		menu.LoadToolBar(IDR_MAINFRAME);
		CNewMenu *pPopup = NULL;
		if (HOLDSTOCK == nCurFundFlag)
		{
			pPopup=(CNewMenu *)menu.GetSubMenu(2); //股票右键菜单
			pPopup->SetDefaultItem(ID_TOPFUND);
		}
		else if(CLOSEDFUND == nCurFundFlag)
		{
			pPopup=(CNewMenu *)menu.GetSubMenu(1); //封闭基金右键菜单
			pPopup->SetDefaultItem(ID_TOP10STOCK);
		}
		else 
		{
			pPopup=(CNewMenu *)menu.GetSubMenu(0); //开放基金右键菜单
			pPopup->SetDefaultItem(ID_TOP10STOCK);
		}
		(pDC->GetWindow())->ClientToScreen (&point);
		pPopup->TrackPopupMenu (TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,AfxGetMainWnd());
	}
}

void JJGrid::OnLButtonDown(UINT nFlags, CPoint point) 
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
				DrawGrid(&dc);
			}
			else
			{
				bAscendSort = !bAscendSort;
				Sort();
				ToSomeRow(0);
				DrawFlag = DRAW_ALL;
				DrawGrid(&dc);
			}
		}
	}
}

void JJGrid::OnLButtonDbClick(UINT nFlags, CPoint point) 
{
	CClientDC dc(m_pView);	
	OnClick (&dc,point,FALSE);
	
	if(!m_pFundBase) 
		return;
	CFundHoldStockDlg dlg;
	dlg.m_nType = nCurFundFlag;
	dlg.m_fTotalNet = m_pFundBase[lpnUsedStkIndex[nCurRow]].fTotalNet;
	if(HOLDSTOCK == nCurFundFlag)
	{
		strcpy(dlg.m_lpszSymbol,m_vtHoldStock[lpnUsedStkIndex[nCurRow]].szSymbol);
		dlg.m_strTitle.Format("%s主要被下列基金所持有",m_vtHoldStock[lpnUsedStkIndex[nCurRow]].szSymbol);
	}
	else 
	{
		strcpy(dlg.m_lpszSymbol,m_pFundBase[lpnUsedStkIndex[nCurRow]].szSymbol);
		dlg.m_strTitle.Format("%s主要持有下列股票",m_pFundBase[lpnUsedStkIndex[nCurRow]].szName);
	}
	dlg.DoModal();
}

void JJGrid::SelectSomeRow(const char* lpszSymbol)
{
	if(NULL == lpszSymbol) return;
	for(int i=0;i<nFundNum;i++)
	{
		if(0 == strcmp(m_pFundBase[i].szSymbol,lpszSymbol))	break;
	}
	if(i >= nFundNum) return;
	int nType = m_pFundBase[i].chType == '0'?OPENFUND:(m_pFundBase[i].chType == '1'?CLOSEDFUND:HBFUND);
	if(nType != nCurFundFlag)
	{
		InitGrid(nType);
		SetCurLabel(nType);
	}
	for(int j=0;j<nFundNum;j++)
	{
		if(lpnUsedStkIndex[j] == i)
			break;
	}
	if (j >= nFundNum) return;
	ToSomeRow(j);
	DrawFlag = DRAW_ALL;
	m_pView->Invalidate(FALSE);	
}

void JJGrid::OnCtrlLeft()
{
	if(nStartCol == 0) return;
	
	int xtmp,width,i;

	xtmp = nCellHeadWidth+(int)(lpfWidth[0]*FontSize.cx/2)+(int)(lpfWidth[1]*FontSize.cx/2);
	for(i = nStartCol;i >= 0&&xtmp < m_DRect.Width();i--)
	{
		width = (int)(lpfWidth[i+2]*FontSize.cx/2);
		xtmp += width;
	}
	
	if(xtmp > m_DRect.Width()) nStartCol = i+1;
	else if(xtmp == m_DRect.Width()) nStartCol = i;
	else nStartCol = 0;
	DrawFlag = DRAW_NOREADDATA;
	m_pView->Invalidate(FALSE);
}

int JJGrid::GetCellLeft(int nCol,int& nWidth)
{
	int i=0,xtmp=nCellHeadWidth ;
	if(m_nFixedLeadCol<1) i=nStartCol;	//没有固定的派头列

	while(i < nCol)
	{   
		xtmp+=(int)(lpfWidth[i]*FontSize.cx/2);
		if (i == m_nFixedLeadCol-1) 
			i = nStartCol+m_nFixedLeadCol;
		else		
			i++;
	}
	nWidth = (int)(lpfWidth[nCol]*FontSize.cx/2);
	return xtmp;
}

void JJGrid::GetSomeSize(CDC* pDC)
{
	FontSize = pDC->GetTextExtent("试");
	nSigTitleHeight = FontSize.cy+6;
	nSigCellHeight = FontSize.cy+7;
	nTitleHeight = 2*nSigTitleHeight+4; //得到标题栏高度
	nCellHeight =  nSigCellHeight;		//得到方格的高度
	nCellHeadWidth = 2.2*FontSize.cx;
	if(g_GH.NoXHCol) 
		nCellHeadWidth=0;
}

float JJGrid::fGetSortValue(short xh)
{
	if (nCurSortIndex == 0) return 0.0;
	float fMax;
	if (HOLDSTOCK == nCurFundFlag)
	{
		switch(nCurSortIndex)
		{
		case 2:
			return m_vtHoldStock[xh].nHoldNum;
		case 3:
			return m_vtHoldStock[xh].fHoldAmount;
		case 4:
			return m_vtHoldStock[xh].fAmountInc;
		case 5:
			return m_vtHoldStock[xh].fComRate;
		case 6:
			return m_vtHoldStock[xh].fMarketV;		
		default:
			return MEANLESS_DATA;
		}
	}
	else
	{
		switch(nCurSortIndex) 
		{
		case 2:
			return m_pFundBase[xh].lNetDate;
		case 3:
			return m_pFundBase[xh].fNet;
		case 4:
			return m_pFundBase[xh].fLastNet;
		case 5:
			return m_pFundBase[xh].fCumNet;
		case 6:
			return m_pFundBase[xh].fFundShare;
		case 7:
			return m_pFundBase[xh].fTotalNet;
		case 8:
			return m_pFundBase[xh].fTotalNet-m_pFundBase[xh].fLastTotalNet;
		}
		if(HBFUND != nCurFundFlag)
		{
			switch(nCurSortIndex)
			{
			case 9:
				return m_pFundBase[xh].fStockRate;
			case 11:
				{
					GetTopStock(m_pFundBase[xh].szSymbol,fMax);
					return fMax;
				}			
			default:
				return MEANLESS_DATA;
			}
		}		
	}
	return 0.0;
}

char*   JJGrid::GetSortString(short xh)
{
	if(HOLDSTOCK == nCurFundFlag)
	{
		switch(nCurSortIndex)
		{
		case 1:
			return m_vtHoldStock[xh].szName;
		default:
			return "";
		}
	}
	else if(HBFUND == nCurFundFlag)
	{
		switch(nCurSortIndex)
		{
		case 1:
			return m_pFundBase[xh].szName;
		case 9:
			return m_pFundBase[xh].szCompanyAName;
		case 10:
			return m_pFundBase[xh].szInvestStyle;
		default:
			return "";
		}
	}
	else
	{
		float ftmp;
		switch(nCurSortIndex)
		{
		case 1:
			return m_pFundBase[xh].szName;
		case 10:
			return GetTopStock(m_pFundBase[xh].szSymbol,ftmp);
		case 12:
			return m_pFundBase[xh].szCompanyAName;
		case 13:
			return m_pFundBase[xh].szInvestStyle;
		case 14:
			return m_pFundBase[xh].szInvestStylePro;
		default:
			return "";
		}
	}	
}

void JJGrid::DrawTitle(CDC *pDC)
{
	int i,j,xtmp,width;
	//写标题区
	CString tempStr1;
	int pos=8;
		
	if(CLOSEDFUND == nCurFundFlag)
		tempStr1.Format("封闭式基金列表");
	else if(HBFUND == nCurFundFlag)
		tempStr1.Format("货币型基金列表");
	else if(HOLDSTOCK == nCurFundFlag)
		tempStr1.Format("基金重仓股票列表");
	else
		tempStr1.Format("开放式基金列表");

	CSize size = g_d.DisplayText(pDC,pos,3,GridColor.Grid_Up_Color,0,_F(tempStr1));
	pos += size.cx+5;
	if(m_pWriteFile)	m_WriteStr+=_F(tempStr1);
	
	if(nCurSortIndex != 0)
	{
		tempStr1.Format("-- %s %s",lpszItemName[nCurSortIndex],bAscendSort?"降序":"升序");
		size = g_d.DisplayText(pDC,pos,3,GridColor.Grid_Volume_Color,0,_F(tempStr1));
		pos += size.cx+10;
		if(m_pWriteFile)	
			m_WriteStr+=(CString)"  "+_F(tempStr1);
	}
	
	if(m_pWriteFile)	
	{
		m_pWriteFile->WriteString(m_WriteStr+(CString)"\n");
		m_WriteStr = "";
	}
	int tempTitlePos = nSigTitleHeight+6;
	xtmp = nCellHeadWidth;
	for(i = j = 0;j < nTotalCol+m_nFixedLeadCol-1 && xtmp < m_DRect.right;i++) 
	{
		if (i>=m_nFixedLeadCol) 
			j = i+nStartCol;
		else	  
			j = i;
		width = (int)(lpfWidth[j]*FontSize.cx/2);
		
		if( (nCurSortIndex != ((i>=m_nFixedLeadCol)?(nStartCol+i):i)) || (nCurSortIndex == 0) )
			g_d.DisplayText(pDC,(j==0)?(xtmp+2):xtmp,tempTitlePos,GridColor.GridTitleTextColor,(j==0)?0:(width-3),_F(lpszItemName[j]));
		else
		{	
			g_d.DisplayText(pDC,xtmp,tempTitlePos,GridColor.GridTitleTextColor,width-FontSize.cx/2-3,_F(lpszItemName[j]));
			g_d.DisplayText(pDC,xtmp,tempTitlePos,GridColor.GridTitleArrowColor,width,(bAscendSort==TRUE)?_F("↓"):_F("↑"));
		}
		xtmp += width;
		if(m_pWriteFile) 
			m_WriteStr+=_F(lpszItemName[j])+(CString)"\t";
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

void JJGrid::DrawItem(CDC *pDC,int row,int col)
{
	if (HOLDSTOCK == nCurFundFlag) 
		DrawHoldStock(pDC,row,col);
	else
		DrawFundBase(pDC,row,col);
}

void JJGrid::DrawFundBase(CDC *pDC,int row,int col)
{
	int tempInt = lpnUsedStkIndex[nStartRow+row];
	float ftmp = 0.0;
	CString strtmp;
	char* lpszStockCode = NULL;
	MemStkInfo *tempStkInfo = NULL;
	static float fMax = 0.0;
	static BOOL bFlag = FALSE;
	switch(col)
	{
	case 0:
		DrawCellText(pDC,row,col,0,m_pFundBase[tempInt].szSymbol,GridColor.Grid_CodeName_Color,TRUE);
		if(m_pWriteFile)	m_WriteStr += m_pFundBase[tempInt].szSymbol;
		break;
	case 1:
		DrawCellText(pDC,row,col,0,_F(m_pFundBase[tempInt].szName),GridColor.Grid_CodeName_Color);
		if(m_pWriteFile)	m_WriteStr += _F(m_pFundBase[tempInt].szName);
		break;	
	case 2:
		strtmp.Format("%d",m_pFundBase[tempInt].lNetDate);
		DrawCellText(pDC,row,col,0,strtmp.GetBuffer(10),VipColor.TXTCOLOR);	
		strtmp.ReleaseBuffer();
		if(m_pWriteFile)	m_WriteStr += strtmp;
		break;
	case 3:
		DrawCellText(pDC,row,col,0,m_pFundBase[tempInt].fNet,4,GridColor.Grid_Other_Color);
		strtmp.Format("%.4f",m_pFundBase[tempInt].fNet);
		if(m_pWriteFile)	m_WriteStr += strtmp;
		break;
	case 4:
		DrawCellText(pDC,row,col,0,m_pFundBase[tempInt].fLastNet,4,GridColor.Grid_Other_Color);
		strtmp.Format("%.4f",m_pFundBase[tempInt].fLastNet);
		if(m_pWriteFile)	m_WriteStr += strtmp;
		break;
	case 5:
		DrawCellText(pDC,row,col,0,m_pFundBase[tempInt].fCumNet,4,GridColor.Grid_Other_Color);
		strtmp.Format("%.4f",m_pFundBase[tempInt].fCumNet);
		if(m_pWriteFile)	m_WriteStr += strtmp;
		break;
	case 6:
		DrawCellText(pDC,row,col,0,m_pFundBase[tempInt].fFundShare/10000,2,GridColor.Grid_Volume_Color);
		strtmp.Format("%.2f",m_pFundBase[tempInt].fFundShare/10000);
		if(m_pWriteFile)	m_WriteStr += strtmp;
		break;
	case 7:
		DrawCellText(pDC,row,col,0,m_pFundBase[tempInt].fTotalNet/10000,1,GridColor.Grid_Other_Color);
		strtmp.Format("%.1f",m_pFundBase[tempInt].fTotalNet/10000);
		if(m_pWriteFile)	m_WriteStr += strtmp;
		break;
	case 8:
		ftmp = m_pFundBase[tempInt].fTotalNet-m_pFundBase[tempInt].fLastTotalNet;
		DrawCellText(pDC,row,col,0,ftmp/10000,1,GridColor.Grid_Other_Color);
		strtmp.Format("%.1f",ftmp/10000);
		if(m_pWriteFile)	m_WriteStr += strtmp;
		break;
	case 9:
		if(HBFUND == nCurFundFlag)
		{
			DrawCellText(pDC,row,col,0,_F(m_pFundBase[tempInt].szCompanyAName),VipColor.TXTCOLOR,FALSE);
			if(m_pWriteFile)	m_WriteStr += _F(m_pFundBase[tempInt].szCompanyAName);
		}
		else
		{
			DrawCellText(pDC,row,col,0,m_pFundBase[tempInt].fStockRate,2,VipColor.TXTCOLOR);
			strtmp.Format("%.2f",m_pFundBase[tempInt].fStockRate);
			if(m_pWriteFile)	m_WriteStr += strtmp;
		}		
		break;
	case 10:
		if(HBFUND == nCurFundFlag)
		{
			DrawCellText(pDC,row,col,0,_F(m_pFundBase[tempInt].szInvestStyle),VipColor.TXTCOLOR,FALSE);
			if(m_pWriteFile)	m_WriteStr += _F(m_pFundBase[tempInt].szInvestStyle);
		}
		else
		{
			lpszStockCode = GetTopStock(m_pFundBase[tempInt].szSymbol,fMax);
			if(strlen(lpszStockCode)<1)
			{
				bFlag = TRUE;
				break;
			}
			tempStkInfo = g_pStockIO->Get_MemStkInfo(lpszStockCode);
			if (!tempStkInfo)
			{
				DrawCellText(pDC,row,col,0,lpszStockCode,GridColor.Grid_CodeName_Color,TRUE);
				if(m_pWriteFile)	m_WriteStr += lpszStockCode;	
			}
			else
			{
				DrawCellText(pDC,row,col,0,tempStkInfo->Name,GridColor.Grid_CodeName_Color,TRUE);
				if(m_pWriteFile)	m_WriteStr += tempStkInfo->Name;
			}		
		}
		break;
	case 11:
		if(bFlag)
		{
			bFlag = FALSE;
			break;
		}
		DrawCellText(pDC,row,col,0,fMax/10000,0.0,2);
		strtmp.Format("%.2f",fMax/10000);
		if(m_pWriteFile)	m_WriteStr += strtmp;
		break;
	case 12:
		DrawCellText(pDC,row,col,0,_F(m_pFundBase[tempInt].szCompanyAName),VipColor.TXTCOLOR,FALSE);
		if(m_pWriteFile)	m_WriteStr += _F(m_pFundBase[tempInt].szCompanyAName);
		break;
	case 13:
		DrawCellText(pDC,row,col,0,_F(m_pFundBase[tempInt].szInvestStyle),VipColor.TXTCOLOR,FALSE);
		if(m_pWriteFile)	m_WriteStr += _F(m_pFundBase[tempInt].szInvestStyle);
		break;
	case 14:
		DrawCellText(pDC,row,col,0,_F(m_pFundBase[tempInt].szInvestStylePro),VipColor.TXTCOLOR,FALSE);
		if(m_pWriteFile)	m_WriteStr += _F(m_pFundBase[tempInt].szInvestStylePro);
		break;
	default:
		break;
	}
}

void JJGrid::DrawHbFund(CDC *pDC,int row,int col)
{
	int tempInt = lpnUsedStkIndex[nStartRow+row];
	float ftmp = 0.0;
	CString strtmp;
	char* lpszStockCode = NULL;
	MemStkInfo *tempStkInfo = NULL;
	static float fMax = 0.0;
	static BOOL bFlag = FALSE;
	switch(col)
	{
	case 0:
		DrawCellText(pDC,row,col,0,m_pFundBase[tempInt].szSymbol,GridColor.Grid_CodeName_Color,TRUE);
		if(m_pWriteFile)	m_WriteStr += m_pFundBase[tempInt].szSymbol;
		break;
	case 1:
		DrawCellText(pDC,row,col,0,_F(m_pFundBase[tempInt].szName),GridColor.Grid_CodeName_Color);
		if(m_pWriteFile)	m_WriteStr += _F(m_pFundBase[tempInt].szName);
		break;	
	case 2:
		strtmp.Format("%d",m_pFundBase[tempInt].lNetDate);
		DrawCellText(pDC,row,col,0,strtmp.GetBuffer(10),VipColor.TXTCOLOR);	
		strtmp.ReleaseBuffer();
		if(m_pWriteFile)	m_WriteStr += strtmp;
		break;
	case 3:
		DrawCellText(pDC,row,col,0,m_pFundBase[tempInt].fNet,4,GridColor.Grid_Other_Color);
		strtmp.Format("%.4f",m_pFundBase[tempInt].fNet);
		if(m_pWriteFile)	m_WriteStr += strtmp;
		break;
	case 4:
		DrawCellText(pDC,row,col,0,m_pFundBase[tempInt].fLastNet,4,GridColor.Grid_Other_Color);
		strtmp.Format("%.4f",m_pFundBase[tempInt].fLastNet);
		if(m_pWriteFile)	m_WriteStr += strtmp;
		break;
	case 5:
		DrawCellText(pDC,row,col,0,m_pFundBase[tempInt].fCumNet,4,GridColor.Grid_Other_Color);
		strtmp.Format("%.4f",m_pFundBase[tempInt].fCumNet);
		if(m_pWriteFile)	m_WriteStr += strtmp;
		break;
	case 6:
		DrawCellText(pDC,row,col,0,m_pFundBase[tempInt].fFundShare/10000,2,GridColor.Grid_Volume_Color);
		strtmp.Format("%.2f",m_pFundBase[tempInt].fFundShare/10000);
		if(m_pWriteFile)	m_WriteStr += strtmp;
		break;
	case 7:
		DrawCellText(pDC,row,col,0,m_pFundBase[tempInt].fTotalNet/10000,1,GridColor.Grid_Other_Color);
		strtmp.Format("%.1f",m_pFundBase[tempInt].fTotalNet/10000);
		if(m_pWriteFile)	m_WriteStr += strtmp;
		break;
	case 8:
		ftmp = m_pFundBase[tempInt].fTotalNet-m_pFundBase[tempInt].fLastTotalNet;
		DrawCellText(pDC,row,col,0,ftmp/10000,1,GridColor.Grid_Other_Color);
		strtmp.Format("%.1f",ftmp/10000);
		if(m_pWriteFile)	m_WriteStr += strtmp;
		break;
	case 9:
		DrawCellText(pDC,row,col,0,_F(m_pFundBase[tempInt].szCompanyAName),VipColor.TXTCOLOR,FALSE);
		if(m_pWriteFile)	m_WriteStr += _F(m_pFundBase[tempInt].szCompanyAName);
		break;
	case 10:
		DrawCellText(pDC,row,col,0,_F(m_pFundBase[tempInt].szInvestStyle),VipColor.TXTCOLOR,FALSE);
		if(m_pWriteFile)	m_WriteStr += _F(m_pFundBase[tempInt].szInvestStyle);
		break;
	default:
		break;
	}
}

void JJGrid::DrawHoldStock(CDC *pDC,int row,int col)
{
	int tempInt = lpnUsedStkIndex[nStartRow+row];
	CString strtmp;
	switch(col)
	{
	case 0:
		DrawCellText(pDC,row,col,0,m_vtHoldStock[tempInt].szSymbol,GridColor.Grid_CodeName_Color,TRUE);
		if(m_pWriteFile)	m_WriteStr += m_vtHoldStock[tempInt].szSymbol;
		break;
	case 1:		
		DrawCellText(pDC,row,col,0,m_vtHoldStock[tempInt].szName,GridColor.Grid_CodeName_Color,TRUE);
		if (m_pWriteFile)	m_WriteStr += m_vtHoldStock[tempInt].szName;
		break;
	case 2:
		DrawCellText(pDC,row,col,0,VipColor.TXTCOLOR,"%d",m_vtHoldStock[tempInt].nHoldNum);
		strtmp.Format("%d",m_vtHoldStock[tempInt].nHoldNum);
		if(m_pWriteFile)	m_WriteStr += strtmp;
		break;
	case 3:
		DrawCellText(pDC,row,col,0,m_vtHoldStock[tempInt].fHoldAmount/10000,2,GridColor.Grid_Volume_Color);
		strtmp.Format("%.2f",m_vtHoldStock[tempInt].fHoldAmount/10000);
		if(m_pWriteFile)	m_WriteStr += strtmp;
		break;
	case 4:
		DrawCellText(pDC,row,col,0,m_vtHoldStock[tempInt].fAmountInc/10000,0.0,2);
		strtmp.Format("%.2f",m_vtHoldStock[tempInt].fAmountInc/10000);
		if(m_pWriteFile)	m_WriteStr += strtmp;
		break;
	case 5:
		DrawCellText(pDC,row,col,0,m_vtHoldStock[tempInt].fComRate,2,VipColor.TXTCOLOR);
		strtmp.Format("%.2f",m_vtHoldStock[tempInt].fComRate);
		if(m_pWriteFile)	m_WriteStr += strtmp;
		break;
	case 6:
		DrawCellText(pDC,row,col,0,m_vtHoldStock[tempInt].fMarketV/10000,2,GridColor.Grid_Other_Color);
		strtmp.Format("%.2f",m_vtHoldStock[tempInt].fMarketV/10000);
		if(m_pWriteFile)	m_WriteStr += strtmp;
		break;	
	default:
		break;
	}
}

char*	JJGrid::GetTopStock(const char* lpszFund,float& fTotalInc)
{
	if (NULL == lpszFund) 	return "";
	for(int nFundIndex=0;nFundIndex<nFHSNum;nFundIndex++)
		if (strncmp(m_pFundHoldStock[nFundIndex].szSymbol,lpszFund,6)==0) break;
	if(nFundIndex >= nFHSNum) return "";

	float ftmp = 0.0;
	float fMax = 0.0;
	int nStockMaxIndex = 0;
	MemStkInfo *pStkInfo = NULL;
	struct CurrStockData CurrHq;
	for(int nStockIndex=0;nStockIndex<m_pFundHoldStock[nFundIndex].nNum;nStockIndex++)
	{
		pStkInfo = g_pStockIO->Get_MemStkInfo(m_pFundHoldStock[nFundIndex].stockinfo[nStockIndex].szSymbol);
		if(!pStkInfo) continue;
		g_pStockIO->ReadCurrStockData(pStkInfo,&CurrHq); //取得行情
		if(CurrHq.Now < COMPPREC)
			ftmp = 0.0;
		else
			ftmp = (CurrHq.Now - CurrHq.Close)*m_pFundHoldStock[nFundIndex].stockinfo[nStockIndex].fAcount;
		if(0 == nStockIndex)
			fMax = ftmp;
		if(ftmp>fMax)
		{
			fMax = ftmp;
			nStockMaxIndex = nStockIndex;
		}
	}
	fTotalInc = fMax;
	return m_pFundHoldStock[nFundIndex].stockinfo[nStockMaxIndex].szSymbol;
}

void JJGrid::InsertStockInfo()
{
	m_vtHoldStock.clear();
	m_map.RemoveAll();
	m_map.InitHashTable(107);
	
	int nStock = 0;
	int nPos;
	int nSize = 0;
	CString strtmp;
	MemStkInfo *pStkInfo = NULL;
	struct CurrStockData CurrHq;
	HOLDSTOCKINFO tmpStockinfo;
	
	for(int nFund = 0;nFund<nFHSNum;nFund++)
	{
		for(nStock=0;nStock<m_pFundHoldStock[nFund].nNum;nStock++)
		{
			if(m_map.Lookup(m_pFundHoldStock[nFund].stockinfo[nStock].szSymbol,strtmp))
			{
				nPos = atoi(strtmp);
				tmpStockinfo = m_vtHoldStock[nPos];
				tmpStockinfo.nHoldNum++;
				tmpStockinfo.fHoldAmount += m_pFundHoldStock[nFund].stockinfo[nStock].fAcount;
				tmpStockinfo.fAmountInc += m_pFundHoldStock[nFund].stockinfo[nStock].fAcount-m_pFundHoldStock[nFund].stockinfo[nStock].fLastAcount;
				m_vtHoldStock[nPos] = tmpStockinfo;
			}
			else
			{
				memset(&tmpStockinfo,0,sizeof(HOLDSTOCKINFO));
				nSize = m_vtHoldStock.size();
				pStkInfo = g_pStockIO->Get_MemStkInfo(m_pFundHoldStock[nFund].stockinfo[nStock].szSymbol);
				if(NULL == pStkInfo) continue;
				strtmp.Format("%d",nSize);
				m_map.SetAt(m_pFundHoldStock[nFund].stockinfo[nStock].szSymbol,strtmp);
				strncpy(tmpStockinfo.szSymbol,m_pFundHoldStock[nFund].stockinfo[nStock].szSymbol,6);
				strcpy(tmpStockinfo.szName,pStkInfo->Name);
				tmpStockinfo.nHoldNum = 1;
				tmpStockinfo.fHoldAmount = m_pFundHoldStock[nFund].stockinfo[nStock].fAcount;
				tmpStockinfo.fAmountInc += (m_pFundHoldStock[nFund].stockinfo[nStock].fAcount-m_pFundHoldStock[nFund].stockinfo[nStock].fLastAcount);
				m_vtHoldStock.push_back(tmpStockinfo);
			}
		}
	}
	nFHSTotalNum = m_vtHoldStock.size();
	for(nPos=0;nPos<nFHSTotalNum;nPos++)
	{
		pStkInfo = g_pStockIO->Get_MemStkInfo(m_vtHoldStock[nPos].szSymbol);
		if(!pStkInfo) continue;
		g_pStockIO->ReadCurrStockData(pStkInfo,&CurrHq); //取得行情
		m_vtHoldStock[nPos].fComRate = m_vtHoldStock[nPos].fHoldAmount*100/(pStkInfo->J_zgb*10000);
		if(CurrHq.Now < COMPPREC)
			m_vtHoldStock[nPos].fMarketV = m_vtHoldStock[nPos].fHoldAmount*CurrHq.Close;
		else
			m_vtHoldStock[nPos].fMarketV = m_vtHoldStock[nPos].fHoldAmount*CurrHq.Now;
	}
}

char* JJGrid::GetStockCode(short& setcode)
{
	char* lpszSymbol=NULL;
	if(HOLDSTOCK == nCurFundFlag)
		lpszSymbol = m_vtHoldStock[lpnUsedStkIndex[nCurRow]].szSymbol;
	else
		lpszSymbol = m_pFundBase[lpnUsedStkIndex[nCurRow]].szSymbol;
	if(!lpszSymbol) return NULL;
	MemStkInfo *pStkInfo = g_pStockIO->Get_MemStkInfo(lpszSymbol);
	if(!pStkInfo) return NULL;
	setcode = pStkInfo->setcode;
	return lpszSymbol;
}
