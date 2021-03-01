// ZHSortDlg.cpp : implementation file

#include "stdafx.h"
#include "tdxw.h"
#include "ZHSortDlg.h"
#include "MainFrm.h"
#include "ComView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CZHSortDlg dialog

CZHSortDlg::CZHSortDlg(CWnd* pParent /*=NULL*/)
	: CCaptionButtonDlg(CZHSortDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CZHSortDlg)
	//}}AFX_DATA_INIT
	WhichType = 0;
	m_nCurSel = -1;
	m_bHasData = TRUE;
	m_nMainID = 0;
	m_bRefreshDraw = FALSE;

	for(int i=0;i<9;i++) TypeFlag[i] = 0;
	CalcSomeInfo(g_nZHStyle);
	
	memset(TypeStr,0,sizeof(char)*9*25);
}


void CZHSortDlg::DoDataExchange(CDataExchange* pDX)
{
	CCaptionButtonDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CZHSortDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CZHSortDlg, CCaptionButtonDlg)
	//{{AFX_MSG_MAP(CZHSortDlg)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_MESSAGE(WM_CHANGESCTYPE,OnChangeSCType)
	ON_MESSAGE(WM_CHANGEZFPERIOD,OnChangeZFPeriod)
	ON_MESSAGE(UM_GETDATA_ACK,OnGetDataAck)
	ON_WM_ERASEBKGND()
	ON_WM_SHOWWINDOW()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void MakeZHSortTitle(char (*TypeStr)[25])
{
	CString tempStr;
	
	strcpy(TypeStr[0],"涨幅排名");
	strcpy(TypeStr[1],"跌幅排名");
	strcpy(TypeStr[2],"振幅排名");
	tempStr.Format("%d分钟涨速排名",g_nZFFzNum);
	strcpy(TypeStr[3],tempStr);
	tempStr.Format("%d分钟跌速排名",g_nZFFzNum);
	strcpy(TypeStr[4],tempStr);
	strcpy(TypeStr[5],"量比排名");
	if(!g_bSupportDDE)
	{
		strcpy(TypeStr[6],"委比前排名");
		strcpy(TypeStr[7],"委比后排名");
	}
	else
	{
		strcpy(TypeStr[6],"总量换手率排名");
		strcpy(TypeStr[7],"现量换手率排名");
	}
	strcpy(TypeStr[8],"总金额排名");
}

BOOL CZHSortDlg::OnInitDialog() 
{
	CCaptionButtonDlg::OnInitDialog();
	//	
	MakeZHSortTitle(TypeStr);
	//
	SetSCType(WhichType);

	m_MarkToolTip.Create(this,TTS_ALWAYSTIP);
	m_MarkToolTip.Activate(FALSE);
	m_MarkToolTip.SetMaxTipWidth(500);
	m_MarkToolTip.SetDelayTime(500);
	return m_MarkToolTip.AddTool(this,"");

	if(g_pToolTipMark) 
		g_pToolTipMark->AddTool(this, ""); 
	//用于定时更新行情的定时器
	SetTimer(1,(HQRefreshTime+2)*1000,NULL);

	return TRUE;
}

void	CZHSortDlg::SetSCType(int nWhichType)
{
	CString tempStr;
	int		nSetDomain;          //SZ/SH/SZ_SH
	union   flunion ftype;       //股票过滤条件
	nSetDomain = lpnSysDomainType[nWhichType];
	ftype.fshort = lpsSysClassType_new[nWhichType];
	//取使用股票
	nGpNum = g_pStockIO->FiltCode(nSetDomain, ftype, lpnUsedStkIndex);
	if(nGpNum > 0) m_nCurSel = 0;
	
	tempStr.Format("全景排行 - %s",GPType[nWhichType]);
	SetWindowText(_F(tempStr));
	ProcessSort();
}

void	CZHSortDlg::SetStyle(int nStyle)
{
	g_nZHStyle = nStyle;
	for(int i=0;i<9;i++) TypeFlag[i] = 0;
	CalcSomeInfo(nStyle);
	ProcessSort();
}

void	CZHSortDlg::CalcSomeInfo(int nFlag)
{
	switch(nFlag) 
	{
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

void CZHSortDlg::DrawOneCell(CDC *pDC,int type)
{
	int i;
	CRect rc = CellRect[type];
	//先写标题
	CRect TitileRect = rc;
	TitileRect.top += 2;
	TitileRect.bottom = TitileRect.top+20;
	CFont *pOld = g_d.SelectFont(pDC,INFO_FONT);
	g_d.DisplayText(pDC, TitileRect, VipColor.BIGTITLECOLOR, DT_CENTER, _F(TypeStr[type]));

	if((type==3||type==4) && !OnLine) 
	{
		g_d.RestoreFont(pDC, pOld);
		return;
	}
	
	if(g_nZHStyle==STYLE_1X5)
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
			CSize zs=DrawMarkFlag(tempStkInfo,pDC,rc.left+3+size.cx, rc.top+22+m_nItemHeight*i-1,0,0,this);
			if(tempStkInfo->fl_flag>0)
				size.cx += zs.cx;
			if(m_nCurSel == type*MAX_ZHROW_ITEM+i)
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
					g_d.DisplayText(pDC,rc.left+2*rc.Width()/3-5,rc.top+22+m_nItemHeight*i,VipColor.TXTCOLOR,rc.Width()/3+3,MakeJE3(ftmp));
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
			CSize zs=DrawMarkFlag(tempStkInfo,pDC,rc.left+3+size.cx, rc.top+22+m_nItemHeight*i-1,0,0,this);
			if(tempStkInfo->fl_flag>0)
				size.cx += zs.cx;
			if(m_nCurSel == type*MAX_ZHROW_ITEM+i)
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
					g_d.DisplayText(pDC,rc.left+2*rc.Width()/3-5,rc.top+22+m_nItemHeight*i,VipColor.TXTCOLOR,rc.Width()/3+3,MakeJE3(ftmp));
				break;
			}
		}
	}
	g_d.RestoreFont(pDC, pOld);	
}

void CZHSortDlg::OnPaint() 
{
	CPaintDC dc(this);

	if(g_pToolTipMark) 
		g_pToolTipMark->ClearToolTipInfo(this);
	
	CRect rect;
	GetClientRect(&rect);
	CBitmap		tempBitmap;				//作出临时位图
	CDC memDC;							//作出内存DC
	memDC.CreateCompatibleDC (&dc);
	tempBitmap.CreateCompatibleBitmap (&dc,rect.Width (),rect.Height());
	CBitmap *pOldBitmap = memDC.SelectObject (&tempBitmap);
	g_d.Bar(&memDC,&rect,BACK_BRUSH);
	memDC.SetBkMode (TRANSPARENT);
	CPen *oldpen = g_d.SelectPen(&memDC,AXIS_PEN);
	
	//计算每个排名区的Rect
	int i,j,tmpWidth = rect.Width()/m_nUnitCol,tmpHeight = rect.Height()/m_nUnitRow;
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
			rc.top = j*tmpHeight;
			rc.right = rc.left + tmpWidth;
			rc.bottom = rc.top + tmpHeight;
			if(pValidFlag[i*m_nUnitRow+j]>=0) 
				CellRect[pValidFlag[i*m_nUnitRow+j] ] = rc;
		}
	}
	
	for(i = 0;i < m_nUnitRow-1;i++)
		g_d.DrawLine(&memDC,0,(i+1)*tmpHeight,rect.Width(),(i+1)*tmpHeight);
	for(i = 0;i < m_nUnitCol-1;i++)
		g_d.DrawLine(&memDC,(i+1)*tmpWidth,0,(i+1)*tmpWidth,rect.Height());

	if(m_bHasData) //如果有数据
	{
		//画具体的每一个排名
		g_d.SelectPen(&memDC,GRIDSEL_PEN);
		for(i = 0;i < 9;i++)
		{
			if(CellRect[i].Width()<3||CellRect[i].Height()<3||TypeFlag[i]==0) continue;
			DrawOneCell(&memDC,i);
		}
	}
	dc.BitBlt (0,0,rect.Width(),rect.Height(),&memDC,0,0,SRCCOPY);//从内存DC拷贝到屏幕DC

	g_d.RestorePen(&memDC,oldpen);
	memDC.SelectObject(pOldBitmap);
	tempBitmap.DeleteObject();
	memDC.DeleteDC();
}

float CZHSortDlg::fGetValue(int Type,short GpIndex)
{
	float	ftmp;
	int		ii;
	long	wl,wlc;
	struct CurrStockData	CurHQ;

	MemStkInfo *tempInfo = (*g_pStockIO)[GpIndex];
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

void CZHSortDlg::ProcessSort()
{
	int i;	
	MakeZHSortTitle(TypeStr);
	if(OnLine)
	{
		m_bHasData = FALSE;
		g_nMainID = MainIDPlus(m_nMainID,ZHSORT_WIN);
		g_nAssisID = ZHSORT_ASSISID;
		g_pGetData->GetZHSort(WhichType,m_nRowNum);
		if(!m_bRefreshDraw)
		{
			KillTimer(1);
			SetTimer(1,HQRefreshTime*2*1000,NULL);
		}
	}
	else
	{
		memset(SortResult,-1,9*MAX_ZHROW_ITEM*sizeof(short));//SortResult为short型

		Sort(ZH_ZANGF,TRUE); //涨幅排名
		for(i = 0;i < min(MAX_ZHROW_ITEM,nGpNum);i++)
			SortResult[0][i] = lpnUsedStkIndex[i];
		Sort(ZH_ZANGF,FALSE);//跌幅排名
		for(i = 0;i < min(MAX_ZHROW_ITEM,nGpNum);i++)
			SortResult[1][i] = lpnUsedStkIndex[i];
		Sort(ZH_ZENGF,TRUE); //振幅排名
		for(i = 0;i < min(MAX_ZHROW_ITEM,nGpNum);i++)
			SortResult[2][i] = lpnUsedStkIndex[i];
		Sort(ZH_FZZANG,TRUE); //分钟涨幅排名
		for(i = 0;i < min(MAX_ZHROW_ITEM,nGpNum);i++)
			SortResult[3][i] = lpnUsedStkIndex[i];
		Sort(ZH_FZZANG,FALSE);//分钟跌幅排名
		for(i = 0;i < min(MAX_ZHROW_ITEM,nGpNum);i++)
			SortResult[4][i] = lpnUsedStkIndex[i];
		Sort(ZH_LIANB,TRUE); //量比排名
		for(i = 0;i < min(MAX_ZHROW_ITEM,nGpNum);i++)
			SortResult[5][i] = lpnUsedStkIndex[i];
		Sort(ZH_WT,TRUE);//委托前5名
		for(i = 0;i < min(MAX_ZHROW_ITEM,nGpNum);i++)
			SortResult[6][i] = lpnUsedStkIndex[i];
		Sort(ZH_WT,FALSE);//委托后5名
		for(i = 0;i < min(MAX_ZHROW_ITEM,nGpNum);i++)
			SortResult[7][i] = lpnUsedStkIndex[i];
		Sort(ZH_JE,TRUE);//最大金额
		for(i = 0;i < min(MAX_ZHROW_ITEM,nGpNum);i++)
			SortResult[8][i] = lpnUsedStkIndex[i];
	}
}

void CZHSortDlg::Sort(int Type,BOOL bAscendSort)
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

void CZHSortDlg::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == 1)
	{
		BOOL bIdle = IsIdleStatus();
		if(g_bDowning || bIdle)	
			return;
		m_bRefreshDraw = TRUE;
		g_nPriority = 2;
		ProcessSort();
		g_nPriority = 1;
		m_bHasData = TRUE; //行情刷新时不闪烁
		m_bRefreshDraw = FALSE;
	}
	
	CCaptionButtonDlg::OnTimer(nIDEvent);
}

void CZHSortDlg::OnChangeSCType(WPARAM wParam,LPARAM lParam)
{
	int		nSetDomain;          //SZ/SH/SZ_SH
	union   flunion ftype;       //股票过滤条件
	nSetDomain = lpnSysDomainType[WhichType];
	ftype.fshort = lpsSysClassType_new[WhichType];
	//取使用股票
	nGpNum = g_pStockIO->FiltCode(nSetDomain, ftype, lpnUsedStkIndex);

	CString tempStr;
	tempStr.Format("全景排行 - %s",GPType[WhichType]);
	SetWindowText(_F(tempStr));

	ProcessSort();

	Invalidate(FALSE);
}

void CZHSortDlg::OnChangeZFPeriod(WPARAM wParam,LPARAM lParam)
{
	CString tempStr;
	tempStr.Format("%d分钟涨幅排名",g_nZFFzNum);
	strcpy(TypeStr[3],tempStr);
	tempStr.Format("%d分钟跌幅排名",g_nZFFzNum);
	strcpy(TypeStr[4],tempStr);

	ProcessSort();

	Invalidate(FALSE);
}

void CZHSortDlg::OnOK() //不允许用户按OK
{
}

void CZHSortDlg::OnCancel() 
{
	GetParent() ->PostMessage (WM_CANCELPRESSED,(WPARAM)this,0);
}

//不擦除背景
BOOL CZHSortDlg::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

BOOL CZHSortDlg::PreTranslateMessage(MSG* pMsg) 
{
	//将除ESC外的所有键盘输入都发给激活的视图
	if(m_MarkToolTip.m_hWnd) m_MarkToolTip.RelayEvent(pMsg);
	if(pMsg->message==WM_KEYDOWN && m_nCurSel >= 0)
	{
		if(pMsg->wParam == VK_LEFT)
		{
			int nCurrUnit = m_nCurSel/MAX_ZHROW_ITEM, nCurrItem = m_nCurSel%MAX_ZHROW_ITEM;
			if(g_nZHStyle==STYLE_3X3) 
				nCurrUnit = ToNextUnit(nCurrUnit, m_nUnitRow, FALSE);
			else if(g_nZHStyle==STYLE_1X5)
				nCurrUnit = ToNextUnit(nCurrUnit, m_nUnitRow, FALSE);
			m_nCurSel = MAX_ZHROW_ITEM*nCurrUnit+nCurrItem;
			Invalidate(FALSE);
			return TRUE;
		}
		if(pMsg->wParam == VK_RIGHT)
		{
			int nCurrUnit = m_nCurSel/MAX_ZHROW_ITEM, nCurrItem = m_nCurSel%MAX_ZHROW_ITEM;
			if(g_nZHStyle==STYLE_3X3) 
				nCurrUnit = ToNextUnit(nCurrUnit, m_nUnitRow, TRUE);
			else if(g_nZHStyle==STYLE_1X5)
				nCurrUnit = ToNextUnit(nCurrUnit, m_nUnitRow, TRUE);
			m_nCurSel = MAX_ZHROW_ITEM*nCurrUnit+nCurrItem;
			Invalidate(FALSE);
			return TRUE;
		}
		if(pMsg->wParam == VK_UP)
		{
			int nCurrUnit = m_nCurSel/MAX_ZHROW_ITEM, nCurrItem = m_nCurSel%MAX_ZHROW_ITEM;
			if(nCurrItem==0) 
			{
				nCurrItem = m_nRowNum-1;
				nCurrUnit = ToNextUnit(nCurrUnit, 1, FALSE);
			}
			else nCurrItem--;
			m_nCurSel = MAX_ZHROW_ITEM*nCurrUnit+nCurrItem;
			Invalidate(FALSE);
			return TRUE;
		}
		if(pMsg->wParam == VK_DOWN)
		{
			int nCurrUnit = m_nCurSel/MAX_ZHROW_ITEM, nCurrItem = m_nCurSel%MAX_ZHROW_ITEM;
			if(nCurrItem==m_nRowNum-1) 
			{
				nCurrItem = 0;
				nCurrUnit = ToNextUnit(nCurrUnit, 1, TRUE);
			}
			else nCurrItem++;
			m_nCurSel = MAX_ZHROW_ITEM*nCurrUnit+nCurrItem;
			Invalidate(FALSE);
			return TRUE;
		}
		if(pMsg->wParam == VK_RETURN)
		{
			int iIndex;
			if(!OnLine)
				iIndex = SortResult[m_nCurSel/MAX_ZHROW_ITEM][m_nCurSel%MAX_ZHROW_ITEM];
			else
				iIndex = g_pStockIO->GetIndex(m_ZHBuf[m_nCurSel/MAX_ZHROW_ITEM].Code[m_nCurSel%MAX_ZHROW_ITEM]);
			OnGpVip(iIndex);
			return TRUE;
		}
	}
	//将除ESC外的所有键盘输入都发给激活的视图
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==96) //对于小键盘上的0作特殊处理
			pMsg->wParam=48;
		if((pMsg->wParam>=0x30&&pMsg->wParam<=0x39)||(pMsg->wParam>='a'&&pMsg->wParam<='z')
			||(pMsg->wParam>='A'&&pMsg->wParam<='Z')||pMsg->wParam== '.'||pMsg->wParam== '?')
			pMsg->hwnd = theViewManager.pActiveView->m_hWnd;
	}
	return CCaptionButtonDlg::PreTranslateMessage(pMsg);
}


void CZHSortDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	SHOW_NOMODAL_CORNER

	CCaptionButtonDlg::OnShowWindow(bShow, nStatus);	
}

int CZHSortDlg::HitTest(CPoint point)
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

void CZHSortDlg::OnGpVip(int iIndex)
{
	MemStkInfo *ps = (*g_pStockIO)[iIndex];
	if(!ps)	return;
	ToZst(ps->Code,ps->setcode,FALSE);
}

void CZHSortDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int iNo = HitTest(point);
	if(iNo != -1)
	{
		m_nCurSel = iNo;
		Invalidate(FALSE);
	}
	
	CCaptionButtonDlg::OnLButtonDown(nFlags, point);
}

void CZHSortDlg::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CNewMenu menu;
	menu.LoadMenu (IDR_MAINFRAME);
	CNewMenu *pPopup0 = (CNewMenu *)menu.GetSubMenu(2);
	CNewMenu *pPopup = (CNewMenu *)pPopup0->GetSubMenu(2);
	ClientToScreen (&point);
	pPopup->TrackPopupMenu (TPM_LEFTALIGN,point.x,point.y,AfxGetMainWnd());
	
	CCaptionButtonDlg::OnLButtonDown(nFlags, point);
}

void CZHSortDlg::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	int iNo = HitTest(point);
	if(iNo != -1)
	{
		m_nCurSel = iNo;
		Invalidate(FALSE);
		int iIndex;
		if(!OnLine)
			iIndex = SortResult[m_nCurSel/MAX_ZHROW_ITEM][m_nCurSel%MAX_ZHROW_ITEM];
		else
			iIndex = g_pStockIO->GetIndex(m_ZHBuf[m_nCurSel/MAX_ZHROW_ITEM].Code[m_nCurSel%MAX_ZHROW_ITEM]);
		OnGpVip(iIndex);
	}	
	CCaptionButtonDlg::OnLButtonDblClk(nFlags, point);
}

void CZHSortDlg::OnGetDataAck(WPARAM wParam,LPARAM lParam)
{
	if (m_nMainID==g_AnsHeader.MainID && ZHSORT_ASSISID==g_AnsHeader.AssisID)
	{
		memset(m_ZHBuf,0,sizeof(ZHRESULT)*9);
		g_pGetData->GetZHSortACK(m_ZHBuf);
		m_bHasData = TRUE;
		Invalidate(FALSE);
	}
}

short CZHSortDlg::GetCurrentGPIndex()
{
	if(m_nCurSel<0) return -1;
	if(!OnLine)
		return SortResult[m_nCurSel/MAX_ZHROW_ITEM][m_nCurSel%MAX_ZHROW_ITEM];
	else
		return g_pStockIO->GetIndex(m_ZHBuf[m_nCurSel/MAX_ZHROW_ITEM].Code[m_nCurSel%MAX_ZHROW_ITEM]);
}

int	  CZHSortDlg::GetValidIndex(int nIndex, BOOL bNext)
{
	int nType = nIndex/MAX_ZHROW_ITEM;
	if(TypeFlag[nType]==1)		//有效
		return nIndex;
	if(!bNext)
	{
		--nType;
		while (nType>=0&&TypeFlag[nType]==0)
			nType--;
		if(nType>=0&&TypeFlag[nType]==1)
			return MAX_ZHROW_ITEM*nType+nIndex%MAX_ZHROW_ITEM;
		if(nType<0) nType = 8;
		return GetValidIndex(MAX_ZHROW_ITEM*nType+nIndex%MAX_ZHROW_ITEM, bNext);
	}
	else 
	{
		++nType;
		while (nType<9&&TypeFlag[nType]==0)
			nType++;
		if(nType<9&&TypeFlag[nType]==1)
			return MAX_ZHROW_ITEM*nType+nIndex%MAX_ZHROW_ITEM;
		if(nType>=8) nType = 0;
		return GetValidIndex(MAX_ZHROW_ITEM*nType+nIndex%MAX_ZHROW_ITEM, bNext);
	}
}

int	 CZHSortDlg::ToNextUnit(int nCurrUnit, int nSkipVal, BOOL bNext)
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

void CZHSortDlg::OnCaptionButton1()
{
	m_bt1.SetCheck(TRUE);
	m_bt2.SetCheck(FALSE);
	AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_ZHSORT_STYLE1);
}

void CZHSortDlg::OnCaptionButton2()
{
	m_bt1.SetCheck(FALSE);
	m_bt2.SetCheck(TRUE);
	AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_ZHSORT_STYLE0);
}

void CZHSortDlg::InitCaptionButtons()
{
	m_bt1.Init(IDB_BITMAP_CAPTIONBUTTON,8,RGB(0,255,0),1,this,g_nZHStyle==1,"1行4列");
	m_bt2.Init(IDB_BITMAP_CAPTIONBUTTON,8,RGB(0,255,0),1,this,g_nZHStyle==0,"3行3列");
	m_bt1.ShowWindow(TRUE);
	m_bt2.ShowWindow(TRUE);
}

void CZHSortDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	CString strTip;
	g_pToolTipMark->GetToolTextEx(strTip, point, this);
	if(strTip)
	{
		m_MarkToolTip.UpdateTipText(strTip, this);
		m_MarkToolTip.Update();
		m_MarkToolTip.Activate(TRUE);
	}
	
	CCaptionButtonDlg::OnMouseMove(nFlags, point);
}
