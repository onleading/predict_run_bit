#include "stdafx.h"
#include "tdxw.h"
#include "Grid.h"
//////////////////////////////////////////////////////////////////////

Grid::Grid(int nFixedLeadCol,BOOL bHasSwitch,LPCOMINFO pComInfo):UBase(pComInfo)
{
	m_WriteStr = "";
	m_pWriteFile = NULL;
	m_bWriteExcel = FALSE;
	//设置首列排序标志
	nCurSortIndex = 2;
	bAscendSort = TRUE;
	m_WhichGPType = TYPE_AG;
	m_nBlockFlag = -1;
	//
	m_bHasSwitch = bHasSwitch;
	m_nFixedLeadCol = nFixedLeadCol;
	//一定要初始化，否则在弹出对话框时，OnDraw有问题
    nTotalRow = nTotalCol = nCurRow = nStartRow = nStartCol = 0;
	tempRowNum = tempColNum = 0;
	tempRowFlag = tempColFlag = FALSE;
	//将一些自适应控制变量置初值
	nTitleHeight0 = 0;
	nTitleHeight = 22;
	nCellHeight = 20;
	nSigTitleHeight = 22;
	nSigCellHeight = 20;
	nCellHeadWidth = 36;
	FontSize = CSize(8,16);

	pSwitchBar = NULL;
	
	m_bPartMode = TRUE;
	m_bWriteAllHQ = FALSE;
	m_bFixedGrid  = FALSE;
	m_SwitchWidth = 0;
}

BOOL Grid::Create(CWnd *pParentWnd,CRect rect, int nSCtype,int whichunit)
{
	m_Rect = rect;
	m_pView = pParentWnd;
	DrawFlag = DRAW_ALL;

	pSwitchBar = new CNewZfjsTab(m_pView->m_hWnd);
	pSwitchBar->SetStyle(SwitchStyle_switchbutton);
	pSwitchBar->SetFontColorSchema(VipColor.BACKCOLOR,VipColor.BACKCOLOR,VipColor.BACKCOLOR,VipColor.BACKCOLOR,VipColor.AXISCOLOR,VipColor.LEVELCOLOR,VipColor.KNCOLOR,VipColor.LEVELCOLOR,RGB(128,0,0),18,KAI_FONT);
	if(!pSwitchBar || !pSwitchBar->Create(_T("NewZfjsTab"),"",WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),m_pView,ID_GRIDSWITCHBAR))
		return FALSE;
	pSwitchBar->SetMenuCallBack((PMENUCALLBACK)SwitchMenuCallBack);
	InsertSwitch(nSCtype);
	return TRUE;
}

Grid::~Grid()
{
	if(pSwitchBar)	delete pSwitchBar;
}

void Grid::GetDomain(int &nGPType, int &nBlockFlag)
{
	nGPType = m_WhichGPType;
	nBlockFlag = m_nBlockFlag;
}

BOOL Grid::IsFixedGrid()
{
	return m_bFixedGrid;
}

void Grid::HideSwitchBar()
{
	if(pSwitchBar) pSwitchBar->SetWindowPos(0,0,0,0,0,SWP_HIDEWINDOW);
}

void Grid::GetGridTitle(CString &str)
{
	str = m_strTitle;
}

void Grid::SetPartMode(BOOL bPart)
{
	m_bPartMode = bPart;
}

void Grid::ReCalcSwitchNeedWidth()
{
	if(!pSwitchBar) return;
	m_SwitchWidth = pSwitchBar->GetNeedWidth();
	m_SwitchWidth = min(m_SwitchWidth,960);			
}

//设置区域并调整滚动条
void Grid::SetRect(CRect rect, BOOL bReresh)
{
	if(!bReresh) 
	{
		CopyRect(rect);
		m_Rect = rect;
	}
	SetControlPos(NULL,m_Rect);
	pSwitchBar->SetCurLabel(-1); //设置当前的Switch
}

void Grid::GetClientRect(CRect &rect)
{
	rect=m_Rect;
}

void Grid::SetControlPos(CWnd *pWnd, CRect &rect)
{
	if(!pSwitchBar) return;
	ReCalcSwitchNeedWidth();
	if(!m_bHasSwitch)
		pSwitchBar->SetWindowPos(pWnd,0,0,0,0,SWP_SHOWWINDOW);
	else
	{
		/*上侧
		if(m_bFixedGrid)
			pSwitchBar->SetWindowPos(pWnd,rect.left,rect.top,rect.Width()-1,SWITCH_HEIGHT,SWP_SHOWWINDOW);
		else
			pSwitchBar->SetWindowPos(pWnd,rect.left,rect.top,rect.Width()-1,SWITCH_HEIGHT,SWP_SHOWWINDOW);
		*/
		//下
		pSwitchBar->SetWindowPos(pWnd,rect.left,rect.bottom-SWITCH_HEIGHT,rect.Width()-1,SWITCH_HEIGHT,SWP_SHOWWINDOW);
	}
	CRect rc; pSwitchBar->GetWindowRect(&rc);
	int Width = rc.Width();
}

//得到画图区的区域大小
void Grid::GetDrawRect(CRect &rect, short nSwitchFlag)
{
	rect = m_Rect;
	if(m_bHasSwitch)
		rect.bottom -= SWITCH_HEIGHT;

	if(!m_bPartMode)
		rect.right -= 16;
	rect.bottom = max(0,rect.bottom);
	rect.right =  max(0,rect.right); //不要变成负数
}

//重新组织滚动条
void Grid::RefreshScrollBar()
{
	//对切换按钮的处理
	if(pSwitchBar&&FALSE)
	{
		if(g_bTranparentTab)
			pSwitchBar->SetFontColorSchema(VipColor.BACKCOLOR,VipColor.BACKCOLOR,VipColor.BACKCOLOR,VipColor.BACKCOLOR,VipColor.AXISCOLOR,VipColor.TXTCOLOR,VipColor.KPCOLOR,VipColor.TXTCOLOR,VipColor.AXISCOLOR,18,"Arial");
		else
		{
			if(g_d.IsBlackColorScheme(VipColor.BACKCOLOR))
				pSwitchBar->SetFontColorSchema(VipColor.BACKCOLOR,RGB(128,0,0),RGB(150,150,150),RGB(45,45,45),VipColor.BACKCOLOR,VipColor.BACKCOLOR,RGB(128,255,255),VipColor.BACKCOLOR,RGB(110,0,0),18,"Arial");
			else
				pSwitchBar->SetFontColorSchema(VipColor.BACKCOLOR,VipColor.BACKCOLOR,RGB(210,210,210),VipColor.BACKCOLOR,VipColor.AXISCOLOR,VipColor.TXTCOLOR,VipColor.KPCOLOR,VipColor.TXTCOLOR,VipColor.AXISCOLOR,18,"Arial");
		}
	}
	SetControlPos(NULL,m_Rect);
}

void  Grid::SetSwitchBarSel(int whichFl,int subFl)
{
	if(!pSwitchBar) return;
	int i,j;
	int which = -1;	
	if(m_bFixedGrid)	
	{

	}
	else if(g_bHasDS&&whichFl>=DS_START&&whichFl<FY_START)
	{
		for(i=0;i<g_GuiMarketNum;i++)
		{
			for(j=0;j<MAX_MKT_NUM&&g_GuiMarketSet[i].MarketSet[j]!=g_pDSMarket[whichFl-DS_START].MktType;j++);
			if(j<MAX_MKT_NUM) 
			{
				BOOL bLCShift=FALSE;
				for(j=0;j<g_CustomGridCat.size();j++)
				{
					int Tmp_lData=g_CustomGridCat[j].lData;
					if(g_DS.HasHKMarket) Tmp_lData--;
					if(g_CustomGridCat[j].wParam==ID_RIGHT_TYPE20 && (g_pLcIdxMng==NULL || g_pLcIdxMng->GetLcNum()==0))
						bLCShift=TRUE;					
					if(g_CustomGridCat[j].FlagType==DSOPT_ITEM && Tmp_lData==i)
					{
						which = j-(bLCShift?1:0);
						break;
					}
				}
			}
			if(which<0)
			{
				for(j=0;j<g_CustomGridCat.size();j++)
				{
					if(g_CustomGridCat[j].FlagType==DSOPT_ITEM && DSGetMarketXH(g_CustomGridCat[j].wParam-IDM_DS1)==(whichFl-DS_START))
					{
						which = j;
						break;
					}
				}
			}
			if(which>=0) break;
		}
		int nNum=0;
		if(pSwitchBar->m_bhasFLBtn) nNum++;
		if(pSwitchBar->m_bhasDPBtn) nNum++;
		pSwitchBar->SetCurLabel(which+nNum);
	}
	else if(whichFl>=FY_START)
	{
		int nNum=0;
		if(pSwitchBar->m_bhasFLBtn) nNum++;
		if(pSwitchBar->m_bhasDPBtn) nNum++;
		pSwitchBar->SetCurLabel(subFl+nNum);
	}
	else if(whichFl>=0)
	{
		BOOL bLCShift=FALSE;
		for(j=0;j<g_CustomGridCat.size();j++)
		{
			if(g_CustomGridCat[j].wParam==ID_RIGHT_TYPE20 && (g_pLcIdxMng==NULL || g_pLcIdxMng->GetLcNum()==0))
				bLCShift=TRUE;
			if(g_CustomGridCat[j].FlagType==ABDEF_ITEM && g_CustomGridCat[j].lData==whichFl)
			{
				which = j-(bLCShift?1:0);
				break;
			}
		}
		int nNum=0;
		if(pSwitchBar->m_bhasFLBtn) nNum++;
		if(pSwitchBar->m_bhasDPBtn) nNum++;
		if(whichFl==16&&subFl==-1)
			pSwitchBar->SetCurLabel(g_CustomGridCat.size()-1);
		else
			pSwitchBar->SetCurLabel(which+nNum);
	}
}

short Grid::GetCurrentGPIndex()
{
	if(nTotalRow<1)	return(-1);	//如果没有行情，则返回-1;
	if(lpnUsedStkIndex[nCurRow]<0) return -1;
	return(lpnUsedStkIndex[nCurRow]);
}

short Grid::GetCurrentGPSet(short *pPageGPIndex)
{
	if(nTotalRow<1)	return(0);	//如果没有行情，则返回-1;
	memcpy(pPageGPIndex, lpnUsedStkIndex, nTotalRow*sizeof(short));
	return nTotalRow;
}

void Grid::ProcessAutoPage()
{
	CClientDC dc(m_pView);
	if(nRowNum == nTotalRow) return;
	if(nCurRow == nTotalRow - 1) //如果到末尾，则回来第一个
		OnCtrlHome(&dc);
	else
	{
		OnPageDown(&dc,FALSE);
		if(m_bPartMode)
			OnPageDown(&dc);
	}
}

BOOL Grid::ProcessGPCode(const char *code,short setcode)
{
	int index = g_pStockIO->GetIndex(code,setcode);
	BOOL bFind = FALSE;
	for(int i = 0;i < nTotalRow;i++)
	{
		if(index == lpnUsedStkIndex[i])
		{
			bFind = TRUE;
			break;
		}
	}
	if(bFind) 
	{
		if(i > nStartRow && i <= nEndRow)
			nCurRow = i;
		else
			nCurRow = nStartRow = i;

		DrawFlag = DRAW_ALL;
		ProcessMsg(WM_PAINT);

		return TRUE;
	}
	return FALSE;
}

//********************************************************//
//                主画图函数辅助函数                      //
//********************************************************//
//画高亮度指示线
void Grid::DrawHighLine(CDC *pDC)
{
	if(nTotalRow < 1) return;
	int tempMode = pDC->SetROP2 (R2_XORPEN);
	int RightPos,Width=0,Top;
	if(nTotalCol==nEndCol+1)
		RightPos = GetCellLeft(nEndCol+m_nFixedLeadCol,Width);
	else
		RightPos = m_DRect.Width();
	RightPos = min(m_DRect.Width(),RightPos);
	Top = (nCurRow-nStartRow+1)*nCellHeight + nTitleHeight-1;
	if(Top < m_DRect.Height())
	{
		if(g_bLongHighLine)
			g_d.DrawLine(pDC,m_DRect.left+nCellHeadWidth+1,m_DRect.top+Top,m_DRect.left+RightPos-1+Width,m_DRect.top+Top,GridColor.GridHighLineColor^VipColor.BACKCOLOR,PS_SOLID,1);//右边减1,Width=3
		else
		{
			CRect rc;
			rc.SetRect(m_DRect.left+1,m_DRect.top+Top-nCellHeight+2,m_DRect.left+RightPos-1+Width,m_DRect.top+Top-1);

			CRect   XRc(0,0,rc.Width(),rc.Height());
			CREATE_MEMDC(XRc)
			MemDC.FillSolidRect(&XRc,GridColor.GridBackColor^VipColor.FLOATCURSORCOLOR);
			pDC->BitBlt(m_DRect.left+1,m_DRect.top+Top-nCellHeight+2,
				XRc.Width(),XRc.Height(),&MemDC,0,0,SRCINVERT);
			DELETE_MEMDC
		}
	}
	pDC->SetROP2 (tempMode); 
}

//row为-1表示在标题栏中写 PosFlag表示在方格中写的位置，0:居中,1:偏上,2:偏下
void Grid::DrawCellText(CDC *pDC,int row,int col,int PosFlag,char *lpszStr,COLORREF color,BOOL LeftFlag,BOOL SameWidth,MemStkInfo *pStock)
{
	int width = 0,tempPos;
 	int x = GetCellLeft(col,width);
	//某些列不画可如此处理
	if(width<1) return;
	int nLeftPos  = x;
	CSize zs = CSize(0, 0);
	if(row == -1)	//用在某些特殊情况,如强弱分析
	{
		nLeftPos = x+2;
		tempPos = nSigTitleHeight + 2;
		if(SameWidth)
			zs = g_d.DisplaySameWidtchText(pDC,nLeftPos,nSigTitleHeight + 2,color,lpszStr);
		else
			zs = g_d.DisplayText(pDC,nLeftPos,nSigTitleHeight + 2,color,width-3,lpszStr);
		return;
	}
	if(PosFlag == 0) 
		tempPos = nTitleHeight + row*nCellHeight + (nCellHeight-FontSize.cy)/2;
	else if(PosFlag == 1) 
		tempPos = nTitleHeight + row*nCellHeight + (nCellHeight-2*FontSize.cy)/2;
	else 
		tempPos = nTitleHeight + row*nCellHeight + nCellHeight/2-4;
	if(x+width < m_DRect.Width()+3) //对于超出来的部分,不画
	{
		nLeftPos = LeftFlag?x+5:x;
		if(SameWidth)
			zs = g_d.DisplaySameWidtchText(pDC,nLeftPos,tempPos,color,lpszStr);
		else
			zs = g_d.DisplayText(pDC,nLeftPos,tempPos,color,LeftFlag?0:width-5,lpszStr);
	}
	DrawMarkFlag(pStock,pDC,nLeftPos+zs.cx, tempPos-2,left,top,m_pView);
}

void Grid::DrawCellText(CDC *pDC,int row,int col,int PosFlag,COLORREF color,char *fmt,...)
{
    va_list arg_ptr;
    char tmp[80];
    va_start(arg_ptr,fmt);
    vsprintf(tmp,fmt,arg_ptr);
    va_end(arg_ptr);
	AllTrim(tmp);
	DrawCellText( pDC, row, col, PosFlag, tmp, color);
}
	
void Grid::DrawCellText(CDC *pDC,int row,int col,int PosFlag,float f1,int xsflag,COLORREF color,int postfix_flag)
{
	char buf[80];			//增为80，防止异常数据导致出错
	switch (xsflag) 
	{
	case	NOXS://没有小数
		sprintf(buf,"%20.f",f1);
		break;
	case	1:
		sprintf(buf,"%20.1f",f1);
		break;
	case	XS2:
		sprintf(buf,"%20.2f",f1);
		break;
	case	XS3:
		sprintf(buf,"%20.3f",f1);
		break;
	case	XS4:
		sprintf(buf,"%20.4f",f1);
		break;
	case	XS5:
		sprintf(buf,"%20.5f",f1);
		break;
	default:
		sprintf(buf,"%20.2f",f1);
		break;
	}
	buf[20] = 0;
	Format_PostFix(postfix_flag,(f1>COMPPREC),buf,sizeof(buf));
	DrawCellText( pDC, row, col, PosFlag, buf, color);
}

void Grid::DrawCellText(CDC *pDC,int row,int col,int PosFlag,float f1,float f2,int xsflag,int postfix_flag)
{
	 COLORREF tempcolor=GetItemColor(f1,f2);
	 DrawCellText(pDC,row,col,PosFlag,f1,xsflag,tempcolor,postfix_flag);
}

void Grid::AdjustRowColNum()
{
	tempRowFlag = tempColFlag = FALSE;	
	//计算行数
	nRowNum = (m_DRect.Height()-nTitleHeight)/nCellHeight;
	int tmpMod = (m_DRect.Height()-nTitleHeight)%nCellHeight;
	if(tmpMod < nCellHeight-2)
		tempRowFlag = TRUE;
	if(nStartRow + nRowNum >= nTotalRow)
	{
		nRowNum = nTotalRow-nStartRow;
		tempRowFlag = TRUE;
	}
	nEndRow = nStartRow + nRowNum - 1;
	if(nEndRow < 0) nEndRow = -1;
	//计算列数
	if(nStartCol + nColNum >= nTotalCol)
	{
		nColNum = nTotalCol-nStartCol;
		tempColFlag = TRUE;
	}
	nEndCol = nStartCol + nColNum-1;
	if(nEndCol < 0) nEndCol = 0;
	//
	tempRowNum = nRowNum+((tempRowFlag==TRUE)?0:1);
	tempColNum = nColNum+((tempColFlag==TRUE)?0:1);
}

//*******************************//
//		按键事件的处理			 //
//*******************************//
void Grid::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if((nChar==VK_UP)||(nChar==VK_DOWN)||(nChar==VK_NEXT)||(nChar==VK_PRIOR)||
		(nChar==VK_LEFT)||(nChar==VK_RIGHT)||(nChar==VK_HOME)||(nChar==VK_END))
	{
		CClientDC dc(m_pView);
		SHORT CtrlDown;
		switch(nChar)
		{
		case VK_UP:
			OnUp(&dc);
			break;
	    case VK_DOWN:
			OnDown(&dc);
			break;
		case VK_NEXT:
			OnPageDown(&dc,FALSE);	
			break;
		case VK_PRIOR:
			OnPageUp(&dc,FALSE);
			break;
		case VK_LEFT:
			CtrlDown=::GetKeyState(VK_CONTROL);
			if(CtrlDown<0)
				OnCtrlLeft();
			else
				OnLeft ();
			break;
		case VK_RIGHT:
			CtrlDown=::GetKeyState(VK_CONTROL);
			if(CtrlDown<0)
				OnCtrlRight();
			else
				OnRight ();
			break;
		case VK_HOME:
			CtrlDown=::GetKeyState(VK_CONTROL);
			if(CtrlDown<0)
				OnCtrlHome (&dc);
			else
				OnHome();
			break;
		case VK_END:
			CtrlDown=::GetKeyState(VK_CONTROL);
			if(CtrlDown<0)
				OnCtrlEnd (&dc);
			else
				OnEnd();
			break;
		default:
			break;
		}
	}
}

void Grid::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(!m_bPartMode) return;

	if((nChar==VK_NEXT)||(nChar==VK_PRIOR))
	{
		CClientDC dc(m_pView);
		switch(nChar)
		{
		case VK_NEXT:
			OnPageDown(&dc,TRUE);
			break;
	    case VK_PRIOR:
			OnPageUp(&dc,TRUE);
			break;
		}
	}
}

//按Up时
void Grid::OnUp(CDC *pDC)
{
	//如果nCurRow超出范围，则及时纠正
	if(nCurRow > nEndRow)
	{
		DrawHighLine(pDC);
		nCurRow = nEndRow;
		DrawHighLine(pDC);
	}
	else
	{
		if(nCurRow!=0)
		{
			if(nCurRow==nStartRow)
			{
				if(!m_bPartMode)
				{
					nCurRow--;
					nStartRow--;
					DrawFlag = DRAW_ALL;
					ProcessMsg(WM_PAINT);
				}
				else
				{
					DrawHighLine(pDC);
					nCurRow = nEndRow;
					DrawHighLine(pDC);
				}
			}
			else
			{
				DrawHighLine(pDC);
				nCurRow--;
				DrawHighLine(pDC);
			}
		}
		else if(m_bPartMode)
		{
			DrawHighLine(pDC);
			nCurRow = nEndRow;
			DrawHighLine(pDC);
		}
	}
}

//按Down时
void Grid::OnDown(CDC *pDC)
{
	//如果nCurRow超出范围，则及时纠正
	if(nCurRow > nEndRow)
	{
		DrawHighLine(pDC);
		nCurRow = nEndRow;
		DrawHighLine(pDC);
	}
	else
	{
		if(nCurRow != nTotalRow-1)
		{
			if(nCurRow==nEndRow)
			{
				if(!m_bPartMode)
				{
					nCurRow++;
					nStartRow++;
					DrawFlag = DRAW_ALL;
					ProcessMsg(WM_PAINT);
				}
				else
				{
					DrawHighLine(pDC);
					nCurRow = nStartRow;
					DrawHighLine(pDC);
				}
			}
			else
			{
				DrawHighLine(pDC);
				nCurRow++;
				DrawHighLine(pDC);
			}
		}
	}
}

//按PageDown时
void Grid::OnPageDown(CDC *pDC,BOOL bGetData)
{
	if(m_bPartMode && bGetData)
	{
		DrawFlag = DRAW_ALL;
		ProcessMsg(WM_PAINT);
		return;
	}
	//如果nCurRow超出范围，则及时纠正
	if(nCurRow > nEndRow)
	{
		DrawHighLine(pDC);
		nCurRow = nEndRow;
		DrawHighLine(pDC);
	}
	else
	{
		if(nEndRow == nTotalRow - 1)
		{
			if(nRowNum == nTotalRow) 
			{
				DrawHighLine(pDC);
				nCurRow=nTotalRow-1;
				DrawHighLine(pDC);
			}
			else					//如果翻到最后一屏
			{
				nStartRow = 0;
				nCurRow = 0;
				if(m_bPartMode)
				{
					bDrawBlue = FALSE;
					DrawFlag = DRAW_FRAME;
				}
				else
					DrawFlag = DRAW_ALL;
				ProcessMsg(WM_PAINT);
			}
		}
		else if(nCurRow+nRowNum>nTotalRow-1)
		{
			nStartRow=nEndRow+1;
			nCurRow=nTotalRow-1;
			if(m_bPartMode)
			{
				bDrawBlue = FALSE;
				DrawFlag = DRAW_FRAME;
			}
			else
				DrawFlag = DRAW_ALL;
			ProcessMsg(WM_PAINT);
		}
		else
		{
			nStartRow=nEndRow+1;
			nCurRow+=nRowNum;
			if(m_bPartMode)
			{
				bDrawBlue = FALSE;
				DrawFlag = DRAW_FRAME;
			}
			else
				DrawFlag = DRAW_ALL;
			ProcessMsg(WM_PAINT);
		}
	}
}

//按PageUp时
void Grid::OnPageUp(CDC *pDC,BOOL bGetData)
{
	if(m_bPartMode && bGetData)
	{
		DrawFlag = DRAW_ALL;
		ProcessMsg(WM_PAINT);
		return;
	}
	//计算整个屏幕能显示多少行
	int tmpRowNumPre = (m_DRect.Height()-nTitleHeight)/nCellHeight;
	//如果nCurRow超出范围，则及时纠正
	if(nCurRow > nEndRow)
	{
		DrawHighLine(pDC);
		nCurRow = nEndRow;
		DrawHighLine(pDC);
	}
	else
	{
		if(nStartRow == 0)
		{
			if(nRowNum == nTotalRow) 
			{
				DrawHighLine(pDC);
				nCurRow = 0;
				DrawHighLine(pDC);
			}
			else					//如果翻到第一屏
			{
				nStartRow = nTotalRow-nRowNum;
				nCurRow = nStartRow;
				if(m_bPartMode)
				{
					bDrawBlue = FALSE;
					DrawFlag = DRAW_FRAME;
				}
				else
					DrawFlag = DRAW_ALL;
				ProcessMsg(WM_PAINT);
			}
		}
		else if( nStartRow - tmpRowNumPre <= 0 )
		{
			nStartRow = 0;
			nCurRow = 0;
			if(m_bPartMode)
			{
				bDrawBlue = FALSE;
				DrawFlag = DRAW_FRAME;
			}
			else
				DrawFlag = DRAW_ALL;
			ProcessMsg(WM_PAINT);
		}
		else
		{
			nStartRow -= tmpRowNumPre;
			nCurRow -= tmpRowNumPre;
			if(m_bPartMode)
			{
				bDrawBlue = FALSE;
				DrawFlag = DRAW_FRAME;
			}
			else
				DrawFlag = DRAW_ALL;
			ProcessMsg(WM_PAINT);
		}
	}
}

//按Ctrl + PageUp时
void Grid::OnCtrlHome(CDC *pDC)
{
	if(nStartRow == 0)
	{
		DrawHighLine(pDC);
		nCurRow = 0;
		DrawHighLine(pDC);
	}
	else
	{
		nStartRow = nCurRow = 0;
		if(m_bPartMode)
			bDrawBlue = FALSE;
		DrawFlag = DRAW_ALL;
		ProcessMsg(WM_PAINT);
	}
}

//按Ctrl + PageDown时
void Grid::OnCtrlEnd(CDC *pDC)
{
	if(nEndRow == nTotalRow - 1)
	{
		DrawHighLine(pDC);
		nCurRow = nTotalRow - 1;
		DrawHighLine(pDC);
	}
	else
	{
		nStartRow = nCurRow = nTotalRow-1;
		if(m_bPartMode)
			bDrawBlue = FALSE;
		DrawFlag = DRAW_ALL;
		ProcessMsg(WM_PAINT);
	}
}

//按Left时
void Grid::OnLeft()
{
	if(nStartCol!=0)
	{
		nStartCol--;
		nEndCol--;
		DrawFlag = DRAW_NOREADDATA;
		ProcessMsg(WM_PAINT);
	}
}

//按Right时
void Grid::OnRight()
{
	if(nStartCol!= nTotalCol-1)	//需改
	{
		nStartCol++;
		nEndCol++;
		DrawFlag = DRAW_NOREADDATA;
		ProcessMsg(WM_PAINT);
	}
}

//按Home时
void Grid::OnHome()
{
	if(nStartCol!=0)
	{
		nStartCol=0;
		DrawFlag = DRAW_NOREADDATA;
		ProcessMsg(WM_PAINT);
	}
}

//按End时
void Grid::OnCtrlRight()
{
	if(nEndCol >= nTotalCol - 1) 
		nStartCol = nTotalCol-1;
	else
		nStartCol = nEndCol  + 1; //注意nStartCol从0开始的，而不是从2开始的
	DrawFlag = DRAW_NOREADDATA;
	ProcessMsg(WM_PAINT);
}

void Grid::OnEnd()
{
	if(nStartCol!=nTotalCol-1)	//需改
	{
		nStartCol=nEndCol=nTotalCol-1;
		DrawFlag = DRAW_NOREADDATA;
		ProcessMsg(WM_PAINT);
	}
}	

static long s_scrolltick=GetTickCount();
void Grid::OnHScrollMessage(WPARAM wParam,LPARAM lParam)
{
	if(GetTickCount() - s_scrolltick < 30) return;
	s_scrolltick=GetTickCount();

	int nPos = (int)wParam;
	if(nPos != nStartCol)
	{
		nStartCol = nPos;
		DrawFlag = DRAW_NOREADDATA;
		ProcessMsg(WM_PAINT);
	}
}

void Grid::OnVScrollMessage(WPARAM wParam,LPARAM lParam)
{
	if(GetTickCount() - s_scrolltick < 30) return;
	s_scrolltick=GetTickCount();
	
	int nPos = (int)wParam;
	if(nPos != nStartRow)
	{
		int tempint = nCurRow-nStartRow;
		nStartRow=nPos;
		if(nStartRow+tempint<nTotalRow-1)
			nCurRow=nStartRow+tempint;
		else
			nCurRow=nTotalRow-1;
		DrawFlag = DRAW_ALL;
		ProcessMsg(WM_PAINT);
	}	
}

void Grid::GetCornerRect(CRect &rc)
{
	rc = m_DRect;
	rc.top += (nTitleHeight0>0?nTitleHeight0:nTitleHeight)-nSigTitleHeight;
	rc.bottom = rc.top+nSigTitleHeight;
	rc.right = rc.left+nCellHeadWidth;
}

void Grid::SetDomain(int nBlockFlag,int WhichType)
{	
	if(WhichType>=DS_START&&WhichType<FY_START&&!g_bHasDS) 
	{
	}
	else if( (nBlockFlag!=-1||WhichType!=-1) && !m_bFixedGrid  || 
			 (nBlockFlag!=-1 && WhichType==-1 && m_bFixedGrid) ||//对于可以选择板块的FixedGrid型
			 (WhichType>=DS_START && m_bFixedGrid)	||				//对于可以选择的三方行情
		   WhichType==TYPE_DEFTJ_POOL)
	{
		m_WhichGPType = WhichType;
		m_nBlockFlag = nBlockFlag;
	}

	nTotalRow = 0;

	if(m_WhichGPType>=FY_START&&m_WhichGPType<FY_START+MAX_FY_GROUP)
	{
		m_WhichGPType = WhichType;
		m_nBlockFlag = nBlockFlag;
		for(int i=0;i<MAXGPNUM;++i) lpnUsedStkIndex[i] = -1;
	}
	else 
		nTotalRow=GetGpIndex(m_nBlockFlag,m_WhichGPType,lpnUsedStkIndex);

	if(m_bHasSwitch)
	{
		SetSwitchBarSel(WhichType,nBlockFlag);
		if(m_bFixedGrid && pSwitchBar && nBlockFlag>=0)
		{
			CString tmpCaption = GetBlockCaption(nBlockFlag);
			pSwitchBar->ModifySwitchLabel(0,_F(tmpCaption),"");
			pSwitchBar->Invalidate(FALSE);
		}
	}
}

CString Grid::GetBlockCaption(int nBlockFlag)
{
	CString tmpCaption="";
	if(nBlockFlag<DY_START)			tmpCaption = g_BlockInfo[nBlockFlag].Name;
	else if(nBlockFlag<HY_START)	tmpCaption = g_cwAddrTopic[nBlockFlag-DY_START];
	else if(nBlockFlag<CON_START)	tmpCaption = g_cwHYstr[nBlockFlag-HY_START];
	else if(nBlockFlag<ZHB_START)	tmpCaption = g_ConceptBlock[nBlockFlag-CON_START].strBlockName;
	else if(nBlockFlag<ZJHHY_START)	tmpCaption = g_ZHBBlock[nBlockFlag-ZHB_START].zhmc;
	else if(nBlockFlag<HKHY_START)	tmpCaption = g_pConstMng->GetHYFromCommandIDXH(m_nBlockFlag-ZJHHY_START);
	else if(nBlockFlag<HKHY_START+g_SectorCN.size())	
									tmpCaption = g_SectorCN[nBlockFlag-HKHY_START].SectorName;
	return tmpCaption;
}

void Grid::ToSomeRow(int row)
{
	nCurRow = nStartRow = row;
}

void Grid::ToSomeCol(int col)
{
	if(col<0) return;
	nStartCol = col;
}

int Grid::GetDragItem(CPoint pt)
{
	if(pt.y < m_DRect.top + nTitleHeight) return -1;

	int temp,tempRow;
	temp = (pt.y-m_DRect.top-nTitleHeight) / nCellHeight ;
	if(temp >= 0 && temp < nRowNum)
	{
		tempRow = temp + nStartRow;
		return lpnUsedStkIndex[tempRow];
	}
	return -1;
}

void Grid::WriteToTxtFile(CString filename,BOOL bWriteExcel)
{
	m_WriteStr = "";
	CFileException ex;
	m_pWriteFile = new CStdioFile;
	m_bWriteExcel = bWriteExcel;
	if(m_pWriteFile->Open(filename,CFile::modeCreate|CFile::modeWrite,&ex))
	{
		CDC *pDC = m_pView->GetDC();
		DrawFlag = DRAW_NOREADDATA;
		m_pView->SendMessage(WM_PAINT);
		m_pView->ReleaseDC(pDC);
		m_pWriteFile->Close();
		TDEL(m_pWriteFile);
	}
	else
	{
		TDEL(m_pWriteFile);
		char strError[1024];
		ex.GetErrorMessage(strError,1024);
		TDX_MessageBox(NULL,"文件创建错误!\n"+(CString)strError,MB_ICONEXCLAMATION|MB_OK);
	}
}


void GetInitDomain(int &WhichGPType,int &BlockFlag,int FirstType,int FirstBlock)
{
	//给市场类型变量赋值
	WhichGPType = FirstType;
 	if(WhichGPType == -1)
	{
		BlockFlag = FirstBlock;
		if(BlockFlag<0)					
			BlockFlag = 0;
		else if(BlockFlag < DY_START)
		{
			if(BlockFlag>=g_nBlockNum)	BlockFlag = g_nBlockNum-1;
			if(g_nBlockNum < 1) 
			{
				WhichGPType = TYPE_AG;
				BlockFlag = -1;
			}
		}
		else if(BlockFlag < HY_START)
		{
			if(BlockFlag>=TOTAL_DY_NUM+DY_START)	BlockFlag = DY_START+TOTAL_DY_NUM-1;
		}
		else if(BlockFlag < CON_START)
		{
			if(BlockFlag>=TOTAL_HY_NUM+HY_START)	BlockFlag = HY_START+TOTAL_HY_NUM-1;
		}
		else if(BlockFlag < ZHB_START)
		{
			if(BlockFlag>=g_ConceptBlockNum+CON_START)	BlockFlag = CON_START+g_ConceptBlockNum-1;
			if(g_ConceptBlockNum < 1)
			{
				WhichGPType = TYPE_AG;
				BlockFlag = -1;
			}
		}
		else if(BlockFlag < ZJHHY_START)
		{
			if(BlockFlag>=g_ZHBBlockNum+ZHB_START)		BlockFlag = ZHB_START+g_ZHBBlockNum-1;
			if(g_ZHBBlockNum < 1) 
			{
				WhichGPType = TYPE_AG;
				BlockFlag = -1;
			}
		}
		else if(BlockFlag < HKHY_START)
		{
			int nTotalZJHHY = g_pConstMng->GetZJHHYBlockNum();
			if(BlockFlag>=nTotalZJHHY+ZJHHY_START)		BlockFlag = ZJHHY_START+nTotalZJHHY-1;
			if(nTotalZJHHY < 1) 
			{
				WhichGPType = TYPE_AG;
				BlockFlag = -1;
			}
		}
	}
	else	
		BlockFlag = -1;
	if(g_GC.bOnlyHK)
	{
		WhichGPType = TYPE_ZXG;
		BlockFlag = -1;
	}
}
