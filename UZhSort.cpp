#include "stdafx.h"
#include "tdxw.h"
#include "comview2.h"
#include "UZhSort.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define	MAX_ZHROW_ITEM	20

#define	STYLE_2X2		0
#define	STYLE_3X3		1
#define STYLE_1X5		2

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern void MakeZHSortTitle(char (*TypeStr)[25]);

UZhSort::UZhSort(LPCOMINFO pComInfo):UBase(pComInfo)
{
	m_nDSFlag = FALSE;
	WhichType = TYPE_AG;
	m_nCurSel = -1;
	m_bHasData = TRUE;
	m_nMainID = 0;
	m_bRefreshDraw = FALSE;
	memset(TypeStr,0,sizeof(char)*9*25);
	m_bHasFL = FALSE;

	for(int i=0;i<9;i++)
	{
		m_aShowSort[i]=i;
		TypeFlag[i] = 0;
	}

	m_nStyle = g_nZHStyle;

	CalcSomeInfo(m_nStyle);
	
	m_bDSFlag = (WhichType>=DS_START);	
	//得到股票集合
	nGpNum = GetGpIndex(-1,WhichType,lpnUsedStkIndex,MAXGPNUM);
	if(nGpNum > 0) m_nCurSel = 0;
	MakeZHSortTitle(TypeStr);

	memset(SortResult,-1,9*20*sizeof(short));//SortResult为short型
	memset(m_ZHBuf,0,sizeof(ZHRESULT)*9);

	nOptBW		= 0;
	m_nItems	= 20;
}

UZhSort::~UZhSort()
{
}

//////////////////////////////////////////////////////////////////////////
void	UZhSort::SetStyle(int nStyle)
{
	for(int i=0;i<9;i++) TypeFlag[i] = 0;
	m_nStyle = nStyle;
	CalcSomeInfo(nStyle);
	ProcessMsg(UM_ZH_READSORT);
}

void	UZhSort::CalcSomeInfo(int nFlag)
{
	switch(nFlag) 
	{
	case STYLE_2X2:
		{
			for(int i=0;i<4;i++)
				TypeFlag[i] = 1;
			m_nUnitCol = m_nUnitRow = 2;
			m_nRowNum = 6;
			m_nItemHeight = 20;
			m_nCurSel = 0;
		}
		break;
	case STYLE_3X3:	
		{
			for(int i=0;i<9;i++)
				TypeFlag[i] = 1;
			m_nUnitCol = m_nUnitRow = 3;
			m_nRowNum = 6;
			m_nItemHeight = 20;
			m_nCurSel = 0;
		}
		break;
	case STYLE_1X5:	
		{
			TypeFlag[0] = 1;
			TypeFlag[1] = 1;
			TypeFlag[3] = 1;
			TypeFlag[5] = 1;

			m_nUnitCol = 4;
			m_nUnitRow = 1;
			m_nRowNum = MAX_ZHROW_ITEM;
			m_nItemHeight = 20;
			m_nCurSel = 0;
		}
		break;
	default:
		break;
	}
}

#define ZH_ZANGF	0
#define ZH_ZENGF	1
#define ZH_FZZANG	2
#define ZH_LIANB	3
#define ZH_WT		4
#define ZH_JE		5

void UZhSort::DrawOneCell(CDC *pDC,int xh,int type)
{
	int i;
	CRect rc = CellRect[xh];
	//先写标题
	g_d.DisplayText(pDC,rc.left+50,rc.top+2,VipColor.BIGTITLECOLOR,0,_F(TypeStr[type]));

	if((type==3||type==4) && !OnLine) 
		return;
	
	if(m_nStyle==STYLE_1X5)
		rc.top+=4;

	MemStkInfo *tempStkInfo;
	int xsflag;
	float ftmp;
	CSize size;
	if(!OnLine)
	{
		CurrStockData CurHQ;
		for(i = 0;i < m_nRowNum;i++)
		{
			if(SortResult[type][i] == -1) break;
			tempStkInfo = (*g_pStockIO)[SortResult[type][i]];
			if(!tempStkInfo) break;
			g_pStockIO->ReadCurrStockData(tempStkInfo,&CurHQ);
			xsflag = GetXSFlag(tempStkInfo);
			size = g_d.DisplaySameWidtchText(pDC,rc.left+3,rc.top+22+m_nItemHeight*i,GetBlockGPColor(tempStkInfo,GridColor.Grid_CodeName_Color),tempStkInfo->Name);
			CSize zs=DrawMarkFlag(tempStkInfo,pDC,rc.left+3+size.cx, rc.top+22+m_nItemHeight*i-1,0,0,m_pView);
			if(tempStkInfo->fl_flag>0)
				size.cx += zs.cx;
			if(m_nCurSel == xh*MAX_ZHROW_ITEM+i)
				g_d.DrawRect(pDC,rc.left+2,rc.top+21+m_nItemHeight*i,rc.left+2+size.cx+4,rc.top+21+m_nItemHeight*i+size.cy+4);
			if(CurHQ.Now<COMPPREC) continue;
			g_d.BjPrintf(pDC,rc.left+rc.Width()/3+5,rc.top+22+m_nItemHeight*i,CurHQ.Now,CurHQ.Close,xsflag,rc.Width()/3+5);
			switch(type)
			{
			case 0:	//涨幅排名
			case 1: //跌幅排名
				ftmp = fGetValue(ZH_ZANGF,SortResult[type][i]);
				if(ftmp != MEANLESS_DATA)
					g_d.BjPrintf(pDC,rc.left+2*rc.Width()/3-5,rc.top+22+m_nItemHeight*i,ftmp,0.0,2,rc.Width()/3+3);
				break;
			case 2:	//振幅排名
				ftmp = fGetValue(ZH_ZENGF,SortResult[type][i]);
				if(ftmp != MEANLESS_DATA)
					g_d.DisplayText(pDC,rc.left+2*rc.Width()/3-5,rc.top+22+m_nItemHeight*i,VipColor.TXTCOLOR,rc.Width()/3+3,"%9.2f",ftmp);
				break;
			case 3: //分钟涨幅排名
			case 4: //分钟跌幅排名
				ftmp = fGetValue(ZH_FZZANG,SortResult[type][i]);
				if(ftmp != MEANLESS_DATA)
					g_d.BjPrintf(pDC,rc.left+2*rc.Width()/3-5,rc.top+22+m_nItemHeight*i,ftmp,0.0,2,rc.Width()/3+3);
				break;
			case 5: //量比排名
				ftmp = fGetValue(ZH_LIANB,SortResult[type][i]);
				if(ftmp != MEANLESS_DATA)
					g_d.DisplayText(pDC,rc.left+2*rc.Width()/3-5,rc.top+22+m_nItemHeight*i,VipColor.TXTCOLOR,rc.Width()/3+3,"%9.2f",ftmp);
				break;
			case 6: //委托前5名
			case 7: //委托后5名
				ftmp = fGetValue(ZH_WT,SortResult[type][i]);
				if(ftmp != MEANLESS_DATA)
					g_d.BjPrintf(pDC,rc.left+2*rc.Width()/3-5,rc.top+22+m_nItemHeight*i,ftmp,0.0,2,rc.Width()/3+3);
				break;
			case 8: //最大金额
				ftmp = fGetValue(ZH_JE,SortResult[type][i]);
				if(ftmp != MEANLESS_DATA)
					g_d.DisplayText(pDC,rc.left+2*rc.Width()/3-5,rc.top+22+m_nItemHeight*i,VipColor.TXTCOLOR,rc.Width()/3+3,MakeJE(ftmp));
				break;
			}
		}
	}
	else
	{
		float fClose;
		for(i = 0;i < m_nRowNum;i++)
		{
			if(i >= m_ZHBuf[type].Num) break;
			tempStkInfo = g_pStockIO->Get_MemStkInfo(m_ZHBuf[type].Code[i],m_ZHBuf[type].SetCode[i]);
			if(!tempStkInfo) break;
			fClose = g_pStockIO->GetStockClose(tempStkInfo);
			xsflag = GetXSFlag(tempStkInfo);
			size = g_d.DisplaySameWidtchText(pDC,rc.left+3,rc.top+22+m_nItemHeight*i,GetBlockGPColor(tempStkInfo,GridColor.Grid_CodeName_Color),tempStkInfo->Name);
			CSize zs=DrawMarkFlag(tempStkInfo,pDC,rc.left+3+size.cx, rc.top+22+m_nItemHeight*i-1,0,0,m_pView);
			if(tempStkInfo->fl_flag>0)
				size.cx += zs.cx;
			if(m_nCurSel == xh*MAX_ZHROW_ITEM+i)
				g_d.DrawRect(pDC,rc.left+2,rc.top+21+m_nItemHeight*i,rc.left+2+size.cx+4,rc.top+21+m_nItemHeight*i+size.cy+4);
			if(m_ZHBuf[type].Now[i]==MEANLESS_DATA || m_ZHBuf[type].Now[i]<COMPPREC) continue;
			g_d.BjPrintf(pDC,rc.left+rc.Width()/3+5,rc.top+22+m_nItemHeight*i,m_ZHBuf[type].Now[i],fClose,xsflag,rc.Width()/3+5);
			switch(type)
			{
			case 0:	//涨幅排名
			case 1: //跌幅排名
				ftmp = m_ZHBuf[type].Other[i];
				if(ftmp != MEANLESS_DATA)
					g_d.BjPrintf(pDC,rc.left+2*rc.Width()/3-5,rc.top+22+m_nItemHeight*i,ftmp,0.0,2,rc.Width()/3+3);
				break;
			case 2:	//振幅排名
				ftmp = m_ZHBuf[type].Other[i];
				if(ftmp != MEANLESS_DATA)
					g_d.DisplayText(pDC,rc.left+2*rc.Width()/3-5,rc.top+22+m_nItemHeight*i,VipColor.TXTCOLOR,rc.Width()/3+3,"%9.2f",ftmp);
				break;
			case 3: //分钟涨幅排名
			case 4: //分钟跌幅排名
				ftmp = m_ZHBuf[type].Other[i];
				if(ftmp != MEANLESS_DATA)
					g_d.BjPrintf(pDC,rc.left+2*rc.Width()/3-5,rc.top+22+m_nItemHeight*i,ftmp,0.0,2,rc.Width()/3+3);
				break;
			case 5: //量比排名
				ftmp = m_ZHBuf[type].Other[i];
				if(ftmp != MEANLESS_DATA)
					g_d.DisplayText(pDC,rc.left+2*rc.Width()/3-5,rc.top+22+m_nItemHeight*i,VipColor.TXTCOLOR,rc.Width()/3+3,"%9.2f",ftmp);
				break;
			case 6: //委托前5名
			case 7: //委托后5名
				ftmp = m_ZHBuf[type].Other[i];
				if(ftmp != MEANLESS_DATA)
					g_d.BjPrintf(pDC,rc.left+2*rc.Width()/3-5,rc.top+22+m_nItemHeight*i,ftmp,0.0,2,rc.Width()/3+3);
				break;
			case 8: //最大金额
				ftmp = m_ZHBuf[type].Other[i];
				if(ftmp != MEANLESS_DATA)
					g_d.DisplayText(pDC,rc.left+2*rc.Width()/3-5,rc.top+22+m_nItemHeight*i,VipColor.TXTCOLOR,rc.Width()/3+3,MakeJE(ftmp));
				break;
			}
		}
	}
}

void	UZhSort::SetShowSort(int **aShowSort)
{
	*aShowSort = m_aShowSort;
}

void	UZhSort::GetShowSort(int *aShowSort)
{
	memcpy(m_aShowSort, aShowSort, sizeof(m_aShowSort));
}

float UZhSort::fGetValue(int Type,short GpIndex)
{
	float	ftmp;
	int		ii;
	long	wl,wlc;
	struct CurrStockData	CurHQ;

	MemStkInfo *tempInfo = (*g_pStockIO)[GpIndex];
	if(!tempInfo) return MEANLESS_DATA;
	g_pStockIO->ReadCurrStockData (tempInfo,&CurHQ);

	switch(Type)
	{
	case ZH_ZANGF:
		if (CurHQ.Close>COMPPREC)
		{	
			if ( CurHQ.Now < COMPPREC )
				ftmp = 0.0;
			else
				ftmp = CurHQ.Now-CurHQ.Close;
			return ftmp/CurHQ.Close*100.0;
		}
		return MEANLESS_DATA;
	case ZH_ZENGF:
		if (CurHQ.Close > 0)
		{
			if ( CurHQ.Max > COMPPREC && CurHQ.Min > COMPPREC )
				ftmp = CurHQ.Max - CurHQ.Min;
			else
				ftmp = 0.0;
			return	ftmp/CurHQ.Close*100.0;
		}
		return MEANLESS_DATA;
	case ZH_FZZANG:
		return MEANLESS_DATA;
	case ZH_LIANB:
		if( !testzs(tempInfo) )
		{
			short fz[8];
			InitStkTime(tempInfo->setcode,fz);
			ii = GetTdxMinute(fz);
			if (ii > 0 && tempInfo->VolBase > 0l)
				return (float)CurHQ.Volume/ii/tempInfo->VolBase;
		}
		return MEANLESS_DATA;		
	case ZH_WT:
		if( !testzs(tempInfo) )
		{
			wlc = wl = 0;
			for (ii = 0;ii < 3;ii++)
			{   
				wlc = wlc+(long)CurHQ.Buyv[ii]-(long)CurHQ.Sellv[ii];
				wl = wl +(long)CurHQ.Buyv[ii]+(long)CurHQ.Sellv[ii];
			}
			if (wl != 0l)
				return (float)100.0*wlc/wl;
		}
		return MEANLESS_DATA;
	case ZH_JE:
		return CurHQ.Amount;
	default:
		return MEANLESS_DATA;
	}
}

void UZhSort::Sort(int Type,BOOL bAscendSort)
{
	int i;
    sort_struc  lpSort[MAXGPNUM];
	for (i = 0;i < nGpNum;i++) 
	{
		lpSort[i].fValue = fGetValue(Type,lpnUsedStkIndex[i]); //取排序值
		lpSort[i].nIndex = lpnUsedStkIndex[i];
		lpSort[i].tag = 0;
	}

	qsort((void *) lpSort, nGpNum, sizeof(struct sort_struc), sort_func );
	if (bAscendSort == FALSE)
		for ( i=0;i < nGpNum;i++)
			lpnUsedStkIndex[i] = lpSort[i].nIndex;
	else
		for ( i=0;i < nGpNum;i++)
			lpnUsedStkIndex[i] = lpSort[nGpNum-1-i].nIndex;

	//总是将MEANLESS_DATA丢在最后，不参与排序
	for(i = 0;i < nGpNum && fGetValue(Type,lpnUsedStkIndex[i]) != MEANLESS_DATA;i++);
	if(i < nGpNum)
	{
		int startpos,endpos,tempIndex;
		startpos = i;
		for(i = startpos+1;i < nGpNum && fGetValue(Type,lpnUsedStkIndex[i]) == MEANLESS_DATA;i++);
		endpos = i - 1;
		for(i = endpos + 1;i < nGpNum;i++)
		{
			tempIndex = lpnUsedStkIndex[i];
			lpnUsedStkIndex[i] = lpnUsedStkIndex[startpos+i-endpos-1];
			lpnUsedStkIndex[startpos+i-endpos-1] = tempIndex;
		}
	}
	return;
}

int UZhSort::HitTest(CPoint point)
{
	CRect rc,rc2;
	int i,j;
	for(i = 0;i<9;i++)
	{
		rc = CellRect[i];
		if(rc.PtInRect(point)) break;
	}
	if(i >= 9) return -1;
	rc2.CopyRect(&rc);
	rc2.left += 5;
	for(j = 0;j < min(m_nRowNum,nGpNum);j++)
	{
		rc2.top = rc.top+21+m_nItemHeight*j;
		rc2.bottom = rc.top+21+m_nItemHeight*(j+1);
		if(rc2.PtInRect(point))
			return i*MAX_ZHROW_ITEM+j;
	}
	return -1;
}

int  UZhSort::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case UM_GET_STOCK:
		int iIndex;
		if(!OnLine)
			iIndex = SortResult[m_aShowSort[m_nCurSel/m_nItems]][m_nCurSel%m_nItems];
		else
			iIndex = g_pStockIO->GetIndex(m_ZHBuf[m_aShowSort[m_nCurSel/m_nItems]].Code[m_nCurSel%m_nItems]);
		return iIndex;
	case WM_PAINT:
		{
			if(wParam == 1)
				ProcessMsg(UM_ZH_READSORT);
			
			CDC *pDC = m_pView->GetDC();   
			CBitmap		tempBitmap;				//作出临时位图
			CDC memDC;							//作出内存DC
			memDC.CreateCompatibleDC (pDC);
			tempBitmap.CreateCompatibleBitmap (pDC,m_rcIn.Width (),m_rcIn.Height());
			CBitmap *pOldBitmap = memDC.SelectObject (&tempBitmap);				
			g_d.Bar(&memDC,&m_rcIn,BACK_BRUSH);
			memDC.SetBkMode (TRANSPARENT);
			CFont *oldfont = g_d.SelectFont(&memDC,INFO_FONT);
			CPen *oldpen = g_d.SelectPen(&memDC,AXIS_PEN);
			CSize sz = memDC.GetTextExtent("深沪");
			int nTitleHeight = sz.cy + 8;
			CRect rcTitle;rcTitle.CopyRect(m_rcIn);
			rcTitle.bottom = rcTitle.top + nTitleHeight;
			CString tempStr;
			if(!m_bDSFlag && WhichType<DS_START) 
				tempStr.Format("综合排名 - %s",GPType[WhichType]);
			else if(m_bDSFlag && WhichType>=DS_START)
				tempStr.Format("综合排名 - %s",g_pDSMarket[WhichType-DS_START].Name);
			CSize sz0 = g_d.DisplayText(&memDC, rcTitle, VipColor.BIGTITLECOLOR, DT_VCENTER|DT_CENTER|DT_SINGLELINE, _F(tempStr));
		//	memDC.DrawIcon(rcTitle.right-20,3,m_hIcon_Pu[3]);
			nOptBW = rcTitle.right-20;
			g_d.DrawLine(&memDC,0,nTitleHeight,m_rcIn.Width(),nTitleHeight);
			
			//计算每个排名区的Rect
			int i,j,tmpWidth = m_rcIn.Width()/m_nUnitCol,tmpHeight = (m_rcIn.Height()-nTitleHeight)/m_nUnitRow;
			CRect rc;
			for(i=0;i<9;i++) CellRect[i].SetRect(0, 0, 0, 0);
			int pValidFlag[9] = {-1};
			int nXH = 0;
			for(i=0;i<9;i++)
			{
				if(TypeFlag[i]==1)
					pValidFlag[nXH++] = i;
			}
			for(i = 0;i < m_nUnitCol;i++) //列
			{
				rc.left = i*tmpWidth;
				for(j = 0;j < m_nUnitRow;j++) //行
				{
					rc.top = j*tmpHeight+nTitleHeight;
					rc.right = rc.left + tmpWidth;
					rc.bottom = rc.top + tmpHeight;
					if(pValidFlag[i*m_nUnitRow+j]>=0) 
						CellRect[pValidFlag[i*m_nUnitRow+j] ] = rc;
				}
			}

			m_nRowNum = (tmpHeight-m_nItemHeight)/m_nItemHeight;
			
			for(i = 0;i < m_nUnitRow-1;i++)
				g_d.DrawLine(&memDC,0,(i+1)*tmpHeight+nTitleHeight,m_rcIn.Width(),(i+1)*tmpHeight+nTitleHeight);
			for(i = 0;i < m_nUnitCol-1;i++)
				g_d.DrawLine(&memDC,(i+1)*tmpWidth,nTitleHeight,(i+1)*tmpWidth,m_rcIn.Height());
			
			if(m_bHasData) //如果有数据
			{
				//画具体的每一个排名
				g_d.SelectPen(&memDC,GRIDSEL_PEN);
				for(i = 0;i < 9;i++)
				{
					if(CellRect[i].Width()<3||CellRect[i].Height()<3||TypeFlag[i]==0) continue;
					DrawOneCell(&memDC,i,m_aShowSort[i]);
				}
			}
			pDC->BitBlt (left,top,m_rcIn.Width(),m_rcIn.Height(),&memDC,0,0,SRCCOPY);//从内存DC拷贝到屏幕DC
			
			g_d.RestorePen(&memDC,oldpen);
			memDC.SelectObject(pOldBitmap);
			tempBitmap.DeleteObject();
			memDC.DeleteDC();
		}
		break;
	case WM_TIMER:
		if(wParam == 0x6666)
		{
			m_bRefreshDraw = TRUE;
			g_nPriority = 2;
			ProcessMsg(WM_PAINT, 1);
			g_nPriority = 1;
			m_bRefreshDraw = FALSE;
		}
		break;
	case WM_CHANGESCTYPE:
		{
			m_bHasFL = TRUE;
			m_bDSFlag = (WhichType>=DS_START);
			//得到股票集合
			nGpNum = GetGpIndex(-1,WhichType,lpnUsedStkIndex,MAXGPNUM);
			ProcessMsg(WM_PAINT,1);
		}
		break;
	case WM_CHANGEZFPERIOD:
		{
			CString tempStr;
			tempStr.Format("%d分钟涨幅排名",g_nZFFzNum);
			strcpy(TypeStr[3],tempStr);
			tempStr.Format("%d分钟跌幅排名",g_nZFFzNum);
			strcpy(TypeStr[4],tempStr);
			
			ProcessMsg(WM_PAINT,1);
		}
		break;
	case WM_ERASEBKGND:
		return TRUE;
	case WM_KEYDOWN:
		if(m_nCurSel >= 0)
		{
			int index = m_nCurSel%m_nItems;
			if(wParam == VK_LEFT)
			{
				int nCurrUnit = m_nCurSel/MAX_ZHROW_ITEM, nCurrItem = m_nCurSel%MAX_ZHROW_ITEM;
				nCurrUnit = ToNextUnit(nCurrUnit, m_nUnitRow, FALSE);
				m_nCurSel = MAX_ZHROW_ITEM*nCurrUnit+nCurrItem;
				ProcessMsg(WM_PAINT);
				return TRUE;
			}
			if(wParam == VK_RIGHT)
			{
				int nCurrUnit = m_nCurSel/MAX_ZHROW_ITEM, nCurrItem = m_nCurSel%MAX_ZHROW_ITEM;
				nCurrUnit = ToNextUnit(nCurrUnit, m_nUnitRow, TRUE);
				m_nCurSel = MAX_ZHROW_ITEM*nCurrUnit+nCurrItem;
				ProcessMsg(WM_PAINT);
				return TRUE;
			}
			if(wParam == VK_UP)
			{
				int nCurrUnit = m_nCurSel/MAX_ZHROW_ITEM, nCurrItem = m_nCurSel%MAX_ZHROW_ITEM;
				if(nCurrItem==0) 
				{
					nCurrItem = m_nRowNum-1;
					nCurrUnit = ToNextUnit(nCurrUnit, 1, FALSE);
				}
				else nCurrItem--;
				m_nCurSel = MAX_ZHROW_ITEM*nCurrUnit+nCurrItem;
				ProcessMsg(WM_PAINT);
				return TRUE;
			}
			if(wParam == VK_DOWN)
			{
				int nCurrUnit = m_nCurSel/MAX_ZHROW_ITEM, nCurrItem = m_nCurSel%MAX_ZHROW_ITEM;
				if(nCurrItem==m_nRowNum-1) 
				{
					nCurrItem = 0;
					nCurrUnit = ToNextUnit(nCurrUnit, 1, TRUE);
				}
				else nCurrItem++;
				m_nCurSel = MAX_ZHROW_ITEM*nCurrUnit+nCurrItem;
				ProcessMsg(WM_PAINT);
				return TRUE;
			}
			if(wParam == VK_RETURN)
			{
				int iIndex;
				if(!OnLine)
					iIndex = SortResult[m_aShowSort[m_nCurSel/MAX_ZHROW_ITEM]][m_nCurSel%MAX_ZHROW_ITEM];
				else
					iIndex = g_pStockIO->GetIndex(m_ZHBuf[m_aShowSort[m_nCurSel/MAX_ZHROW_ITEM]].Code[m_nCurSel%MAX_ZHROW_ITEM]);
				return TRUE;
			}
		}
		break;
	case WM_LBUTTONDOWN:
		{
			CPoint	point(LOWORD(lParam),HIWORD(lParam));
			CPoint	RelPoint=point;
			RelPoint.x-=left; RelPoint.y-=top;
			int iNo = HitTest(RelPoint);
			if(iNo != -1)
			{
				m_nCurSel = iNo;
				ProcessMsg(WM_PAINT);
			}
		}
		break;
	case WM_LBUTTONDBLCLK:
		{
			CPoint point(LOWORD(lParam),HIWORD(lParam));
			int iNo = HitTest(point);
			if(iNo != -1)
				m_nCurSel = iNo;
		}
		break;
	case WM_MOUSEMOVE:
		{
			CPoint pt(LOWORD(lParam),HIWORD(lParam));			
			g_pToolTipMark->ShowTool(pt, m_pView);
		}
		break;
	case UM_GETDATA_ACK:
		{
			if(wParam!=8) break;
			if(!m_bDSFlag) 
				g_pGetData->GetZHSortACK(m_ZHBuf);
			else
				g_pGetData->DSGetZHSortACK(m_ZHBuf);
			ProcessMsg(WM_PAINT);
		}
		break;
	case UM_COM2SWITCH1:
		{
			if(m_nInCom2&&theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView2)))
			{
				MemStkInfo *tempStkInfo = g_pStockIO->Get_MemStkInfo(m_ZHBuf[0].Code[0],m_ZHBuf[0].SetCode[0]);
				if(!tempStkInfo) break;
				
				short bZfIndex = g_pStockIO->GetIndex(tempStkInfo->Code,tempStkInfo->setcode);
				if(bZfIndex>=0)
				{
					CComView2 *pView2 = (CComView2*)theViewManager.pActiveView;
					pView2->ChangeSyncUnitGP(bZfIndex);
				}
			}
		}
		return 1;
	case UM_ZH_READSORT:
		{
			int i;
			MakeZHSortTitle(TypeStr);
			if(!m_bDSFlag && OnLine)
			{
				m_pView->SendMessage(UM_GETMAINID);			//直接让父窗口填写g_nMainID
				g_nAssisID = SetComAssisID(m_nWhichUnit,8);
				g_pGetData->GetZHSort(WhichType,m_nItems);
				if(!m_bRefreshDraw)
				{
					KillTimer(m_pView->m_hWnd,0x6666);
					SetTimer(m_pView->m_hWnd,0x6666,(HQRefreshTime+1)*1000,NULL);
				}
			}
			else if(m_bDSFlag && DSOnLine)
			{
				m_pView->SendMessage(UM_GETMAINID);			//直接让父窗口填写g_nMainID
				g_nAssisID = SetComAssisID(m_nWhichUnit,8);
				g_pGetData->DSGetZHSort(g_pDSMarket[WhichType-DS_START].MktType,m_nItems);
				if(!m_bRefreshDraw)
				{
					KillTimer(m_pView->m_hWnd,0x6666);
					SetTimer(m_pView->m_hWnd,0x6666,(HQRefreshTime+1)*1000,NULL);
				}
			}
			else
			{
				Sort(ZH_ZANGF,TRUE);	//涨幅排名
				for(i = 0;i < min(m_nItems,nGpNum);i++)
					SortResult[0][i] = lpnUsedStkIndex[i];
				Sort(ZH_ZANGF,FALSE);	//跌幅排名
				for(i = 0;i < min(m_nItems,nGpNum);i++)
					SortResult[1][i] = lpnUsedStkIndex[i];
				Sort(ZH_ZENGF,TRUE);	//振幅排名
				for(i = 0;i < min(m_nItems,nGpNum);i++)
					SortResult[2][i] = lpnUsedStkIndex[i];
				Sort(ZH_FZZANG,TRUE);	//分钟涨幅排名
				for(i = 0;i < min(m_nItems,nGpNum);i++)
					SortResult[3][i] = lpnUsedStkIndex[i];
				Sort(ZH_FZZANG,FALSE);	//分钟跌幅排名
				for(i = 0;i < min(m_nItems,nGpNum);i++)
					SortResult[4][i] = lpnUsedStkIndex[i];
				Sort(ZH_LIANB,TRUE);	//量比排名
				for(i = 0;i < min(m_nItems,nGpNum);i++)
					SortResult[5][i] = lpnUsedStkIndex[i];
				Sort(ZH_WT,TRUE);		//委托前5名
				for(i = 0;i < min(m_nItems,nGpNum);i++)
					SortResult[6][i] = lpnUsedStkIndex[i];
				Sort(ZH_WT,FALSE);		//委托后5名
				for(i = 0;i < min(m_nItems,nGpNum);i++)
					SortResult[7][i] = lpnUsedStkIndex[i];
				Sort(ZH_JE,TRUE);		//最大金额
				for(i = 0;i < min(m_nItems,nGpNum);i++)
					SortResult[8][i] = lpnUsedStkIndex[i] ;
			}
		}
	default:
		break;
	}
	return UBase::ProcessMsg(message,wParam,lParam);
}

void UZhSort::GetOptValue(int &nSort, CString &aSort, int &nType)
{
	nSort = m_nUnitRow;
	nType = WhichType;
	for(int i=0;i<9;i++) 
	{
		if(m_aShowSort[i]<0||m_aShowSort[i]>8)
			aSort.Format("012345678");
	}
	aSort.Format("%d%d%d%d%d%d%d%d%d", m_aShowSort[0], m_aShowSort[1], m_aShowSort[2], m_aShowSort[3],m_aShowSort[4], m_aShowSort[5], m_aShowSort[6], m_aShowSort[7], m_aShowSort[8]);
}

int	 UZhSort::ToNextUnit(int nCurrUnit, int nSkipVal, BOOL bNext)
{
	if(nCurrUnit<0||nCurrUnit>=9) return 0;
	int res = nCurrUnit;
	int nSkipped = 0;
	if(bNext)
	{
		while (nSkipped < nSkipVal)
		{
			if(res<8)
				++res;
			else res = 0;
			if(TypeFlag[res]==1)
				nSkipped++;
		}
	}
	else
	{
		while (nSkipped < nSkipVal)
		{
			if(res>0)
				--res;
			else res = 8;
			if(TypeFlag[res]==1)
				nSkipped++;
		}
	}
	return res;
}

short UZhSort::GetCurrentGPIndex()
{
	if(m_nCurSel<0) return -1;
	if(!OnLine)
		return SortResult[m_aShowSort[m_nCurSel/m_nItems]][m_nCurSel%m_nItems];
	else
		return g_pStockIO->GetIndex(m_ZHBuf[m_aShowSort[m_nCurSel/m_nItems]].Code[m_nCurSel%m_nItems]);
}

