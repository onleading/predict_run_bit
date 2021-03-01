/*
* Copyright (c) 2002,通达信开发部
* All rights reserved.
* 
* 文件名称：UCb.cpp
* 文件标识：
* 摘    要：完成移动筹码、火焰山功能.
*
*/
#include "stdafx.h"
#include "UCb.h"
#include "SetCBDlg.h"

#include <vector>

static	COLORREF	OutColor[MAX_MON]={RGB(255,0,0),RGB(255,128,128),RGB(255,0,128),RGB(255,128,0),RGB(255,255,128),RGB(255,255,0)};
static	COLORREF	InColor[MAX_MON]={RGB(0,0,128),RGB(0,40,160),RGB(80,80,255),RGB(120,120,255),RGB(160,160,255),RGB(120,80,255)};

UCb::UCb(LPCOMINFO pComInfo):UBase(pComInfo)
{	
	m_nAmple	= 100;
	m_fCBPrice	= 0;
	m_bValid	= 0;
	m_dPRatio	= 0;
	m_nCurr		= 0;
	m_pData		= NULL;
	m_nDataNum	= 0;
	m_nMinPrice	= 0;
	m_nMaxPrice	= 0;
	m_dMaxPrice	= 0;
	m_dMinPrice	= 0;
	m_nPGap		= 1;
	m_FxBottom	= 0;
	m_FxTop		= 0;
	m_dMaxVol	= 0;
	m_nDotSpan	= 1;
	if(g_nCBSwitch < 0 || g_nCBSwitch > 2) g_nCBSwitch = 1;
	m_nSwitch	= g_nCBSwitch;
	m_dRatio	= 0;

	memset(m_anCache,0,sizeof(m_anCache));
	memset(&m_nGivePt,0,sizeof(POINT));
	memset(&m_Setup,0,sizeof(m_Setup));
	memset(&m_DispInfo,0,sizeof(m_DispInfo));
	memset(m_pOut,0,sizeof(POINTCOST*));
	memset(m_pIn,0,sizeof(POINTCOST*));
	

	for ( int i=0;i<CHNUM;++i )
	{
		m_anCache[i] = 0;
		m_aVolCache[i] = new double[NPRICE];
		memset(m_aVolCache[i],0,NPRICE*sizeof(double));
	}

	m_aVolNow	 = new double[NPRICE];
	m_aDrawVol	 = new double[NPRICE];
	memset(m_aVolNow,0,NPRICE*sizeof(double));
	memset(m_aDrawVol,0,NPRICE*sizeof(double));

	m_fLTGB = new float[3000];
	memset(m_fLTGB,0,3000*sizeof(float));

	m_Setup.fWeakK = 1.0;
	LoadSetup();	
}

UCb::~UCb()
{
	g_nCBSwitch = m_nSwitch;

	DeleteIt();	

	SaveSetup();
}

void	UCb::LoadSetup()
{
	CString	FileName = g_WSXHStr + "cbset.dat";
	CFile	f;
	if ( f.Open(FileName,CFile::modeRead) )
	{
		f.Read(&m_Setup,sizeof(m_Setup));
		f.Close();
	}
	else
	{
		m_Setup.Way = TRIANGLE;
		m_Setup.nSkip[0] = 6;
		m_Setup.nSkip[1] = 6;
		m_Setup.aN[0] = 10;	m_Setup.aN[1] = 5;
		m_Setup.aN[2] = 20;	m_Setup.aN[3] = 30;
		m_Setup.aN[4] = 60;	m_Setup.aN[5] = 100;
		memcpy(m_Setup.aN+6,m_Setup.aN,6*sizeof(short));
		m_Setup.fWeakK = 1;
		m_Setup.IsPrecious = 1;
	}

	for ( int i=0;i<MAX_MON;++i )
	{
		m_aOutCost[i].N = m_Setup.aN[i];
		m_aInCost[i].N  = m_Setup.aN[i+MAX_MON];
	}
	SortIt();
}

inline	void	UCb::SortIt()
{
	int	i,j;
	POINTCOST	*	t;
	for ( i=0;i<MAX_MON;++i )
	{
		m_pOut[i] = m_aOutCost+i;
		m_pIn[i]  = m_aInCost+i;
	}
	//如果是0就排在最后
	for ( i=0;i<MAX_MON;++i )
	{
		for ( j=i+1;j<MAX_MON;++j )
		{
			if ( m_pOut[i]->N > m_pOut[j]->N || m_pOut[i]->N ==0 )
			{
				t = m_pOut[i];
				m_pOut[i] = m_pOut[j];
				m_pOut[j] = t;
			}
			if ( m_pIn[i]->N < m_pIn[j]->N  || m_pIn[i]->N == 0 )
			{
				t = m_pIn[i];
				m_pIn[i] = m_pIn[j];
				m_pIn[j] = t;
			}
		}
	}
}

void	UCb::SaveSetup()
{
	return;
	CFile	f;

	CString	FileName = g_WSXHStr + "cbset.dat";
	
	if ( f.Open(FileName,CFile::modeCreate|CFile::modeWrite) )
	{
		f.Write(&m_Setup,sizeof(m_Setup));
		f.Close();
	}
}

void	UCb::DeleteIt()
{
	int		i;
	delete [] m_aDrawVol;
	delete [] m_aVolNow;
	for ( i=0;i < MAX_MON;++i )
	{
		m_aInCost[i].Reset();
		m_aOutCost[i].Reset();
	}
	for ( i=0;i<CHNUM;++i )
	{
		delete [] m_aVolCache[i];
	}
	delete [] m_fLTGB;
}

int	 UCb::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	int		i;
	if(m_nCurGP==-1) //如果Unit为空，则另行处理
	{
		if(message==WM_PAINT)
		{
			CREATE_TMPDC
			g_d.Bar(&m_MyDC,&m_rcIn,BACK_BRUSH);
			pDC->BitBlt (left,top,Width(),Height(),&m_MyDC,0,0,SRCCOPY); //从内存DC拷贝到屏幕DC
			DESTORY_TMPDC
		}
		return UBase::ProcessMsg(message,wParam,lParam);
	}
	switch(message)
	{
	case WM_SIZE:
		m_aRect[3] = CRect(g_nFixedWidth-36,2,g_nFixedWidth-20,18);
		m_aRect[0] = m_aRect[3];
		m_aRect[1] = m_aRect[3];
		m_aRect[2] = m_aRect[3];
		m_aRect[0].OffsetRect(-54,0);
		m_aRect[1].OffsetRect(-36,0);
		m_aRect[2].OffsetRect(-18,0);
		break;
	case UM_READDATA:
		ProcessMsg(UM_CB_FXTEND); 
		return 1;
	case WM_PAINT:
		{
			CREATE_TMPDC
			g_d.Bar(&m_MyDC,&m_rcIn,BACK_BRUSH);
			m_MyDC.SetBkMode(TRANSPARENT);
			CFont *oldfont = g_d.SelectFont(&m_MyDC,GRAPH_FONT);
			CPen *oldpen = g_d.SelectPen(&m_MyDC,AXIS_PEN);
			g_d.DrawLine(&m_MyDC,0,0,0,m_rcIn.bottom);
			if(g_GH.bLeftHQInfo && !m_nInCom2)
				g_d.DrawLine(&m_MyDC,m_rcIn.right-1,0,m_rcIn.right-1,m_rcIn.bottom);
			g_d.DrawLine(&m_MyDC,0,m_rcIn.bottom,m_rcIn.right,m_rcIn.bottom);
			if(m_bValid && m_pStkInfo)
			{
				if(m_nDataNum>0 && m_pData)
					DrawEx(&m_MyDC);			
				g_d.DisplayText(&m_MyDC,2,2,VipColor.BIGTITLECOLOR,"%6s",m_pStkInfo->Code);
				for(int i=0;i<4;++i)
				{
					m_MyDC.DrawIcon(m_aRect[i].left,m_aRect[i].top,m_hIcon_Pu[i]);
					if ( i == m_nSwitch )
						g_d.DrawRect(&m_MyDC,&m_aRect[i],VipColor.KPCOLOR);
				}
			}
			g_d.RestorePen(&m_MyDC,oldpen);
			g_d.RestoreFont(&m_MyDC,oldfont);			
			pDC->BitBlt (left,top,Width(),Height(),&m_MyDC,0,0,SRCCOPY); //从内存DC拷贝到屏幕DC
			DESTORY_TMPDC
		}
		return 1;
	case WM_LBUTTONDOWN:
		{
			int		i;
			CPoint pt(LOWORD(lParam),HIWORD(lParam));	
			pt.x -= left;	pt.y -= top;
			if ( m_aRect[3].PtInRect(pt) )
			{
				CSetCBDlg	One;
				if ( One.DoModal() == IDOK )
				{
					m_Setup = One.m_Setup;
					for ( i=0;i<MAX_MON;++i )
					{
						m_aOutCost[i].N = m_Setup.aN[i];
						m_aInCost[i].N  = m_Setup.aN[i+MAX_MON];
					}
					SortIt();
					MainCalc();
					ProcessMsg(WM_PAINT);
				}
				break;
			}
			for ( i=0;i<3;++i )
				if ( m_aRect[i].PtInRect(pt) )
				{
					m_nSwitch = i;
					break;
				}
			if ( i==3 )
				break;
			BeforeCalc();
			MainCalc();
			ProcessMsg(WM_PAINT);
		}
		break;
	case UM_CB_FXTSTART:	//启动移动筹码分布
		memset(m_anCache,0,sizeof(m_anCache));
		for ( i=0;i<CHNUM;++i )
		{
			memset(m_aVolCache[i],0,NPRICE*sizeof(double));
		}
		m_pStkInfo	= g_CbInfo.pStk;
		m_pData   = g_CbInfo.m_pData;
		m_nDataNum= g_CbInfo.m_nDataNum;
		if ( m_nDataNum<=0 || !m_pData )
		{
			m_bValid = FALSE;
			break;
		}
		m_bValid = TRUE;
		///////////////////////////////
		m_nCurr  = g_CbInfo.m_nCurr;
		m_FxTop = g_CbInfo.m_nTop;
		m_FxBottom = g_CbInfo.m_nBottom;
		m_nGivePt.y = g_CbInfo.m_nY;
		m_nGivePt.x = g_CbInfo.m_nYC;
		m_DispInfo.fPrice  = g_CbInfo.m_fNowVal;
		m_dMaxPrice = g_CbInfo.m_dMaxPrice;
		m_dMinPrice = g_CbInfo.m_dMinPrice;
		///////////////////////////////
		ReadData();		//读取流通股本。
		Reset();		//复位、计算基本数据。
		MainCalc();
		ProcessMsg(WM_PAINT);
		break;
	case UM_CB_FXTEND:
		m_bValid = FALSE;
		break;
	case UM_CB_DATE:	//中间的更换过程
		if ( m_bValid )
		{
			if ( m_pStkInfo->setcode == g_CbInfo.pStk->setcode 
				&& stricmp(m_pStkInfo->Code,g_CbInfo.pStk->Code)==0
				&& m_nDataNum == g_CbInfo.m_nDataNum  
				&& m_FxTop == g_CbInfo.m_nTop && m_FxBottom == g_CbInfo.m_nBottom
				&& m_nCurr == g_CbInfo.m_nCurr 
				&& fabs(m_dMaxPrice-g_CbInfo.m_dMaxPrice)<COMPPREC 
				&& fabs(m_dMinPrice-g_CbInfo.m_dMinPrice)<COMPPREC  )
			{	//上下移动，无需大面积计算!还要考虑最值情况!
				m_pData   = g_CbInfo.m_pData;
				m_nGivePt.y = g_CbInfo.m_nY;
				m_nGivePt.x = g_CbInfo.m_nYC;
				m_DispInfo.fPrice = g_CbInfo.m_fNowVal;
				ProcessMsg(WM_PAINT);
				break;
			}

			m_pStkInfo	= g_CbInfo.pStk;
			m_nDataNum= g_CbInfo.m_nDataNum;
			m_nCurr  = g_CbInfo.m_nCurr;
			m_FxTop = g_CbInfo.m_nTop;
			m_FxBottom = g_CbInfo.m_nBottom;

			m_pData   = g_CbInfo.m_pData;
			m_nGivePt.y = g_CbInfo.m_nY;
			m_nGivePt.x = g_CbInfo.m_nYC;
			m_DispInfo.fPrice = g_CbInfo.m_fNowVal;
			m_dMaxPrice = g_CbInfo.m_dMaxPrice;
			m_dMinPrice = g_CbInfo.m_dMinPrice;
			BeforeCalc();
			MainCalc();
			ProcessMsg(WM_PAINT);
		}
		break;
	default:
		break;
	}
	return UBase::ProcessMsg(message,wParam,lParam);
}

void	UCb::DrawEx(CDC * pDC)
{
	if( !m_bValid || m_nCurr <= 0 || m_nCurr>=m_nDataNum)
		return;
	float	fVol;
	short	nOutX;
	long	Y,OldY;
	double	start;
	vector<POINT>	aPoint(400);
	int		nP;
	int		h = m_FxBottom-m_FxTop,	w = Width()-2 ,i;
	pDC->FillSolidRect(CRect(1,m_FxTop,w,h+m_FxTop),VipColor.BACKCOLOR);
	if( m_dMaxVol<COMPPREC || m_fLTGB[m_nCurr]<COMPPREC)
		return;
	//////////////////////////////////
	if ( m_nSwitch != 0 )
	{
		for ( i=0;i<h/m_nDotSpan;++i )
		{
			start = i*m_dPRatio+m_dMinPrice;
			Y = m_FxBottom - m_dRatio*(start-m_dMinPrice);
			fVol  = m_aDrawVol[i];
			if(fVol<COMPPREC) continue;
			nOutX = w*fVol/m_dMaxVol;
			g_d.DrawLine(pDC,0,Y,nOutX,Y,SystemColor.VipColor.ZBCOLOR[0]);
		}
		if ( m_nSwitch == 1 )
		{
			for ( int	Out = 0;Out<MAX_MON; ++Out )
			{
				if ( m_pOut[Out]->N && m_nCurr > m_pOut[Out]->N )
				{
					nP = 0;
					OldY = m_FxBottom+1;
					for ( i=0;i<h/m_nDotSpan;++i )
					{
						start = i*m_dPRatio+m_dMinPrice;
						Y = m_FxBottom - m_dRatio*(start-m_dMinPrice);
						fVol  = m_pOut[Out]->pDrawVol[i];
						if ( fVol < COMPPREC )
						{
							OldY = Y;
							continue;
						}
						nOutX = w*fVol/m_dMaxVol;
						if ( nOutX > 0 )
						{
							if ( nP == 0 )
							{
								aPoint[0].x = 0;
								aPoint[0].y = OldY;
								nP = 1;
							}
							else
							{
								aPoint[nP].x = nOutX;
								aPoint[nP].y = Y;
								++ nP;
								if ( nP >= 400-10 )
									aPoint.resize(aPoint.size()+400);
							}
						}
						OldY = Y;
					}
					OldY = m_FxTop;
					for ( i= h/m_nDotSpan-1;i>=0;--i )
					{
						start = i*m_dPRatio+m_dMinPrice;
						Y = m_FxBottom - m_dRatio*(start-m_dMinPrice);
						fVol  = m_pOut[Out]->pDrawVol[i];
						if ( fVol < COMPPREC )
						{
							OldY = Y;
							continue;
						}
						nOutX = w*fVol/m_dMaxVol;
						if ( nOutX > 0 )
						{
							aPoint[nP].x = 0;
							aPoint[nP].y = OldY;
							++ nP;
							break;
						}
						OldY = Y;
					}
					CRgn	thRgn;
					CBrush	thBr(OutColor[Out]);
					CBrush	thwBr(RGB(255,255,255));
					thRgn.CreatePolygonRgn(aPoint.begin(),nP,WINDING);
					pDC->FillRgn(&thRgn,&thBr);
					pDC->FrameRgn(&thRgn,&thwBr,1,1);
				}
			}
		}
		else if ( m_nSwitch == 2 )
		{
			for ( int	In = 0;In<MAX_MON; ++In )
				if ( m_pIn[In]->N && m_nCurr > m_pIn[In]->N )
				{
					nP = 0;
					OldY = m_FxBottom+1;
					for ( i=0;i<h/m_nDotSpan;++i )
					{
						start = i*m_dPRatio+m_dMinPrice;
						Y = m_FxBottom - m_dRatio*(start-m_dMinPrice);
						fVol  = m_pIn[In]->pDrawVol[i];
						if ( fVol < COMPPREC )
						{
							OldY = Y;
							continue;
						}
						nOutX = w*fVol/m_dMaxVol;
						if ( nOutX > 0 )
						{
							if ( nP == 0 )
							{
								aPoint[0].x = 0;
								aPoint[0].y = OldY;
								nP = 1;
							}
							else
							{
								aPoint[nP].x = nOutX;
								aPoint[nP].y = Y;
								++ nP;
								if ( nP >= 400-10 )
									aPoint.resize(aPoint.size()+400);
							}
						}
						OldY = Y;
					}
					OldY = m_FxTop;
					for ( i=h/m_nDotSpan-1;i>=0;--i )
					{
						start = i*m_dPRatio+m_dMinPrice;
						Y = m_FxBottom - m_dRatio*(start-m_dMinPrice);
						fVol  = m_pIn[In]->pDrawVol[i];
						if ( fVol < COMPPREC )
						{
							OldY = Y;
							continue;
						}
						nOutX = w*fVol/m_dMaxVol;
						if ( nOutX > 0 )
						{
							aPoint[nP].x = 0;
							aPoint[nP].y = OldY;
							++ nP;
							break;
						}
						OldY = Y;
					}
					static POINT	tmpPoint[1000];
					for ( i=0;i<aPoint.size() && i<1000 ;++i )
						tmpPoint[i] = aPoint[i];
					CRgn	thRgn;
					CBrush	thBr(InColor[In]);
					CBrush	thwBr(RGB(255,255,255));
					thRgn.CreatePolygonRgn(aPoint.begin(),nP,WINDING);
					pDC->FillRgn(&thRgn,&thBr);
					pDC->FrameRgn(&thRgn,&thwBr,1,1);
				}
		}
	}
	else
		DrawIt(pDC);
	DispInfo(pDC);
}

inline	void UCb::DispInfo(CDC * pDC)
{
	SIZE	size;
	char	buff[1024];
	short	YOffset= bottom - 100;	
	short	XOffset = 4,i;
	const	int HH = 17;
	RECT	aRc[MAX_MON];
	int		no;	
	for ( i=0;i<MAX_MON;++i )
	{
		aRc[i].left = Width()-130;
		aRc[i].right= Width()-4;
		aRc[i].top  = YOffset - (i+1)*HH;
		aRc[i].bottom=YOffset - i*HH;
		no = MAX_MON-i-1;
		switch ( m_nSwitch )
		{
		case 0:
			break;
		case 1:
			if ( m_pOut[no]->N )
			{
				pDC->FillSolidRect(aRc+i,OutColor[no]);
				sprintf(buff,"%d日前成本%.1f%%",m_pOut[no]->N,m_pOut[no]->dCost*100);
				g_d.DisplayText(pDC,aRc[i].left+6,aRc[i].top,VipColor.BACKCOLOR,0,"%s",_F(buff));
			}
			break;
		case 2:
			if ( m_pIn[i]->N )
			{
				pDC->FillSolidRect(aRc+i,InColor[i]);
				sprintf(buff,"%d日内成本%.1f%%",m_pIn[i]->N,m_pIn[i]->dCost*100);
				g_d.DisplayText(pDC,aRc[i].left+6,aRc[i].top,VipColor.BACKCOLOR,0,"%s",_F(buff));
			}
			break;
		}
	}
	sprintf(buff,"%04d/%02d/%02d",m_pData[m_nCurr].Time.Date/10000,
			(m_pData[m_nCurr].Time.Date%10000)/100,
			(m_pData[m_nCurr].Time.Date%10000)%100);
	size = g_d.DisplayText(pDC,XOffset,YOffset,VipColor.TXTCOLOR,0,_F("成本分布,日期: "));
	g_d.DisplayText(pDC,XOffset+size.cx+6,YOffset,VipColor.TXTCOLOR,0,"%s",_F(buff));

	YOffset += HH;
	size = g_d.DisplayText(pDC,XOffset,YOffset,VipColor.TXTCOLOR,0,_F("获利比例:"));
	g_d.DrawRect(pDC,XOffset+size.cx+6,YOffset,Width()-8,YOffset+16,
		SystemColor.VipColor.ZBCOLOR[0],PS_SOLID,1);
	g_d.DrawRect(pDC,XOffset+size.cx+6,YOffset,
		m_DispInfo.dGotRate*(Width()-8-(XOffset+size.cx+6))+(XOffset+size.cx+6),
		YOffset+16,SystemColor.VipColor.ZBCOLOR[1],PS_SOLID,1);
	g_d.DisplayText(pDC,(XOffset+size.cx+6+Width()-8)/2-14,
		YOffset,VipColor.TXTCOLOR,0,"%3.1f%%",m_DispInfo.dGotRate*100);

	YOffset += HH;
	size = g_d.DisplayText(pDC,XOffset,YOffset,VipColor.TXTCOLOR,0,_F("%.2f处获利盘:"),g_CbInfo.m_fNowVal);
	g_d.DisplayText(pDC,XOffset+size.cx+6,YOffset,VipColor.TXTCOLOR,0,"%.2f%%",GetHLP(g_CbInfo.m_fNowVal)*100);

	YOffset += HH;
	size = g_d.DisplayText(pDC,XOffset,YOffset,VipColor.TXTCOLOR,0,_F("平均成本:  %.2f元"),m_fCBPrice);

	YOffset += HH;
	size = g_d.DisplayText(pDC,XOffset,YOffset,VipColor.TXTCOLOR,0,_F("90%%成本%.2f-%.2f集中%.1f%%"),
		m_DispInfo.p90[0],m_DispInfo.p90[1],m_DispInfo.j90);

	YOffset += HH;
	size = g_d.DisplayText(pDC,XOffset,YOffset,VipColor.TXTCOLOR,0,_F("70%%成本%.2f-%.2f集中%.1f%%"),
		m_DispInfo.p70[0],m_DispInfo.p70[1],m_DispInfo.j70);
}

inline	void UCb::DrawIt(CDC * pDC)
{
	if(Height() < 20) 
		return;	//如果空间太小，不画
	if( !m_bValid || m_nCurr <= 0 )
		return;
	float	fVol;
	short	nOutX;
	long	Y;
	double	start;
	int		h = m_FxBottom-m_FxTop,	w = Width()-2 ,i;
	//////////////////////////////////

	for ( i=0;i<h/m_nDotSpan;++i )
	{
		start = i*m_dPRatio+m_dMinPrice;
		Y = m_FxBottom - m_dRatio*(start-m_dMinPrice);
		fVol  = m_aDrawVol[i];
		if(fVol<COMPPREC) continue;
		nOutX = w*fVol/m_dMaxVol;
		if ( Y > m_nGivePt.x )
			g_d.DrawLine(pDC,0,Y,nOutX,Y,SystemColor.VipColor.ZBCOLOR[1]);
		else
			g_d.DrawLine(pDC,0,Y,nOutX,Y,SystemColor.VipColor.ZBCOLOR[0]);
	}
	g_d.DrawLine(pDC,0,m_nGivePt.y-top,w,m_nGivePt.y-top,VipColor.LEVELCOLOR);
}

extern void GetzsGb_Ltgb(short domain,float& zsTotalGb,float& zsTotalLtgb,float& zsTotalSz,float& zsTotalLtSz,float& zsSyl,float& zsSjl,float& zsYieldRate);

void	UCb::ReadData()
{
	if ( m_nDataNum > 3000 )
	{
		delete [] m_fLTGB;
		m_fLTGB = new float[m_nDataNum];
	}

	if(testzs(m_pStkInfo) )		//指数
	{
		float zsTotalGb, zsTotalLtgb, zsTotalSz ,zsTotalLtSz ,zsSyl,zsSjl,zsYieldRate;
		GetzsGb_Ltgb(m_Domain, zsTotalGb, zsTotalLtgb, zsTotalSz, zsTotalLtSz, zsSyl,zsSjl,zsYieldRate);
		for(int i=0;i<m_nDataNum;i++)
			m_fLTGB[i] = zsTotalLtgb;
		return;
	}

	g_pStockIO->ReadLTGB(m_pStkInfo,m_pData,m_fLTGB,m_nDataNum,m_pStkInfo->ActiveCapital);
}

void	UCb::BeforeCalc()
{
	int	i;
	for ( i=0;i<MAX_MON;++i )
	{
		m_aInCost[i].ResetDraw();
		m_aOutCost[i].ResetDraw();
	}
	memset(m_aDrawVol,0,NPRICE*sizeof(double));
}

void	UCb::Reset()
{//因为股票变化等,起始点变化，全部作废。
 //得到全部历史的最值
	long i =0;
	m_nPGap = 1;
	m_nDotSpan = 1;
	m_nAmple	=	100;
	m_nMinPrice	=	0;		//考虑价格间距
	m_nMaxPrice =   0;
	double	dVal[2],tVal;
	dVal[0]=dVal[1]=0.0;
	long	nNewSize = (m_FxBottom - m_FxTop)/m_nDotSpan;
	//全部数据计算m_nMinPrice
	for ( i=0;i<m_nDataNum; ++i )
	{
		tVal = (m_pData[i].High*m_nAmple+0.5)/m_nPGap;
		if ( m_nMaxPrice < tVal )
		{
			dVal[1] = tVal;
			m_nMaxPrice = tVal;
		}
	}
	///////////////////////////////////
	i = m_nMaxPrice - m_nMinPrice + 1;
	if ( nNewSize > i )
	{
		m_nAmple = 1000;
		m_nMinPrice = dVal[0]*10;
		m_nMaxPrice = dVal[1]*10;
		i = m_nMaxPrice - m_nMinPrice + 1;
	}
	//如果数据量太大，简化计算。
	if ( i > NPRICE )
	{
		m_nPGap = (i+NPRICE-1)/NPRICE;
		m_nMinPrice	= m_nMinPrice/m_nPGap;	//考虑价格间距
		m_nMaxPrice = m_nMaxPrice/m_nPGap;
		i = m_nMaxPrice - m_nMinPrice + 1;
	}
	
	while ( nNewSize > NPRICE )
	{
		++m_nDotSpan;
		nNewSize = (m_FxBottom - m_FxTop)/m_nDotSpan;
	}
	for ( i=0;i<CHNUM;++i )
	{
		memset(m_aVolCache[i],0,NPRICE*sizeof(double));
	}
	memset(m_aVolNow,0,NPRICE*sizeof(double));
	memset(m_anCache,0,sizeof(m_anCache));
	for ( i=0;i<MAX_MON;++i )
	{
		m_aInCost[i].Reset();
		m_aOutCost[i].Reset();
	}
}

void	UCb::CalcEach()
{
	int			i,j,nMaxBack=0,n;
	long		nLow,nHigh;
	double		dSHSL = 1,dHSL;
	for ( i=0;i<MAX_MON;++i )
	{
		m_aOutCost[i].ResetDraw();
		m_aInCost[i].ResetDraw();
		if ( nMaxBack < m_aOutCost[i].N )
			nMaxBack = m_aOutCost[i].N;
		if ( nMaxBack < m_aInCost[i].N )
			nMaxBack = m_aInCost[i].N;
		if ( m_nCurr - m_aOutCost[i].N > 0 ) 
			memcpy(m_aOutCost[i].pTabVol,m_aVolNow,NPRICE*sizeof(double));
	}
	//防止第一个的流通盘没有
	n = 0;
	for ( i=m_nCurr;i>m_nCurr-nMaxBack && i>0 ;--i ,++n)
	{
		nLow = m_pData[i].Low*m_nAmple/m_nPGap - m_nMinPrice;
		nHigh= m_pData[i].High*m_nAmple/m_nPGap- m_nMinPrice;
		for ( j=0;j<MAX_MON;++j )
		{
			if ( m_nCurr - m_aOutCost[j].N > 0  && m_aOutCost[j].N > n ) 
			{
				SubPerVol(m_aOutCost[j].pTabVol,m_pData[i].Volume*dSHSL,nHigh,nLow);
			}
			if ( m_nCurr - m_aInCost[j].N > 0 && m_aInCost[j].N > n ) 
			{
				GetPerVol(m_aInCost[j].pTabVol,m_pData[i].Volume*dSHSL,nHigh,nLow);
			}
		}
		dHSL = GetHSL(i);
		dSHSL *= (1-dHSL);
	}
}

inline void UCb::SubPerVol(double * pVol,double dVol,long H,long L)
{
	if(H<0 || L<0 || H>=NPRICE || L>=NPRICE) return;

	int			i;
	double		dPerVol = 0;
	switch ( m_Setup.Way )
	{
	case AVERAGE:
		dPerVol	= dVol/(H-L+1);
		//成千上万个中的一个区间段。
		for ( i=L;i<=H; ++i )
			pVol[i] -= dPerVol;
		break;
	case TRIANGLE:
		{
			long	half = (H+L)/2;
			if ( half == H || half == L )
				pVol[half] = max(0,pVol[half]-dVol);
			else
			{
				double	vh = dVol/(half-L);
				for ( i=L;i<half;++i )
				{
					pVol[i] = max(0,pVol[i]-(i-L)*(vh/(half-L)));
				}
				for ( i;i<=H;++i )
				{
					pVol[i] = max(0,pVol[i]-(i-H)*(vh/(half-H)));
				}
			}
		}
		break;
	case NORMAL:
		break;
	}
	return	;
}

inline void	UCb::GetPerVol(double * pVol,double	dVol,long H,long L)
{
	if(H<0 || L<0 || H>=NPRICE || L>=NPRICE) return;

	int			i;
	double		dPerVol = 0;
	switch ( m_Setup.Way )
	{
	case AVERAGE:
		dPerVol	= dVol/(H-L+1);
		//成千上万个中的一个区间段。
		for ( i=L;i<=H; ++i )
			pVol[i] += dPerVol;
		break;
	case TRIANGLE:
		{
			long	half = (H+L)/2;
			if ( half == H || half == L )
				pVol[half] += dVol;
			else
			{
				double	vh = dVol/(half-L);
				for ( i=L;i<half;++i )
				{
					pVol[i] += (i-L)*(vh/(half-L));
				}
				for ( i;i<=H;++i )
				{
					pVol[i] += (i-H)*(vh/(half-H));
				}
			}
		}
		break;
	case NORMAL:
		break;
	}
	return	;
}

inline	int	UCb::ReadCache()
{
	int		i,nNow = 0;
	for ( i=CHNUM-1;i>=0;--i )
	{
		if ( m_anCache[i] <= m_nCurr )
		{
			memcpy(m_aVolNow,m_aVolCache[i],NPRICE*sizeof(double));
			nNow = m_anCache[i];
			break;
		}
	}
	return	nNow;
}

inline	void UCb::WriteCache(int nPosNo)
{
	int	i;
	for ( i=0;i<CHNUM;++i )
	{
		if (  m_anCache[i] + CACHEA*(i+1) >= nPosNo )
		{
			memcpy(m_aVolCache[i],m_aVolNow,NPRICE*sizeof(double));
			m_anCache[i] = nPosNo;
		}
	}
}

void	UCb::MainCalc()
{
	int		i,j,ll;
	double	dHSL;
	long	nLow,nHigh;
	long	h = m_FxBottom-m_FxTop;
	int		nNewSize = h/m_nDotSpan;	//DrawVol直接根据h来。
	long    nPrice = (m_nMaxPrice - m_nMinPrice + 1)/m_nPGap;
	if ( h <= 0 || m_dMinPrice < 0 || m_dMaxPrice < 0)
		return;
	if (m_nCurr >= m_nDataNum)
		return;
	//避免重新申请空间
	memset(m_aVolNow,0,sizeof(double)*NPRICE);
	i = ReadCache();	//读取缓存
	for ( ;i<=m_nCurr;++i )
	{
		if ( m_fLTGB[i] < COMPPREC )
		{	
			if ( i == 0 )	continue;
			m_fLTGB[i] = max(m_pData[i].Volume,m_fLTGB[i-1]);
		}
		dHSL = GetHSL(i);
		//如果存在就处理
		for ( j=0;j<NPRICE;++j )
		{
			m_aVolNow[j] *= (1-dHSL);
		}
		nLow = m_pData[i].Low*m_nAmple/m_nPGap - m_nMinPrice;
		nHigh= m_pData[i].High*m_nAmple/m_nPGap- m_nMinPrice;
		GetPerVol(m_aVolNow,m_pData[i].Volume,nHigh,nLow);
		for ( j = 0;j<CHNUM; ++j )
		{
			if ( CACHEA*(j+1) == i  ) //&& m_anCache[j]<CACHEA*(j+1) )	//减少操作
			{
				memcpy(m_aVolCache[j],m_aVolNow,NPRICE*sizeof(double));
				m_anCache[j] = i;
			}
		}
	}
	////////////////////////////
	CalcEach();
	memset(m_aDrawVol,0,NPRICE*sizeof(double));
	/////////////////////////////////////////
	long	VGrid = nNewSize,nOf;
	m_dPRatio = (m_dMaxPrice-m_dMinPrice)/nNewSize;
	double	start,offp,tmpVol=0;
	//如果m_nMinPrice很小，当前屏幕的m_dMaxPrice较大，会导致空间不够。
	//start = m_dMinPrice*100/m_nPGap- m_nMinPrice;
	//m_aDrawVol对应m_dMinPrice开始,到时反过来，根据i求Price
	start = m_nAmple*m_dMinPrice/m_nPGap;
	/////////////////////////
	nOf = -1;
	for ( i=0,offp=start;i<VGrid;++i )
	{
		start = m_nAmple*(i*m_dPRatio+m_dMinPrice)/m_nPGap;
		while ( offp <= start && offp>=0 && offp<NPRICE )
		{	//从Min开始统计
			j = offp;
			if ( nOf != j )
			{	//不要累加同样的值
				m_aDrawVol[i] += m_aVolNow[j];
				for ( ll=0;ll<MAX_MON;++ll )
				{
					m_aOutCost[ll].pDrawVol[i] += m_aOutCost[ll].pTabVol[j];
					m_aInCost[ll].pDrawVol[i]  += m_aInCost[ll].pTabVol[j];
				}
				nOf = j;
			}
			offp = start+0.001;
		}
	}
	m_dRatio	=  (m_FxBottom-m_nGivePt.x)/(m_pData[m_nCurr].Close-m_dMinPrice);
	m_dMaxVol = 0;
	for ( i=0;i<nNewSize;++i )
	{
		if ( m_dMaxVol < m_aDrawVol[i] )
			m_dMaxVol = m_aDrawVol[i];
	}
	m_bValid = TRUE;
	GetGotLostRate();
}

void	UCb::GetGotLostRate()
{
	double		 dTotalGot;
	double		 dTotalLost;
	double		 adTotal[2*MAX_MON];
	double		 dClose;
	long		 nCREdage,i,j;
	//对应点的价格。
	dClose	 = m_pData[m_nCurr].Close;
	//计算出当前价位的整数值。
	nCREdage = MIN(m_nMaxPrice,MAX(m_nMinPrice,dClose*m_nAmple/m_nPGap));
	dTotalLost = 0.0;
	dTotalGot  = 0.0;
	for(i=m_nMinPrice; i<=nCREdage; ++i )		//当前价位下
		dTotalGot += m_aVolNow[i-m_nMinPrice]/100;				//防止溢出
	for(i=nCREdage+1; i<=m_nMaxPrice; ++i )		//当前价位上
		dTotalLost+= m_aVolNow[i-m_nMinPrice]/100;
	m_DispInfo.dGotRate  = dTotalGot /(dTotalGot+dTotalLost);	//得到的百分比
	dTotalGot = (dTotalGot+dTotalLost)/2;
	dTotalLost= 0;
	m_DispInfo.j70 = m_DispInfo.j90 = 0;
	m_DispInfo.p70[0] = m_DispInfo.p70[1] = 0;
	m_DispInfo.p90[0] = m_DispInfo.p90[1] = 0;
	memset(adTotal,0,sizeof(adTotal));
	double fMinToAverCb = 0.1;
	for(i=m_nMinPrice;i<=m_nMaxPrice;++i )
	{
		for ( j=0;j<MAX_MON;++j )
		{
			adTotal[j] += m_aOutCost[j].pTabVol[i-m_nMinPrice]/100;
			adTotal[j+MAX_MON] += m_aInCost[j].pTabVol[i-m_nMinPrice]/100;
		}
		dTotalLost += m_aVolNow[i-m_nMinPrice]/100;
		if ( !m_DispInfo.p90[0] )
		{
			m_DispInfo.j90 ++;
			if ( dTotalLost/(2*dTotalGot) >= 0.05 )
				m_DispInfo.p90[0] = i*m_nPGap/(float)m_nAmple;
		}
		if ( !m_DispInfo.p70[0] )
		{
			m_DispInfo.j70 ++;
			if ( dTotalLost/(2*dTotalGot) >= 0.15 )
				m_DispInfo.p70[0] = i*m_nPGap/(float)m_nAmple;
		}
		if(fabs(dTotalLost - dTotalGot)/(2*dTotalGot) < fMinToAverCb)
		{
			m_fCBPrice = (i*m_nPGap*1.0)/m_nAmple;
			fMinToAverCb = fabs(dTotalLost - dTotalGot)/(2*dTotalGot);
		}
		if ( !m_DispInfo.p90[1] )
		{
			if ( dTotalLost/(2*dTotalGot) >= 0.95 )
				m_DispInfo.p90[1] = i*m_nPGap/(float)m_nAmple;
		}
		else
			m_DispInfo.j90 ++;
		if ( !m_DispInfo.p70[1] )
		{
			if ( dTotalLost/(2*dTotalGot) >= 0.85 )
				m_DispInfo.p70[1] = i*m_nPGap/(float)m_nAmple;
		}
		else
			m_DispInfo.j70 ++;
	}
	m_DispInfo.j90 = 100-100.0*m_DispInfo.j90/(m_nMaxPrice-m_nMinPrice+1);
	m_DispInfo.j70 = 100-100.0*m_DispInfo.j70/(m_nMaxPrice-m_nMinPrice+1);
	for ( j=0;j<MAX_MON;++j )
	{
		m_aOutCost[j].dCost = adTotal[j]/(2*dTotalGot);
		m_aInCost[j].dCost  = adTotal[MAX_MON+j]/(2*dTotalGot);
	}
}

double	UCb::GetHLP(float fPrice)
{
	double  dTotalGot;
	double  dTotalLost;
	long	nCREdage,i;
	if ( fabs(fPrice-m_pData[m_nCurr].Close) < COMPPREC )
		return m_DispInfo.dGotRate;
	//计算出当前价位的整数值。
	nCREdage = MIN(m_nMaxPrice,MAX(m_nMinPrice,fPrice*m_nAmple/m_nPGap));
	dTotalLost = 0.0;
	dTotalGot  = 0.0;
	for(i=m_nMinPrice; i<=nCREdage; ++i )	//当前价位下
		dTotalGot += m_aVolNow[i-m_nMinPrice]/100;			//防止溢出
	for(i=nCREdage+1; i<=m_nMaxPrice; ++i )	//当前价位上
		dTotalLost+= m_aVolNow[i-m_nMinPrice]/100;
	m_DispInfo.dGot = dTotalGot /(dTotalGot+dTotalLost);	//得到的百分比
	return m_DispInfo.dGot;
}

double	UCb::GetHSL(int no)
{
	if ( m_fLTGB[no] > 0.001 && no<m_nDataNum )
		return	min(1,m_Setup.fWeakK*m_pData[no].Volume/(m_fLTGB[no]*10000));	//万为单位
	else
		return	0.01;
}
