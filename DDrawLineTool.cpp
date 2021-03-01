#include "stdafx.h"
#include "DDrawLineTool.h"

double fGold[12]	= {0.0,0.236,0.382,0.5,0.618,0.809,1.0,1.382,1.618,2.0,2.382,2.618};
double fPercent[5]	= {0.0,0.25,0.333,0.5,1.0};
double fSameSect[9]	= {0.0,0.125,0.25,0.375,0.5,0.625,0.75,0.875,1.0};
double fGoldPx[13]	= {0.191,0.236,0.382,0.5,0.618,0.809,1.0,1.191,1.382,1.5,1.618,1.809,2.0};
double fGoldAim[7]	= {0,0.191,0.382,0.5,0.618,0.809,1};
///////////////////////////////////////////////////////////////////
char   *szGold[12]	= {"","23.6%%","38.2%%","50%%","61.8%%","80.9%%","","138.2%%","161.8%%","200%%","238.2%%","261.8%%"};
char   *szPercent[5]= {"","25.0%%","33.3%%","50.0%%",""};
char   *szSameSec[9]= {"","12.5%%","25.0%%","37.5%%","50.0%%","62.5%%","75.0%%","87.5%%",""};
///////////////////////////////////////////////////////////////////
char   nDotLine_Gold[12]= {0,1,1,1,1,1,0,1,1,1,1,1};
char   nDotLine_Cent[5]	= {0,1,1,1,0};
char   nDotLine_Sec[9]	= {0,1,1,1,1,1,1,1,0};
///////////////////////////////////////////////////////////////////
long xGannRate[11]	= {1,1,1,1,1,1,1,2,3,4,8};
long yGannRate[11]	= {1,1,8,4,3,2,1,1,1,1,1};

DDrawLineTool::DDrawLineTool()
{
	m_FilePath		= "";
	m_bLoaded		= FALSE;
	//
	m_aDrawLine		= NULL;
	m_nDrawLine		= 0;
	m_nDrawSpace	= 0;
	//
	m_bShowSelfLine = TRUE;
	//
	m_DrawTextFont.CreateFont(14, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,DEFAULT_PITCH|FF_DONTCARE,"Arial");	
}

DDrawLineTool::~DDrawLineTool()
{
	m_DrawTextFont.DeleteObject();
	TDELA(m_aDrawLine);
}

void DDrawLineTool::LoadLine()
{
	m_FilePath	= g_WSXHStr;
	m_FilePath	+= "ldraw.dat";
	m_bLoaded	= TRUE;
	//
	CFile File;
	m_nDrawLine = 0;
	if(File.Open(m_FilePath,CFile::modeRead|CFile::shareDenyNone))
	{
		m_nDrawLine = File.GetLength()/sizeof(DRAWLINE);
		if(m_nDrawLine>0)
		{
			if(CheckSpace(m_nDrawLine))
				File.Read(m_aDrawLine,m_nDrawLine*sizeof(DRAWLINE));
			else m_nDrawLine = 0;
			long i = 0;
			for(i=0;i<m_nDrawLine;i++)
			{
				if(m_aDrawLine[i].bDelete)
					m_aDrawLine[i].lLineNo = -1;
			}
			CleanDel();
			CleanZstLine();
		}
		File.Close();
	}
}

void DDrawLineTool::SaveLine()
{
	if(!m_bLoaded) return;
	CleanDel();
	CFile File;
	if(File.Open(m_FilePath,CFile::modeCreate|CFile::modeWrite|CFile::shareDenyNone))
	{
		if(m_nDrawLine>0)
			File.Write(m_aDrawLine,m_nDrawLine*sizeof(DRAWLINE));
		File.Close();
	}
}

BOOL DDrawLineTool::CheckSpace(long lReqSpc)
{
	BOOL bSLRet = TRUE;
	LPDRAWLINE tmpDL = NULL;
	CHECKSELFLINESPACE(bSLRet,m_aDrawLine,tmpDL,m_nDrawSpace,lReqSpc,SELFLINESTEP,DRAWLINE);
	return bSLRet;
}

BOOL DDrawLineTool::AddLine(LPDRAWLINE pDrawLine)
{
	if(pDrawLine==NULL) return FALSE;
	if(m_nDrawLine < SELFLINEMAX)
	{
		if(!CheckSpace(m_nDrawLine+1))
		{
			TDX_MessageBox(NULL,"添加画线时内存不足!",MB_OK|MB_ICONEXCLAMATION);
			return FALSE;
		}
		//
		memmove(&(m_aDrawLine[m_nDrawLine]),pDrawLine,sizeof(DRAWLINE));
		m_aDrawLine[m_nDrawLine].lLineNo = m_nDrawLine;
		m_nDrawLine++;
	}
	else
	{
		TDX_MessageBox(NULL,"不能再画新的线了!",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}
	return TRUE;
}

void DDrawLineTool::DeleteLine(long lineno)
{
	if(lineno<0)
	{
		long i = 0;
		for(i=0;i<m_nDrawLine;i++)
			m_aDrawLine[i].lLineNo = -1;
	}
	else if(lineno>=m_nDrawLine) return;
	m_aDrawLine[lineno].lLineNo = -1;
}

void DDrawLineTool::CleanDel()
{
	long i = 0;
	long lSaveNum = 0;
	for(i=0;i<m_nDrawLine;i++)
	{
		if(m_aDrawLine[i].lLineNo>=0)
		{
			if(i!=lSaveNum)
				memmove(&(m_aDrawLine[lSaveNum]),&(m_aDrawLine[i]),sizeof(DRAWLINE));
			m_aDrawLine[lSaveNum].lLineNo = lSaveNum;
			lSaveNum++;
		}
	}
	if(lSaveNum<m_nDrawLine)
		memset(&(m_aDrawLine[lSaveNum]),0,(m_nDrawLine-lSaveNum)*sizeof(DRAWLINE));
	m_nDrawLine = lSaveNum;
}

void DDrawLineTool::CleanZstLine()
{
	long i = 0;
	long lSaveNum = 0;
	for(i=0;i<m_nDrawLine;i++)
	{
		switch(m_aDrawLine[i].nPeriod)
		{
		case LINEPERIOD_MIN5:
		case LINEPERIOD_MIN15:
		case LINEPERIOD_MIN30:
		case LINEPERIOD_HOUR:
		case LINEPERIOD_DAY:
		case LINEPERIOD_WEEK:
		case LINEPERIOD_MONTH:
		case LINEPERIOD_MIN1:
		case LINEPERIOD_MINN:
		case LINEPERIOD_DAYN:
		case LINEPERIOD_SEASON:
		case LINEPERIOD_YEAR:
			if(i!=lSaveNum)
				memmove(&(m_aDrawLine[lSaveNum]),&(m_aDrawLine[i]),sizeof(DRAWLINE));
			m_aDrawLine[lSaveNum].lLineNo = lSaveNum;
			lSaveNum++;
			break;
		default:
			break;
		}
	}
	if(lSaveNum<m_nDrawLine)
		memset(&(m_aDrawLine[lSaveNum]),0,(m_nDrawLine-lSaveNum)*sizeof(DRAWLINE));
	m_nDrawLine = lSaveNum;
}

long DDrawLineTool::GetLineNum()
{
	return m_nDrawLine;
}

long DDrawLineTool::GetAllLine(LPDRAWLINE &pDrawLine)
{
	pDrawLine = m_aDrawLine;
	return m_nDrawLine;
}

BOOL DDrawLineTool::SetAllLine(LPDRAWLINE pDrawLine,long lLineNum)
{
	if(lLineNum<=0 || pDrawLine==NULL)
	{
		if(m_nDrawSpace>0)
			memset(m_aDrawLine,0,m_nDrawSpace*sizeof(DRAWLINE));
		m_nDrawLine = 0;
		return TRUE;
	}
	if(!CheckSpace(lLineNum)) return FALSE;
	memmove(m_aDrawLine,pDrawLine,lLineNum*sizeof(DRAWLINE));
	m_nDrawLine = lLineNum;
	return TRUE;
}

long DDrawLineTool::CheckSel(LPDRAWLINE pDrawLine,CPoint *linepoint,long pointnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	rc.OffsetRect(lLeft,lTop);
	switch(pDrawLine->nWay)
	{
	case ID_PXX:
		return SelPriceBelt(pDrawLine,linepoint,pointnum,point,rc,lLeft,lTop,pAssist,lAssist);
	case ID_PXZX:
		return SelParallel(pDrawLine,linepoint,pointnum,point,rc,lLeft,lTop,pAssist,lAssist);
	case ID_GOLDAIM:
		return SelGoldAim(pDrawLine,linepoint,pointnum,point,rc,lLeft,lTop,pAssist,lAssist);
	case ID_GOLDPRICE:
		return SelGoldPrice(pDrawLine,linepoint,pointnum,point,rc,lLeft,lTop,pAssist,lAssist);
	case ID_LDXD:
		return SelSegment(pDrawLine,linepoint,pointnum,point,rc,lLeft,lTop,pAssist,lAssist);
	case ID_ARROW:
		return SelArrow(pDrawLine,linepoint,pointnum,point,rc,lLeft,lTop,pAssist,lAssist);
	case ID_RAY:
		return SelRay(pDrawLine,linepoint,pointnum,point,rc,lLeft,lTop,pAssist,lAssist);
	case ID_GOLD:
		return SelGold(pDrawLine,linepoint,pointnum,point,rc,lLeft,lTop,pAssist,lAssist);
	case ID_DFBX:
		return SelPercent(pDrawLine,linepoint,pointnum,point,rc,lLeft,lTop,pAssist,lAssist);
	case ID_BDX:
		return SelSameSect(pDrawLine,linepoint,pointnum,point,rc,lLeft,lTop,pAssist,lAssist);
	case ID_ZQX:
		return SelSameSpan(pDrawLine,linepoint,pointnum,point,rc,lLeft,lTop,pAssist,lAssist);
	case ID_ZSX:
		return SelHoldFast(pDrawLine,linepoint,pointnum,point,rc,lLeft,lTop,pAssist,lAssist);
	case ID_SSGS:
		return SelAngle(pDrawLine,linepoint,pointnum,point,rc,lLeft,lTop,pAssist,lAssist);
	case ID_ZX:
		return SelRect(pDrawLine,linepoint,pointnum,point,rc,lLeft,lTop,pAssist,lAssist);
	case ID_LDZX:
		return SelBeeLine(pDrawLine,linepoint,pointnum,point,rc,lLeft,lTop,pAssist,lAssist);
	case ID_FBLQ:
		return SelFibonacci(pDrawLine,linepoint,pointnum,point,rc,lLeft,lTop,pAssist,lAssist);
	case ID_GANNTIME:
		return SelGannTime(pDrawLine,linepoint,pointnum,point,rc,lLeft,lTop,pAssist,lAssist);
	case ID_YHX:
		return SelArc(pDrawLine,linepoint,pointnum,point,rc,lLeft,lTop,pAssist,lAssist);
	case ID_XXHGD:
		return SelLinearBelt(pDrawLine,linepoint,pointnum,point,rc,lLeft,lTop,pAssist,lAssist);
	case ID_XXHGC:
		return SelLinearRay(pDrawLine,linepoint,pointnum,point,rc,lLeft,lTop,pAssist,lAssist);
	case ID_XXHG:
		return SelLinear(pDrawLine,linepoint,pointnum,point,rc,lLeft,lTop,pAssist,lAssist);
	case ID_TXTTOOLS:
		return SelText(pDrawLine,linepoint,pointnum,point,rc,lLeft,lTop,pAssist,lAssist);
	case ID_UPFLAG:
		return SelRise(pDrawLine,linepoint,pointnum,point,rc,lLeft,lTop,pAssist,lAssist);
	case ID_DOWNFLAG:
		return SelDive(pDrawLine,linepoint,pointnum,point,rc,lLeft,lTop,pAssist,lAssist);
	}
	return LINESEL_SELNONE;
}

void DDrawLineTool::DrawOne(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist)
{
	rc.OffsetRect(lLeft,lTop);
	switch(pDrawLine->nWay)
	{
	case ID_PXX:
		DrawPriceBelt(pDC,pDrawLine,newpoint,newnum,rc,lLeft,lTop,bCover,bSel,pAssist,lAssist);
		break;
	case ID_PXZX:
		DrawParallel(pDC,pDrawLine,newpoint,newnum,rc,lLeft,lTop,bCover,bSel,pAssist,lAssist);
		break;
	case ID_GOLDAIM:
		DrawGoldAim(pDC,pDrawLine,newpoint,newnum,rc,lLeft,lTop,bCover,bSel,pAssist,lAssist);
		break;
	case ID_GOLDPRICE:
		DrawGoldPrice(pDC,pDrawLine,newpoint,newnum,rc,lLeft,lTop,bCover,bSel,pAssist,lAssist);
		break;
	case ID_LDXD:
		DrawSegment(pDC,pDrawLine,newpoint,newnum,rc,lLeft,lTop,bCover,bSel,pAssist,lAssist);
		break;
	case ID_ARROW:
		DrawArrow(pDC,pDrawLine,newpoint,newnum,rc,lLeft,lTop,bCover,bSel,pAssist,lAssist);
		break;
	case ID_RAY:
		DrawRay(pDC,pDrawLine,newpoint,newnum,rc,lLeft,lTop,bCover,bSel,pAssist,lAssist);
		break;
	case ID_GOLD:
		DrawGold(pDC,pDrawLine,newpoint,newnum,rc,lLeft,lTop,bCover,bSel,pAssist,lAssist);
		break;
	case ID_DFBX:
		DrawPercent(pDC,pDrawLine,newpoint,newnum,rc,lLeft,lTop,bCover,bSel,pAssist,lAssist);
		break;
	case ID_BDX:
		DrawSameSect(pDC,pDrawLine,newpoint,newnum,rc,lLeft,lTop,bCover,bSel,pAssist,lAssist);
		break;
	case ID_ZQX:
		DrawSameSpan(pDC,pDrawLine,newpoint,newnum,rc,lLeft,lTop,bCover,bSel,pAssist,lAssist);
		break;
	case ID_ZSX:
		DrawHoldFast(pDC,pDrawLine,newpoint,newnum,rc,lLeft,lTop,bCover,bSel,pAssist,lAssist);
		break;
	case ID_SSGS:
		DrawAngle(pDC,pDrawLine,newpoint,newnum,rc,lLeft,lTop,bCover,bSel,pAssist,lAssist);
		break;
	case ID_ZX:
		DrawRect(pDC,pDrawLine,newpoint,newnum,rc,lLeft,lTop,bCover,bSel,pAssist,lAssist);
		break;
	case ID_LDZX:
		DrawBeeLine(pDC,pDrawLine,newpoint,newnum,rc,lLeft,lTop,bCover,bSel,pAssist,lAssist);
		break;
	case ID_FBLQ:
		DrawFibonacci(pDC,pDrawLine,newpoint,newnum,rc,lLeft,lTop,bCover,bSel,pAssist,lAssist);
		break;
	case ID_GANNTIME:
		DrawGannTime(pDC,pDrawLine,newpoint,newnum,rc,lLeft,lTop,bCover,bSel,pAssist,lAssist);
		break;
	case ID_YHX:
		DrawArc(pDC,pDrawLine,newpoint,newnum,rc,lLeft,lTop,bCover,bSel,pAssist,lAssist);
		break;
	case ID_XXHGD:
		DrawLinearBelt(pDC,pDrawLine,newpoint,newnum,rc,lLeft,lTop,bCover,bSel,pAssist,lAssist);
		break;
	case ID_XXHGC:
		DrawLinearRay(pDC,pDrawLine,newpoint,newnum,rc,lLeft,lTop,bCover,bSel,pAssist,lAssist);
		break;
	case ID_XXHG:
		DrawLinear(pDC,pDrawLine,newpoint,newnum,rc,lLeft,lTop,bCover,bSel,pAssist,lAssist);
		break;
	case ID_TXTTOOLS:
		DrawText(pDC,pDrawLine,newpoint,newnum,rc,lLeft,lTop,bCover,bSel,pAssist,lAssist);
		break;
	case ID_UPFLAG:
		DrawRise(pDC,pDrawLine,newpoint,newnum,rc,lLeft,lTop,bCover,bSel,pAssist,lAssist);
		break;
	case ID_DOWNFLAG:
		DrawDive(pDC,pDrawLine,newpoint,newnum,rc,lLeft,lTop,bCover,bSel,pAssist,lAssist);
		break;
	}
}

BOOL DDrawLineTool::GetShowFlag()
{
	return m_bShowSelfLine;
}

void DDrawLineTool::SetShowFlag(BOOL bShowSelfLine)
{
	m_bShowSelfLine = bShowSelfLine;
}

long DDrawLineTool::GetMaxSetPtNum(DWORD dwway)
{
	switch(dwway)
	{
	case ID_PXX:
	case ID_PXZX:
	case ID_GOLDAIM:
		return 3;
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
	case ID_FBLQ:
	case ID_GANNTIME:
	case ID_YHX:
	case ID_XXHGD:
	case ID_XXHGC:
	case ID_XXHG:
	case ID_UPFLAG:
	case ID_DOWNFLAG:
		return 2;
	case ID_TXTTOOLS:
		return 1;
	}
	return 0;
}

BOOL DDrawLineTool::CalcBeeLine(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(newnum<2) return FALSE;
	m_xPos[0] = newpoint[0].x;	m_yPos[0] = newpoint[0].y;
	m_xPos[1] = newpoint[1].x;	m_yPos[1] = newpoint[1].y;
	m_xPos[2] = m_xPos[0];		m_yPos[2] = m_yPos[0];
	m_xPos[3] = m_xPos[1];		m_yPos[3] = m_yPos[1];
	ExtendLine(m_xPos[2],m_yPos[2],m_xPos[3],m_yPos[3],LINEEXTENDTYPE_BOTH,rc,lLeft,lTop);
	return TRUE;
}

BOOL DDrawLineTool::DrawBeeLine(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist)
{
	if(!CalcBeeLine(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return FALSE;
	//
	COLORREF nColor = VipColor.DRAWLINETOOLCOLOR;
	int nStyle = PS_SOLID;
	int nWidth = 1;
	int nDrawMode = R2_COPYPEN;
	INITLINEPROPERTY();
	//
	g_d.ClipLine(pDC,m_xPos[2],m_yPos[2],m_xPos[3],m_yPos[3],rc,nColor,nDrawMode,nStyle,nWidth);
	//
	if(bSel)
	{
		DrawBar(pDC,m_xPos[0],m_yPos[0],rc);
		DrawBar(pDC,m_xPos[1],m_yPos[1],rc);
	}
	return TRUE;
}

long DDrawLineTool::SelBeeLine(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(!CalcBeeLine(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return LINESEL_SELNONE;
	if(abs(point.x-m_xPos[0])<SELECT_ON && abs(point.y-m_yPos[0])<SELECT_ON)
		return 0;
	else if(abs(point.x-m_xPos[1])<SELECT_ON && abs(point.y-m_yPos[1])<SELECT_ON)
		return 1;
	else if(m_xPos[2]==m_xPos[3] && point.x==m_xPos[2])
		return LINESEL_SELNOTPOINT;
	else if(PointToLine(point.x,point.y,m_xPos[2],m_yPos[2],m_xPos[3],m_yPos[3])<SELECT_ON)
		return LINESEL_SELNOTPOINT;
	return LINESEL_SELNONE;
}

BOOL DDrawLineTool::CalcSegment(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(newnum<2) return FALSE;
	m_xPos[0] = newpoint[0].x;
	m_yPos[0] = newpoint[0].y;
	m_xPos[1] = newpoint[1].x;
	m_yPos[1] = newpoint[1].y;
	return TRUE;
}

BOOL DDrawLineTool::DrawSegment(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist)
{
	if(!CalcSegment(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return FALSE;
	//
	COLORREF nColor = VipColor.DRAWLINETOOLCOLOR;
	int nStyle = PS_SOLID;
	int nWidth = 1;
	int nDrawMode = R2_COPYPEN;
	INITLINEPROPERTY();
	//
	g_d.ClipLine(pDC,m_xPos[0],m_yPos[0],m_xPos[1],m_yPos[1],rc,nColor,nDrawMode,nStyle,nWidth);
	//
	if(bSel)
	{
		DrawBar(pDC,m_xPos[0],m_yPos[0],rc);
		DrawBar(pDC,m_xPos[1],m_yPos[1],rc);
	}
	return TRUE;
}

long DDrawLineTool::SelSegment(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(!CalcSegment(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return LINESEL_SELNONE;
	if(abs(point.x-m_xPos[0])<SELECT_ON && abs(point.y-m_yPos[0])<SELECT_ON)
		return 0;
	else if(abs(point.x-m_xPos[1])<SELECT_ON && abs(point.y-m_yPos[1])<SELECT_ON)
		return 1;
	else if(point.x>min(m_xPos[0],m_xPos[1])-SELECT_ON && point.x<max(m_xPos[0],m_xPos[1])+SELECT_ON
		&& point.y>min(m_yPos[0],m_yPos[1])-SELECT_ON && point.y<max(m_yPos[0],m_yPos[1])+SELECT_ON
		&& PointToLine(point.x,point.y,m_xPos[0],m_yPos[0],m_xPos[1],m_yPos[1])<SELECT_ON)
		return LINESEL_SELNOTPOINT;
	return LINESEL_SELNONE;
}

BOOL DDrawLineTool::CalcRay(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(newnum<2) return FALSE;
	m_xPos[0] = newpoint[0].x;	m_yPos[0] = newpoint[0].y;
	m_xPos[1] = newpoint[1].x;	m_yPos[1] = newpoint[1].y;
	m_xPos[2] = m_xPos[1];		m_yPos[2] = m_yPos[1];
	ExtendLine(m_xPos[0],m_yPos[0],m_xPos[2],m_yPos[2],LINEEXTENDTYPE_ONE,rc,lLeft,lTop);
	return TRUE;
}

BOOL DDrawLineTool::DrawRay(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist)
{
	if(!CalcRay(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return FALSE;
	//
	COLORREF nColor = VipColor.DRAWLINETOOLCOLOR;
	int nStyle = PS_SOLID;
	int nWidth = 1;
	int nDrawMode = R2_COPYPEN;
	INITLINEPROPERTY();
	//
	g_d.ClipLine(pDC,m_xPos[0],m_yPos[0],m_xPos[2],m_yPos[2],rc,nColor,nDrawMode,nStyle,nWidth);
	//
	if(bSel)
	{
		DrawBar(pDC,m_xPos[0],m_yPos[0],rc);
		DrawBar(pDC,m_xPos[1],m_yPos[1],rc);
	}
	return TRUE;
}

long DDrawLineTool::SelRay(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(!CalcRay(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return LINESEL_SELNONE;
	if(abs(point.x-m_xPos[0])<SELECT_ON && abs(point.y-m_yPos[0])<SELECT_ON)
		return 0;
	else if(abs(point.x-m_xPos[1])<SELECT_ON && abs(point.y-m_yPos[1])<SELECT_ON)
		return 1;
	else if(m_xPos[2]==m_xPos[0] && point.x==m_xPos[0] && point.y>=min(m_yPos[2],m_yPos[0]) && point.y<=max(m_yPos[2],m_yPos[0]))
		return LINESEL_SELNOTPOINT;
	else if(point.x>=min(m_xPos[2],m_xPos[0]) && point.x<=max(m_xPos[2],m_xPos[0])
		&& point.y>=min(m_yPos[2],m_yPos[0]) && point.y<=max(m_yPos[2],m_yPos[0])
		&& PointToLine(point.x,point.y,m_xPos[0],m_yPos[0],m_xPos[2],m_yPos[2])<SELECT_ON)
		return LINESEL_SELNOTPOINT;
	return LINESEL_SELNONE;
}

BOOL DDrawLineTool::CalcArrow(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(newnum<2) return FALSE;
	m_xPos[0] = newpoint[0].x;	m_yPos[0] = newpoint[0].y;
	m_xPos[1] = newpoint[1].x;	m_yPos[1] = newpoint[1].y;
	return TRUE;
}

BOOL DDrawLineTool::DrawArrow(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist)
{
	if(!CalcArrow(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return FALSE;
	//
	COLORREF nColor = VipColor.DRAWLINETOOLCOLOR;
	int nStyle = PS_SOLID;
	int nWidth = 1;
	int nDrawMode = R2_COPYPEN;
	INITLINEPROPERTY();
	//
	g_d.ClipLine(pDC,m_xPos[0],m_yPos[0],m_xPos[1],m_yPos[1],rc,nColor,nDrawMode,nStyle,nWidth);
	if(m_xPos[0]!=m_xPos[1] || m_yPos[0]!=m_yPos[1])
	{
		double cos0 = (1.0*(m_yPos[0]-m_yPos[1]))/sqrt((m_xPos[0]-m_xPos[1])*(m_xPos[0]-m_xPos[1])+(m_yPos[0]-m_yPos[1])*(m_yPos[0]-m_yPos[1]));
		double sin0 = (1.0*(m_xPos[0]-m_xPos[1]))/sqrt((m_xPos[0]-m_xPos[1])*(m_xPos[0]-m_xPos[1])+(m_yPos[0]-m_yPos[1])*(m_yPos[0]-m_yPos[1]));
		double cos2 = cos0*0.866+sin0*0.5;
		double sin2 = sin0*0.866-cos0*0.5;
		double cos3 = cos0*0.866-sin0*0.5;
		double sin3 = sin0*0.866+cos0*0.5;
		g_d.ClipLine(pDC,m_xPos[1],m_yPos[1],m_xPos[1]+8*sin2,m_yPos[1]+8*cos2,rc,nColor,nDrawMode,nStyle,nWidth);
		g_d.ClipLine(pDC,m_xPos[1],m_yPos[1],m_xPos[1]+8*sin3,m_yPos[1]+8*cos3,rc,nColor,nDrawMode,nStyle,nWidth);
	}
	//
	if(bSel)
	{
		DrawBar(pDC,m_xPos[0],m_yPos[0],rc);
		DrawBar(pDC,m_xPos[1],m_yPos[1],rc);
	}
	return TRUE;
}

long DDrawLineTool::SelArrow(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(!CalcArrow(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return LINESEL_SELNONE;
	if(abs(point.x-m_xPos[0])<SELECT_ON && abs(point.y-m_yPos[0])<SELECT_ON)
		return 0;
	else if(abs(point.x-m_xPos[1])<SELECT_ON && abs(point.y-m_yPos[1])<SELECT_ON)
		return 1;
	else if(point.x>min(m_xPos[0],m_xPos[1])-SELECT_ON && point.x<max(m_xPos[0],m_xPos[1])+SELECT_ON
		&& point.y>min(m_yPos[0],m_yPos[1])-SELECT_ON && point.y<max(m_yPos[0],m_yPos[1])+SELECT_ON
		&& PointToLine(point.x,point.y,m_xPos[0],m_yPos[0],m_xPos[1],m_yPos[1])<SELECT_ON)
		return LINESEL_SELNOTPOINT;
	return LINESEL_SELNONE;
}

BOOL DDrawLineTool::CalcPriceBelt(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(newnum<2) return FALSE;
	m_xPos[0] = newpoint[0].x;	m_yPos[0] = newpoint[0].y;
	m_xPos[1] = newpoint[1].x;	m_yPos[1] = newpoint[1].y;
	m_xPos[6] = m_xPos[0];	m_yPos[6] = m_yPos[0];
	m_xPos[7] = m_xPos[1];	m_yPos[7] = m_yPos[1];
	ExtendLine(m_xPos[6],m_yPos[6],m_xPos[7],m_yPos[7],LINEEXTENDTYPE_BOTH,rc,lLeft,lTop);
	if(newnum>2)
	{
		m_xPos[2] = newpoint[2].x;					m_yPos[2] = newpoint[2].y;
		m_xPos[3] = m_xPos[2]-m_xPos[0]+m_xPos[1];	m_yPos[3] = m_yPos[2]-m_yPos[0]+m_yPos[1];
		m_xPos[4] = m_xPos[0]*2-m_xPos[2];			m_yPos[4] = m_yPos[0]*2-m_yPos[2];
		m_xPos[5] = m_xPos[4]-m_xPos[0]+m_xPos[1];	m_yPos[5] = m_yPos[4]-m_yPos[0]+m_yPos[1];
		//
		m_xPos[8] = m_xPos[2];	m_yPos[8] = m_yPos[2];
		m_xPos[9] = m_xPos[3];	m_yPos[9] = m_yPos[3];
		m_xPos[10] = m_xPos[4];	m_yPos[10] = m_yPos[4];
		m_xPos[11] = m_xPos[5];	m_yPos[11] = m_yPos[5];
		ExtendLine(m_xPos[8],m_yPos[8],m_xPos[9],m_yPos[9],LINEEXTENDTYPE_BOTH,rc,lLeft,lTop);
		ExtendLine(m_xPos[10],m_yPos[10],m_xPos[11],m_yPos[11],LINEEXTENDTYPE_BOTH,rc,lLeft,lTop);
	}
	return TRUE;
}

BOOL DDrawLineTool::DrawPriceBelt(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist)
{
	if(!CalcPriceBelt(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return FALSE;
	//
	COLORREF nColor = VipColor.DRAWLINETOOLCOLOR;
	int nStyle = PS_SOLID;
	int nWidth = 1;
	int nDrawMode = R2_COPYPEN;
	INITLINEPROPERTY();
	//
	g_d.ClipLine(pDC,m_xPos[6],m_yPos[6],m_xPos[7],m_yPos[7],rc,nColor,nDrawMode,nStyle,nWidth);
	if(newnum>2)
	{
		g_d.ClipLine(pDC,m_xPos[8],m_yPos[8],m_xPos[9],m_yPos[9],rc,nColor,nDrawMode,nStyle,nWidth);
		g_d.ClipLine(pDC,m_xPos[10],m_yPos[10],m_xPos[11],m_yPos[11],rc,nColor,nDrawMode,nStyle,nWidth);
	}
	//
	if(bSel)
	{
		DrawBar(pDC,m_xPos[0],m_yPos[0],rc);
		DrawBar(pDC,m_xPos[1],m_yPos[1],rc);
		DrawBar(pDC,m_xPos[2],m_yPos[2],rc);
	}
	return TRUE;
}

long DDrawLineTool::SelPriceBelt(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(!CalcPriceBelt(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return LINESEL_SELNONE;
	if(abs(point.x-m_xPos[0])<SELECT_ON && abs(point.y-m_yPos[0])<SELECT_ON)
		return 0;
	else if(abs(point.x-m_xPos[1])<SELECT_ON && abs(point.y-m_yPos[1])<SELECT_ON)
		return 1;
	else if(abs(point.x-m_xPos[2])<SELECT_ON && abs(point.y-m_yPos[2])<SELECT_ON)
		return 2;
	else if(m_xPos[6]==m_xPos[7] && (point.x==m_xPos[6] || point.x==m_xPos[7]))
		return LINESEL_SELNOTPOINT;
	else if(PointToLine(point.x,point.y,m_xPos[6],m_yPos[6],m_xPos[7],m_yPos[7])<SELECT_ON)
		return LINESEL_SELNOTPOINT;
	else if(PointToLine(point.x,point.y,m_xPos[8],m_yPos[8],m_xPos[9],m_yPos[9])<SELECT_ON)
		return LINESEL_SELNOTPOINT;
	else if(PointToLine(point.x,point.y,m_xPos[10],m_yPos[10],m_xPos[11],m_yPos[11])<SELECT_ON)
		return LINESEL_SELNOTPOINT;
	return LINESEL_SELNONE;
}

BOOL DDrawLineTool::CalcParallel(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(newnum<2) return FALSE;
	m_xPos[0] = newpoint[0].x;	m_yPos[0] = newpoint[0].y;
	m_xPos[1] = newpoint[1].x;	m_yPos[1] = newpoint[1].y;
	m_xPos[4] = m_xPos[0];	m_yPos[4] = m_yPos[0];
	m_xPos[5] = m_xPos[1];	m_yPos[5] = m_yPos[1];
	ExtendLine(m_xPos[4],m_yPos[4],m_xPos[5],m_yPos[5],LINEEXTENDTYPE_BOTH,rc,lLeft,lTop);
	if(newnum>=2)
	{
		m_xPos[2] = newpoint[2].x;					m_yPos[2] = newpoint[2].y;
		m_xPos[3] = m_xPos[2]-m_xPos[0]+m_xPos[1];	m_yPos[3] = m_yPos[2]-m_yPos[0]+m_yPos[1];
		//
		m_xPos[6] = m_xPos[2];	m_yPos[6] = m_yPos[2];
		m_xPos[7] = m_xPos[3];	m_yPos[7] = m_yPos[3];
		ExtendLine(m_xPos[6],m_yPos[6],m_xPos[7],m_yPos[7],LINEEXTENDTYPE_BOTH,rc,lLeft,lTop);
	}
	return TRUE;
}

BOOL DDrawLineTool::DrawParallel(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist)
{
	if(!CalcParallel(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return FALSE;
	//
	COLORREF nColor = VipColor.DRAWLINETOOLCOLOR;
	int nStyle = PS_SOLID;
	int nWidth = 1;
	int nDrawMode = R2_COPYPEN;
	INITLINEPROPERTY();
	//
	g_d.ClipLine(pDC,m_xPos[4],m_yPos[4],m_xPos[5],m_yPos[5],rc,nColor,nDrawMode,nStyle,nWidth);
	if(newnum>2)
		g_d.ClipLine(pDC,m_xPos[6],m_yPos[6],m_xPos[7],m_yPos[7],rc,nColor,nDrawMode,nStyle,nWidth);
	//
	if(bSel)
	{
		DrawBar(pDC,m_xPos[0],m_yPos[0],rc);
		DrawBar(pDC,m_xPos[1],m_yPos[1],rc);
		DrawBar(pDC,m_xPos[2],m_yPos[2],rc);
	}
	return TRUE;
}

long DDrawLineTool::SelParallel(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(!CalcParallel(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return LINESEL_SELNONE;
	if(abs(point.x-m_xPos[0])<SELECT_ON && abs(point.y-m_yPos[0])<SELECT_ON)
		return 0;
	else if(abs(point.x-m_xPos[1])<SELECT_ON && abs(point.y-m_yPos[1])<SELECT_ON)
		return 1;
	else if(abs(point.x-m_xPos[2])<SELECT_ON && abs(point.y-m_yPos[2])<SELECT_ON)
		return 2;
	else if(m_xPos[4]==m_xPos[5] && (point.x==m_xPos[4] || point.x==m_xPos[6]))
		return LINESEL_SELNOTPOINT;
	else if(PointToLine(point.x,point.y,m_xPos[4],m_yPos[4],m_xPos[5],m_yPos[5])<SELECT_ON)
		return LINESEL_SELNOTPOINT;
	else if(PointToLine(point.x,point.y,m_xPos[6],m_yPos[6],m_xPos[7],m_yPos[7])<SELECT_ON)
		return LINESEL_SELNOTPOINT;
	return LINESEL_SELNONE;
}

BOOL DDrawLineTool::CalcRect(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(newnum<2) return FALSE;
	m_xPos[0] = newpoint[0].x;	m_yPos[0] = newpoint[0].y;
	m_xPos[1] = newpoint[1].x;	m_yPos[1] = newpoint[1].y;
	return TRUE;
}

BOOL DDrawLineTool::DrawRect(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist)
{
	if(!CalcRect(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return FALSE;
	//
	COLORREF nColor = VipColor.DRAWLINETOOLCOLOR;
	int nStyle = PS_SOLID;
	int nWidth = 1;
	int nDrawMode = R2_COPYPEN;
	INITLINEPROPERTY();
	//
	g_d.ClipLine(pDC,m_xPos[0],m_yPos[0],m_xPos[0],m_yPos[1],rc,nColor,nDrawMode,nStyle,nWidth);
	g_d.ClipLine(pDC,m_xPos[0],m_yPos[1],m_xPos[1],m_yPos[1],rc,nColor,nDrawMode,nStyle,nWidth);
	g_d.ClipLine(pDC,m_xPos[1],m_yPos[1],m_xPos[1],m_yPos[0],rc,nColor,nDrawMode,nStyle,nWidth);
	g_d.ClipLine(pDC,m_xPos[1],m_yPos[0],m_xPos[0],m_yPos[0],rc,nColor,nDrawMode,nStyle,nWidth);
	//
	if(bSel)
	{
		DrawBar(pDC,m_xPos[0],m_yPos[0],rc);
		DrawBar(pDC,m_xPos[1],m_yPos[1],rc);
	}
	return TRUE;
}

long DDrawLineTool::SelRect(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(!CalcRect(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return LINESEL_SELNONE;
	if(abs(point.x-m_xPos[0])<SELECT_ON && abs(point.y-m_yPos[0])<SELECT_ON)
		return 0;
	else if(abs(point.x-m_xPos[1])<SELECT_ON && abs(point.y-m_yPos[1])<SELECT_ON)
		return 1;
	else if(point.x>min(m_xPos[0],m_xPos[1])-SELECT_ON && point.x<max(m_xPos[0],m_xPos[1])+SELECT_ON
		&& point.y>min(m_yPos[0],m_yPos[1])-SELECT_ON && point.y<max(m_yPos[0],m_yPos[1])+SELECT_ON
		&& (PointToLine(point.x,point.y,m_xPos[0],m_yPos[0],m_xPos[1],m_yPos[0])<SELECT_ON
		|| PointToLine(point.x,point.y,m_xPos[1],m_yPos[0],m_xPos[1],m_yPos[1])<SELECT_ON
		|| PointToLine(point.x,point.y,m_xPos[1],m_yPos[1],m_xPos[0],m_yPos[1])<SELECT_ON
		|| PointToLine(point.x,point.y,m_xPos[0],m_yPos[1],m_xPos[0],m_yPos[0])<SELECT_ON))
		return LINESEL_SELNOTPOINT;
	return LINESEL_SELNONE;
}

BOOL DDrawLineTool::CalcArc(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(newnum<2) return FALSE;
	m_xPos[0] = newpoint[0].x;	m_yPos[0] = newpoint[0].y;
	m_xPos[1] = newpoint[1].x;	m_yPos[1] = newpoint[1].y;
	return TRUE;
}

BOOL DDrawLineTool::DrawArc(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist)
{
	if(!CalcArc(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return FALSE;
	m_xPos[2] = m_xPos[0];	m_yPos[2] = m_yPos[0];
	m_xPos[3] = m_xPos[1];	m_yPos[3] = m_yPos[1];
	//
	double aArc=fabs(m_xPos[0]-m_xPos[1]);
	double bArc=fabs(m_yPos[0]-m_yPos[1]);
	long xCentre,yCentre;
	long lStartDgr,lEndDgr;
	long i = 0;
	if(m_xPos[0]>m_xPos[1] && m_yPos[0]>m_yPos[1])	//开口向下
	{
		xCentre = m_xPos[1]; yCentre = m_yPos[0];
		m_xPos[2] = max(2*m_xPos[1]-m_xPos[0],rc.left);
		m_yPos[2] = yCentre-long(sqrt(bArc*bArc*(1.0-(m_xPos[2]-xCentre)*(m_xPos[2]-xCentre)/(aArc*aArc))));
		//
		lStartDgr	= 0;
		lEndDgr		= 180;
	}
	else if(m_xPos[0]>m_xPos[1] && m_yPos[0]<m_yPos[1])	//开口向右
	{
		xCentre = m_xPos[0]; yCentre = m_yPos[1];
		m_yPos[2] = min(2*m_yPos[1]-m_yPos[0],rc.bottom);
		m_xPos[2] = xCentre-long(sqrt(aArc*aArc*(1.0-(m_yPos[2]-yCentre)*(m_yPos[2]-yCentre)/(bArc*bArc))));
		//
		lStartDgr	= 90;
		lEndDgr		= 270;
	}
	else if(m_xPos[0]<m_xPos[1] && m_yPos[0]>m_yPos[1])	//开口向左
	{
		xCentre = m_xPos[0]; yCentre = m_yPos[1];
		m_yPos[2] = max(2*m_yPos[1]-m_yPos[0],rc.top);
		m_xPos[2] = xCentre+long(sqrt(aArc*aArc*(1.0-(m_yPos[2]-yCentre)*(m_yPos[2]-yCentre)/(bArc*bArc))));
		//
		lStartDgr	= 270;
		lEndDgr		= 450;
	}
	else if(m_xPos[0]<m_xPos[1] && m_yPos[0]<m_yPos[1])	//开口向上
	{
		xCentre = m_xPos[1]; yCentre = m_yPos[0];
		m_xPos[2] = min(2*m_xPos[1]-m_xPos[0],rc.right);
		m_yPos[2] = yCentre+long(sqrt(bArc*bArc*(1.0-(m_xPos[2]-xCentre)*(m_xPos[2]-xCentre)/(aArc*aArc))));
		//
		lStartDgr	= 180;
		lEndDgr		= 360;
	}
	else return FALSE;
	//
	COLORREF nColor = VipColor.DRAWLINETOOLCOLOR;
	int nStyle = PS_SOLID;
	int nWidth = 1;
	int nDrawMode = R2_COPYPEN;
	INITLINEPROPERTY();
	//
	CPen *oldpen;
	CPen curpen(nStyle,nWidth,nColor);
	int tempMode = pDC->SetROP2(nDrawMode);
	oldpen = pDC->SelectObject(&curpen);
	//判断是不是在矩形里面,确实很麻烦,先用笨办法做了再说.直接算各角度坐标
	CPoint ptStartPt	= newpoint[0];
	CPoint ptNowPt	= newpoint[0];
	CPoint ptLastPt	= newpoint[0];
	BOOL bLastIn= rc.PtInRect(ptLastPt);
	BOOL bNowIn	= FALSE;
	for(i=lStartDgr;i<lEndDgr;i++)
	{
		ptNowPt = CPoint(long(xCentre+cos(DRAW_PI*i/180)*aArc),long(yCentre-sin(DRAW_PI*i/180)*bArc));
		bNowIn = rc.PtInRect(ptNowPt);
		if(bNowIn)
		{
			if(!bLastIn) ptStartPt = ptNowPt;
		}
		else
		{
			if(bLastIn && ptLastPt!=ptStartPt)
				pDC->Arc(xCentre-aArc,yCentre-bArc,xCentre+aArc,yCentre+bArc,ptStartPt.x,ptStartPt.y,ptLastPt.x,ptLastPt.y);
		}
		ptLastPt = ptNowPt;
		bLastIn = bNowIn;
	}
	if(bLastIn && ptLastPt!=ptStartPt)
		pDC->Arc(xCentre-aArc,yCentre-bArc,xCentre+aArc,yCentre+bArc,ptStartPt.x,ptStartPt.y,ptLastPt.x,ptLastPt.y);
	//
	pDC->SelectObject(oldpen);
	pDC->SetROP2(tempMode);
	//
	if(bSel)
	{
		DrawBar(pDC,m_xPos[0],m_yPos[0],rc);
		DrawBar(pDC,m_xPos[1],m_yPos[1],rc);
	}
	return TRUE;
}

long DDrawLineTool::SelArc(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(!CalcArc(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return LINESEL_SELNONE;
	if(!rc.PtInRect(point)) return LINESEL_SELNONE;
	if(abs(point.x-m_xPos[0])<SELECT_ON && abs(point.y-m_yPos[0])<SELECT_ON)
		return 0;
	else if(abs(point.x-m_xPos[1])<SELECT_ON && abs(point.y-m_yPos[1])<SELECT_ON)
		return 1;
	long aArc=abs(m_xPos[0]-m_xPos[1]);					//用定义判断,2a是到固定两点的距离
	long bArc=abs(m_yPos[0]-m_yPos[1]);
	long cArc=long(aArc>bArc?sqrt(aArc*aArc-bArc*bArc):sqrt(bArc*bArc-aArc*aArc));
	long xCentre,yCentre,cxPos1,cxPos2,cyPos1,cyPos2;	//固定的点
	if(m_xPos[0]>m_xPos[1] && m_yPos[0]>m_yPos[1])		//开口向下,椭圆上半部
	{
		xCentre = m_xPos[1]; yCentre = m_yPos[0];
		if(point.y>=yCentre+SELECT_ON) return LINESEL_SELNONE;
	}
	else if(m_xPos[0]>m_xPos[1] && m_yPos[0]<m_yPos[1])	//开口向右,椭圆左半部
	{
		xCentre = m_xPos[0]; yCentre = m_yPos[1];
		if(point.x>=xCentre+SELECT_ON) return LINESEL_SELNONE;
	}
	else if(m_xPos[0]<m_xPos[1] && m_yPos[0]>m_yPos[1])	//开口向左,椭圆右半部
	{
		xCentre = m_xPos[0]; yCentre = m_yPos[1];
		if(point.x<=xCentre-SELECT_ON) return LINESEL_SELNONE;
	}
	else if(m_xPos[0]<m_xPos[1] && m_yPos[0]<m_yPos[1])	//开口向上,椭圆下半部
	{
		xCentre = m_xPos[1]; yCentre = m_yPos[0];
		if(point.y<=yCentre-SELECT_ON) return LINESEL_SELNONE;
	}
	else return LINESEL_SELNONE;
	if(aArc>bArc)
	{
		cxPos1 = xCentre-cArc;
		cxPos2 = xCentre+cArc;
		cyPos1 = cyPos2 = yCentre;
	}
	else
	{
		cyPos1 = yCentre-cArc;
		cyPos2 = yCentre+cArc;
		cxPos1 = cxPos2 = xCentre;
	}
	long nDistance = long(sqrt((point.x-cxPos1)*(point.x-cxPos1)+(point.y-cyPos1)*(point.y-cyPos1))+sqrt((point.x-cxPos2)*(point.x-cxPos2)+(point.y-cyPos2)*(point.y-cyPos2)))/2;
	if((aArc>bArc && abs(nDistance-aArc)<SELECT_ON) || (aArc<=bArc && abs(nDistance-bArc)<SELECT_ON))
		return LINESEL_SELNOTPOINT;
	return LINESEL_SELNONE;
}

BOOL DDrawLineTool::CalcGold(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(newnum<2) return FALSE;
	m_xPos[0] = (rc.left+rc.right)/2;	m_yPos[0] = newpoint[0].y;
	m_xPos[1] = (rc.left+rc.right)/2;	m_yPos[1] = newpoint[1].y;
	return TRUE;
}

BOOL DDrawLineTool::DrawGold(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist)
{
	if(!CalcGold(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return FALSE;
	//
	COLORREF nColor = VipColor.DRAWLINETOOLCOLOR;
	int nStyle = PS_SOLID;
	int nWidth = 1;
	int nDrawMode = R2_COPYPEN;
	INITLINEPROPERTY();
	//
	double *fRatio	= fGold;
	char   *nDotLine= nDotLine_Gold;
	char   **szInfo	= szGold;
	//
	CFont  *pOldfont = NULL;
	pOldfont = pDC->SelectObject(&m_DrawTextFont);
	//
	HORIZON(12);
	//
	pDC->SelectObject(pOldfont);
	return TRUE;
}

long DDrawLineTool::SelGold(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(!CalcGold(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return LINESEL_SELNONE;
	SELHORIZON(12,fGold);
	return LINESEL_SELNONE;
}

BOOL DDrawLineTool::CalcPercent(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(newnum<2) return FALSE;
	m_xPos[0] = (rc.left+rc.right)/2;	m_yPos[0] = newpoint[0].y;
	m_xPos[1] = (rc.left+rc.right)/2;	m_yPos[1] = newpoint[1].y;
	return TRUE;
}

BOOL DDrawLineTool::DrawPercent(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist)
{
	if(!CalcPercent(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return FALSE;
	//
	COLORREF nColor = VipColor.DRAWLINETOOLCOLOR;
	int nStyle = PS_SOLID;
	int nWidth = 1;
	int nDrawMode = R2_COPYPEN;
	INITLINEPROPERTY();
	//
	double *fRatio	= fPercent;
	char   *nDotLine= nDotLine_Cent;
	char   **szInfo	= szPercent;
	//
	CFont  *pOldfont = NULL;
	pOldfont = pDC->SelectObject(&m_DrawTextFont);
	//
	HORIZON(5);
	//
	pDC->SelectObject(pOldfont);
	return TRUE;
}

long DDrawLineTool::SelPercent(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(!CalcPercent(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return LINESEL_SELNONE;
	SELHORIZON(5,fPercent);
	return LINESEL_SELNONE;
}

BOOL DDrawLineTool::CalcSameSect(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(newnum<2) return FALSE;
	m_xPos[0] = (rc.left+rc.right)/2;	m_yPos[0] = newpoint[0].y;
	m_xPos[1] = (rc.left+rc.right)/2;	m_yPos[1] = newpoint[1].y;
	return TRUE;
}

BOOL DDrawLineTool::DrawSameSect(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist)
{
	if(!CalcSameSect(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return FALSE;
	//
	COLORREF nColor = VipColor.DRAWLINETOOLCOLOR;
	int nStyle = PS_SOLID;
	int nWidth = 1;
	int nDrawMode = R2_COPYPEN;
	INITLINEPROPERTY();
	//
	double *fRatio	= fSameSect;
	char   *nDotLine= nDotLine_Sec;
	char   **szInfo	= szSameSec;
	//
	CFont  *pOldfont = NULL;
	pOldfont = pDC->SelectObject(&m_DrawTextFont);
	//
	HORIZON(9);
	//
	pDC->SelectObject(pOldfont);
	return TRUE;
}

long DDrawLineTool::SelSameSect(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(!CalcSameSect(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return LINESEL_SELNONE;
	SELHORIZON(9,fSameSect);
	return LINESEL_SELNONE;
}

BOOL DDrawLineTool::CalcGoldPrice(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(newnum<2) return FALSE;
	m_xPos[0] = (rc.left+rc.right)/2;	m_yPos[0] = newpoint[0].y;
	m_xPos[1] = (rc.left+rc.right)/2;	m_yPos[1] = newpoint[1].y;
	m_xPos[2] = (rc.left+rc.right)/2;	m_yPos[2] = newpoint[2].y;
	m_xPos[3] = (rc.left+rc.right)/2;	m_yPos[3] = newpoint[3].y;
	return TRUE;
}

BOOL DDrawLineTool::DrawGoldPrice(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist)
{
	if(!CalcGoldPrice(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return FALSE;
	//
	COLORREF nColor = VipColor.DRAWLINETOOLCOLOR;
	int nStyle = PS_SOLID;
	int nWidth = 1;
	int nDrawMode = R2_COPYPEN;
	INITLINEPROPERTY();
	//
	CFont  *pOldfont = NULL;
	pOldfont = pDC->SelectObject(&m_DrawTextFont);
	//
	long i = 0;
	float fBaseVal = *((float *)pAssist);
	for(i=0;i<13;i++)
	{
		long lTmpYPos = (m_yPos[2]-m_yPos[3])*fGoldPx[i]+m_yPos[3];
		if(i==6) g_d.ClipLine(pDC,rc.left,lTmpYPos,rc.right,lTmpYPos,rc,nColor,R2_XORPEN,PS_SOLID);
		else g_d.ClipLine(pDC,rc.left,lTmpYPos,rc.right,lTmpYPos,rc,nColor,R2_XORPEN,PS_DOT);
		if(!bCover)
		{
			if(lTmpYPos>rc.top+20 && lTmpYPos<rc.bottom)
			{
				if(i==6)
					g_d.DisplayText(pDC,rc.left,lTmpYPos-14,nColor,0,"BASE %.3f",fGoldPx[i]*fBaseVal);
				else 
					g_d.DisplayText(pDC,rc.left,lTmpYPos-14,nColor,0,"%.1f%% %.3f",fGoldPx[i]*100,fGoldPx[i]*fBaseVal);
			}
		}
	}
	if(bSel)
		DrawBar(pDC,m_xPos[1],m_yPos[1],rc);
	pDC->SelectObject(pOldfont);
	return TRUE;
}

long DDrawLineTool::SelGoldPrice(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(!CalcGoldPrice(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return LINESEL_SELNONE;
	long i = 0;
	for(i=0;i<13;i++)
	{
		long lTmpYPos = (m_yPos[2]-m_yPos[3])*fGoldPx[i]+m_yPos[3];
		if(abs(lTmpYPos-point.y)<SELECT_ON)
			return LINESEL_SELNOTPOINT;
	}
	return LINESEL_SELNONE;
}

BOOL DDrawLineTool::CalcGoldAim(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(newnum<2) return FALSE;
	m_xPos[0] = newpoint[0].x;	m_yPos[0] = newpoint[0].y;
	m_xPos[1] = newpoint[1].x;	m_yPos[1] = newpoint[1].y;
	m_xPos[2] = newpoint[2].x;	m_yPos[2] = newpoint[2].y;
	return TRUE;
}

BOOL DDrawLineTool::DrawGoldAim(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist)
{
	if(!CalcGoldAim(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return FALSE;
	//
	COLORREF nColor = VipColor.DRAWLINETOOLCOLOR;
	int nStyle = PS_SOLID;
	int nWidth = 1;
	int nDrawMode = R2_COPYPEN;
	INITLINEPROPERTY();
	//
	CFont  *pOldfont = NULL;
	pOldfont = pDC->SelectObject(&m_DrawTextFont);
	//
	long i = 0;
	float *fVal = (float *)pAssist;
	g_d.ClipLine(pDC,m_xPos[0],m_yPos[0],m_xPos[1],m_yPos[1],rc,VipColor.DRAWLINETOOLCOLOR,R2_XORPEN);
	if(newnum>2)
	{
		g_d.ClipLine(pDC,m_xPos[1],m_yPos[1],m_xPos[2],m_yPos[2],rc,VipColor.DRAWLINETOOLCOLOR,R2_XORPEN);
		for(i=0;i<7;i++)
		{
			long lTmpYPos = (m_yPos[1]-m_yPos[0])*fGoldAim[i]+m_yPos[2];
			float fTmpVal = (fVal[1]-fVal[0])*fGoldAim[i]+fVal[2];
			if(i==0)
			{
				g_d.ClipLine(pDC,rc.left,lTmpYPos,rc.right,lTmpYPos,rc,nColor,R2_XORPEN,PS_SOLID);
				if(!bCover)
				{
					if(lTmpYPos>rc.top+20 && lTmpYPos<rc.bottom)
						g_d.DisplayText(pDC,rc.left,lTmpYPos-14,nColor,0,"BASE %.3f",fVal[2]);
				}
			}
			else
			{
				g_d.ClipLine(pDC,rc.left,lTmpYPos,rc.right,lTmpYPos,rc,nColor,R2_XORPEN,PS_DOT);
				if(!bCover)
				{
					if(lTmpYPos>rc.top+20 && lTmpYPos<rc.bottom)
						g_d.DisplayText(pDC,rc.left,lTmpYPos-14,nColor,0,"%.1f%% %.3f",fGoldAim[i]*100,fTmpVal);
				}
			}
		}
	}
	//
	if(bSel)
	{
		DrawBar(pDC,m_xPos[0],m_yPos[0],rc);
		DrawBar(pDC,m_xPos[1],m_yPos[1],rc);
		DrawBar(pDC,m_xPos[2],m_yPos[2],rc);
	}
	pDC->SelectObject(pOldfont);
	return TRUE;
}

long DDrawLineTool::SelGoldAim(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(!CalcGoldAim(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return LINESEL_SELNONE;
	if(abs(point.x-m_xPos[0])<SELECT_ON && abs(point.y-m_yPos[0])<SELECT_ON)
		return 0;
	else if(abs(point.x-m_xPos[1])<SELECT_ON && abs(point.y-m_yPos[1])<SELECT_ON)
		return 1;
	else if(abs(point.x-m_xPos[2])<SELECT_ON && abs(point.y-m_yPos[2])<SELECT_ON)
		return 2;
	else if(point.x>min(m_xPos[0],m_xPos[1])-SELECT_ON && point.x<max(m_xPos[0],m_xPos[1])+SELECT_ON
		&& point.y>min(m_yPos[0],m_yPos[1])-SELECT_ON && point.y<max(m_yPos[0],m_yPos[1])+SELECT_ON
		&& PointToLine(point.x,point.y,m_xPos[0],m_yPos[0],m_xPos[1],m_yPos[1])<SELECT_ON)
		return LINESEL_SELNOTPOINT;
	else if(point.x>min(m_xPos[1],m_xPos[2])-SELECT_ON && point.x<max(m_xPos[1],m_xPos[2])+SELECT_ON
		&& point.y>min(m_yPos[1],m_yPos[2])-SELECT_ON && point.y<max(m_yPos[1],m_yPos[2])+SELECT_ON
		&& PointToLine(point.x,point.y,m_xPos[1],m_yPos[1],m_xPos[2],m_yPos[2])<SELECT_ON)
		return LINESEL_SELNOTPOINT;
	else
	{
		long i = 0;
		for(i=0;i<7;i++)
		{
			long lTmpYPos = (m_yPos[1]-m_yPos[0])*fGoldAim[i]+m_yPos[2];
			if(abs(lTmpYPos-point.y)<SELECT_ON)
				return LINESEL_SELNOTPOINT;
		}
	}
	return LINESEL_SELNONE;
}

BOOL DDrawLineTool::CalcSameSpan(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(newnum<2) return FALSE;
	m_xPos[0] = newpoint[0].x;	m_yPos[0] = (rc.top+rc.bottom)/2;
	m_xPos[1] = newpoint[1].x;	m_yPos[1] = m_yPos[0];
	m_xPos[2] = newpoint[2].x;	m_yPos[2] = m_yPos[0];
	m_xPos[3] = newpoint[3].x;	m_yPos[3] = m_yPos[0];
	return TRUE;
}

BOOL DDrawLineTool::DrawSameSpan(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist)
{
	if(!CalcSameSpan(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return FALSE;
	//
	COLORREF nColor = VipColor.DRAWLINETOOLCOLOR;
	int nStyle = PS_SOLID;
	int nWidth = 1;
	int nDrawMode = R2_COPYPEN;
	INITLINEPROPERTY();
	//
	long i = 0;
	long gap = m_xPos[3] - m_xPos[2];
	if(pDrawLine->nPeriod==LINEPERIOD_ZST)
	{
		if(gap<=0) return FALSE;
		//
		long lTmpXPos = 0;
		for(i=m_xPos[2];i<=240;i+=gap)
		{
			lTmpXPos = long((i-0)*(rc.right-rc.left)*1.0/(240-0)+0.5)+rc.left;
			g_d.ClipLine(pDC,lTmpXPos,rc.top,lTmpXPos,rc.bottom,rc,nColor,nDrawMode,nStyle,nWidth);
		}
		if(bSel)
		{
			lTmpXPos = long((m_xPos[2]-0)*(rc.right-rc.left)*1.0/(240-0)+0.5)+rc.left;
			DrawBar(pDC,lTmpXPos,m_yPos[2],rc);
			lTmpXPos = long((m_xPos[3]-0)*(rc.right-rc.left)*1.0/(240-0)+0.5)+rc.left;
			DrawBar(pDC,lTmpXPos,m_yPos[3],rc);
		}
	}
	else
	{
		long lWidth = rc.Width();
		long *pAllPosOfX = (long *)pAssist;
		if(pAllPosOfX==NULL || lAssist<=0) return FALSE;
		for(i=m_xPos[2];i<lAssist && pAllPosOfX[i]<lWidth;i+=gap)
		{
			g_d.ClipLine(pDC,pAllPosOfX[i]+rc.left,rc.top,pAllPosOfX[i]+rc.left,rc.bottom,rc,nColor,nDrawMode,nStyle,nWidth);
		}
		if(bSel)
		{
			DrawBar(pDC,pAllPosOfX[m_xPos[2]]+rc.left,m_yPos[2],rc);
			DrawBar(pDC,pAllPosOfX[m_xPos[3]]+rc.left,m_yPos[3],rc);
		}
	}
	return TRUE;
}

long DDrawLineTool::SelSameSpan(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(!CalcSameSpan(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return LINESEL_SELNONE;
	if(abs(point.x-m_xPos[0])<SELECT_ON && abs(point.y-m_yPos[0])<SELECT_ON)
		return 0;
	else if(abs(point.x-m_xPos[1])<SELECT_ON && abs(point.y-m_yPos[1])<SELECT_ON)
		return 1;
	else
	{
		long i = 0;
		long gap = m_xPos[3] - m_xPos[2];
		if(pDrawLine->nPeriod==LINEPERIOD_ZST)
		{
			if(gap<=0) return FALSE;
			long lTmpXPos = 0;
			for(i=m_xPos[2];i<=240;i+=gap)
			{
				lTmpXPos = long((i-0)*(rc.right-rc.left)*1.0/(240-0)+0.5)+rc.left;
				if(abs(lTmpXPos-point.x)<SELECT_ON)
					return LINESEL_SELNOTPOINT;
			}
		}
		else
		{
			long lWidth = rc.Width();
			long *pAllPosOfX = (long *)pAssist;
			if(pAllPosOfX==NULL || lAssist<=0) return LINESEL_SELNONE;
			for(i=m_xPos[2];i<lAssist && pAllPosOfX[i]<lWidth;i+=gap)
			{
				if(abs(pAllPosOfX[i]+rc.left-point.x)<SELECT_ON)
					return LINESEL_SELNOTPOINT;
			}
		}
	}
	return LINESEL_SELNONE;
}

BOOL DDrawLineTool::CalcFibonacci(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(newnum<2) return FALSE;
	m_xPos[0] = newpoint[0].x;	m_yPos[0] = (rc.top+rc.bottom)/2;
	m_xPos[1] = newpoint[1].x;	m_yPos[1] = m_yPos[0];
	m_xPos[2] = newpoint[2].x;	m_yPos[2] = m_yPos[0];
	return TRUE;
}

BOOL DDrawLineTool::DrawFibonacci(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist)
{
	if(!CalcFibonacci(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return FALSE;
	//
	COLORREF nColor = VipColor.DRAWLINETOOLCOLOR;
	int nStyle = PS_SOLID;
	int nWidth = 1;
	int nDrawMode = R2_COPYPEN;
	INITLINEPROPERTY();
	//
	long i = 0;
	long nStep1=0,nStep2=1;
	long lTmpXPos = 0;
	if(pDrawLine->nPeriod==LINEPERIOD_ZST)
	{
		for(i=m_xPos[2];i<=240;)
		{
			lTmpXPos = long((i-0)*(rc.right-rc.left)*1.0/(240-0)+0.5)+rc.left;
			g_d.ClipLine(pDC,lTmpXPos,rc.top,lTmpXPos,rc.bottom,rc,nColor,nDrawMode,nStyle,nWidth);
			i += nStep2;
			nStep2 += nStep1;
			nStep1 = nStep2-nStep1;
			if(!bCover)
			{
				if(lTmpXPos>rc.left+10 && lTmpXPos<rc.right-40)
					g_d.DisplayText(pDC,lTmpXPos+1,rc.top,nColor,0,"%d",nStep1);
			}
		}
		if(bSel)
		{
			lTmpXPos = long((m_xPos[2]-0)*(rc.right-rc.left)*1.0/(240-0)+0.5)+rc.left;
			DrawBar(pDC,lTmpXPos,m_yPos[2],rc);
		}
	}
	else
	{
		long lWidth = rc.Width();
		long *pAllPosOfX = (long *)pAssist;
		if(pAllPosOfX==NULL || lAssist<=0) return FALSE;
		for(i=m_xPos[2];i<lAssist && pAllPosOfX[i]<lWidth;)
		{
			lTmpXPos = pAllPosOfX[i]+rc.left;
			g_d.ClipLine(pDC,lTmpXPos,rc.top,lTmpXPos,rc.bottom,rc,nColor,nDrawMode,nStyle,nWidth);
			i += nStep2;
			nStep2 += nStep1;
			nStep1 = nStep2-nStep1;
			if(!bCover)
			{
				if(lTmpXPos>rc.left+10 && lTmpXPos<rc.right-40)
					g_d.DisplayText(pDC,lTmpXPos+1,rc.top,nColor,0,"%d",nStep2);
			}
		}
		if(bSel)
			DrawBar(pDC,pAllPosOfX[m_xPos[2]]+rc.left,m_yPos[2],rc);
	}
	return TRUE;
}

long DDrawLineTool::SelFibonacci(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(!CalcFibonacci(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return LINESEL_SELNONE;
	
	long i = 0;
	long nStep1=0,nStep2=1;
	long lTmpXPos = 0;
	if(pDrawLine->nPeriod==LINEPERIOD_ZST)
	{
		for(i=m_xPos[2];i<=240;)
		{
			lTmpXPos = long((i-0)*(rc.right-rc.left)*1.0/(240-0)+0.5)+rc.left;
			if(abs(lTmpXPos-point.x)<SELECT_ON)
				return LINESEL_SELNOTPOINT;
			i += nStep2;
			nStep2 += nStep1;
			nStep1 = nStep2-nStep1;
		}
	}
	else
	{
		long lWidth = rc.Width();
		long *pAllPosOfX = (long *)pAssist;
		if(pAllPosOfX==NULL || lAssist<=0) return LINESEL_SELNONE;
		for(i=m_xPos[2];i<lAssist && pAllPosOfX[i]<lWidth;)
		{
			lTmpXPos = pAllPosOfX[i]+rc.left;
			if(abs(lTmpXPos-point.x)<SELECT_ON)
				return LINESEL_SELNOTPOINT;
			i += nStep2;
			nStep2 += nStep1;
			nStep1 = nStep2-nStep1;
		}
	}

	return LINESEL_SELNONE;
}
BOOL DDrawLineTool::CalcGannTime(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(newnum<2) return FALSE;
	m_xPos[0] = newpoint[0].x;	m_yPos[0] = (rc.top+rc.bottom)/2;
	m_xPos[1] = newpoint[1].x;	m_yPos[1] = m_yPos[0];
	m_xPos[2] = newpoint[2].x;	m_yPos[2] = m_yPos[0];
	return TRUE;
}

BOOL DDrawLineTool::DrawGannTime(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist)
{
	if(!CalcGannTime(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return FALSE;
	//
	COLORREF nColor = VipColor.DRAWLINETOOLCOLOR;
	int nStyle = PS_SOLID;
	int nWidth = 1;
	int nDrawMode = R2_COPYPEN;
	INITLINEPROPERTY();
	//
	long i = 0;
	long lxPos = 0;
	if(pDrawLine->nPeriod==LINEPERIOD_ZST)
	{
		for(i=m_xPos[2];i<=240;i++)
		{
			if( (i>=m_xPos[2] && i<=m_xPos[2]+80) 
				&& ( ((i-m_xPos[2])%10==0&&(i-m_xPos[2])!=30&&(i-m_xPos[2])!=50)
				|| ((i-m_xPos[2])%8==0&&(i-m_xPos[2])!=32&&(i-m_xPos[2])!=48)
				|| (i-m_xPos[2]>=4 && (i-m_xPos[2]-4)%9==0)
				|| (i-m_xPos[2]==36 || i-m_xPos[2]==44) ) )
			{
				lxPos = long((i-0)*(rc.right-rc.left)*1.0/(240-0)+0.5)+rc.left;
				if(lxPos>rc.left && lxPos<rc.right)
					g_d.ClipLine(pDC,lxPos,rc.top,lxPos,rc.bottom,rc,nColor,nDrawMode,nStyle,nWidth);
			}
		}
		if(bSel)
		{
			lxPos = long((m_xPos[2]-0)*(rc.right-rc.left)*1.0/(240-0)+0.5)+rc.left;
			DrawBar(pDC,lxPos,m_yPos[2],rc);
		}
	}
	else
	{
		long *pAllPosOfX = (long *)pAssist;
		long lWidth = rc.Width();
		if(pAllPosOfX==NULL || lAssist<=0) return FALSE;
		for(i=m_xPos[2];i<lAssist && pAllPosOfX[i]<lWidth;i++)
		{
			if( (i>=m_xPos[2] && i<=m_xPos[2]+80) 
				&& ( ((i-m_xPos[2])%10==0&&(i-m_xPos[2])!=30&&(i-m_xPos[2])!=50)
				|| ((i-m_xPos[2])%8==0&&(i-m_xPos[2])!=32&&(i-m_xPos[2])!=48)
				|| (i-m_xPos[2]>=4 && (i-m_xPos[2]-4)%9==0)
				|| (i-m_xPos[2]==36 || i-m_xPos[2]==44) ) )
			{
				lxPos = pAllPosOfX[i]+rc.left;
				if(lxPos>rc.left && lxPos<rc.right)
					g_d.ClipLine(pDC,lxPos,rc.top,lxPos,rc.bottom,rc,nColor,nDrawMode,nStyle,nWidth);
			}
		}
		if(bSel)
			DrawBar(pDC,pAllPosOfX[m_xPos[2]]+rc.left,m_yPos[2],rc);
	}
	return TRUE;
}

long DDrawLineTool::SelGannTime(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(!CalcGannTime(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return LINESEL_SELNONE;
	if(abs(point.x-m_xPos[0])<SELECT_ON && abs(point.y-m_yPos[0])<SELECT_ON)
		return LINESEL_SELNOTPOINT;
	else
	{
		long i = 0;
		long lxPos = 0;
		if(pDrawLine->nPeriod==LINEPERIOD_ZST)
		{
			for(i=m_xPos[2];i<=240;i++)
			{
				if( (i>=m_xPos[2] && i<=m_xPos[2]+80) 
					&& ( ((i-m_xPos[2])%10==0&&(i-m_xPos[2])!=30&&(i-m_xPos[2])!=50)
					|| ((i-m_xPos[2])%8==0&&(i-m_xPos[2])!=32&&(i-m_xPos[2])!=48)
					|| (i-m_xPos[2]>=4 && (i-m_xPos[2]-4)%9==0)
					|| (i-m_xPos[2]==36 || i-m_xPos[2]==44) ) )
				{
					lxPos = long((i-0)*(rc.right-rc.left)*1.0/(240-0)+0.5)+rc.left;
					if(abs(lxPos-point.x)<SELECT_ON)
						return LINESEL_SELNOTPOINT;
				}
			}
		}
		else
		{
			long *pAllPosOfX = (long *)pAssist;
			long lWidth = rc.Width();
			for(i=m_xPos[2];i<lAssist && pAllPosOfX[i]<lWidth;i++)
			{
				if( (i>=m_xPos[2] && i<=m_xPos[2]+80) 
					&& ( ((i-m_xPos[2])%10==0&&(i-m_xPos[2])!=30&&(i-m_xPos[2])!=50)
					|| ((i-m_xPos[2])%8==0&&(i-m_xPos[2])!=32&&(i-m_xPos[2])!=48)
					|| (i-m_xPos[2]>=4 && (i-m_xPos[2]-4)%9==0)
					|| (i-m_xPos[2]==36 || i-m_xPos[2]==44) ) )
				{
					lxPos = pAllPosOfX[i]+rc.left;
					if(abs(lxPos-point.x)<SELECT_ON)
						return LINESEL_SELNOTPOINT;
				}
			}
		}
	}
	return LINESEL_SELNONE;
}

BOOL DDrawLineTool::CalcLinearBelt(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(newnum<2) return FALSE;
	m_xPos[0] = newpoint[0].x;	m_yPos[0] = newpoint[0].y;
	m_xPos[1] = newpoint[1].x;	m_yPos[1] = newpoint[1].y;
	m_xPos[2] = newpoint[2].x;	m_yPos[2] = newpoint[2].y;
	m_xPos[3] = newpoint[3].x;	m_yPos[3] = newpoint[3].y;
	m_xPos[4] = newpoint[4].x;	m_yPos[4] = newpoint[4].y;
	m_xPos[5] = newpoint[5].x;	m_yPos[5] = newpoint[5].y;
	m_xPos[6] = newpoint[6].x;	m_yPos[6] = newpoint[6].y;
	m_xPos[7] = newpoint[7].x;	m_yPos[7] = newpoint[7].y;
	return TRUE;
}

BOOL DDrawLineTool::DrawLinearBelt(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist)
{
	if(!CalcLinearBelt(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return FALSE;
	//
	COLORREF nColor = VipColor.DRAWLINETOOLCOLOR;
	int nStyle = PS_SOLID;
	int nWidth = 1;
	int nDrawMode = R2_COPYPEN;
	INITLINEPROPERTY();
	//
	g_d.ClipLine(pDC,m_xPos[2],m_yPos[2],m_xPos[3],m_yPos[3],rc,nColor,nDrawMode,PS_SOLID,nWidth);
	g_d.ClipLine(pDC,m_xPos[4],m_yPos[4],m_xPos[5],m_yPos[5],rc,nColor,nDrawMode,PS_SOLID,nWidth);
	g_d.ClipLine(pDC,m_xPos[6],m_yPos[6],m_xPos[7],m_yPos[7],rc,nColor,nDrawMode,PS_SOLID,nWidth);
	//
	if(bCover)
	{
		g_d.ClipLine(pDC,m_xPos[0],rc.top,m_xPos[0],rc.bottom,rc,nColor,nDrawMode,PS_SOLID,nWidth);
		g_d.ClipLine(pDC,m_xPos[1],rc.top,m_xPos[1],rc.bottom,rc,nColor,nDrawMode,PS_SOLID,nWidth);
	}
	//
	if(bSel)
	{
		DrawBar(pDC,m_xPos[2],m_yPos[2],rc);
		DrawBar(pDC,m_xPos[3],m_yPos[3],rc);
	}
	return TRUE;
}

long DDrawLineTool::SelLinearBelt(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(!CalcLinearBelt(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return LINESEL_SELNONE;
	if((abs(point.x-m_xPos[2])<SELECT_ON && abs(point.y-m_yPos[2])<SELECT_ON)
		|| (abs(point.x-m_xPos[3])<SELECT_ON && abs(point.y-m_yPos[3])<SELECT_ON))
	{
		if(abs(point.x-m_xPos[0])<SELECT_ON) return 0;
		else return 1;
	}
	else if(point.x>min(m_xPos[2],m_xPos[3])-SELECT_ON && point.x<max(m_xPos[2],m_xPos[3])+SELECT_ON
		&& (PointToLine(point.x,point.y,m_xPos[2],m_yPos[2],m_xPos[3],m_yPos[3])<SELECT_ON
		|| PointToLine(point.x,point.y,m_xPos[4],m_yPos[4],m_xPos[5],m_yPos[5])<SELECT_ON
		|| PointToLine(point.x,point.y,m_xPos[6],m_yPos[6],m_xPos[7],m_yPos[7])<SELECT_ON))
		return LINESEL_SELNOTPOINT;
	return LINESEL_SELNONE;
}

BOOL DDrawLineTool::CalcLinearRay(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(newnum<2) return FALSE;
	m_xPos[0] = newpoint[0].x;	m_yPos[0] = newpoint[0].y;
	m_xPos[1] = newpoint[1].x;	m_yPos[1] = newpoint[1].y;
	m_xPos[2] = newpoint[2].x;	m_yPos[2] = newpoint[2].y;
	m_xPos[3] = newpoint[3].x;	m_yPos[3] = newpoint[3].y;
	m_xPos[4] = newpoint[4].x;	m_yPos[4] = newpoint[4].y;
	m_xPos[5] = newpoint[5].x;	m_yPos[5] = newpoint[5].y;
	m_xPos[6] = newpoint[6].x;	m_yPos[6] = newpoint[6].y;
	m_xPos[7] = newpoint[7].x;	m_yPos[7] = newpoint[7].y;
	m_xPos[8] = m_xPos[3];	m_yPos[8] = m_yPos[3];
	m_xPos[9] = m_xPos[5];	m_yPos[9] = m_yPos[5];
	m_xPos[10] = m_xPos[7];	m_yPos[10] = m_yPos[7];
	ExtendLine(m_xPos[2],m_yPos[2],m_xPos[8],m_yPos[8],LINEEXTENDTYPE_ONE,rc,lLeft,lTop);
	m_xPos[9] = m_xPos[8];	m_xPos[10] = m_xPos[8];
	m_yPos[9] = m_yPos[8]+(m_yPos[5]-m_yPos[3]);
	m_yPos[10] = m_yPos[8]+(m_yPos[7]-m_yPos[3]);
	return TRUE;
}

BOOL DDrawLineTool::DrawLinearRay(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist)
{
	if(!CalcLinearRay(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return FALSE;
	//
	COLORREF nColor = VipColor.DRAWLINETOOLCOLOR;
	int nStyle = PS_SOLID;
	int nWidth = 1;
	int nDrawMode = R2_COPYPEN;
	INITLINEPROPERTY();
	//
	g_d.ClipLine(pDC,m_xPos[2],m_yPos[2],m_xPos[3],m_yPos[3],rc,nColor,nDrawMode,PS_SOLID,nWidth);
	g_d.ClipLine(pDC,m_xPos[4],m_yPos[4],m_xPos[5],m_yPos[5],rc,nColor,nDrawMode,PS_SOLID,nWidth);
	g_d.ClipLine(pDC,m_xPos[6],m_yPos[6],m_xPos[7],m_yPos[7],rc,nColor,nDrawMode,PS_SOLID,nWidth);
	//
	if(m_xPos[8]>m_xPos[3])
	{
		g_d.ClipLine(pDC,m_xPos[3],m_yPos[3],m_xPos[8],m_yPos[8],rc,nColor,nDrawMode,PS_DOT,nWidth);
		g_d.ClipLine(pDC,m_xPos[5],m_yPos[5],m_xPos[9],m_yPos[9],rc,nColor,nDrawMode,PS_DOT,nWidth);
		g_d.ClipLine(pDC,m_xPos[7],m_yPos[7],m_xPos[10],m_yPos[10],rc,nColor,nDrawMode,PS_DOT,nWidth);
	}
	//
	if(bCover)
	{
		g_d.ClipLine(pDC,m_xPos[0],rc.top,m_xPos[0],rc.bottom,rc,nColor,nDrawMode,PS_SOLID,nWidth);
		g_d.ClipLine(pDC,m_xPos[1],rc.top,m_xPos[1],rc.bottom,rc,nColor,nDrawMode,PS_SOLID,nWidth);
	}
	//
	if(bSel)
	{
		DrawBar(pDC,m_xPos[2],m_yPos[2],rc);
		DrawBar(pDC,m_xPos[3],m_yPos[3],rc);
	}
	return TRUE;
}

long DDrawLineTool::SelLinearRay(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(!CalcLinearRay(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return LINESEL_SELNONE;
	if((abs(point.x-m_xPos[2])<SELECT_ON && abs(point.y-m_yPos[2])<SELECT_ON)
		|| (abs(point.x-m_xPos[3])<SELECT_ON && abs(point.y-m_yPos[3])<SELECT_ON))
	{
		if(abs(point.x-m_xPos[0])<SELECT_ON) return 0;
		else return 1;
	}
	else if(point.x>min(m_xPos[2],m_xPos[3])-SELECT_ON
		&& (PointToLine(point.x,point.y,m_xPos[2],m_yPos[2],m_xPos[8],m_yPos[8])<SELECT_ON
		|| PointToLine(point.x,point.y,m_xPos[4],m_yPos[4],m_xPos[9],m_yPos[9])<SELECT_ON
		|| PointToLine(point.x,point.y,m_xPos[6],m_yPos[6],m_xPos[10],m_yPos[10])<SELECT_ON))
		return LINESEL_SELNOTPOINT;
	return LINESEL_SELNONE;
}

BOOL DDrawLineTool::CalcLinear(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(newnum<2) return FALSE;
	m_xPos[0] = newpoint[0].x;	m_yPos[0] = newpoint[0].y;
	m_xPos[1] = newpoint[1].x;	m_yPos[1] = newpoint[1].y;
	m_xPos[2] = newpoint[2].x;	m_yPos[2] = newpoint[2].y;
	m_xPos[3] = newpoint[3].x;	m_yPos[3] = newpoint[3].y;
	return TRUE;
}

BOOL DDrawLineTool::DrawLinear(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist)
{
	if(!CalcLinear(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return FALSE;
	//
	COLORREF nColor = VipColor.DRAWLINETOOLCOLOR;
	int nStyle = PS_SOLID;
	int nWidth = 1;
	int nDrawMode = R2_COPYPEN;
	INITLINEPROPERTY();
	//
	g_d.ClipLine(pDC,m_xPos[2],m_yPos[2],m_xPos[3],m_yPos[3],rc,nColor,nDrawMode,PS_SOLID,nWidth);
	//
	if(bCover)
	{
		g_d.ClipLine(pDC,m_xPos[2],rc.top,m_xPos[2],rc.bottom,rc,nColor,nDrawMode,PS_SOLID,nWidth);
		g_d.ClipLine(pDC,m_xPos[3],rc.top,m_xPos[3],rc.bottom,rc,nColor,nDrawMode,PS_SOLID,nWidth);
	}
	//
	if(bSel)
	{
		DrawBar(pDC,m_xPos[2],m_yPos[2],rc);
		DrawBar(pDC,m_xPos[3],m_yPos[3],rc);
	}
	return TRUE;
}

long DDrawLineTool::SelLinear(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(!CalcLinear(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return LINESEL_SELNONE;
	if((abs(point.x-m_xPos[2])<SELECT_ON && abs(point.y-m_yPos[2])<SELECT_ON)
		|| (abs(point.x-m_xPos[3])<SELECT_ON && abs(point.y-m_yPos[3])<SELECT_ON))
	{
		if(abs(point.x-m_xPos[0])<SELECT_ON) return 0;
		else return 1;
	}
	else if(point.x>min(m_xPos[2],m_xPos[3])-SELECT_ON && point.x<max(m_xPos[2],m_xPos[3])+SELECT_ON
		&& (PointToLine(point.x,point.y,m_xPos[2],m_yPos[2],m_xPos[3],m_yPos[3])<SELECT_ON))
		return LINESEL_SELNOTPOINT;
	return LINESEL_SELNONE;
}

BOOL DDrawLineTool::CalcHoldFast(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(newnum<2) return FALSE;
	m_xPos[0] = newpoint[0].x;	m_yPos[0] = newpoint[0].y;
	m_xPos[1] = newpoint[1].x;	m_yPos[1] = newpoint[1].y;
	if(m_xPos[0]>=m_xPos[1]) return FALSE;
	//
	m_xPos[2] = m_xPos[1];
	m_yPos[2] = m_yPos[1];
	m_xPos[3] = m_xPos[1];
	m_yPos[3] = (m_yPos[1]-m_yPos[0])*2/3+m_yPos[0];
	m_xPos[4] = m_xPos[1];
	m_yPos[4] = (m_yPos[1]-m_yPos[0])/3+m_yPos[0];
	ExtendLine(m_xPos[0],m_yPos[0],m_xPos[2],m_yPos[2],LINEEXTENDTYPE_ONE,rc,lLeft,lTop);
	ExtendLine(m_xPos[0],m_yPos[0],m_xPos[3],m_yPos[3],LINEEXTENDTYPE_ONE,rc,lLeft,lTop);
	ExtendLine(m_xPos[0],m_yPos[0],m_xPos[4],m_yPos[4],LINEEXTENDTYPE_ONE,rc,lLeft,lTop);
	//
	return TRUE;
}

BOOL DDrawLineTool::DrawHoldFast(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist)
{
	if(!CalcHoldFast(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return FALSE;
	//
	COLORREF nColor = VipColor.DRAWLINETOOLCOLOR;
	int nStyle = PS_SOLID;
	int nWidth = 1;
	int nDrawMode = R2_COPYPEN;
	INITLINEPROPERTY();
	//
	g_d.ClipLine(pDC,m_xPos[0],m_yPos[0],m_xPos[2],m_yPos[2],rc,nColor,nDrawMode,PS_SOLID,nWidth);
	g_d.ClipLine(pDC,m_xPos[0],m_yPos[0],m_xPos[3],m_yPos[3],rc,nColor,nDrawMode,PS_DOT,nWidth);
	g_d.ClipLine(pDC,m_xPos[0],m_yPos[0],m_xPos[4],m_yPos[4],rc,nColor,nDrawMode,PS_DOT,nWidth);
	//
	if(bSel)
	{
		DrawBar(pDC,m_xPos[0],m_yPos[0],rc);
		DrawBar(pDC,m_xPos[1],m_yPos[1],rc);
	}
	return TRUE;
}

long DDrawLineTool::SelHoldFast(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(!CalcHoldFast(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return LINESEL_SELNONE;
	if(abs(point.x-m_xPos[0])<SELECT_ON && abs(point.y-m_yPos[0])<SELECT_ON)
		return 0;
	else if(abs(point.x-m_xPos[1])<SELECT_ON && abs(point.y-m_yPos[1])<SELECT_ON)
		return 1;
	else if(point.x>m_xPos[0]-SELECT_ON && ((m_yPos[1]-m_yPos[0])*(point.y-m_yPos[0]+SELECT_ON)>=0 || (m_yPos[1]-m_yPos[0])*(point.y-m_yPos[0]-SELECT_ON)>=0))
	{
		if(PointToLine(point.x,point.y,m_xPos[0],m_yPos[0],m_xPos[2],m_yPos[2])<SELECT_ON
			|| PointToLine(point.x,point.y,m_xPos[0],m_yPos[0],m_xPos[3],m_yPos[3])<SELECT_ON
			|| PointToLine(point.x,point.y,m_xPos[0],m_yPos[0],m_xPos[4],m_yPos[4])<SELECT_ON)
			return LINESEL_SELNOTPOINT;
	}
	return LINESEL_SELNONE;
}

BOOL DDrawLineTool::CalcAngle(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(newnum<2) return FALSE;
	m_xPos[0] = newpoint[0].x;	m_yPos[0] = newpoint[0].y;
	m_xPos[1] = newpoint[1].x;	m_yPos[1] = newpoint[1].y;
	if(m_xPos[0]>=m_xPos[1]) return FALSE;
	long xRange = m_xPos[1]-m_xPos[0];
	long yRange = m_yPos[1]-m_yPos[0];
	int i=0;										//序号按照逆时针顺序依次增加
	m_xPos[2] = m_xPos[3] = m_xPos[4] = m_xPos[5] = m_xPos[6] = m_xPos[1];
	for(i=7;i<11;i++)	m_xPos[i] = m_xPos[0]+xRange/xGannRate[i];
	for(i=2;i<6;i++)	m_yPos[i] = m_yPos[0]+yRange/yGannRate[i];
	m_yPos[6] = m_yPos[7] = m_yPos[8] = m_yPos[9] = m_yPos[10] = m_yPos[1];
	for(i=2;i<11;i++)
		ExtendLine(m_xPos[0],m_yPos[0],m_xPos[i],m_yPos[i],LINEEXTENDTYPE_ONE,rc,lLeft,lTop);
	return TRUE;
}

BOOL DDrawLineTool::DrawAngle(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist)
{
	if(!CalcAngle(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return FALSE;
	CSize size=pDC->GetTextExtent("00O");
	long xRange = m_xPos[1]-m_xPos[0];
	long yRange = m_yPos[1]-m_yPos[0];
	long i = 0;
	//
	COLORREF nColor = VipColor.DRAWLINETOOLCOLOR;
	int nStyle = PS_SOLID;
	int nWidth = 1;
	int nDrawMode = R2_COPYPEN;
	INITLINEPROPERTY();
	//
	CFont  *pOldfont = NULL;
	pOldfont = pDC->SelectObject(&m_DrawTextFont);
	//
	long TxtXPos = 0;
	long TxtYPos = 0;
	for(i=2;i<11;i++)
	{
		if ( i == 6 ) g_d.ClipLine(pDC,m_xPos[0],m_yPos[0],m_xPos[i],m_yPos[i],rc,nColor,nDrawMode,PS_SOLID,nWidth);
		else g_d.ClipLine(pDC,m_xPos[0],m_yPos[0],m_xPos[i],m_yPos[i],rc,nColor,nDrawMode,PS_DOT,nWidth);
		if(!bCover)
		{
			if(yRange<0)
			{
				TxtXPos = m_xPos[0]+xRange/xGannRate[i];
				TxtYPos = m_yPos[0]+yRange/yGannRate[i];
			}
			else if(yRange>0)
			{
				TxtXPos = m_xPos[0]+xRange/xGannRate[i];
				TxtYPos = m_yPos[0]+yRange/yGannRate[i]-size.cy-2;
			}
			else continue;
			if(TxtXPos>rc.left && TxtXPos+size.cx<rc.right && TxtYPos>rc.top && TxtYPos+size.cy<rc.bottom)
				g_d.DisplayText(pDC,TxtXPos,TxtYPos,nColor,0,"%d:%d",yGannRate[i],xGannRate[i]);
		}
	}
	g_d.ClipLine(pDC,m_xPos[0],m_yPos[0],rc.right,m_yPos[0],rc,nColor,nDrawMode,PS_SOLID,nWidth);
	if(yRange>0) g_d.ClipLine(pDC,m_xPos[0],m_yPos[0],m_xPos[0],rc.bottom,rc,nColor,nDrawMode,PS_SOLID,nWidth);
	else if(yRange<0) g_d.ClipLine(pDC,m_xPos[0],m_yPos[0],m_xPos[0],rc.top,rc,nColor,nDrawMode,PS_SOLID,nWidth);
	//
	if(bSel)
	{
		DrawBar(pDC,m_xPos[0],m_yPos[0],rc);
		DrawBar(pDC,m_xPos[1],m_yPos[1],rc);
	}
	pDC->SelectObject(pOldfont);
	return TRUE;
}

long DDrawLineTool::SelAngle(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(!CalcAngle(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return LINESEL_SELNONE;
	if(abs(point.x-m_xPos[0])<SELECT_ON && abs(point.y-m_yPos[0])<SELECT_ON)
		return 0;
	else if(abs(point.x-m_xPos[1])<SELECT_ON && abs(point.y-m_yPos[1])<SELECT_ON)
		return 1;
	else if(point.x>m_xPos[0]-SELECT_ON && ((m_yPos[1]-m_yPos[0])*(point.y-m_yPos[0]+SELECT_ON)>=0 || (m_yPos[1]-m_yPos[0])*(point.y-m_yPos[0]-SELECT_ON)>=0))
	{
		for(int i=2;i<11;i++)
		{
			if(PointToLine(point.x,point.y,m_xPos[0],m_yPos[0],m_xPos[i],m_yPos[i])<SELECT_ON)
				return LINESEL_SELNOTPOINT;
		}
		if(PointToLine(point.x,point.y,m_xPos[0],m_yPos[0],rc.right,m_yPos[0])<SELECT_ON
			|| (m_yPos[1]>m_yPos[0] && PointToLine(point.x,point.y,m_xPos[0],m_yPos[0],m_xPos[0],rc.bottom)<SELECT_ON)
			|| (m_yPos[1]<m_yPos[0] && PointToLine(point.x,point.y,m_xPos[0],m_yPos[0],m_xPos[0],rc.top)<SELECT_ON))
			return LINESEL_SELNOTPOINT;
	}
	return LINESEL_SELNONE;
}

BOOL DDrawLineTool::CalcRise(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(newnum<2) return FALSE;
	m_xPos[0] = newpoint[0].x;	m_yPos[0] = newpoint[0].y;
	m_xPos[1] = newpoint[1].x;	m_yPos[1] = newpoint[1].y;
	return TRUE;
}

BOOL DDrawLineTool::DrawRise(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist)
{
	if(!CalcRise(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return FALSE;
	if(bSel) pDC->DrawIcon(m_xPos[0]-SPACE_ICONOFFSET,m_yPos[0],m_hExpUpShow_Sel);
	else pDC->DrawIcon(m_xPos[0]-SPACE_ICONOFFSET,m_yPos[0],m_hExpUpShow);
	return TRUE;
}

long DDrawLineTool::SelRise(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(!CalcRise(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return LINESEL_SELNONE;
	if(abs(point.x-m_xPos[0])<SELECT_ON*2 && abs(point.y-(m_yPos[0]+SPACE_ICONOFFSET/2))<SELECT_ON*2)
		return LINESEL_SELNOTPOINT;
	return LINESEL_SELNONE;
}

BOOL DDrawLineTool::CalcDive(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(newnum<2) return FALSE;
	m_xPos[0] = newpoint[0].x;	m_yPos[0] = newpoint[0].y;
	m_xPos[1] = newpoint[1].x;	m_yPos[1] = newpoint[1].y;
	return TRUE;
}

BOOL DDrawLineTool::DrawDive(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist)
{
	if(!CalcDive(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return FALSE;
	if(bSel) pDC->DrawIcon(m_xPos[0]-SPACE_ICONOFFSET,m_yPos[0]-SPACE_ICONOFFSET*2,m_hExpDownShow_Sel);
	else pDC->DrawIcon(m_xPos[0]-SPACE_ICONOFFSET,m_yPos[0]-SPACE_ICONOFFSET*2,m_hExpDownShow);
	return TRUE;
}

long DDrawLineTool::SelDive(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(!CalcDive(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return LINESEL_SELNONE;
	if(abs(point.x-m_xPos[0])<SELECT_ON*2 && abs(point.y-(m_yPos[0]-SPACE_ICONOFFSET/2))<SELECT_ON*2)
		return LINESEL_SELNOTPOINT;
	return LINESEL_SELNONE;
}

BOOL DDrawLineTool::CalcText(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(newnum<1) return FALSE;
	m_xPos[0] = newpoint[0].x;	m_yPos[0] = newpoint[0].y;
	m_xPos[1] = newpoint[1].x;	m_yPos[1] = newpoint[1].y;
	return TRUE;
}

BOOL DDrawLineTool::DrawText(CDC *pDC,LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CRect rc,long lLeft,long lTop,BOOL bCover,BOOL bSel,void *pAssist,long lAssist)
{
	if(!CalcText(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return FALSE;
	
	CFont  *pOldfont = NULL;
	CFont	DrawTextFont;
	int nFontSize = -1*(pDrawLine->FontSize+6);
	if(nFontSize<-56 || nFontSize>-7) nFontSize=-14;
	DrawTextFont.CreateFont(nFontSize,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,g_strFontName[1]);
	pOldfont = pDC->SelectObject(&DrawTextFont);
	//解决出界问题
	int nBkMode = pDC->SetBkMode(TRANSPARENT);
	CSize TxtSize = pDC->GetTextExtent(pDrawLine->szBuffer);
	CDC       MemDC;
	CBitmap   BitMap,*OldBit;
	CRect	  MemRc;
	MemDC.CreateCompatibleDC(pDC);
	MemRc.left = m_xPos[0]-1;
	MemRc.right = MemRc.left+TxtSize.cx+2;
	MemRc.top = m_yPos[0]-1;
	MemRc.bottom = MemRc.top+TxtSize.cy+2;
	BitMap.CreateCompatibleBitmap(pDC,TxtSize.cx+2,TxtSize.cy+2);
	OldBit = (CBitmap* )MemDC.SelectObject(&BitMap);
	MemDC.FillSolidRect(&MemRc,VipColor.BACKCOLOR);
	MemDC.SelectObject(&DrawTextFont);
	MemDC.SetBkMode(TRANSPARENT);
	MemDC.BitBlt(0,0,TxtSize.cx+2,TxtSize.cy+2,pDC,m_xPos[0]-1,m_yPos[0]-1,SRCCOPY);
	//
	switch(pDrawLine->tversion)
	{
	case 0:
		pDrawLine->tcrf = VipColor.TXTCOLOR;
		break;
	default:
		break;
	}
	CSize size = g_d.DisplayText(&MemDC,0,0,pDrawLine->tcrf,"%s",pDrawLine->szBuffer);
	//保存长宽
	pDrawLine->width = size.cx;
	pDrawLine->height = size.cy;
	if(bSel)
	{
		CRect tmprc(0,0,size.cx,size.cy);
		g_d.DrawRect(&MemDC,tmprc,VipColor.FLOATCURSORCOLOR);
	}
	if(m_xPos[0]-1+TxtSize.cx+2 > rc.right)
		TxtSize.cx = rc.right-m_xPos[0]-1;
	if(m_yPos[0]-1+TxtSize.cy+2 > rc.bottom)
		TxtSize.cy = rc.bottom-m_yPos[0]-1;
	//
	pDC->BitBlt(m_xPos[0]-1,m_yPos[0]-1,TxtSize.cx+2,TxtSize.cy+2,&MemDC,0,0,SRCCOPY);
	MemDC.SelectObject(OldBit);
	BitMap.DeleteObject();
	MemDC.DeleteDC();
	//
	pDC->SelectObject(pOldfont);
	DrawTextFont.DeleteObject();
	pDC->SetBkMode(nBkMode);

	return TRUE;
}

long DDrawLineTool::SelText(LPDRAWLINE pDrawLine,CPoint *newpoint,long newnum,CPoint point,CRect rc,long lLeft,long lTop,void *pAssist,long lAssist)
{
	if(!CalcText(pDrawLine,newpoint,newnum,rc,lLeft,lTop,pAssist,lAssist)) return LINESEL_SELNONE;
	if(point.x>m_xPos[0]-SELECT_ON && point.x<m_xPos[0]+pDrawLine->width+SELECT_ON
		&& point.y>m_yPos[0]-SELECT_ON && point.y<m_yPos[0]+pDrawLine->height+SELECT_ON)
		return LINESEL_SELNOTPOINT;
	return LINESEL_SELNONE;
}

BOOL DDrawLineTool::ExtendLine(long &xPos1,long &yPos1,long &xPos2,long &yPos2,long nExtendType,CRect rc,long lLeft,long lTop)
{
	long ySwap;
	if(nExtendType==LINEEXTENDTYPE_NOEXTEND) return TRUE;
	if(nExtendType<LINEEXTENDTYPE_NOEXTEND || nExtendType>LINEEXTENDTYPE_BOTH) return FALSE;
	long xAim1 = rc.left;
	long xAim2 = rc.right;
	if(xPos1>xPos2)
	{
		xAim1 = rc.right;
		xAim2 = rc.left;
	}
	if(xPos1==xPos2)
	{
		if(nExtendType==LINEEXTENDTYPE_ONE)
		{
			if(yPos1>yPos2) yPos2 = rc.top;
			else yPos2 = rc.bottom;
			return TRUE;
		}
		yPos1 = rc.top;
		yPos2 = rc.bottom;
	}
	else
	{
		ySwap = yPos2;
		yPos2 = yPos2+(yPos2-yPos1)*(xAim2-xPos2)/(xPos2-xPos1);
		if(nExtendType==LINEEXTENDTYPE_BOTH) yPos1 = yPos1+(ySwap-yPos1)*(xAim1-xPos1)/(xPos2-xPos1);
		xPos2 = xAim2;
		if(nExtendType==LINEEXTENDTYPE_BOTH) xPos1 = xAim1;
	}
	return TRUE;
}

void DDrawLineTool::DrawBar(CDC *pDC,long xp,long yp,CRect rc)
{
	if(xp<rc.left+BARSIZE_SELFLINE || xp>rc.right-BARSIZE_SELFLINE) return;
	if(yp<rc.top+BARSIZE_SELFLINE || yp>rc.bottom-BARSIZE_SELFLINE) return;
	g_d.Bar(pDC,xp-BARSIZE_SELFLINE,yp-BARSIZE_SELFLINE,xp+BARSIZE_SELFLINE,yp+BARSIZE_SELFLINE,LEVEL_BRUSH);
}
