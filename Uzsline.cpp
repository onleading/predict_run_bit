#include "stdafx.h"
#include "UZst.h"
#include "SelfLineShowDlg.h"

//画线区:x:[xStart,xEnd];y:[yStart,yEndBot]

BOOL UZst::BeginDrawSelfLine(CDC * pDC,CPoint point)
{
	long i = 0;
	CPoint	XDPt = point;
	XDPt.x -= left;
	XDPt.y -= top;
	//
	if(XDPt.x<=xStart || XDPt.x>=xEnd || XDPt.y<=yStart || XDPt.y>=yEndBot)
		return FALSE;
	if(m_nOverlapNum>0)
	{
		TDX_MessageBox(NULL,"在叠加品种时不能画线!",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}
	if(m_bManyDayZST && m_nZSTManyDay>0)
	{
		TDX_MessageBox(NULL,"在浏览多日分时图时不能画线!",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}
	if(g_DrawLineTool.GetLineNum() >= SELFLINEMAX)
	{
		TDX_MessageBox(m_pView->m_hWnd,"画线空间已满,不能再画新的自画线!\n请删除其它画线后再画新线.",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}
	m_nSelfLineIn = 0;
	long xSPos = xStart;
	long xEPos = xEnd;
	if(m_bManyDayZST && m_nZSTManyDay>0)
		xSPos = xStart+m_nZSTManyDay*(xEnd-xStart)/(m_nZSTManyDay+1);
	if(XDPt.y<yVol)
	{
		m_nSelfLineIn	= ZST_PARTI;
		m_PtInArea		= CRect(xSPos,yStart,xEPos,yVol);
		m_AbsPtInArea	= CRect(min(xSPos+1,xEPos),yStart,xEPos,yVol);
	}
	else if(XDPt.y<yStartBot)
	{
		m_nSelfLineIn	= ZST_PARTII;
		m_PtInArea		= CRect(xSPos,yVol,xEPos,yStartBot);
		m_AbsPtInArea	= CRect(min(xSPos+1,xEPos),yVol,xEPos,yStartBot);
	}
	else if(XDPt.y<yEndBot)
	{
		m_nSelfLineIn	= ZST_PARTIII;
		m_PtInArea		= CRect(xSPos,yStartBot,xEPos,yEndBot);
		m_AbsPtInArea	= CRect(min(xSPos+1,xEPos),yStartBot,xEPos,yEndBot);
	}
	if(m_nSelfLineIn!=ZST_PARTI)
	{
		TDX_MessageBox(m_pView->m_hWnd,"目前只允许在价格区画线!",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}
//	if((m_nWay == ID_XXHG || m_nWay == ID_XXHGC || m_nWay == ID_XXHGD) && m_nSelfLineIn != 1)	
//	{
//		TDX_MessageBox(m_pView->m_hWnd,"线性回归等画线只允许在价格区绘制!",MB_OK|MB_ICONEXCLAMATION);
//		return FALSE;
//	}
	m_AbsPtInArea.OffsetRect(left,top);
	CRect r(&m_AbsPtInArea);
	m_pView->ClientToScreen(&r);
	ClipCursor(&r);
	::SetClassLong(m_pView->GetSafeHwnd(),GCL_HCURSOR,NULL);
	SetCursor(m_hDraw);
	
	MemStkInfo *pscode = m_pStkInfo;
	if(pscode==NULL) return FALSE;
	
	memset(&m_NewDrawLine,0,sizeof(DRAWLINE));
	//
	switch(m_nSelfLineIn)
	{
	case ZST_PARTI:
		strcpy(m_NewDrawLine.acCode,ZST_STANDK);
		break;
	case ZST_PARTII:
		strcpy(m_NewDrawLine.acCode,ZST_STANDV);
		break;
	case ZST_PARTIII:
		switch(m_nStyle)
		{
		case ZST_LB:
			strcpy(m_NewDrawLine.acCode,ZST_STANDLB);
			break;
		case ZST_MMLD:
			strcpy(m_NewDrawLine.acCode,ZST_STANDLD);
			break;
		case ZST_TOTALORDER:
			strcpy(m_NewDrawLine.acCode,ZST_STANDTO);
			break;
		case ZST_TOTALCANCEL:
			strcpy(m_NewDrawLine.acCode,ZST_STANDTOC);
			break;
		case ZST_CANCELSUM:
			strcpy(m_NewDrawLine.acCode,ZST_STANDCS);
			break;
		case ZST_TICKNUM:
			strcpy(m_NewDrawLine.acCode,ZST_STANDTN);
			break;
		case ZST_LARGETICK:
			strcpy(m_NewDrawLine.acCode,ZST_STANDLT);
			break;
		case ZST_IOVOL:
			strcpy(m_NewDrawLine.acCode,ZST_STANDIO);
			break;
		case ZST_LARGEVOL:
			strcpy(m_NewDrawLine.acCode,ZST_STANDLV);
			break;
		default:
			return FALSE;
		}
		break;
	}
	m_NewDrawLine.nWay    = m_nWay;
	m_NewDrawLine.SetCode = pscode->setcode;
	strcpy(m_NewDrawLine.Code,pscode->Code);
	m_NewDrawLine.nPeriod = LINEPERIOD_ZST;
	////////////////////
	m_Point[0] = point;
	m_Point[1] = point;
	m_Point[2] = point;
	m_Point[3] = point;
	m_Point[4] = point;
	TransSelfLine(m_Point[0],NEWLINE_ID,0);
	
	return TRUE;
}

void UZst::MoveDrawPanRatio( CDC *pDC,int nNo,BOOL bShow,int nDrawMode)
{
	int	nSelect = SELNONE_ID;
	CFont *oldfont = NULL;
	if(m_nOverlapNum>0) return;
	if(bShow)
	{
		oldfont = g_d.SelectFont(pDC,GRAPH_FONT);
		pDC->SetBkMode(TRANSPARENT);
		if(m_SelectInfo.bExist)
		{
			nSelect = m_SelectInfo.nSelfLineNo;
			m_bEvenSelected = TRUE;
		}
	}
	LPDRAWLINE pDrawLine = NULL;
	long lDrawLineNum = g_DrawLineTool.GetAllLine(pDrawLine);
	if(nNo!=NEWLINE_ID && (nNo<0 || nNo>=lDrawLineNum)) return;

	DRAWLINE &OneLine = ((nNo<0)?m_NewDrawLine:(pDrawLine[nNo]));
	long lFixPoint = 0;
	//
	switch(OneLine.nWay)
	{
	case ID_READY:		break;
	case ID_GOLDPRICE: 
		{
			m_Point[2] = m_Point[1];
			m_Point[3] = m_Point[1];
			float maxVal = GetPxValFromY(m_Point[1].y-top);
			m_Point[3].y = GetYFromPxVal(0)+top;
			//
			lFixPoint = g_DrawLineTool.GetMaxSetPtNum(OneLine.nWay);
			if(m_bDrawLine)
			{
				lFixPoint = m_nPtNum+1;
				if(lFixPoint>=2 && !m_bDraw2nd)
				{
					m_bDraw2nd = TRUE;
					break;
				}
			}
			//
			g_DrawLineTool.DrawOne(pDC,&OneLine,m_Point,lFixPoint,m_PtInArea,left,top,!bShow,(nSelect==nNo),&maxVal);
		}
		break;	
	case ID_GOLDAIM:
		{
			float fVal[3];
			fVal[0] = GetPxValFromY(m_Point[0].y-top);
			fVal[1] = GetPxValFromY(m_Point[1].y-top);
			fVal[2] = GetPxValFromY(m_Point[2].y-top);
			//
			lFixPoint = g_DrawLineTool.GetMaxSetPtNum(OneLine.nWay);
			if(m_bDrawLine)
			{
				lFixPoint = m_nPtNum+1;
				if(lFixPoint>=2 && !m_bDraw2nd)
				{
					m_bDraw2nd = TRUE;
					break;
				}
				if(lFixPoint>=3 && !m_bDraw3rd)
				{
					m_bDraw3rd = TRUE;
					lFixPoint = 2;
				}
			}
			//
			g_DrawLineTool.DrawOne(pDC,&OneLine,m_Point,lFixPoint,m_PtInArea,left,top,!bShow,(nSelect==nNo),fVal);
		}
		break;
	case ID_XXHGD:
	case ID_XXHGC:
	case ID_XXHG:
		if(!m_bDrawLine && !(m_SelectInfo.bExist && m_SelectInfo.IsMovable))
		{
			long SItemNo = GetTimeIndexFromX(m_Point[0].x-left);
			long EItemNo = GetTimeIndexFromX(m_Point[1].x-left);
			//
			if(SItemNo>=m_nTodayMin)	SItemNo = m_nTodayMin-1;
			else if(SItemNo<0)	SItemNo = 0;
			//
			if(EItemNo>=m_nTodayMin)	EItemNo = m_nTodayMin-1;
			else if(EItemNo<0)	EItemNo = 0;
			//
			m_Point[0].x = GetXFromTimeIndex(SItemNo)+left;
			m_Point[1].x = GetXFromTimeIndex(EItemNo)+left;
		}
		if(CalcLinear(m_Point[0].x-left,m_Point[1].x-left))
		{
			//
			lFixPoint = g_DrawLineTool.GetMaxSetPtNum(OneLine.nWay);
			if(m_bDrawLine) lFixPoint = m_nPtNum+1;
			//
			g_DrawLineTool.DrawOne(pDC,&OneLine,m_Point,lFixPoint,m_PtInArea,left,top,!bShow,(nSelect==nNo));
		}
		break;
	case ID_GOLD:
	case ID_DFBX:
	case ID_BDX:
	case ID_LDZX:
	case ID_LDXD:
	case ID_ARROW:
	case ID_RAY:
	case ID_ZSX:
	case ID_SSGS:
	case ID_ZX:
	case ID_YHX:
		//
		lFixPoint = g_DrawLineTool.GetMaxSetPtNum(OneLine.nWay);
		if(m_bDrawLine)
		{
			lFixPoint = m_nPtNum+1;
			if(lFixPoint>=2 && !m_bDraw2nd)
			{
				m_bDraw2nd = TRUE;
				break;
			}
		}
		//
		g_DrawLineTool.DrawOne(pDC,&OneLine,m_Point,lFixPoint,m_PtInArea,left,top,!bShow,(nSelect==nNo));
		break;
	case ID_PXZX:
	case ID_PXX:
		//
		lFixPoint = g_DrawLineTool.GetMaxSetPtNum(OneLine.nWay);
		if(m_bDrawLine)
		{
			lFixPoint = m_nPtNum+1;
			if(lFixPoint>=2 && !m_bDraw2nd)
			{
				m_bDraw2nd = TRUE;
				break;
			}
			if(lFixPoint>=3 && !m_bDraw3rd)
			{
				m_bDraw3rd = TRUE;
				lFixPoint = 2;
			}
		}
		//
		g_DrawLineTool.DrawOne(pDC,&OneLine,m_Point,lFixPoint,m_PtInArea,left,top,!bShow,(nSelect==nNo));
		break;
	case ID_UPFLAG:
	case ID_DOWNFLAG:
	case ID_TXTTOOLS:
		if(bShow && m_AbsPtInArea.PtInRect(m_Point[0]))
		{
			//
			lFixPoint = g_DrawLineTool.GetMaxSetPtNum(OneLine.nWay);
			if(m_bDrawLine)	lFixPoint = m_nPtNum+1;
			//
			g_DrawLineTool.DrawOne(pDC,&OneLine,m_Point,lFixPoint,m_PtInArea,left,top,!bShow,(nSelect==nNo));
		}
		break;
	case ID_ZQX:
		{
			m_Point[2] = m_Point[0];
			m_Point[3] = m_Point[1];
			m_Point[2].x = GetTimeIndexFromX(m_Point[2].x-left);
			m_Point[3].x = GetTimeIndexFromX(m_Point[3].x-left);
			if(m_Point[3].x<m_Point[2].x)
			{
				m_Point[3].x += m_Point[2].x;
				m_Point[2].x = m_Point[3].x - m_Point[2].x;
				m_Point[3].x = m_Point[3].x - m_Point[2].x;
			}
			else if(m_Point[3].x == m_Point[2].x)
				break;
			//
			lFixPoint = g_DrawLineTool.GetMaxSetPtNum(OneLine.nWay);
			if(m_bDrawLine) lFixPoint = m_nPtNum+1;
			//
			g_DrawLineTool.DrawOne(pDC,&OneLine,m_Point,lFixPoint,m_PtInArea,left,top,!bShow,(nSelect==nNo));
		}
		break;
	case ID_FBLQ:
	case ID_GANNTIME: 
		{
			m_Point[2] = m_Point[1];
			m_Point[2].x = GetTimeIndexFromX(m_Point[2].x-left);
			//
			lFixPoint = g_DrawLineTool.GetMaxSetPtNum(OneLine.nWay);
			if(m_bDrawLine)
			{
				lFixPoint = m_nPtNum+1;
				if(lFixPoint>=2 && !m_bDraw2nd)
				{
					m_bDraw2nd = TRUE;
					break;
				}
				if(lFixPoint>=3 && !m_bDraw3rd)
				{
					m_bDraw3rd = TRUE;
					lFixPoint = 2;
				}
			}
			//
			g_DrawLineTool.DrawOne(pDC,&OneLine,m_Point,lFixPoint,m_PtInArea,left,top,!bShow,(nSelect==nNo));
		}
		break;
	case ID_DEL:
		break;
	}
	if(bShow)
		g_d.RestoreFont(pDC,oldfont);
}

void UZst::DrawingMouseMove(CDC *pDC,CPoint point,int nNo)
{
	if(m_nOverlapNum>0 || m_Point[1] == point)
		return;

	LPDRAWLINE pDrawLine = NULL;
	long lDrawLineNum = g_DrawLineTool.GetAllLine(pDrawLine);
	if(nNo!=NEWLINE_ID && (nNo<0 || nNo>=lDrawLineNum)) return;

	DRAWLINE  &OneLine = ((nNo==NEWLINE_ID)?m_NewDrawLine:(pDrawLine[nNo]));

	MoveDrawPanRatio(pDC,nNo);
	if(m_nPtNum == 0)		m_Point[0] = point;
	else if(m_nPtNum == 1)	m_Point[1] = point;
	else if(m_nPtNum == 2)	m_Point[2] = point;
	MoveDrawPanRatio(pDC,nNo);
	
	return;
}

void UZst::SelfLineChg(CDC *pDC,CPoint point)
{
	if(m_nOverlapNum>0 || point==m_SelectInfo.nBeginPt )	return;
	if(m_SelectInfo.bExist && m_SelectInfo.IsMovable)
	{
		LPDRAWLINE pDrawLine = NULL;
		long lDrawLineNum = g_DrawLineTool.GetAllLine(pDrawLine);
		if(m_SelectInfo.nSelfLineNo<0 || m_SelectInfo.nSelfLineNo>=lDrawLineNum) return;

		HCURSOR hTmpCurSor = m_hDrawTurn;
		if(m_SelectInfo.nInOnePoint==1 || m_SelectInfo.nInOnePoint==2 || m_SelectInfo.nInOnePoint == 3)
			hTmpCurSor = m_hDrawTurn;
		else hTmpCurSor = m_hDrawMove;

		DRAWLINE  &OneLine = pDrawLine[m_SelectInfo.nSelfLineNo]; 	
		if(!m_SelectInfo.bMoved)
		{
			::SetClassLong(m_pView->GetSafeHwnd(),GCL_HCURSOR,NULL);
			SetCursor(hTmpCurSor);
			m_Point[0] = UTransToPoint(m_SelectInfo.nSelfLineNo,0);
			m_Point[1] = UTransToPoint(m_SelectInfo.nSelfLineNo,1);
			m_Point[2] = UTransToPoint(m_SelectInfo.nSelfLineNo,2);
			COLORREF crBak = VipColor.DRAWLINETOOLCOLOR;
			VipColor.DRAWLINETOOLCOLOR = VipColor.ZBCOLOR[4];
			MoveDrawPanRatio(pDC,m_SelectInfo.nSelfLineNo);
			VipColor.DRAWLINETOOLCOLOR = crBak;
		}
		else MoveDrawPanRatio(pDC,m_SelectInfo.nSelfLineNo);
		//
		if(m_SelectInfo.nInOnePoint == 1)
			m_Point[0] += (point - m_SelectInfo.nBeginPt);
		else if(m_SelectInfo.nInOnePoint == 2)
			m_Point[1] += (point - m_SelectInfo.nBeginPt);
		else if(m_SelectInfo.nInOnePoint == 3)
			m_Point[2] += (point - m_SelectInfo.nBeginPt);
		else
		{
			m_Point[0] += (point - m_SelectInfo.nBeginPt);
			m_Point[1] += (point - m_SelectInfo.nBeginPt);
			m_Point[2] += (point - m_SelectInfo.nBeginPt);
		}
		MoveDrawPanRatio(pDC,m_SelectInfo.nSelfLineNo);
		m_SelectInfo.nBeginPt = point;
		m_SelectInfo.bMoved   = TRUE;
	}
}

void UZst::TransSelfLine(CPoint point,int nNo,int nWhitch)
{
	LPDRAWLINE pDrawLine = NULL;
	long lDrawLineNum = g_DrawLineTool.GetAllLine(pDrawLine);
	if(nNo!=NEWLINE_ID && (nNo<0 || nNo>=lDrawLineNum)) return;
	//
	DRAWLINE  &OneLine = ((nNo==NEWLINE_ID)?m_NewDrawLine:(pDrawLine[nNo]));
	//通常如下，如不在下列，另外方式引用
	long	&lTime = (nWhitch == 0 ? OneLine.TimeS : OneLine.TimeE);
	float	&fVal  = (nWhitch == 0 ? OneLine.fPriceS : OneLine.fPriceE);
	long	lTimeIndex = 0;
	point.Offset(-left,-top);
	lTimeIndex = GetTimeIndexFromX(point.x);
	switch(OneLine.nWay)
	{
	case ID_READY:		break;
	case ID_PXX:
	case ID_PXZX:
	case ID_GOLDAIM:
		if(nWhitch >= 2)
		{
			memcpy(&OneLine.TimeR[0],&lTimeIndex,sizeof(long));
			OneLine.fPriceR[0] = GetPxValFromY(point.y);
		}
		else
		{
			memcpy(&lTime,&lTimeIndex,sizeof(long));
			fVal = GetPxValFromY(point.y);
		}
		break;
	case ID_GOLDPRICE:
	case ID_LDXD:
	case ID_ARROW:
	case ID_RAY:
	case ID_GOLD:
	case ID_DFBX:
	case ID_BDX:
	case ID_ZQX:
	case ID_ZSX: 
	case ID_SSGS:  
	case ID_ZX:
	case ID_LDZX:
	case ID_YHX:
	case ID_XXHGD:
	case ID_XXHGC:
	case ID_XXHG:
	case ID_FBLQ:  
	case ID_GANNTIME:
		if(nWhitch >= 2) return;
		memcpy(&lTime,&lTimeIndex,sizeof(long)),
		fVal = GetPxValFromY(point.y);
		break;
	case ID_TXTTOOLS:
	case ID_UPFLAG:
	case ID_DOWNFLAG:
		if(nWhitch == 0)
		{
			memcpy(&lTime,&lTimeIndex,sizeof(long)),
			fVal = GetPxValFromY(point.y);
		}
		break;
	case ID_DEL:
		break;
	}
}

CPoint UZst::UTransToPoint(int nNo,int nWhitch)
{
	int		  dx = 0;
	CPoint	  point(0,0);
	
	LPDRAWLINE pDrawLine = NULL;
	long lDrawLineNum = g_DrawLineTool.GetAllLine(pDrawLine);
	if(nNo!=NEWLINE_ID && (nNo<0 || nNo>=lDrawLineNum)) return point;

	DRAWLINE  &OneLine = ((nNo<0)?m_NewDrawLine:(pDrawLine[nNo]));
	//通常如下，如不在下列，另外方式引用
	long	  &lTime = (nWhitch == 0 ? OneLine.TimeS : OneLine.TimeE);
	float	  &fVal  = (nWhitch == 0 ? OneLine.fPriceS : OneLine.fPriceE);
	switch ( OneLine.nWay )
	{
	case ID_READY:		break;
	case ID_PXX:
	case ID_PXZX:
	case ID_GOLDAIM:
		point.x = GetXFromTimeIndex(lTime);
		point.y = GetYFromPxVal(fVal);
		if ( nWhitch == 2 )
		{
			point.x = GetXFromTimeIndex(OneLine.TimeR[0]);
			point.y = GetYFromPxVal(OneLine.fPriceR[0]);
		}
		break;
	case ID_LDXD:
	case ID_LDZX:
	case ID_ARROW:
	case ID_RAY:
	case ID_GOLD:
	case ID_DFBX:
	case ID_BDX:
	case ID_XXHGD:
	case ID_XXHGC:
	case ID_XXHG:
	case ID_ZSX: 
	case ID_SSGS:
	case ID_ZX:
	case ID_YHX:
	case ID_ZQX:
	case ID_TXTTOOLS:
	case ID_UPFLAG:
	case ID_DOWNFLAG:
	case ID_FBLQ:
	case ID_GANNTIME:
	case ID_GOLDPRICE:
		point.x = GetXFromTimeIndex(lTime);
		point.y = GetYFromPxVal(fVal);
		break;
	case ID_DEL:
		break;
	}
	point.Offset(left,top);
	return point;
}

int UZst::SelectSelfLine(CPoint point)
{
	long i,j;

	if(m_bDrawLine || !g_DrawLineTool.GetShowFlag())
		return SELNONE_ID;
	if(m_nInHisDlg) return SELNONE_ID;
	m_Point[0] = m_Point[1] = m_Point[2] = CPoint(left,top);

	LPDRAWLINE pDrawLine= NULL;
	long lDrawLineNum	= g_DrawLineTool.GetAllLine(pDrawLine);
	if(pDrawLine==NULL || lDrawLineNum<=0) return SELNONE_ID;
	//
	long xSPos = xStart;
	long xEPos = xEnd;
	if(m_bManyDayZST && m_nZSTManyDay>0)
		xSPos = xStart+m_nZSTManyDay*(xEnd-xStart)/(m_nZSTManyDay+1);
	m_PtInArea		= CRect(xSPos,yStart,xEPos,yVol);
	m_AbsPtInArea	= CRect(min(xSPos+1,xEPos),yStart,xEPos,yVol);
	m_AbsPtInArea.OffsetRect(left,top);
	if(!m_AbsPtInArea.PtInRect(point)) return SELNONE_ID;
	//
	char *kCode = "STANDK";
	MemStkInfo * pscode = NULL;
	char *acCode = NULL;
	long lMaxPoint = 0;
	long lSelPos = LINESEL_SELNONE;
	//
	for(i=0;i<lDrawLineNum;i++)
	{
		if(pDrawLine[i].lLineNo==i
			&& pDrawLine[i].nPeriod==LINEPERIOD_ZST
			&& strcmp(pDrawLine[i].acCode,ZST_STANDK)==0
			&& pDrawLine[i].SetCode==m_pStkInfo->setcode
			&& strcmp(pDrawLine[i].Code,m_pStkInfo->Code)==0)
		{
			lMaxPoint = g_DrawLineTool.GetMaxSetPtNum(pDrawLine[i].nWay);
			if(lMaxPoint<1 || lMaxPoint>3) continue;
			for(j=lMaxPoint;j>0;j--)
			{
				switch(j)
				{
				case 3:
					m_Point[j-1].x = GetXFromTimeIndex(pDrawLine[i].TimeR[0])+left;
					m_Point[j-1].y = GetYFromPxVal(pDrawLine[i].fPriceR[0])+top;
					break;
				case 2:
					m_Point[j-1].x = GetXFromTimeIndex(pDrawLine[i].TimeE)+left;
					m_Point[j-1].y = GetYFromPxVal(pDrawLine[i].fPriceE)+top;
					break;
				case 1:
					m_Point[j-1].x = GetXFromTimeIndex(pDrawLine[i].TimeS)+left;
					m_Point[j-1].y = GetYFromPxVal(pDrawLine[i].fPriceS)+top;
					break;
				}
			}
			switch(pDrawLine[i].nWay)
			{
			case ID_XXHGD:
			case ID_XXHGC:
			case ID_XXHG:
				{
					long SItemNo = GetTimeIndexFromX(m_Point[0].x-left);
					long EItemNo = GetTimeIndexFromX(m_Point[1].x-left);
					//
					if(SItemNo>=m_nTodayMin)	SItemNo = m_nTodayMin-1;
					else if(SItemNo<0)	SItemNo = 0;
					//
					if(EItemNo>=m_nTodayMin)	EItemNo = m_nTodayMin-1;
					else if(EItemNo<0)	EItemNo = 0;
					//
					m_Point[0].x = GetXFromTimeIndex(SItemNo)+left;
					m_Point[1].x = GetXFromTimeIndex(EItemNo)+left;
					//
					if(CalcLinear(m_Point[0].x-left,m_Point[1].x-left))
						lSelPos = g_DrawLineTool.CheckSel(&(pDrawLine[i]),m_Point,8,point,m_PtInArea,left,top);
					else lSelPos = LINESEL_SELNONE;
				}
				break;
			case ID_ZQX:
				m_Point[2] = m_Point[0];
				m_Point[3] = m_Point[1];
				m_Point[2].x = GetTimeIndexFromX(m_Point[2].x-left);
				m_Point[3].x = GetTimeIndexFromX(m_Point[3].x-left);
				if(m_Point[3].x<m_Point[2].x)
				{
					m_Point[3].x += m_Point[2].x;
					m_Point[2].x = m_Point[3].x - m_Point[2].x;
					m_Point[3].x = m_Point[3].x - m_Point[2].x;
				}
				else if(m_Point[3].x == m_Point[2].x)
				{
					lSelPos = LINESEL_SELNONE;
					break;
				}
				lSelPos = g_DrawLineTool.CheckSel(&(pDrawLine[i]),m_Point,4,point,m_PtInArea,left,top);
				break;
			case ID_FBLQ:
				m_Point[2] = m_Point[1];
				m_Point[2].x = GetTimeIndexFromX(m_Point[2].x-left);
				lSelPos = g_DrawLineTool.CheckSel(&(pDrawLine[i]),m_Point,3,point,m_PtInArea,left,top);
				break;
			case ID_GANNTIME:
				m_Point[2] = m_Point[1];
				m_Point[2].x = GetTimeIndexFromX(m_Point[2].x-left);
				lSelPos = g_DrawLineTool.CheckSel(&(pDrawLine[i]),m_Point,3,point,m_PtInArea,left,top);
				break;
			case ID_GOLDPRICE:
				m_Point[2] = m_Point[1];
				m_Point[3] = m_Point[1];
				m_Point[3].y = GetYFromPxVal(0)+top;
				lSelPos = g_DrawLineTool.CheckSel(&(pDrawLine[i]),m_Point,4,point,m_PtInArea,left,top);
				break;
			default:
				lSelPos = g_DrawLineTool.CheckSel(&(pDrawLine[i]),m_Point,lMaxPoint,point,m_PtInArea,left,top);
				break;
			}
			switch(lSelPos)
			{
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
				if(lSelPos<0 || lSelPos>=lMaxPoint) break;
				m_SelectInfo.nInOnePoint = lSelPos+1;
			case LINESEL_SELNOTPOINT:
				m_SelectInfo.bExist		= TRUE;
				m_SelectInfo.IsMovable	= TRUE;
				m_SelectInfo.nInRegion	= ZST_PARTI;
				m_SelectInfo.nSelfLineNo= i;
				m_SelectInfo.nBeginPt.x	= point.x;
				m_SelectInfo.nBeginPt.y	= point.y;
				m_SelectInfo.bMoved		= 0;
				return i;
			default:
				break;
			}
		}
	}
	return SELNONE_ID;
}

BOOL UZst::CalcLinear(long xPosS,long xPosE)
{
	//f=Slope*(i-Ex)+Ey
	//Slope=Sxy/Sxx
	//Sxy=Σ((i-Ex)*(V-Ey))=Σ(i*V)-n*Ex*Ey
	//Sxx=Σ((i-Ex)*(i-Ex))=Σ(i*i)-n*Ex*Ex
	//Ex=Σi/n
	//Ey=ΣV/n
	if(xPosS>xPosE)						//调整顺序
	{
		xPosS += xPosE;
		xPosE = xPosS - xPosE;
		xPosS -= xPosE;
	}
	long SItemNo = GetTimeIndexFromX(xPosS);
	long EItemNo = GetTimeIndexFromX(xPosE);
	//
	if(SItemNo>=m_nTodayMin)	SItemNo = m_nTodayMin-1;
	else if(SItemNo<0)	SItemNo = 0;
	//
	if(EItemNo>=m_nTodayMin)	EItemNo = m_nTodayMin-1;
	else if(EItemNo<0)	EItemNo = 0;
	//
	if(SItemNo==EItemNo) return FALSE;
	double Ex=0,Ey=0,Sxy=0,Sxx=0,Slope=0;
	long nItemNum = EItemNo-SItemNo+1;
	int i=0;
	for(i=SItemNo;i<=EItemNo;i++)
	{
		Ex	+= i;
		Ey	+= m_MinuteData[i].Now;
		Sxy	+= i*m_MinuteData[i].Now;
		Sxx += i*i;
	}
	Ex /= nItemNum;
	Ey /= nItemNum;
	Sxy -= Ex*Ey*nItemNum;
	Sxx -= Ex*Ex*nItemNum;
	Slope = Sxy/Sxx;
	m_Point[2].x = m_Point[4].x = m_Point[6].x = xPosS+left;
	m_Point[3].x = m_Point[5].x = m_Point[7].x = xPosE+left;
	m_Point[2].y = GetYFromPxVal(Slope*(SItemNo-Ex)+Ey)+top;
	m_Point[3].y = GetYFromPxVal(Slope*(EItemNo-Ex)+Ey)+top;
	double dMax=0,dMin=0;
	double dLinear=0;
	for(i=SItemNo;i<=EItemNo;i++)
	{
		dLinear = Slope*(i-Ex)+Ey;
		dMax = max(dMax,m_MinuteData[i].Now-dLinear);
		dMin = max(dMin,dLinear-m_MinuteData[i].Now);
	}
	m_Point[4].y = GetYFromPxVal(Slope*(SItemNo-Ex)+Ey+dMax)+top;
	m_Point[5].y = GetYFromPxVal(Slope*(EItemNo-Ex)+Ey+dMax)+top;
	m_Point[6].y = GetYFromPxVal(Slope*(SItemNo-Ex)+Ey-dMin)+top;
	m_Point[7].y = GetYFromPxVal(Slope*(EItemNo-Ex)+Ey-dMin)+top;

	return TRUE;
}

void UZst::ShowSelfLine(CDC *pDC)
{
	int i;
	//必须互斥
	if(m_bDrawLine || !g_DrawLineTool.GetShowFlag() || (m_SelectInfo.bExist && m_SelectInfo.IsMovable))
		return;
	if(m_lDate!=0) return;
	long xSPos = xStart;
	long xEPos = xEnd;
	if(m_bManyDayZST && m_nZSTManyDay>0)
		xSPos = xStart+m_nZSTManyDay*(xEnd-xStart)/(m_nZSTManyDay+1);
	m_PtInArea		= CRect(xSPos,yStart,xEPos,yVol);
	m_AbsPtInArea	= CRect(min(xSPos+1,xEPos),yStart,xEPos,yVol);
	m_AbsPtInArea.OffsetRect(left,top);
	LPDRAWLINE pDrawLine;
	long lDrawLine = g_DrawLineTool.GetAllLine(pDrawLine);
	for(i=0;i<lDrawLine;i++)
	{
		m_nSelfLineIn = SELNONE_ID;
		if(pDrawLine[i].lLineNo==i && pDrawLine[i].nPeriod==LINEPERIOD_ZST)	//未被删除
		{	//先比较K，再比较股票。因为可能有多股。
			if(strcmp(pDrawLine[i].acCode,ZST_STANDK)==0 && pDrawLine[i].SetCode==m_pStkInfo->setcode
				&& strcmp(pDrawLine[i].Code,m_pStkInfo->Code)==0)
			{
				m_Point[0] = UTransToPoint(i,0);
				m_Point[1] = UTransToPoint(i,1);
				m_Point[2] = UTransToPoint(i,2);
				if(m_Point[1]!=CPoint(left-1,top-1) && m_Point[0]!=CPoint(left-1,top-1))
					MoveDrawPanRatio(pDC,i,TRUE,R2_COPYPEN);
			}
		}
	}
	m_Point[0] = m_Point[1] = m_Point[2] = CPoint(left,top);
}

void UZst::SetSelfLine(UINT nID)
{
	switch(nID)
	{
	case ID_DEL:
		{
			LPDRAWLINE pDrawLine;
			long lDrawLine = g_DrawLineTool.GetAllLine(pDrawLine);
			if(g_DrawLineTool.GetShowFlag() && m_SelectInfo.bExist)
			{
				if(m_SelectInfo.nSelfLineNo >= 0 && m_SelectInfo.nSelfLineNo < lDrawLine)
				{
					g_DrawLineTool.DeleteLine(m_SelectInfo.nSelfLineNo);
					g_DrawLineTool.CleanDel();
					memset(&m_SelectInfo,0,sizeof(ZSTSELINFO));
					ProcessMsg(WM_PAINT);
				}
			}
			else 
			{
				BOOL bIsExist = 0;	
				for(int i=0;i<lDrawLine;i++)
				{
					if(strcmp(pDrawLine[i].Code,m_pStkInfo->Code)==0 && pDrawLine[i].SetCode==m_pStkInfo->setcode)
					{
						bIsExist = 1;
						break;
					}
				}
				if(bIsExist &&  TDX_MessageBox(m_pView->m_hWnd,"您要删除该证券关联的所有画线吗 ?",MB_YESNO|MB_ICONQUESTION) == IDYES)
				{
					for(int i=0;i<lDrawLine;i++)
					{
						if(strcmp(pDrawLine[i].Code,m_pStkInfo->Code)==0 && pDrawLine[i].SetCode==m_pStkInfo->setcode)
							g_DrawLineTool.DeleteLine(i);
					}
					g_DrawLineTool.CleanDel();
				}
				ProcessMsg(WM_PAINT);
			}
		}
	case ID_READY:
		ResetSelfLine();
		break;
	case ID_SHOWALLLINE:
		{
			CSelfLineShowDlg dlg;
			if(dlg.DoModal()==IDOK)
				ProcessMsg(WM_PAINT);
		}
		break;
	case ID_HIDESELFLINE:
		g_DrawLineTool.SetShowFlag(!g_DrawLineTool.GetShowFlag());
		ProcessMsg(WM_PAINT);
		break;
	case ID_PROPERTES:
		if(g_DrawLineTool.GetShowFlag() && m_SelectInfo.bExist
			&& m_SelectInfo.nSelfLineNo >= 0 && m_SelectInfo.nSelfLineNo < g_DrawLineTool.GetLineNum())
		{
			ProcessMsg(WM_PAINT);
		}
		break;
	default:
		ResetSelfLine();
		m_nWay = nID;
		m_bDrawLine = TRUE;
		break;
	}
}

UINT UZst::GetSelfLine()
{
	return m_nWay;
}

long UZst::GetTimeIndexFromX(long x)
{
	long lxStartPos	= xStart;
	long lxEndPos	= xEnd;
	if(m_bManyDayZST)
	{
		lxStartPos= xStart+m_nZSTManyDay*(xEnd-xStart)/(m_nZSTManyDay+1);
	}
	if(lxEndPos<=lxStartPos) return lxStartPos;
	return long((x-lxStartPos)*(240-0)*1.0/(lxEndPos-lxStartPos)+0.5);
}

long UZst::GetXFromTimeIndex(long ti)
{
	long lxStartPos	= xStart;
	long lxEndPos	= xEnd;
	if(m_bManyDayZST)
	{
		lxStartPos= xStart+m_nZSTManyDay*(xEnd-xStart)/(m_nZSTManyDay+1);
	}
	return long((ti-0)*(lxEndPos-lxStartPos)*1.0/(240-0)+lxStartPos+0.5);
}

float UZst::GetPxValFromY(long y)
{
	if(yVol<=yStart) return fBasePrice;
	return (fMaxPxOff+fBasePrice-(y-yStart)*(2*fMaxPxOff)/(yVol-yStart));
}

long UZst::GetYFromPxVal(float fVal)
{
	if(fMaxPxOff<=COMPPREC) return (yVol+yStart)/2;
	return (long)((fMaxPxOff+fBasePrice-fVal)*(yVol-yStart)/(2*fMaxPxOff)+yStart);
}
