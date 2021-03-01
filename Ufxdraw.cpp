/*
* Copyright (c) 2002,通达信开发部
* All rights reserved.
* 
* 文件名称：UFxdraw.cpp
* 文件标识：
* 摘    要：完成自画线、平移等功能.
*
*/

#include "stdafx.h"
#include "Ufxt.h"
#include "interface.h"

#define  YUP_TUP       11
#define  YUP_TDOWN     10
#define  YDOWN_TUP      1
#define  YDOWN_TDOWN    0

#define XFLAG  1
#define OFLAG -1

static char Table[]={"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"};
char getAsc(int xh)
{
	if (xh>61) xh=61;
    return Table[xh];
}

extern struct	AnalyData	l_InValidHisDat;

void UFxt::DrawVolStick(CDC * pDC,int nRegionNo,int nObject,int nOutNo)
{
	BOOL	bMore = 0;
	int i=0,yy = 0,Base=  0 , oldx=0,Off = int(m_dWidth*0.5);;
	TOBJECT * p = m_aRegion[nRegionNo].aIndexInfo[nObject];
	CPen  *oldpen = g_d.SelectPen(pDC,KP_PEN);
	float * fData = p->pCalcData[nOutNo]+m_nLeftNo;
	Base = GetYPosOfRegion(nRegionNo,0,nObject);
	for ( i=1;i<m_aRegion[nRegionNo].nObject;++i )
	{
		if ( stricmp(m_aRegion[nRegionNo].aIndexInfo[i]->acCode,"VOL") == 0 )
		{
			bMore = 1;
			break;
		}
	}
	int delta=0;
	for ( i=0;i<m_nDrawNum;i++ )
	{
		if ( TBE_MEANLESS(fData[i]) != 0  )
		{
			yy = GetYPosOfRegion(nRegionNo,fData[i],nObject);
			if ( yy>= Base && fData[i]>0.1 )
				yy = Base+1;
			if ( bMore )
				g_d.DrawRect(pDC,m_pPosOfX[i]-Off,Base+delta,m_pPosOfX[i]+Off,yy,VipColor.ZBCOLOR[nObject%5]);
			else
			{
				if ( m_pObject->pHisDay[m_nLeftNo+i].Close > m_pObject->pHisDay[m_nLeftNo+i].Open + COMPPREC )
				{
					if(Off>0) g_d.Draw3DBar3(pDC,CRect(m_pPosOfX[i]-Off,yy,m_pPosOfX[i]+Off,Base+delta),VipColor.KPCOLOR,TRUE,FALSE,g_bShow3DKLine);
					else g_d.DrawRect(pDC,m_pPosOfX[i]-Off,Base+delta,m_pPosOfX[i]+Off,yy,VipColor.KPCOLOR);
				}
				else if ( m_pObject->pHisDay[m_nLeftNo+i].Close+COMPPREC < m_pObject->pHisDay[m_nLeftNo+i].Open  )
				{
					if(Off>0) g_d.Draw3DBar3(pDC,CRect(m_pPosOfX[i]-Off,yy,m_pPosOfX[i]+Off,Base+delta),VipColor.KNCOLOR,TRUE,FALSE,g_bShow3DKLine);
					else g_d.DrawRect(pDC,m_pPosOfX[i]-Off,Base+delta,m_pPosOfX[i]+Off,yy,VipColor.KNCOLOR);
				}
				else if ( m_nLeftNo+i>0 && m_pObject->pHisDay[m_nLeftNo+i].Close+COMPPREC < m_pObject->pHisDay[m_nLeftNo+i-1].Close )
				{
					if(Off>0) g_d.Draw3DBar3(pDC,CRect(m_pPosOfX[i]-Off,yy,m_pPosOfX[i]+Off,Base+delta),VipColor.KNCOLOR,TRUE,FALSE,g_bShow3DKLine);
					else g_d.DrawRect(pDC,m_pPosOfX[i]-Off,Base+delta,m_pPosOfX[i]+Off,yy,VipColor.KNCOLOR);
				}
				else //其它统一用平线表示。
				{							
					if(Off>0) g_d.Draw3DBar3(pDC,CRect(m_pPosOfX[i]-Off,yy,m_pPosOfX[i]+Off,Base+delta),RGB(128,128,128),TRUE,FALSE,g_bShow3DKLine);
					else g_d.DrawRect(pDC,m_pPosOfX[i]-Off,Base+delta,m_pPosOfX[i]+Off,yy,VipColor.KPCOLOR);
				}
			}
			if ( m_SelectInfo.bExist  && m_SelectInfo.nInRegion == nRegionNo
				&& m_SelectInfo.nInObject == nObject
				&& m_SelectInfo.nSet == ZB_OBJECT
				&& m_SelectInfo.nOutNo == nOutNo && m_pPosOfX[i] - oldx > 40 )
			{
				g_d.Bar(pDC,m_pPosOfX[i]-2,yy-2,m_pPosOfX[i]+2,yy+2,LEVEL_BRUSH);
				oldx = m_pPosOfX[i];
				m_bEvenSelected = TRUE;
			}
		}
	}
	g_d.RestorePen(pDC,oldpen);
}

void UFxt::DrawColorStick(CDC * pDC,int nRegionNo,int nObject,int nOutNo)
{
	int i=0,yy = 0,Base=  0 , oldx=0;
	TOBJECT * p = m_aRegion[nRegionNo].aIndexInfo[nObject];
	CPen  *oldpen = g_d.SelectPen(pDC,KP_PEN);
	float * fData = p->pCalcData[nOutNo]+m_nLeftNo;
	Base = GetYPosOfRegion(nRegionNo,0,nObject);
	for ( i=0;i<m_nDrawNum;i++ )
	{
		if ( TBE_MEANLESS(fData[i]) != 0  && fData[i] >= COMPPREC )
		{
			yy = GetYPosOfRegion(nRegionNo,fData[i],nObject);
			g_d.DrawLine(pDC,m_pPosOfX[i],Base,m_pPosOfX[i],yy);
			if ( m_SelectInfo.bExist  && m_SelectInfo.nInRegion == nRegionNo
				&& m_SelectInfo.nInObject == nObject
				&& m_SelectInfo.nSet == ZB_OBJECT
				&& m_SelectInfo.nOutNo == nOutNo && m_pPosOfX[i] - oldx > 40 )
			{
				g_d.Bar(pDC,m_pPosOfX[i]-2,yy-2,m_pPosOfX[i]+2,yy+2,LEVEL_BRUSH);
				oldx = m_pPosOfX[i];
				m_bEvenSelected = TRUE;
			}
		}
	}
	g_d.SelectPen(pDC,KN_PEN);
	for ( i=0;i<m_nDrawNum;i++ )
	{
		if ( TBE_MEANLESS(fData[i]) != 0  && fData[i] < COMPPREC )
		{
			yy = GetYPosOfRegion(nRegionNo,fData[i],nObject);
			g_d.DrawLine(pDC,m_pPosOfX[i],Base,m_pPosOfX[i],yy);
			if ( m_SelectInfo.bExist  && m_SelectInfo.nInRegion == nRegionNo
				&& m_SelectInfo.nInObject == nObject
				&& m_SelectInfo.nSet == ZB_OBJECT
				&& m_SelectInfo.nOutNo == nOutNo && m_pPosOfX[i] - oldx > 40 )
			{
				g_d.Bar(pDC,m_pPosOfX[i]-2,yy-2,m_pPosOfX[i]+2,yy+2,LEVEL_BRUSH);
				oldx = m_pPosOfX[i];
				m_bEvenSelected = TRUE;
			}
		}
	}
	g_d.RestorePen(pDC,oldpen);
}

void UFxt::DrawZBLine(CDC * pDC,int nRegionNo,int nObject,int nOutNo,DWORD nColor, int nWidth)
{
	if(nWidth<1) return;
	
	BOOL	bOver = FALSE;
	int i=0,xx1 = -1,xx2=0,yy1=-1,yy2,oldx=0;
	TOBJECT * p = m_aRegion[nRegionNo].aIndexInfo[nObject];
	CPen  newPen;
	newPen.CreatePen(PS_SOLID, nWidth, (COLORREF)nColor);
	CPen *oldpen = pDC->SelectObject(&newPen);
	
	float * fData = p->pCalcData[nOutNo]+m_nLeftNo;
	int    nCount = 0;
	BOOL bZBStart = FALSE;
	BOOL bZBEnd = TRUE;
	BOOL bFirst = TRUE;
	for ( i=0;i<m_nDrawNum;i++ )
	{	
		if ( TBE_MEANLESS(fData[i]) != 0 )
		{
			if(bZBEnd) 
			{
				bZBStart = TRUE;
				bZBEnd = FALSE;
				nCount = 0;
			}
			xx2 = m_pPosOfX[i];
			yy2 = GetYPosOfRegion(nRegionNo,fData[i],nObject);
			if ( xx1 != -1 )
			{
				if ( !bOver && TBE_MEANLESS(fData[i-1]) != 0)
				{
					bOver = TRUE;
					m_pPoint[nCount]  = CPoint(xx1,yy1);
					++nCount;
				}
				m_pPoint[nCount] = CPoint(xx2,yy2);
				++ nCount;
			}
			xx1 = xx2;	yy1 = yy2;
			if ( m_SelectInfo.bExist  && m_SelectInfo.nInRegion == nRegionNo
				&& m_SelectInfo.nInObject == nObject
				&& m_SelectInfo.nSet == ZB_OBJECT
				&& m_SelectInfo.nOutNo == nOutNo && xx2 - oldx > 40 )
			{
				g_d.Bar(pDC,xx2-2,yy2-2,xx2+2,yy2+2,LEVEL_BRUSH);
				oldx = xx2;
				m_bEvenSelected = TRUE;
			}
		}
		else	
		{
			if(bZBStart)
			{
				bZBStart = FALSE;
				bZBEnd = TRUE;
				if(nCount > 1)	
					pDC->Polyline(m_pPoint,nCount); 
				if(bFirst) bFirst = FALSE;
			}
			if(bFirst) 
				xx1 = -1;
		}
	}
	if(nCount > 1)	
		pDC->Polyline(m_pPoint,nCount); 
	g_d.RestorePen(pDC,oldpen);
}

void UFxt::DrawPartLine(CDC * pDC,int nRegionNo,int nObject,int nOutNo,DWORD nColor, int nWidth)
{
	if(nWidth<1) return;
	
	BOOL	bOver = FALSE;
	int i=0,xx1 = -1,xx2=0,yy1=-1,yy2,oldx=0;
	TOBJECT * p = m_aRegion[nRegionNo].aIndexInfo[nObject];
	CPen  newPen;
	newPen.CreatePen(PS_SOLID, nWidth, (COLORREF)nColor);
	CPen *oldpen = pDC->SelectObject(&newPen);
	
	float * fData = p->pCalcData[nOutNo];
	HandleValue * phv = NULL;
	if(TBE_MEANLESS(fData[m_nDataNum])) 
	{
		DWORD pdVal;
		memcpy(&pdVal, &fData[m_nDataNum], sizeof(DWORD));
		phv = (HandleValue *)pdVal;
		if(phv->nValidFlag!=TMEANLESS_INT)
			phv = NULL;
	}
	if(!phv) return;

	float *fHasPartTmp = &(phv->pFloat[0]);
	float *fPointTmp = &(phv->pFloat[m_nDataNum]);
	float *fHasPart = fHasPartTmp + m_nLeftNo;
	float *fPoint = fPointTmp + m_nLeftNo;

	for ( i=1;i<m_nDrawNum;i++ )
	{	
		if ( TBE_MEANLESS(fHasPart[i])!=0 && TBE_MEANLESS(fPoint[i])!=0 && fabs(fHasPart[i])>COMPPREC)		//满足
		{
			xx1 = m_pPosOfX[i-1];
			yy1 = GetYPosOfRegion(nRegionNo,fPoint[i-1],nObject);

			xx2 = m_pPosOfX[i];
			yy2 = GetYPosOfRegion(nRegionNo,fPoint[i],nObject);

			g_d.DrawLine(pDC, xx1, yy1, xx2, yy2);
			
			if ( m_SelectInfo.bExist  && m_SelectInfo.nInRegion == nRegionNo
				&& m_SelectInfo.nInObject == nObject
				&& m_SelectInfo.nSet == ZB_OBJECT
				&& m_SelectInfo.nOutNo == nOutNo && xx2 - oldx > 40 )
			{
				g_d.Bar(pDC,xx2-2,yy2-2,xx2+2,yy2+2,LEVEL_BRUSH);
				oldx = xx2;
				m_bEvenSelected = TRUE;
			}
		}
	}
	g_d.RestorePen(pDC,oldpen);
}	

void UFxt::DrawStick(CDC * pDC,int nRegionNo,int nObject,int nOutNo,DWORD nColor, int nWidth)
{
	int i=0,yy = 0,Base=  0 , oldx=0;
	TOBJECT * p = m_aRegion[nRegionNo].aIndexInfo[nObject];
	CPen  newPen;
	newPen.CreatePen(PS_SOLID, nWidth, (COLORREF)nColor);
	CPen *oldpen = pDC->SelectObject(&newPen);
	float * fData = p->pCalcData[nOutNo]+m_nLeftNo;
	Base = GetYPosOfRegion(nRegionNo,0,nObject);
	for ( i=0;i<m_nDrawNum;i++ )
	{
		if ( TBE_MEANLESS(fData[i]) != 0 )
		{
			yy = GetYPosOfRegion(nRegionNo,fData[i],nObject);
			g_d.DrawLine(pDC,m_pPosOfX[i],Base,m_pPosOfX[i],yy);
			if ( m_SelectInfo.bExist  && m_SelectInfo.nInRegion == nRegionNo
				&& m_SelectInfo.nInObject == nObject
				&& m_SelectInfo.nSet == ZB_OBJECT
				&& m_SelectInfo.nOutNo == nOutNo && m_pPosOfX[i] - oldx > 40 )
			{
				g_d.Bar(pDC,m_pPosOfX[i]-2,yy-2,m_pPosOfX[i]+2,yy+2,LEVEL_BRUSH);
				oldx = m_pPosOfX[i];
				m_bEvenSelected = TRUE;
			}
		}
	}
	g_d.RestorePen(pDC,oldpen);
}

void UFxt::DrawLineStick(CDC * pDC,int nRegionNo,int nObject,int nOutNo,DWORD nColor, int nWidth)
{
	BOOL	bOver = FALSE;
	int i=0,xx1 = -1,xx2=0,yy1=-1,yy2,Base=0,oldx=0;
	TOBJECT * p = m_aRegion[nRegionNo].aIndexInfo[nObject];
	CPen  newPen;
	newPen.CreatePen(PS_SOLID, nWidth, (COLORREF)nColor);
	CPen *oldpen = pDC->SelectObject(&newPen);
	
	float * fData = p->pCalcData[nOutNo]+m_nLeftNo;
	Base = GetYPosOfRegion(nRegionNo,0,nObject);
	int    nCount = 0;
	BOOL bZBStart = FALSE;
	BOOL bZBEnd = TRUE;
	BOOL bFirst = TRUE;
	int  preMean = 0;
	for ( i=0;i<m_nDrawNum;i++ )
	{
		if ( TBE_MEANLESS(fData[i]) != 0 )
		{	
			if(bZBEnd) 
			{
				bZBStart = TRUE;
				bZBEnd = FALSE;
				nCount = 0;
			}
			xx2 = m_pPosOfX[i];
			yy2 = GetYPosOfRegion(nRegionNo,fData[i],nObject);
			if ( xx1 != -1 )
			{
				if ( !bOver && TBE_MEANLESS(fData[i-1]) != 0)
				{
					bOver = TRUE;
					m_pPoint[nCount]  = CPoint(xx1,yy1);
					++nCount;
				}
				m_pPoint[nCount] = CPoint(xx2,yy2);
				++ nCount;
			}
			xx1 = xx2;	yy1 = yy2;
			g_d.DrawLine(pDC,xx1,Base,xx1,yy1);
			if ( m_SelectInfo.bExist  && m_SelectInfo.nInRegion == nRegionNo
				&& m_SelectInfo.nInObject == nObject
				&& m_SelectInfo.nSet == ZB_OBJECT
				&& m_SelectInfo.nOutNo == nOutNo && xx2 - oldx > 40 )
			{
				g_d.Bar(pDC,xx2-2,yy2-2,xx2+2,yy2+2,LEVEL_BRUSH);
				oldx = xx2;
				m_bEvenSelected = TRUE;
			}
		}
		else
		{
			if(bZBStart)
			{
				bZBStart = FALSE;
				bZBEnd = TRUE;
				if(nCount > 1)	
					pDC->Polyline(m_pPoint,nCount); 
				if(bFirst) bFirst = FALSE;
			}
			if(bFirst) 
				xx1 = -1;
		}
	}
	if(nCount > 1)	
		pDC->Polyline(m_pPoint,nCount); 
	g_d.RestorePen(pDC,oldpen);
}

void UFxt::FillBar(CDC *pDC,int left,int top,int right,int bottom,COLORREF nColor)
{
	right+=1;
	CBrush newBrush, *oldBrush;
	newBrush.CreateSolidBrush(nColor);
	oldBrush = pDC->SelectObject(&newBrush);
	CRect rc;
	if(abs(right-left)>2)
	{
		rc.SetRect(left,top,right,bottom);
		pDC->FillRect(&rc,&newBrush);
	}
	else
		g_d.DrawLine(pDC,(right+left)/2,top,(right+left)/2,bottom,nColor);
	pDC->SelectObject(oldBrush);
}

void UFxt::DrawStickLine(CDC *pDC, int nRegion, int nObject, int nOutNo, DWORD nColor, int nWidth)
{
	int i, yyUp=0, yyDn=0 ,Base = 0, oldx = 0; 
	TOBJECT * p = m_aRegion[nRegion].aIndexInfo[nObject];
	float * fData = p->pCalcData[nOutNo];
	
	HandleValue * phv = NULL;
	if(TBE_MEANLESS(fData[m_nDataNum])) 
	{
		DWORD pdVal;
		memcpy(&pdVal, &fData[m_nDataNum], sizeof(DWORD));
		phv = (HandleValue *)pdVal;
		if(phv->nValidFlag!=TMEANLESS_INT)
			phv = NULL;
	}
	if(!phv) return;
	float *fHasLineTmp = &(phv->pFloat[0]);
	float *fUpTmp = &(phv->pFloat[m_nDataNum]);
	float *fDnTmp = &(phv->pFloat[2*m_nDataNum]);
	float	width = phv->pFloat[4*m_nDataNum-1];
	BOOL bEmpty = (phv->pFloat[5*m_nDataNum-1]<COMPPREC);
	int Off = int(m_dWidth*0.16*width)+1;
	//
	if(width<-COMPPREC)
	{
		width = nWidth;
		Off = int(m_dWidth*0.5*width);
	}
	else if(width<COMPPREC)
		Off = 0;
	int nEmptyType = 1;
	if(fabs(phv->pFloat[5*m_nDataNum-1])<COMPPREC) nEmptyType=0;
	else if(fabs(phv->pFloat[5*m_nDataNum-1]+1)<COMPPREC) nEmptyType=2;

	float *fHasLine = fHasLineTmp + m_nLeftNo;
	float *fUp = fUpTmp + m_nLeftNo;
	float *fDn = fDnTmp + m_nLeftNo;
	for ( i=0;i<m_nDrawNum;i++ )
	{	
		if (fabs(fHasLine[i])>COMPPREC && TBE_MEANLESS(fUp[i]) && TBE_MEANLESS(fDn[i]))
		{
			yyUp = GetYPosOfRegion(nRegion, fUp[i], nObject);
			yyDn = GetYPosOfRegion(nRegion, fDn[i], nObject);
			if(nEmptyType==0)
			{
				if(Off<m_dWidth/6)
					g_d.DrawLine(pDC, m_pPosOfX[i], yyUp,m_pPosOfX[i],yyDn,nColor);
				else
				{
					FillBar(pDC, m_pPosOfX[i]-Off,min(yyUp,yyDn),m_pPosOfX[i]+Off,max(yyUp,yyDn), nColor);
					g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yyUp,m_pPosOfX[i]+Off,yyUp,nColor);
					g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yyDn,m_pPosOfX[i]+Off,yyDn,nColor);
					g_d.DrawLine(pDC,m_pPosOfX[i]+Off,yyUp,m_pPosOfX[i]-Off,yyUp,nColor);
					g_d.DrawLine(pDC,m_pPosOfX[i]+Off,yyDn,m_pPosOfX[i]-Off,yyDn,nColor);
				}
			}
			else
			{
				if(Off<2)
					g_d.DrawLine(pDC, m_pPosOfX[i], yyUp,m_pPosOfX[i],yyDn,nColor);
				else 
				{
					g_d.Bar(pDC,m_pPosOfX[i]-Off,min(yyUp,yyDn),m_pPosOfX[i]+Off,max(yyUp,yyDn),BACK_BRUSH);
					if(nEmptyType==1) 
						g_d.DrawRect(pDC,m_pPosOfX[i]-Off,min(yyUp,yyDn),m_pPosOfX[i]+Off,max(yyUp,yyDn), nColor);
					else
						g_d.DrawDotRect(pDC,m_pPosOfX[i]-Off,min(yyUp,yyDn),m_pPosOfX[i]+Off,max(yyUp,yyDn),nColor,2);
				}
			}
		}
	}
}

void UFxt::DrawIcons(CDC *pDC, int nRegionNo, int nObject, int nOutNo)
{
	int yyPt, i;
	TOBJECT * p = m_aRegion[nRegionNo].aIndexInfo[nObject];
	float * fData = p->pCalcData[nOutNo];
	HandleValue * phv = NULL;
	if(TBE_MEANLESS(fData[m_nDataNum])) 
	{
		DWORD pdVal;
		memcpy(&pdVal, &fData[m_nDataNum], sizeof(DWORD));
		phv = (HandleValue *)pdVal;
		if(phv->nValidFlag!=TMEANLESS_INT)
			phv = NULL;
	}
	if(!phv) return;
	float *fHasIconTmp = &(phv->pFloat[0]);
	float *fIconTmp = &(phv->pFloat[m_nDataNum]);
	float *fnIconTmp = &(phv->pFloat[2*m_nDataNum]);
	float *fHasIcon = fHasIconTmp + m_nLeftNo;
	float *fIcon = fIconTmp + m_nLeftNo;
	float *fnIcon = fnIconTmp + m_nLeftNo;
	int nTotalIconNum=28;
#ifdef PERSONAL
	nTotalIconNum=28+6;
#endif
	HICON hIcon= LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_DRAWICON1+(int)fnIcon[m_nDrawNum-1]%nTotalIconNum-1));
	if(hIcon == NULL)
		return;
	for(i=0;i<m_nDrawNum;i++)
	{
		if(fabs(fHasIcon[i]-1)<COMPPREC && TBE_MEANLESS(fIcon[i]) != 0)	
		{
			yyPt = GetYPosOfRegion(nRegionNo, fIcon[i], nObject);
			::DrawIconEx(pDC->m_hDC, max(0,m_pPosOfX[i]-7), max(0,yyPt-9), hIcon, 16, 16, 0, NULL, DI_NORMAL);
		}
	}
}

void UFxt::DrawIconsEx(CDC *pDC, int nRegionNo, int nObject, int nOutNo)
{
	if(m_nDataNum<=0) return;
	
	int yyPt, i;
	TOBJECT * p = m_aRegion[nRegionNo].aIndexInfo[nObject];
	float * fData = p->pCalcData[nOutNo];
	
	HandleValue * phv = NULL;
	if(TBE_MEANLESS(fData[m_nDataNum])) 
	{
		DWORD pdVal;
		memcpy(&pdVal, &fData[m_nDataNum], sizeof(DWORD));
		phv = (HandleValue *)pdVal;
		if(phv->nValidFlag!=TMEANLESS_INT)
			phv = NULL;
	}
	if(!phv) return;
	float *fHasIconTmp = &(phv->pFloat[0]);
	float *fIconTmp = &(phv->pFloat[m_nDataNum]);
	float *fHasIcon = fHasIconTmp + m_nLeftNo;
	float *fIcon = fIconTmp + m_nLeftNo;
	CString str= phv->pValue;
	str.MakeUpper();
	
	if(str.Find(".ICO",0)!=-1)
	{
		HICON hIcon = (HICON)LoadImage(NULL,LPCTSTR(str),IMAGE_ICON,0,0,LR_LOADFROMFILE|LR_VGACOLOR|LR_SHARED);		
		if(hIcon == NULL)
			return;
		for(i=0;i<m_nDrawNum;i++)
		{
			if(m_nLeftNo+i >= m_nDataNum)
			{
				break;
			}
			if(fabs(fHasIcon[i]-1)<0.001 && TBE_MEANLESS(fIcon[i]) != 0)	
			{
				yyPt = GetYPosOfRegion(nRegionNo, fIcon[i], nObject);
				::DrawIconEx(pDC->m_hDC, m_pPosOfX[i]-6, yyPt, hIcon, 16, 16, 0, NULL, DI_NORMAL);
			}
		}
		DestroyIcon(hIcon);
	}
	else
	{
		//载入
		HBITMAP m_iconbmp = (HBITMAP)LoadImage(0,LPCTSTR(str),IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_VGACOLOR|LR_SHARED);
		if(m_iconbmp==NULL)
			return;
		CDC BitDC;
		BITMAP bbb;
		//面积
		CBitmap *tmpbmp = CBitmap::FromHandle(m_iconbmp);
		tmpbmp->GetBitmap(&bbb);
		//映射
		BitDC.CreateCompatibleDC(pDC);
		HBITMAP oldBitmap = (HBITMAP)BitDC.SelectObject(m_iconbmp);
		for(i=0;i<m_nDrawNum;i++)
		{
			if(m_nLeftNo+i >= m_nDataNum)
			{
				break;
			}
			if(fabs(fHasIcon[i]-1)<0.001 && TBE_MEANLESS(fIcon[i]) != 0)	
			{
				//copy
				yyPt = GetYPosOfRegion(nRegionNo, fIcon[i], nObject);
				pDC->StretchBlt(m_pPosOfX[i]-6,yyPt,16,16,&BitDC,0,0,bbb.bmWidth,bbb.bmHeight,SRCCOPY);
			}
		}
		//复原
		BitDC.SelectObject(oldBitmap);
		DeleteObject(m_iconbmp);
	}
}
	
void UFxt::DrawTextLine(CDC *pDC, int nRegionNo, int nObject, int nOutNo, DWORD nColor)
{
	if(m_nDataNum<=0) return;
	
	int yyPt, i;
	TOBJECT * p = m_aRegion[nRegionNo].aIndexInfo[nObject];
	
	float * fData = p->pCalcData[nOutNo];
	HandleValue * phv = NULL;
	if(TBE_MEANLESS(fData[m_nDataNum])) 
	{
		DWORD pdVal;
		memcpy(&pdVal, &fData[m_nDataNum], sizeof(DWORD));
		phv = (HandleValue *)pdVal;
		if(phv->nValidFlag!=TMEANLESS_INT)
			phv = NULL;
	}
	if(!phv) return;
	float *fHasTextTmp = &(phv->pFloat[0]);
	float *fTextTmp = &(phv->pFloat[m_nDataNum]);
	float *fHasText = fHasTextTmp + m_nLeftNo;
	float *fText = fTextTmp + m_nLeftNo;
	char *Textc = phv->pValue;
	char **Textd = phv->ppValue+ m_nLeftNo;
	
	double dw=m_dWidth+m_dDistance;
	CFont *oldFont = g_d.SelectFont(pDC, GRAPH_FONT);
	if(dw<=9) g_d.SelectFont(pDC, SMALL_FONT);
	
	int oldmode = pDC->SetBkMode(TRANSPARENT);
	for(i=0;i<m_nDrawNum;i++)
	{
		if(m_nLeftNo+i >= m_nDataNum)
		{
			break;
		}
		if(fabs(fHasText[i]-1)<COMPPREC && TBE_MEANLESS(fText[i]) != 0)
		{
			yyPt = GetYPosOfRegion(nRegionNo, fText[i], nObject);
			g_d.DrawO(pDC, m_pPosOfX[i], yyPt, &VipColor.FLOATCURSORCOLOR);
			char *dText = NULL;
			if(Textc) dText = Textc;
			else if(Textd&&Textd[i])
				dText = Textd[i];
			if(dText) g_d.DisplayText(pDC, m_pPosOfX[i]+4, yyPt-6, nColor, dText);
		}
	}
	pDC->SetBkMode(oldmode);
}

void UFxt::DrawTextLineEx(CDC *pDC, int nRegionNo, int nObject, int nOutNo, DWORD nColor)
{
	if(m_nDataNum<=0) return;
	
	//	int yyPt, i;
	TOBJECT * p = m_aRegion[nRegionNo].aIndexInfo[nObject];
	
	float * fData = p->pCalcData[nOutNo];
	HandleValue * phv = NULL;
	if(TBE_MEANLESS(fData[m_nDataNum])) 
	{
		DWORD pdVal;
		memcpy(&pdVal, &fData[m_nDataNum], sizeof(DWORD));
		phv = (HandleValue *)pdVal;
		if(phv->nValidFlag!=TMEANLESS_INT)
			phv = NULL;
	}
	if(!phv) return;
	
	float Xratio = phv->pFloat[0];
	float Yratio = phv->pFloat[1];
	char *Textc = phv->pValue;
	
	double dw=m_dWidth+m_dDistance;
	CFont *oldFont = g_d.SelectFont(pDC, GRAPH_FONT);
	int oldmode = pDC->SetBkMode(TRANSPARENT);
	
	g_d.DrawO(pDC, (1-Xratio/100)*m_aRegion[nRegionNo].DrawRc.left+Xratio/100*m_aRegion[nRegionNo].DrawRc.right,
		(1-Yratio/100)*m_aRegion[nRegionNo].DrawRc.top+Yratio/100*m_aRegion[nRegionNo].DrawRc.bottom, &VipColor.FLOATCURSORCOLOR);
	if(Textc) 
		g_d.DisplayText(pDC, (1-Xratio/100)*m_aRegion[nRegionNo].DrawRc.left+Xratio/100*m_aRegion[nRegionNo].DrawRc.right+4, 
		(1-Yratio/100)*m_aRegion[nRegionNo].DrawRc.top+Yratio/100*m_aRegion[nRegionNo].DrawRc.bottom-6, nColor, Textc);
	
	pDC->SetBkMode(oldmode);
}

void UFxt::DrawNumLine(CDC *pDC, int nRegionNo, int nObject, int nOutNo, DWORD nColor)
{
	if(m_nDataNum<=0) return;
	
	int yyPt, i;
	TOBJECT * p = m_aRegion[nRegionNo].aIndexInfo[nObject];
	
	float * fData = p->pCalcData[nOutNo];
	HandleValue * phv = NULL;
	if(TBE_MEANLESS(fData[m_nDataNum])) 
	{
		DWORD pdVal;
		memcpy(&pdVal, &fData[m_nDataNum], sizeof(DWORD));
		phv = (HandleValue *)pdVal;
		if(phv->nValidFlag!=TMEANLESS_INT)
			phv = NULL;
	}
	if(!phv) return;
	float *fHasTextTmp = &(phv->pFloat[0]);
	float *fTextPosTmp = &(phv->pFloat[m_nDataNum]);
	float *fTextNumTmp = &(phv->pFloat[2*m_nDataNum]);
	float *fTextFragTmp = &(phv->pFloat[3*m_nDataNum]);
	float *fHasText = fHasTextTmp + m_nLeftNo;
	float *fTextPos = fTextPosTmp + m_nLeftNo;
	float *fTextNum = fTextNumTmp + m_nLeftNo;
	float *fTextFrag = fTextFragTmp + m_nLeftNo;
	
	double dw=m_dWidth+m_dDistance;
	CFont *oldFont = g_d.SelectFont(pDC, GRAPH_FONT);
	if(dw<=9) g_d.SelectFont(pDC, SMALL_FONT);
	
	int oldmode = pDC->SetBkMode(TRANSPARENT);
	for(i=0;i<m_nDrawNum;i++)
	{
		if(m_nLeftNo+i >= m_nDataNum)
		{
			break;
		}
		if(fabs(fHasText[i]-1)<0.001 && TBE_MEANLESS(fTextNum[i]) != 0)
		{
			yyPt = GetYPosOfRegion(nRegionNo, fTextPos[i], nObject);
			g_d.DrawO(pDC, m_pPosOfX[i], yyPt, &VipColor.FLOATCURSORCOLOR);

			CString tmpstr,str;
			if(TBE_MEANLESS(fTextNum[i]))
			{
				tmpstr.Format("%%.%df",(int)fTextFrag[i]);
				str.Format(tmpstr,fTextNum[i]);
			}
			else str = "--";

			g_d.DisplayText(pDC, m_pPosOfX[i]+4, yyPt-6, nColor, str);
		}
	}
	pDC->SetBkMode(oldmode);
}

void UFxt::DrawBand(CDC *pDC, int nRegionNo, int nObject, int nOutNo)
{
	int i;
	TOBJECT * p = m_aRegion[nRegionNo].aIndexInfo[nObject];
	int y11,y12,y21,y22,ycross,xcross;

	float * fData = p->pCalcData[nOutNo];
	HandleValue * phv = NULL;
	if(TBE_MEANLESS(fData[m_nDataNum])) 
	{
		DWORD pdVal;
		memcpy(&pdVal, &fData[m_nDataNum], sizeof(DWORD));
		phv = (HandleValue *)pdVal;
		if(phv->nValidFlag!=TMEANLESS_INT)
			phv = NULL;
	}
	if(!phv) return;
	float *fValue1 = &phv->pFloat[0*m_nDataNum+m_nLeftNo];
	COLORREF Color1 = DWORD(phv->pFloat[2*m_nDataNum-1]+0.5);
	float *fValue2 = &phv->pFloat[2*m_nDataNum+m_nLeftNo];
	COLORREF Color2 = DWORD(phv->pFloat[4*m_nDataNum-1]+0.5);

	CBrush brush1,brush2,*poldbrush;
	CPen pen1,pen2,*poldpen;
	brush1.CreateSolidBrush(Color1);
	brush2.CreateSolidBrush(Color2);
	pen1.CreatePen(PS_SOLID,1,Color1);
	pen2.CreatePen(PS_SOLID,1,Color2);
	poldpen		= pDC->SelectObject(&pen1);
	poldbrush	= pDC->SelectObject(&brush1);
	for(i=1;i<m_nDrawNum;i++)
	{
		if(TBE_MEANLESS(fValue1[i-1])!=0 && TBE_MEANLESS(fValue1[i])!=0 && TBE_MEANLESS(fValue2[i-1])!=0 && TBE_MEANLESS(fValue2[i])!=0)	
		{
			y11 = GetYPosOfRegion(nRegionNo, fValue1[i-1], nObject);
			y12 = GetYPosOfRegion(nRegionNo, fValue2[i-1], nObject);
			y21 = GetYPosOfRegion(nRegionNo, fValue1[i], nObject);
			y22 = GetYPosOfRegion(nRegionNo, fValue2[i], nObject);
			//下面用坐标比较来决定颜色,因为纵坐标大小和数值正好相反,此处颜色处理要注意反相
			if((y11-y12)*(y21-y22)<0)	//反相
			{
				xcross = m_pPosOfX[i-1]+(m_pPosOfX[i]-m_pPosOfX[i-1])*(y11-y12)/(y11+y22-y12-y21);
				ycross = y11+(y21-y11)*(y11-y12)/(y11+y22-y12-y21);
				if(y11-y12>0)
				{
					pDC->SelectObject(&brush2);
					pDC->SelectObject(&pen2);
					//
					pDC->BeginPath();
					pDC->MoveTo(m_pPosOfX[i-1],y11);
					pDC->LineTo(m_pPosOfX[i-1],y12);
					pDC->LineTo(xcross,ycross);
					pDC->LineTo(m_pPosOfX[i-1],y11);
					pDC->EndPath();
					pDC->StrokeAndFillPath();
					//
					pDC->SelectObject(&brush1);
					pDC->SelectObject(&pen1);
					//
					pDC->BeginPath();
					pDC->MoveTo(m_pPosOfX[i],y21);
					pDC->LineTo(m_pPosOfX[i],y22);
					pDC->LineTo(xcross,ycross);
					pDC->LineTo(m_pPosOfX[i],y21);
					pDC->EndPath();
					pDC->StrokeAndFillPath();
				}
				else
				{
					pDC->SelectObject(&brush1);
					pDC->SelectObject(&pen1);
					//
					pDC->BeginPath();
					pDC->MoveTo(m_pPosOfX[i-1],y11);
					pDC->LineTo(m_pPosOfX[i-1],y12);
					pDC->LineTo(xcross,ycross);
					pDC->LineTo(m_pPosOfX[i-1],y11);
					pDC->EndPath();
					pDC->StrokeAndFillPath();
					//
					pDC->SelectObject(&brush2);
					pDC->SelectObject(&pen2);
					//
					pDC->BeginPath();
					pDC->MoveTo(m_pPosOfX[i],y21);
					pDC->LineTo(m_pPosOfX[i],y22);
					pDC->LineTo(xcross,ycross);
					pDC->LineTo(m_pPosOfX[i],y21);
					pDC->EndPath();
					pDC->StrokeAndFillPath();
				}
			}
			else
			{
				if(y11>y12 || y21>y22)
				{
					pDC->SelectObject(&brush2);
					pDC->SelectObject(&pen2);
				}
				else
				{
					pDC->SelectObject(&brush1);
					pDC->SelectObject(&pen1);
				}
				pDC->BeginPath();
				pDC->MoveTo(m_pPosOfX[i-1],y11);
				pDC->LineTo(m_pPosOfX[i-1],y12);
				pDC->LineTo(m_pPosOfX[i],y22);
				pDC->LineTo(m_pPosOfX[i],y21);
				pDC->LineTo(m_pPosOfX[i-1],y11);
				pDC->EndPath();
				pDC->StrokeAndFillPath();
			}
		}
	}
	pDC->SelectObject(poldpen);
	pDC->SelectObject(poldbrush);
	brush1.DeleteObject();
	brush2.DeleteObject();
	pen1.DeleteObject();
	pen2.DeleteObject();
}

void UFxt::DrawCircleDot(CDC *pDC, int nRegion, int nObject, int nOutNum, DWORD nColor, int nWidth)
{
	int i=0,yy = 0,oldx = 0;
	TOBJECT * p = m_aRegion[nRegion].aIndexInfo[nObject];
	CPen  newPen, *oldpen;
	newPen.CreatePen(PS_SOLID, nWidth, (COLORREF)nColor);
	oldpen = pDC->SelectObject(&newPen);
	float * fData = p->pCalcData[nOutNum]+m_nLeftNo;
	double dw=m_dWidth+m_dDistance;
	for ( i=0;i<m_nDrawNum;i++ )
	{
		if ( TBE_MEANLESS(fData[i]) != 0 )
		{
			yy = GetYPosOfRegion(nRegion,fData[i],nObject);
			if (dw>=6)
				g_d.DrawO(pDC, m_pPosOfX[i], yy, &nColor);
			else
			{
				pDC->SetPixel(m_pPosOfX[i]-1,yy,nColor);
				pDC->SetPixel(m_pPosOfX[i],yy-1,nColor);
				pDC->SetPixel(m_pPosOfX[i],yy+1,nColor);
				pDC->SetPixel(m_pPosOfX[i]+1,yy,nColor);
			}
			if ( m_SelectInfo.bExist  && m_SelectInfo.nInRegion == nRegion
				&& m_SelectInfo.nInObject == nObject
				&& m_SelectInfo.nSet == ZB_OBJECT
				&& m_SelectInfo.nOutNo == nOutNum && m_pPosOfX[i] - oldx > 40 )
			{
				g_d.Bar(pDC,m_pPosOfX[i]-2,yy-2,m_pPosOfX[i]+2,yy+2,LEVEL_BRUSH);
				oldx = m_pPosOfX[i];
				m_bEvenSelected = TRUE;
			}
		}
	}
	g_d.RestorePen(pDC, oldpen);
}

void UFxt::DrawCrossDot(CDC *pDC, int nRegion, int nObject, int nOutNum, DWORD nColor, int nWidth)
{
	int i=0,yy = 0,oldx = 0;
	TOBJECT * p = m_aRegion[nRegion].aIndexInfo[nObject];
	CPen  newPen, *oldpen;
	newPen.CreatePen(PS_SOLID, nWidth, (COLORREF)nColor);
	oldpen = pDC->SelectObject(&newPen);
	float * fData = p->pCalcData[nOutNum]+m_nLeftNo;
	double dw=m_dWidth+m_dDistance;
	for ( i=0;i<m_nDrawNum;i++ )
	{
		if ( TBE_MEANLESS(fData[i]) != 0)
		{
			yy = GetYPosOfRegion(nRegion,fData[i],nObject);
			if(dw<5)
			{
				g_d.DrawLine(pDC,m_pPosOfX[i]-1,yy-1,m_pPosOfX[i]+1,yy+1);
				g_d.DrawLine(pDC,m_pPosOfX[i]+1,yy-1,m_pPosOfX[i]-1,yy+1);
			}
			else if(dw>=5&&dw<10)
			{
				g_d.DrawLine(pDC,m_pPosOfX[i]-2,yy-2,m_pPosOfX[i]+2,yy+2);
				g_d.DrawLine(pDC,m_pPosOfX[i]+2,yy-2,m_pPosOfX[i]-2,yy+2);
			}
			else g_d.DrawX(pDC, m_pPosOfX[i], yy);
			if ( m_SelectInfo.bExist  && m_SelectInfo.nInRegion == nRegion
				&& m_SelectInfo.nInObject == nObject
				&& m_SelectInfo.nSet == ZB_OBJECT
				&& m_SelectInfo.nOutNo == nOutNum && m_pPosOfX[i] - oldx > 40 )
			{
				g_d.Bar(pDC,m_pPosOfX[i]-2,yy-2,m_pPosOfX[i]+2,yy+2,LEVEL_BRUSH);
				oldx = m_pPosOfX[i];
				m_bEvenSelected = TRUE;
			}
		}
	}
	g_d.RestorePen(pDC, oldpen);
}

void UFxt::DrawPointDot(CDC *pDC, int nRegion, int nObject, int nOutNum, DWORD nColor, int nWidth)
{
	int i=0,yy = 0,oldx = 0;
	TOBJECT * p = m_aRegion[nRegion].aIndexInfo[nObject];
	float * fData = p->pCalcData[nOutNum]+m_nLeftNo;
	
	//使配置颜色生效
	CPen	newpen;
	CPen	*oldpen;
	CBrush	newbrush;
	CBrush	*oldbrush;
	newpen.CreatePen(PS_SOLID, 1, (COLORREF)nColor);
	newbrush.CreateSolidBrush((COLORREF)nColor);
	oldpen = pDC->SelectObject(&newpen);
	oldbrush = pDC->SelectObject(&newbrush);
	
	for ( i=0;i<m_nDrawNum;i++ )
	{
		if ( TBE_MEANLESS(fData[i]) != 0 )
		{
			yy = GetYPosOfRegion(nRegion,fData[i],nObject);
			if(nWidth <= 3) pDC->SetPixel(m_pPosOfX[i], yy, nColor);
			else pDC->Ellipse(m_pPosOfX[i]-0.5*nWidth, yy-0.5*nWidth, m_pPosOfX[i]+0.5*nWidth, yy+0.5*nWidth);
			if ( m_SelectInfo.bExist  && m_SelectInfo.nInRegion == nRegion
				&& m_SelectInfo.nInObject == nObject
				&& m_SelectInfo.nSet == ZB_OBJECT
				&& m_SelectInfo.nOutNo == nOutNum && m_pPosOfX[i] - oldx > 40 )
			{
				g_d.Bar(pDC,m_pPosOfX[i]-2,yy-2,m_pPosOfX[i]+2,yy+2,LEVEL_BRUSH);
				oldx = m_pPosOfX[i];
				m_bEvenSelected = TRUE;
			}
		}
	}
	pDC->SelectObject(oldpen);
	pDC->SelectObject(oldbrush);
}

void UFxt::DrawKLine(CDC *pDC, int nRegion, int nObject, int nOutNum, int nWidth)
{
	int i,yC,yO,yH,yL; 
	TOBJECT * p = m_aRegion[nRegion].aIndexInfo[nObject];
	int Off = int(m_dWidth*0.5*nWidth);
	float * fData = p->pCalcData[nOutNum];
	HandleValue * phv = NULL;
	if(TBE_MEANLESS(fData[m_nDataNum])) 
	{
		DWORD pdVal;
		memcpy(&pdVal, &fData[m_nDataNum], sizeof(DWORD));
		phv = (HandleValue *)pdVal;
		if(phv->nValidFlag!=TMEANLESS_INT)
			phv = NULL;
	}
	if(!phv) return;
	float *fTmpHigh = &(phv->pFloat[0]);
	float *fTmpOpen = &(phv->pFloat[m_nDataNum]);
	float *fTmpLow = &(phv->pFloat[2*m_nDataNum]);
	float *fTmpClose = &(phv->pFloat[3*m_nDataNum]);
	float *fHigh = fTmpHigh + m_nLeftNo;
	float *fOpen = fTmpOpen + m_nLeftNo;
	float *fLow = fTmpLow + m_nLeftNo;
	float *fClose = fTmpClose + m_nLeftNo;

	for ( i=0;i<m_nDrawNum;i++ )
	{	
		CPen *oldpen = g_d.SelectPen(pDC,KP_PEN);
		if (TBE_MEANLESS(fHigh[i])!=0&&TBE_MEANLESS(fOpen[i])!=0&&TBE_MEANLESS(fLow[i])!=0&&TBE_MEANLESS(fClose[i])!=0)
		{
			if ( fClose[i] > fOpen[i]+COMPPREC )
			{
				yC = GetYPosOfRegion(nRegion,fClose[i],nObject) ;
				yO = GetYPosOfRegion(nRegion,fOpen[i],nObject) ;
				yH = GetYPosOfRegion(nRegion,fHigh[i],nObject) ;
				yL = GetYPosOfRegion(nRegion,fLow[i],nObject) ;
				if(g_GH.bRealUPK)
				{
					g_d.Bar(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yO,UP_BRUSH);
					g_d.DrawLine(pDC,m_pPosOfX[i],yL,m_pPosOfX[i],yH);
				}
				else
				{
					g_d.Bar(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yO,BACK_BRUSH);
					g_d.DrawRect(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yO);
					g_d.DrawLine(pDC,m_pPosOfX[i],yC,m_pPosOfX[i],yH);
					g_d.DrawLine(pDC,m_pPosOfX[i],yO,m_pPosOfX[i],yL);
				}
			}
			else if ( fClose[i]+COMPPREC < fOpen[i] )
			{
				g_d.SelectPen(pDC, KN_PEN);
				yC = GetYPosOfRegion(nRegion,fClose[i],nObject) ;
				yO = GetYPosOfRegion(nRegion,fOpen[i],nObject) ;
				yH = GetYPosOfRegion(nRegion,fHigh[i],nObject) ;
				yL = GetYPosOfRegion(nRegion,fLow[i],nObject) ;
				g_d.Bar(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yO,KN_BRUSH);
				g_d.DrawLine(pDC,m_pPosOfX[i],yL,m_pPosOfX[i],yH);
			}
			else
			{
				g_d.SelectPen(pDC, LEVEL_PEN);
				yC = GetYPosOfRegion(nRegion,fClose[i],nObject) ;
				yH = GetYPosOfRegion(nRegion,fHigh[i],nObject) ;
				yL = GetYPosOfRegion(nRegion,fLow[i],nObject) ;
				g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yC);
				g_d.DrawLine(pDC,m_pPosOfX[i]+Off,yC,m_pPosOfX[i]-Off,yC);
				g_d.DrawLine(pDC,m_pPosOfX[i],yH,m_pPosOfX[i],yL);
				g_d.DrawLine(pDC,m_pPosOfX[i],yL,m_pPosOfX[i],yH);
			}
		}
	}
}

void UFxt::DrawKLineEx(CDC *pDC, int nRegion, int nObject, int nOutNum, int nWidth, BOOL bSelfColor, DWORD nColor)
{
	if(m_nDataNum<=0) return;
	int i,yC,yO,yH,yL;
	TOBJECT * p = m_aRegion[nRegion].aIndexInfo[nObject];
	int Off = int(m_dWidth*0.5);
	float * fData = p->pCalcData[nOutNum];

	HandleValue * phv = NULL;
	if(TBE_MEANLESS(fData[m_nDataNum])) 
	{
		DWORD pdVal;
		memcpy(&pdVal, &fData[m_nDataNum], sizeof(DWORD));
		phv = (HandleValue *)pdVal;
		if(phv->nValidFlag!=TMEANLESS_INT)
			phv = NULL;
	}
	if(!phv) return;
	float *fTmpCond = &(phv->pFloat[0]);
	float *fTmpHigh = &(phv->pFloat[m_nDataNum]);
	float *fTmpOpen = &(phv->pFloat[2*m_nDataNum]);
	float *fTmpLow = &(phv->pFloat[3*m_nDataNum]);
	float *fTmpClose = &(phv->pFloat[4*m_nDataNum]);
	float *fCond = fTmpCond + m_nLeftNo;
	float *fHigh = fTmpHigh + m_nLeftNo;
	float *fOpen = fTmpOpen + m_nLeftNo;
	float *fLow = fTmpLow + m_nLeftNo;
	float *fClose = fTmpClose + m_nLeftNo;
	int nMainZbFlag = g_nMainZbFlag;
	BOOL bRealUPK = g_GH.bRealUPK;
	
	CPen  KUpPen;
	CPen  KDownPen;
	CPen  KLevelPen;
	CPen *oldpen;
	if(bSelfColor)
	{
		KUpPen.CreatePen(PS_SOLID, nWidth, (COLORREF)nColor);
		KDownPen.CreatePen(PS_SOLID, nWidth, (COLORREF)nColor);
		KLevelPen.CreatePen(PS_SOLID, nWidth, (COLORREF)nColor);
	}
	else
	{
		KUpPen.CreatePen(PS_SOLID, nWidth, (COLORREF)VipColor.KPCOLOR);
		KDownPen.CreatePen(PS_SOLID, nWidth, (COLORREF)VipColor.KNCOLOR);
		KLevelPen.CreatePen(PS_SOLID, nWidth, (COLORREF)VipColor.LEVELCOLOR);
	}
	oldpen = pDC->SelectObject(&KUpPen);

	switch(nMainZbFlag)
	{
	case 2:
	case 3:
	case 4:
	case 0:		//K线
		for ( i=0;i<m_nDrawNum;i++ )
		{
			if(m_nLeftNo+i >= m_nDataNum)
			{
				break;
			}
			if(TBE_MEANLESS(fCond[i])==0 || fCond[i]<0.0001) continue;
			if(TBE_MEANLESS(fHigh[i])!=0&&TBE_MEANLESS(fOpen[i])!=0&&TBE_MEANLESS(fLow[i])!=0&&TBE_MEANLESS(fClose[i])!=0)
			{
				yC = GetYPosOfRegion(nRegion,fClose[i],nObject) ;
				yO = GetYPosOfRegion(nRegion,fOpen[i],nObject) ;
				yH = GetYPosOfRegion(nRegion,fHigh[i],nObject) ;
				yL = GetYPosOfRegion(nRegion,fLow[i],nObject) ;
				if ( fClose[i] > fOpen[i] )
				{
					pDC->SelectObject(&KUpPen);
					if(!bRealUPK)
					{
						g_d.Bar(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yO,BACK_BRUSH);
						g_d.DrawRect(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yO);
						g_d.DrawLine(pDC,m_pPosOfX[i],yC,m_pPosOfX[i],yH);
						g_d.DrawLine(pDC,m_pPosOfX[i],yO,m_pPosOfX[i],yL);
					}
					else
					{
						if(bSelfColor)	FillBar(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yO,nColor);
						else g_d.Bar(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yO,UP_BRUSH);
						g_d.DrawLine(pDC,m_pPosOfX[i],yL,m_pPosOfX[i],yH);
					}
				}
				else if ( fClose[i] < fOpen[i] )
				{
					pDC->SelectObject(&KDownPen);
					if(bSelfColor)	FillBar(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yO,nColor);
					else g_d.Bar(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yO,KN_BRUSH);
					g_d.DrawLine(pDC,m_pPosOfX[i],yL,m_pPosOfX[i],yH);
				}
				else
				{
					pDC->SelectObject(&KLevelPen);
					g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yC);					
					g_d.DrawLine(pDC,m_pPosOfX[i]+Off,yC,m_pPosOfX[i]-Off,yC);
					g_d.DrawLine(pDC,m_pPosOfX[i],yH,m_pPosOfX[i],yL);
				}
			}
		}
		break;
	case 1:		//美国线
		for ( i=0;i<m_nDrawNum;i++ )
		{
			if(m_nLeftNo+i >= m_nDataNum)
			{
				break;
			}
			if(TBE_MEANLESS(fCond[i])==0 || fCond[i]<0.0001) continue;
			if(TBE_MEANLESS(fHigh[i])!=0&&TBE_MEANLESS(fOpen[i])!=0&&TBE_MEANLESS(fLow[i])!=0&&TBE_MEANLESS(fClose[i])!=0)
			{
				yC = GetYPosOfRegion(nRegion,fClose[i],nObject) ;
				yO = GetYPosOfRegion(nRegion,fOpen[i],nObject) ;
				yH = GetYPosOfRegion(nRegion,fHigh[i],nObject) ;
				yL = GetYPosOfRegion(nRegion,fLow[i],nObject) ;
				if ( fClose[i] > fOpen[i] )
					pDC->SelectObject(&KUpPen);
				else if ( fClose[i] < fOpen[i] )
					pDC->SelectObject(&KDownPen);
				else pDC->SelectObject(&KLevelPen);

				g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yL,m_pPosOfX[i]-Off,yH);
				g_d.DrawLine(pDC,m_pPosOfX[i]+Off,yC,m_pPosOfX[i]-Off,yC);
				g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yC);
			}
		}
		break;
	}

	pDC->SelectObject(oldpen);
	KUpPen.DeleteObject();
	KDownPen.DeleteObject();
	KLevelPen.DeleteObject();
}

void UFxt::DrawQJLine(CDC *pDC)
{
	if(m_nQJPos1>=0)
	{
		for(int i=0;i<m_nRegion;i++)
		{
			g_d.DotLine(pDC, m_pPosOfX[m_nQJPos1-m_nLeftNo]+left, top+m_aRegion[i].Area.top+TEXTHIGH, 
				m_pPosOfX[m_nQJPos1-m_nLeftNo]+left, top+m_aRegion[i].Area.bottom, VipColor.DRAWLINETOOLCOLOR/*^VipColor.BACKCOLOR*/);
		}
	}
	if(m_nQJPos2>=0)
	{
		for(int i=0;i<m_nRegion;i++)
		{
			g_d.DotLine(pDC, m_pPosOfX[m_nQJPos2-m_nLeftNo]+left, top+m_aRegion[i].Area.top+TEXTHIGH, 
				m_pPosOfX[m_nQJPos2-m_nLeftNo]+left, top+m_aRegion[i].Area.bottom, VipColor.DRAWLINETOOLCOLOR/*^VipColor.BACKCOLOR*/);
		}
	}
}

void UFxt::DrawHisLine(CDC * pDC, int nRegionNo,int nObject,int nDrawMode ,int nFixRegion,BYTE nCurrPen,BYTE nCurrBrush)
{
	double dw=m_dWidth+m_dDistance;
	int xx1,yy1,xx2,yy2;
	COLORREF pslcolor;
	int i,j,yC,yO,yH,yL,Off = int(m_dWidth*0.5);
	BOOL bIsOverlap;
	BYTE * pInfo = new BYTE[m_nDrawNum];
	int OldFixRegion=nFixRegion;
	if ( nFixRegion <0 )
		nFixRegion = nRegionNo;
	TOBJECT * p = m_aRegion[nRegionNo].aIndexInfo[nObject];
	TINDEXINFO *pTmpIndex = NULL;
	for (i = 0; i < m_aRegion[0].nObject; i++)
	{
		if (m_aRegion[0].aIndexInfo[i]->nSet == KL_TYPE)
		{
			pTmpIndex = g_pCalc->_CallDTopViewCalc_8(m_aRegion[0].aIndexInfo[i]->nSet, m_aRegion[0].aIndexInfo[i]->nGSIndex);
			break;
		}
	}
	memset(pInfo,0,m_nDrawNum);
	struct AnalyData * pHisDay = p->pHisDay + m_nLeftNo;
	float  *pCalcData = NULL;
	CPen  *oldpen;
	bIsOverlap = strcmp(p->acCode, "OVERLAPK") == 0;
	if(bIsOverlap&&m_nSpecProcDraw>0) //特殊涨跌周期不要画叠加
	{
		TDELA(pInfo);
		return;
	}
	double dOverPer = 0;
	if (bIsOverlap)
		oldpen = g_d.SelectPen(pDC,m_nOverlapPenPos);
	else
		oldpen = g_d.SelectPen(pDC,KP_PEN);
	if ( m_bKColor && nRegionNo == 0)		//五彩K线
	{
		if (bIsOverlap)
			g_d.SelectPen(pDC,m_nOverlapPenPos);
		else
			g_d.SelectPen(pDC, MAX_PEN_NUM+DEFAULTKCOLOR);
		if (nDrawMode == 0)	//画K线
		{
			//首先将整个K线置为一种颜色
			for (i = 0; i < m_nDrawNum; i++)
			{
				if ( memcmp(pHisDay+i,&l_InValidHisDat,sizeof(AnalyData)) == 0 )
					continue;
				if ( pHisDay[i].Close > pHisDay[i].Open )
				{
					yC = GetYPosOfRegion(nFixRegion,pHisDay[i].Close,nObject) ;
					yO = GetYPosOfRegion(nFixRegion,pHisDay[i].Open ,nObject) ;
					yH = GetYPosOfRegion(nFixRegion,pHisDay[i].High ,nObject) ;
					yL = GetYPosOfRegion(nFixRegion,pHisDay[i].Low  ,nObject) ;
					//实心阳线和空心阳线
					if(!g_GH.bRealUPK)
					{
					//	g_d.Bar(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yO,BACK_BRUSH);
						g_d.Draw3DBar2(pDC,CRect(m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yO),VipColor.BACKCOLOR,FALSE);
						g_d.DrawRect(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yO);
						g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yC);
						g_d.DrawLine(pDC,m_pPosOfX[i]+Off,yC,m_pPosOfX[i]-Off,yC);
						g_d.DrawLine(pDC,m_pPosOfX[i],yC,m_pPosOfX[i],yH);
						g_d.DrawLine(pDC,m_pPosOfX[i],yH,m_pPosOfX[i],yC);
						g_d.DrawLine(pDC,m_pPosOfX[i],yO,m_pPosOfX[i],yL);
						g_d.DrawLine(pDC,m_pPosOfX[i],yL,m_pPosOfX[i],yO);
					}
					else
					{
						g_d.Bar(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yO,bIsOverlap?m_nOverlapBrushPos:MAX_BRUSH_NUM + DEFAULTKCOLOR);
						g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yC);
						g_d.DrawLine(pDC,m_pPosOfX[i]+Off,yC,m_pPosOfX[i]-Off,yC);
						g_d.DrawLine(pDC,m_pPosOfX[i],yL,m_pPosOfX[i],yH);
						g_d.DrawLine(pDC,m_pPosOfX[i],yH,m_pPosOfX[i],yL);
					}
				}
				else if ( pHisDay[i].Close < pHisDay[i].Open )
				{
					yC = GetYPosOfRegion(nFixRegion,pHisDay[i].Close,nObject) ;
					yO = GetYPosOfRegion(nFixRegion,pHisDay[i].Open ,nObject) ;
					yH = GetYPosOfRegion(nFixRegion,pHisDay[i].High ,nObject) ;
					yL = GetYPosOfRegion(nFixRegion,pHisDay[i].Low  ,nObject) ;
					g_d.Bar(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yO,bIsOverlap?m_nOverlapBrushPos:MAX_BRUSH_NUM + DEFAULTKCOLOR);
					g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yC);
					g_d.DrawLine(pDC,m_pPosOfX[i]+Off,yC,m_pPosOfX[i]-Off,yC);
					g_d.DrawLine(pDC,m_pPosOfX[i],yL,m_pPosOfX[i],yH);
					g_d.DrawLine(pDC,m_pPosOfX[i],yH,m_pPosOfX[i],yL);
				}
				else
				{
					yC = GetYPosOfRegion(nFixRegion,pHisDay[i].Close,nObject) ;
					yH = GetYPosOfRegion(nFixRegion,pHisDay[i].High ,nObject) ;
					yL = GetYPosOfRegion(nFixRegion,pHisDay[i].Low  ,nObject) ;
					g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yC);
					g_d.DrawLine(pDC,m_pPosOfX[i]+Off,yC,m_pPosOfX[i]-Off,yC);
					g_d.DrawLine(pDC,m_pPosOfX[i],yH,m_pPosOfX[i],yL);
					g_d.DrawLine(pDC,m_pPosOfX[i],yL,m_pPosOfX[i],yH);
				}
			}
			for (j = 0; j < MAX_LINEOUT; j++)
			{
				if (!pTmpIndex) break;
				if (j == pTmpIndex->nLineNum) break;
				if (bIsOverlap)
					g_d.SelectPen(pDC,m_nOverlapPenPos);
				else
					g_d.SelectPen(pDC, pTmpIndex->aLine[j].nType + MAX_PEN_NUM);
				pCalcData = m_KLZS.p->pCalcData[j]+m_nLeftNo;
				for (i = 0; i < m_nDrawNum; i++)
				{
					if ( memcmp(pHisDay+i,&l_InValidHisDat,sizeof(AnalyData)) == 0 )
						continue;
					//是有效数并且不为0
					if (TBE_MEANLESS(*pCalcData) && fabs(*pCalcData - 0) > COMPPREC)
					{
						if ( pHisDay[i].Close > pHisDay[i].Open )
						{
							yC = GetYPosOfRegion(nFixRegion,pHisDay[i].Close,nObject) ;
							yO = GetYPosOfRegion(nFixRegion,pHisDay[i].Open ,nObject) ;
							yH = GetYPosOfRegion(nFixRegion,pHisDay[i].High ,nObject) ;
							yL = GetYPosOfRegion(nFixRegion,pHisDay[i].Low  ,nObject) ;
							//实心阳线和空心阳线
							if(!g_GH.bRealUPK)
							{
							//	g_d.Bar(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yO,BACK_BRUSH);
								g_d.Draw3DBar2(pDC,CRect(m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yO),VipColor.BACKCOLOR,FALSE);
								g_d.DrawRect(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yO);
								g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yC);
								g_d.DrawLine(pDC,m_pPosOfX[i]+Off,yC,m_pPosOfX[i]-Off,yC);
								g_d.DrawLine(pDC,m_pPosOfX[i],yC,m_pPosOfX[i],yH);
								g_d.DrawLine(pDC,m_pPosOfX[i],yH,m_pPosOfX[i],yC);
								g_d.DrawLine(pDC,m_pPosOfX[i],yO,m_pPosOfX[i],yL);
								g_d.DrawLine(pDC,m_pPosOfX[i],yL,m_pPosOfX[i],yO);
							}
							else
							{
								g_d.Bar(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yO,bIsOverlap?m_nOverlapBrushPos:MAX_BRUSH_NUM + pTmpIndex->aLine[j].nType);
								g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yC);
								g_d.DrawLine(pDC,m_pPosOfX[i]+Off,yC,m_pPosOfX[i]-Off,yC);
								g_d.DrawLine(pDC,m_pPosOfX[i],yL,m_pPosOfX[i],yH);
								g_d.DrawLine(pDC,m_pPosOfX[i],yH,m_pPosOfX[i],yL);
							}
						}
						else if ( pHisDay[i].Close < pHisDay[i].Open )
						{
							yC = GetYPosOfRegion(nFixRegion,pHisDay[i].Close,nObject) ;
							yO = GetYPosOfRegion(nFixRegion,pHisDay[i].Open ,nObject) ;
							yH = GetYPosOfRegion(nFixRegion,pHisDay[i].High ,nObject) ;
							yL = GetYPosOfRegion(nFixRegion,pHisDay[i].Low  ,nObject) ;
							g_d.Bar(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yO,bIsOverlap?m_nOverlapBrushPos:MAX_BRUSH_NUM + pTmpIndex->aLine[j].nType);
							g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yC);
							g_d.DrawLine(pDC,m_pPosOfX[i]+Off,yC,m_pPosOfX[i]-Off,yC);
							g_d.DrawLine(pDC,m_pPosOfX[i],yL,m_pPosOfX[i],yH);
							g_d.DrawLine(pDC,m_pPosOfX[i],yH,m_pPosOfX[i],yL);
						}
						else
						{
							yC = GetYPosOfRegion(nFixRegion,pHisDay[i].Close,nObject) ;
							yH = GetYPosOfRegion(nFixRegion,pHisDay[i].High ,nObject) ;
							yL = GetYPosOfRegion(nFixRegion,pHisDay[i].Low  ,nObject) ;
							g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yC);
							g_d.DrawLine(pDC,m_pPosOfX[i]+Off,yC,m_pPosOfX[i]-Off,yC);
							g_d.DrawLine(pDC,m_pPosOfX[i],yH,m_pPosOfX[i],yL);
							g_d.DrawLine(pDC,m_pPosOfX[i],yL,m_pPosOfX[i],yH);
						}
					}
					pCalcData++;
				}
			}
		}
		else if(nDrawMode == 1) //画BAR线
		{
			//首先将整个K线置为一种颜色
			for (i = 0; i < m_nDrawNum; i++)
			{
				if ( memcmp(pHisDay+i,&l_InValidHisDat,sizeof(AnalyData)) == 0 )
					continue;
				if ( pHisDay[i].Close > pHisDay[i].Open )
				{
					yC = GetYPosOfRegion(nFixRegion,pHisDay[i].Close,nObject) ;
					yH = GetYPosOfRegion(nFixRegion,pHisDay[i].High ,nObject) ;
					yL = GetYPosOfRegion(nFixRegion,pHisDay[i].Low  ,nObject) ;
					g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yL,m_pPosOfX[i]-Off,yH);
					g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yC);
				}
				else
				if ( pHisDay[i].Close < pHisDay[i].Open )
				{
					yC = GetYPosOfRegion(nFixRegion,pHisDay[i].Close,nObject) ;
					yH = GetYPosOfRegion(nFixRegion,pHisDay[i].High ,nObject) ;
					yL = GetYPosOfRegion(nFixRegion,pHisDay[i].Low  ,nObject) ;
					g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yL,m_pPosOfX[i]-Off,yH);
					g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yC);
				}
				else
				{
					yC = GetYPosOfRegion(nFixRegion,pHisDay[i].Close,nObject) ;
					yH = GetYPosOfRegion(nFixRegion,pHisDay[i].High ,nObject) ;
					yL = GetYPosOfRegion(nFixRegion,pHisDay[i].Low  ,nObject) ;
					g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yL,m_pPosOfX[i]-Off,yH);
					g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yH,m_pPosOfX[i]-Off,yL);
					g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yC);
					g_d.DrawLine(pDC,m_pPosOfX[i]+Off,yC,m_pPosOfX[i]-Off,yC);
				}
			}
			for (j = 0; j < MAX_LINEOUT; j++)
			{
				if (!pTmpIndex) break;
				if (j == pTmpIndex->nLineNum) break;
				if (bIsOverlap)
					g_d.SelectPen(pDC,m_nOverlapPenPos);
				else
					g_d.SelectPen(pDC, pTmpIndex->aLine[j].nType + MAX_PEN_NUM);
				pCalcData = m_KLZS.p->pCalcData[j]+m_nLeftNo;
				for (i = 0; i < m_nDrawNum; i++)
				{
					if ( memcmp(pHisDay+i,&l_InValidHisDat,sizeof(AnalyData)) == 0 )
						continue;
					//是有效数并且不为0
					if (TBE_MEANLESS(*pCalcData) && fabs(*pCalcData - 0) > COMPPREC)
					{
						if ( pHisDay[i].Close > pHisDay[i].Open )
						{
							yC = GetYPosOfRegion(nFixRegion,pHisDay[i].Close,nObject) ;
							yH = GetYPosOfRegion(nFixRegion,pHisDay[i].High ,nObject) ;
							yL = GetYPosOfRegion(nFixRegion,pHisDay[i].Low  ,nObject) ;
							g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yL,m_pPosOfX[i]-Off,yH);
							g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yC);
						}
						else
						if ( pHisDay[i].Close < pHisDay[i].Open )
						{
							yC = GetYPosOfRegion(nFixRegion,pHisDay[i].Close,nObject) ;
							yH = GetYPosOfRegion(nFixRegion,pHisDay[i].High ,nObject) ;
							yL = GetYPosOfRegion(nFixRegion,pHisDay[i].Low  ,nObject) ;
							g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yL,m_pPosOfX[i]-Off,yH);
							g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yC);
						}
						else
						{
							yC = GetYPosOfRegion(nFixRegion,pHisDay[i].Close,nObject) ;
							yH = GetYPosOfRegion(nFixRegion,pHisDay[i].High ,nObject) ;
							yL = GetYPosOfRegion(nFixRegion,pHisDay[i].Low  ,nObject) ;
							g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yL,m_pPosOfX[i]-Off,yH);
							g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yH,m_pPosOfX[i]-Off,yL);
							g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yC);
							g_d.DrawLine(pDC,m_pPosOfX[i]+Off,yC,m_pPosOfX[i]-Off,yC);
						}
					}
					pCalcData++;
				}
			}
		}
		else if(nDrawMode == 2 || nDrawMode == 3)	//画收盘(站点)线
		{
			if(bIsOverlap)
				pslcolor = l_KLineColor[m_nOverlapPenPos - MAX_PEN_NUM];
			else
				pslcolor = VipColor.KPCOLOR;

			if (bIsOverlap)
				g_d.SelectPen(pDC, m_nOverlapPenPos);
			else
				g_d.SelectPen(pDC,KP_PEN);

			xx2 = m_pPosOfX[0]-1;
			yy2 = GetYPosOfRegion(nFixRegion,pHisDay[0].Close,nObject) ;
			for(i = 0; i < m_nDrawNum - 1; i++)
			{
				if(m_nLeftNo+i+1 >= m_nDataNum - g_RMSp)
					break;
				xx1 = xx2;
				yy1 = yy2;
				xx2 = m_pPosOfX[i+1]-1;
				yy2 = GetYPosOfRegion(nFixRegion,pHisDay[i+1].Close,nObject) ;
				g_d.DrawLine(pDC,xx1,yy1,xx2,yy2,pslcolor);
			}
			if(nDrawMode == 3)
			{
				for(i = 0; i < m_nDrawNum; i++)
				{
					if(m_nLeftNo+i >= m_nDataNum - g_RMSp)
						break;
					if ( memcmp(pHisDay+i,&l_InValidHisDat,sizeof(AnalyData)) == 0 )
						continue;
					if (TBE_MEANLESS(pHisDay[i].Close) && fabs(pHisDay[i].Close - 0) > COMPPREC)
					{				
						yC = GetYPosOfRegion(nFixRegion,pHisDay[i].Close,nObject) ;
						if (dw>=6)
							g_d.DrawO(pDC, m_pPosOfX[i], yC, &pslcolor);
						else
						{
							pDC->SetPixel(m_pPosOfX[i]-1,yC,pslcolor);
							pDC->SetPixel(m_pPosOfX[i],yC-1,pslcolor);
							pDC->SetPixel(m_pPosOfX[i],yC+1,pslcolor);
							pDC->SetPixel(m_pPosOfX[i]+1,yC,pslcolor);
						}
					}
				}
			}
		}
		else
		{
			for (i = 0; i < m_nDrawNum; i++)
			{
				if(m_nLeftNo+i >= m_nDataNum)
				{
					break;
				}
				if (i < 2) continue;
				int flag=0;

				if (pHisDay[i].Close  >=pHisDay[i-1].Close) flag+=1;
				if (pHisDay[i-1].Close>=pHisDay[i-2].Close) flag+=10;
				int y11  = GetYPosOfRegion(nRegionNo, MAX(pHisDay[i].Close,pHisDay[i-1].Close));
				int y22  = GetYPosOfRegion(nRegionNo, MIN(pHisDay[i].Close,pHisDay[i-1].Close));
				int yhh  = GetYPosOfRegion(nRegionNo, MAX(pHisDay[i-2].Close,pHisDay[i-1].Close));
				int yll  = GetYPosOfRegion(nRegionNo, MIN(pHisDay[i-2].Close,pHisDay[i-1].Close));

				switch (flag)
				{
				case YUP_TUP:
				//	g_d.Bar(pDC,m_pPosOfX[i]-Off,yhh,m_pPosOfX[i]+Off,y11,FIX_RED_BRUSH);
					g_d.Draw3DBar2(pDC,CRect(m_pPosOfX[i]-Off,yhh,m_pPosOfX[i]+Off,y11),RGB(255,0,0),FALSE);
					break;
				case YUP_TDOWN:
					if (y22<=yll)
					//	g_d.Bar(pDC,m_pPosOfX[i]-Off,yhh,m_pPosOfX[i]+Off,y22,FIX_RED_BRUSH);
						g_d.Draw3DBar2(pDC,CRect(m_pPosOfX[i]-Off,yhh,m_pPosOfX[i]+Off,y22),RGB(255,0,0),FALSE);
					else
					{
					//	g_d.Bar(pDC,m_pPosOfX[i]-Off,yll,m_pPosOfX[i]+Off,y22,FIX_GREEN_BRUSH);
						g_d.Draw3DBar2(pDC,CRect(m_pPosOfX[i]-Off,yll,m_pPosOfX[i]+Off,y22),RGB(0,255,0),FALSE);
					//	g_d.Bar(pDC,m_pPosOfX[i]-Off,yhh,m_pPosOfX[i]+Off,yll,FIX_RED_BRUSH);
						g_d.Draw3DBar2(pDC,CRect(m_pPosOfX[i]-Off,yhh,m_pPosOfX[i]+Off,yll),RGB(255,0,0),FALSE);
					}
					break;
				case YDOWN_TUP:
					if (y11>=yhh)
					//	g_d.Bar(pDC,m_pPosOfX[i]-Off,yll,m_pPosOfX[i]+Off,y11,FIX_GREEN_BRUSH);
						g_d.Draw3DBar2(pDC,CRect(m_pPosOfX[i]-Off,yll,m_pPosOfX[i]+Off,y11),RGB(0,255,0),FALSE);
					else
					{
					//	g_d.Bar(pDC,m_pPosOfX[i]-Off,yll,m_pPosOfX[i]+Off,yhh,FIX_GREEN_BRUSH);
						g_d.Draw3DBar2(pDC,CRect(m_pPosOfX[i]-Off,yll,m_pPosOfX[i]+Off,yhh),RGB(0,255,0),FALSE);
					//	g_d.Bar(pDC,m_pPosOfX[i]-Off,yhh,m_pPosOfX[i]+Off,y11,FIX_RED_BRUSH);
						g_d.Draw3DBar2(pDC,CRect(m_pPosOfX[i]-Off,yhh,m_pPosOfX[i]+Off,y11),RGB(255,0,0),FALSE);
					}
					break;
				case YDOWN_TDOWN:
				//	g_d.Bar(pDC,m_pPosOfX[i]-Off,yll,m_pPosOfX[i]+Off,y22,FIX_GREEN_BRUSH);
					g_d.Draw3DBar2(pDC,CRect(m_pPosOfX[i]-Off,yll,m_pPosOfX[i]+Off,y22),RGB(0,255,0),FALSE);
					break;
				}
			}
		}
	}
	else
	{
		if(m_nSpecProcDraw==2&&nRegionNo==0&&nObject==0&&m_nDrawNum>1)	//主图根据涨跌K线得到的OX
		{
			int nZJ = (m_pPosOfX[m_nDrawNum-1]-m_pPosOfX[0])/(m_nDrawNum-1);
			nZJ = max(2,nZJ);
			int nVerNum = (m_aRegion[nRegionNo].DrawRc.bottom-m_aRegion[nRegionNo].DrawRc.top)/nZJ;
			for ( i=m_nDrawNum-1;i>=0;i-- )
			{				
				if ( memcmp(pHisDay+i,&l_InValidHisDat,sizeof(AnalyData)) == 0 )
					continue;
				int yH = GetYPosOfRegion(nFixRegion,pHisDay[i].High ,nObject) ;
				int yL = GetYPosOfRegion(nFixRegion,pHisDay[i].Low  ,nObject) ;
				int nyH = (yH-m_aRegion[nRegionNo].DrawRc.top)/nZJ;
				int nyL = (m_aRegion[nRegionNo].DrawRc.bottom-yL)/nZJ;
				yH = m_aRegion[nRegionNo].DrawRc.top+nyH*nZJ;
				yL = m_aRegion[nRegionNo].DrawRc.bottom-nyL*nZJ;
				if(pHisDay[i].Close+COMPPREC < pHisDay[i-1].Close)			//涨画O
				{
					CBrush *oldb = g_d.SelectBrush(pDC,BACK_BRUSH);
					g_d.SelectPen(pDC, KN_PEN);
					for(int k=yH;k<=yL;k+=nZJ)
						g_d.DrawO(pDC,m_pPosOfX[i],k,NULL,nZJ/2);
					g_d.RestoreBrush(pDC,oldb);
				}
				else														//涨画X
				{
					g_d.SelectPen(pDC, KP_PEN);
					for(int k=yH;k<=yL;k+=nZJ)
						g_d.DrawX(pDC,m_pPosOfX[i],k,nZJ/2);
				}
			}
		}
		else if (nDrawMode == 0)	//画K线
		{
			for ( i=0;i<m_nDrawNum;i++ )
			{
				if ( memcmp(pHisDay+i,&l_InValidHisDat,sizeof(AnalyData)) == 0 )
					continue;
				if ( pHisDay[i].Close > pHisDay[i].Open+COMPPREC )
				{
					pInfo[i] = 1;
					yC = GetYPosOfRegion(nFixRegion,pHisDay[i].Close,nObject) ;
					yO = GetYPosOfRegion(nFixRegion,pHisDay[i].Open ,nObject) ;
					yH = GetYPosOfRegion(nFixRegion,pHisDay[i].High ,nObject) ;
					yL = GetYPosOfRegion(nFixRegion,pHisDay[i].Low  ,nObject) ;
					//实心阳线和空心阳线
					if(!g_GH.bRealUPK)
					{
					//	g_d.Bar(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yO,BACK_BRUSH);
						g_d.Draw3DBar3(pDC,CRect(m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yO),VipColor.BACKCOLOR,TRUE,FALSE,g_bShow3DKLine);
						g_d.DrawRect(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yO);
						g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yC);
						g_d.DrawLine(pDC,m_pPosOfX[i]+Off,yC,m_pPosOfX[i]-Off,yC);
						g_d.DrawLine(pDC,m_pPosOfX[i],yC,m_pPosOfX[i],yH);
						g_d.DrawLine(pDC,m_pPosOfX[i],yH,m_pPosOfX[i],yC);
						g_d.DrawLine(pDC,m_pPosOfX[i],yO,m_pPosOfX[i],yL);
						g_d.DrawLine(pDC,m_pPosOfX[i],yL,m_pPosOfX[i],yO);
					}
					else
					{
					//	g_d.Bar(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yO,bIsOverlap?m_nOverlapBrushPos:UP_BRUSH);
						if(bIsOverlap)
							g_d.Bar(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yO,m_nOverlapBrushPos);
						else g_d.Draw3DBar3(pDC,CRect(m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yO),VipColor.KPCOLOR,TRUE,FALSE,g_bShow3DKLine);
						g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yC);
						g_d.DrawLine(pDC,m_pPosOfX[i]+Off,yC,m_pPosOfX[i]-Off,yC);
						g_d.DrawLine(pDC,m_pPosOfX[i],yL,m_pPosOfX[i],yH);
						g_d.DrawLine(pDC,m_pPosOfX[i],yH,m_pPosOfX[i],yL);
					}
				}
			}
			if (bIsOverlap)
				g_d.SelectPen(pDC, m_nOverlapPenPos);
			else
				g_d.SelectPen(pDC,KN_PEN);
			for ( i=0;i<m_nDrawNum;i++ )
			{
				if ( memcmp(pHisDay+i,&l_InValidHisDat,sizeof(AnalyData)) == 0 )
					continue;
				if ( pHisDay[i].Close+COMPPREC < pHisDay[i].Open )
				{
					pInfo[i] = 1;
					yC = GetYPosOfRegion(nFixRegion,pHisDay[i].Close,nObject) ;
					yO = GetYPosOfRegion(nFixRegion,pHisDay[i].Open ,nObject) ;
					yH = GetYPosOfRegion(nFixRegion,pHisDay[i].High ,nObject) ;
					yL = GetYPosOfRegion(nFixRegion,pHisDay[i].Low  ,nObject) ;
				//	g_d.Bar(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yO,bIsOverlap?m_nOverlapBrushPos:KN_BRUSH);
					if(bIsOverlap)
						g_d.Bar(pDC,m_pPosOfX[i]-Off,yO,m_pPosOfX[i]+Off,yC,m_nOverlapBrushPos);
					else g_d.Draw3DBar3(pDC,CRect(m_pPosOfX[i]-Off,yO,m_pPosOfX[i]+Off,yC),VipColor.KNCOLOR,TRUE,FALSE,g_bShow3DKLine);
					g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yC);
					g_d.DrawLine(pDC,m_pPosOfX[i]+Off,yC,m_pPosOfX[i]-Off,yC);
					g_d.DrawLine(pDC,m_pPosOfX[i],yL,m_pPosOfX[i],yH);
					g_d.DrawLine(pDC,m_pPosOfX[i],yH,m_pPosOfX[i],yL);
				}
			}
			if (bIsOverlap)
				g_d.SelectPen(pDC, m_nOverlapPenPos);
			else
				g_d.SelectPen(pDC,LEVEL_PEN);
			for ( i=0;i<m_nDrawNum;i++ )
			{
				if ( memcmp(pHisDay+i,&l_InValidHisDat,sizeof(AnalyData)) == 0 )
					continue;
				if ( !pInfo[i]  )
				{
					yC = GetYPosOfRegion(nFixRegion,pHisDay[i].Close,nObject) ;
					yH = GetYPosOfRegion(nFixRegion,pHisDay[i].High ,nObject) ;
					yL = GetYPosOfRegion(nFixRegion,pHisDay[i].Low  ,nObject) ;
					if(Off<1)
						pDC->SetPixel(m_pPosOfX[i],yC,VipColor.LEVELCOLOR);
					else
					{
						g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yC);
						g_d.DrawLine(pDC,m_pPosOfX[i]+Off,yC,m_pPosOfX[i]-Off,yC);
					}
					g_d.DrawLine(pDC,m_pPosOfX[i],yH,m_pPosOfX[i],yL);
					g_d.DrawLine(pDC,m_pPosOfX[i],yL,m_pPosOfX[i],yH);
				}
			}
		}
		else if(nDrawMode == 1)	//画BAR线
		{
			for ( i=0;i<m_nDrawNum;i++ )
			{
				if ( memcmp(pHisDay+i,&l_InValidHisDat,sizeof(AnalyData)) == 0 )
					continue;
				if ( pHisDay[i].Close > pHisDay[i].Open+COMPPREC )
				{
					pInfo[i] = 1;
					yC = GetYPosOfRegion(nFixRegion,pHisDay[i].Close,nObject) ;
					yH = GetYPosOfRegion(nFixRegion,pHisDay[i].High ,nObject) ;
					yL = GetYPosOfRegion(nFixRegion,pHisDay[i].Low  ,nObject) ;
					g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yL,m_pPosOfX[i]-Off,yH);
					g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yC);
				}
			}
			if (bIsOverlap)
				g_d.SelectPen(pDC, m_nOverlapPenPos);
			else
				g_d.SelectPen(pDC,KN_PEN);
			for ( i=0;i<m_nDrawNum;i++ )
			{
				if ( memcmp(pHisDay+i,&l_InValidHisDat,sizeof(AnalyData)) == 0 )
					continue;
				if ( pHisDay[i].Close+COMPPREC < pHisDay[i].Open )
				{
					pInfo[i] = 1;
					yC = GetYPosOfRegion(nFixRegion,pHisDay[i].Close,nObject) ;
					yH = GetYPosOfRegion(nFixRegion,pHisDay[i].High ,nObject) ;
					yL = GetYPosOfRegion(nFixRegion,pHisDay[i].Low  ,nObject) ;
					g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yL,m_pPosOfX[i]-Off,yH);
					g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yC);
				}
			}
			if (bIsOverlap)
				g_d.SelectPen(pDC, m_nOverlapPenPos);
			else
				g_d.SelectPen(pDC,LEVEL_PEN);
			for ( i=0;i<m_nDrawNum;i++ )
			{
				if ( memcmp(pHisDay+i,&l_InValidHisDat,sizeof(AnalyData)) == 0 )
					continue;
				if ( !pInfo[i]  )
				{
					yC = GetYPosOfRegion(nFixRegion,pHisDay[i].Close,nObject) ;
					yH = GetYPosOfRegion(nFixRegion,pHisDay[i].High ,nObject) ;
					yL = GetYPosOfRegion(nFixRegion,pHisDay[i].Low  ,nObject) ;
					g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yL,m_pPosOfX[i]-Off,yH);
					if(Off<1)
						pDC->SetPixel(m_pPosOfX[i],yC,VipColor.LEVELCOLOR);
					else
						g_d.DrawLine(pDC,m_pPosOfX[i]-Off,yC,m_pPosOfX[i]+Off,yC);
				}
			}
		}
		else if(nDrawMode == 2 || nDrawMode == 3)	//画收盘(站点)线
		{
			if(bIsOverlap)
				pslcolor = l_KLineColor[m_nOverlapPenPos - MAX_PEN_NUM];
			else
				pslcolor = VipColor.KPCOLOR;
			
			if (bIsOverlap)
				g_d.SelectPen(pDC, m_nOverlapPenPos);
			else
				g_d.SelectPen(pDC,KP_PEN);

			xx2 = m_pPosOfX[0]-1;
			yy2 = GetYPosOfRegion(nFixRegion,pHisDay[0].Close,nObject) ;
			for(i = 0; i < m_nDrawNum - 1; i++)
			{
				if(m_nLeftNo+i+1 >= m_nDataNum - g_RMSp)
					break;
				if(memcmp(pHisDay+i,&l_InValidHisDat,sizeof(AnalyData)) == 0 || memcmp(pHisDay+i+1,&l_InValidHisDat,sizeof(AnalyData)) == 0)
					continue;
				xx1 = xx2;
				yy1 = yy2;
				xx2 = m_pPosOfX[i+1]-1;
				yy2 = GetYPosOfRegion(nFixRegion,pHisDay[i+1].Close,nObject) ;
				g_d.DrawLine(pDC,xx1,yy1,xx2,yy2,pslcolor);
			}
			if( nDrawMode == 3)
			{
				for(i = 0; i < m_nDrawNum; i++)
				{
					if(m_nLeftNo+i >= m_nDataNum - g_RMSp)
						break;
					if ( memcmp(pHisDay+i,&l_InValidHisDat,sizeof(AnalyData)) == 0 )
						continue;
					if ( TBE_MEANLESS(pHisDay[i].Close) != 0 )
					{				
						yC = GetYPosOfRegion(nFixRegion,pHisDay[i].Close,nObject) ;
						if (dw>=6)
							g_d.DrawO(pDC, m_pPosOfX[i], yC, &pslcolor);
						else
						{
							pDC->SetPixel(m_pPosOfX[i]-1,yC,pslcolor);
							pDC->SetPixel(m_pPosOfX[i],yC-1,pslcolor);
							pDC->SetPixel(m_pPosOfX[i],yC+1,pslcolor);
							pDC->SetPixel(m_pPosOfX[i]+1,yC,pslcolor);
						}
					}
				}
			}
		}
		else
		{
			for (i = 0; i < m_nDrawNum; i++)
			{
				if(m_nLeftNo+i >= m_nDataNum)
					break;
				if (i < 2) 
					continue;
				int flag=0;
				if (pHisDay[i].Close  >=pHisDay[i-1].Close) flag+=1;
				if (pHisDay[i-1].Close>=pHisDay[i-2].Close) flag+=10;
				int y11  = GetYPosOfRegion(nRegionNo, MAX(pHisDay[i].Close,pHisDay[i-1].Close));
				int y22  = GetYPosOfRegion(nRegionNo, MIN(pHisDay[i].Close,pHisDay[i-1].Close));
				int yhh  = GetYPosOfRegion(nRegionNo, MAX(pHisDay[i-2].Close,pHisDay[i-1].Close));
				int yll  = GetYPosOfRegion(nRegionNo, MIN(pHisDay[i-2].Close,pHisDay[i-1].Close));

				switch (flag)
				{
				case YUP_TUP:
				//	g_d.Bar(pDC,m_pPosOfX[i]-Off,yhh,m_pPosOfX[i]+Off,y11,FIX_RED_BRUSH);
					g_d.Draw3DBar2(pDC,CRect(m_pPosOfX[i]-Off,yhh,m_pPosOfX[i]+Off,y11),RGB(255,0,0),FALSE);
					break;
				case YUP_TDOWN:
					if (y22<=yll)
					//	g_d.Bar(pDC,m_pPosOfX[i]-Off,yhh,m_pPosOfX[i]+Off,y22,FIX_RED_BRUSH);
						g_d.Draw3DBar2(pDC,CRect(m_pPosOfX[i]-Off,yhh,m_pPosOfX[i]+Off,y22),RGB(255,0,0),FALSE);
					else
					{
					//	g_d.Bar(pDC,m_pPosOfX[i]-Off,yll,m_pPosOfX[i]+Off,y22,FIX_GREEN_BRUSH);
						g_d.Draw3DBar2(pDC,CRect(m_pPosOfX[i]-Off,yll,m_pPosOfX[i]+Off,y22),RGB(0,255,0),FALSE);
					//	g_d.Bar(pDC,m_pPosOfX[i]-Off,yhh,m_pPosOfX[i]+Off,yll,FIX_RED_BRUSH);
						g_d.Draw3DBar2(pDC,CRect(m_pPosOfX[i]-Off,yhh,m_pPosOfX[i]+Off,yll),RGB(255,0,0),FALSE);
					}
					break;
				case YDOWN_TUP:
					if (y11>=yhh)
					//	g_d.Bar(pDC,m_pPosOfX[i]-Off,yll,m_pPosOfX[i]+Off,y11,FIX_GREEN_BRUSH);
						g_d.Draw3DBar2(pDC,CRect(m_pPosOfX[i]-Off,yll,m_pPosOfX[i]+Off,y11),RGB(0,255,0),FALSE);
					else
					{
					//	g_d.Bar(pDC,m_pPosOfX[i]-Off,yll,m_pPosOfX[i]+Off,yhh,FIX_GREEN_BRUSH);
						g_d.Draw3DBar2(pDC,CRect(m_pPosOfX[i]-Off,yll,m_pPosOfX[i]+Off,yhh),RGB(0,255,0),FALSE);
					//	g_d.Bar(pDC,m_pPosOfX[i]-Off,yhh,m_pPosOfX[i]+Off,y11,FIX_RED_BRUSH);
						g_d.Draw3DBar2(pDC,CRect(m_pPosOfX[i]-Off,yhh,m_pPosOfX[i]+Off,y11),RGB(255,0,0),FALSE);
					}
					break;
				case YDOWN_TDOWN:
				//	g_d.Bar(pDC,m_pPosOfX[i]-Off,yll,m_pPosOfX[i]+Off,y22,FIX_GREEN_BRUSH);
					g_d.Draw3DBar2(pDC,CRect(m_pPosOfX[i]-Off,yll,m_pPosOfX[i]+Off,y22),RGB(0,255,0),FALSE);
					break;
				}
			}
		}
	}
	if ( m_SelectInfo.bExist && m_SelectInfo.nInRegion == nRegionNo && nFixRegion == nRegionNo 
		&& ( (m_SelectInfo.nInObject == nObject && m_SelectInfo.nSet == HIS_DAT)
			|| m_SelectInfo.nSet == KL_OBJECT )
			)
	{
		int oldx = 0;
		for ( i=0;i<m_nDrawNum;i++ )
		{
			if ( memcmp(pHisDay+i,&l_InValidHisDat,sizeof(AnalyData)) == 0 )
				continue;
			if ( i%6 == 0 && m_pPosOfX[i]-oldx > 40 )
			{
				yC = GetYPosOfRegion(nRegionNo,pHisDay[i].Close,nObject) ;
			//	g_d.Bar(pDC,m_pPosOfX[i]-2,yC-2,m_pPosOfX[i]+2,yC+2,bIsOverlap?OVERLAP_BRUSH:LEVEL_BRUSH);
				if(bIsOverlap)
					g_d.Bar(pDC,m_pPosOfX[i]-2,yC-2,m_pPosOfX[i]+2,yC+2,OVERLAP_BRUSH);
				else g_d.Draw3DBar2(pDC,CRect(m_pPosOfX[i]-2,yC-2,m_pPosOfX[i]+2,yC+2),VipColor.LEVELCOLOR);
				oldx = m_pPosOfX[i];
			}
		}
		m_bEvenSelected = TRUE;
	}
	g_d.RestorePen(pDC,oldpen);
	if(OldFixRegion==-1)
	{
		m_nOverlapPenPos++;
		m_nOverlapBrushPos++;
	}
	if (m_nOverlapPenPos > nCurrPen) m_nOverlapPenPos = MAX_PEN_NUM+5;
	if (m_nOverlapBrushPos > nCurrBrush) m_nOverlapBrushPos = MAX_BRUSH_NUM+5;
	delete []pInfo;pInfo=NULL;
}

float UFxt::GetAllVolume(int num)
{   
	struct AnalyData * pHisDay = m_pObject->pHisDay + m_nLeftNo;
    float all = 0.0;
    for (int i = 0; i<num; i++)
		all+= (float)pHisDay[i].Volume;
    return all;
}

void UFxt::DrawOX(CDC *pDC, short col,float val1,float val2,short flag,double yunit,COLORREF color)
{
	short  ii, last1, last2;
	last1=(m_aRegion[0].dMaxValEx-val1)*yunit;
	last2=(m_aRegion[0].dMaxValEx-val2)*yunit;
	if (flag==XFLAG)
		g_d.SelectPen(pDC, KN_PEN);
	else
		g_d.SelectPen(pDC, KP_PEN);
	for (ii=MIN(last2,last1);ii<=MAX(last1,last2);ii++)
	{
		if (flag==XFLAG)
			g_d.DrawX(pDC,m_aRegion[0].Area.left+4+col*8,m_aRegion[0].Area.top+ii*8+4);
		else
			g_d.DrawO(pDC,m_aRegion[0].Area.left+4+col*8,m_aRegion[0].Area.top+ii*8+4);
	}
}

void UFxt::DrawSAR(CDC * pDC,int nRegionNo,int nObject)
{
	int i,j,nPen,yy1;
	double dw;
	COLORREF color;

	dw=m_dWidth+m_dDistance;
	struct AnalyData * pHisDay = m_pObject->pHisDay;
	float * fp =m_aRegion[nRegionNo].aIndexInfo[nObject]->pCalcData[0];
	for (i=m_nLeftNo,j=0;j<m_nDrawNum;i++,j++)
	{
		yy1 = GetYPosOfRegion(nRegionNo, fp[i]);
		if (fp[i]-pHisDay[i].Close<COMPPREC) 
		{
			nPen = KP_PEN;
			color = VipColor.KPCOLOR;
		}
		else
		{
			nPen = DOWN_PEN;
			color = VipColor.DOWNCOLOR;
		}
		if (dw>=6)
		{
			g_d.SelectPen(pDC, nPen);
			g_d.DrawO(pDC,m_pPosOfX[j],yy1,&color);
		}
		else
		{
			pDC->SetPixel(m_pPosOfX[j]-1,yy1,color);
			pDC->SetPixel(m_pPosOfX[j],yy1-1,color);
			pDC->SetPixel(m_pPosOfX[j],yy1+1,color);
			pDC->SetPixel(m_pPosOfX[j]+1,yy1,color);
		}
	}
	if ( m_SelectInfo.bExist && m_SelectInfo.nInRegion == nRegionNo
		&& m_SelectInfo.nInObject == nObject)
	{
		int oldx = 0;
		for ( i=0;i<m_nDrawNum;i++ )
		{
			if ( i%6 == 0 && m_pPosOfX[i]-oldx > 40 )
			{
				yy1 = GetYPosOfRegion(nRegionNo, fp[i+m_nLeftNo]);
				g_d.Bar(pDC,m_pPosOfX[i]-2,yy1-2,m_pPosOfX[i]+2,yy1+2,LEVEL_BRUSH);
				oldx = m_pPosOfX[i];
			}
		}
		m_bEvenSelected = TRUE;
	}
}

//画逆时钟曲线
void UFxt::DrawPUCU(CDC * pDC,int nRegionNo,int nObject)
{
	TOBJECT * p = m_aRegion[nRegionNo].aIndexInfo[nObject];
	float * PVC =p->pCalcData[0];
	float * PVCL=p->pCalcData[1];
	int     jrd,zrd,jrg,zrg, i,w;
	double   fPervol, fMaxmavval, fMinmavval;
	COLORREF color;
	//占满屏幕。
	w = m_aRegion[nRegionNo].DrawRc.right  - m_aRegion[nRegionNo].DrawRc.left;

	for ( fMaxmavval=0.0,fMinmavval=999999999.9,i=m_nLeftNo;i<m_nLastNo+1;i++)
	{
		if (TBE_MEANLESS(PVCL[i]) == 0 ) 
			continue;
		if (PVCL[i]>fMaxmavval) fMaxmavval=PVCL[i];
		if (PVCL[i]<fMinmavval) fMinmavval=PVCL[i];
	}

	if (equfz(fMaxmavval-fMinmavval, 0.0)) 
		fPervol=1.0;
	else   
		fPervol =(float) (w) / (fMaxmavval-fMinmavval);

	zrd=(short)(m_aRegion[nRegionNo].DrawRc.bottom -   (PVCL[0]-fMinmavval)*fPervol + 0.5 );
	zrg=GetYPosOfRegion(nRegionNo, PVC [0]);
	for (i = m_nLeftNo; i < m_nLeftNo+m_nDrawNum; i++)
	{
		jrd=(short)(m_aRegion[nRegionNo].DrawRc.right - (PVCL[i]-fMinmavval)*fPervol + 0.5 );  //kx2
		jrg=GetYPosOfRegion(nRegionNo, PVC [i]);  //ky2
		if (jrg<zrg)
		{
			if (jrd >zrd)		color=RGB(252,252,84);
			if (jrd==zrd)		color=RGB(0,168,168);
			if (jrd <zrd)		color=RGB(0,0,168);
		}
		if (jrg==zrg)
		{
			if (jrd >zrd)	 color=RGB(0,168,168);
			if (jrd==zrd)	 continue;
			if (jrd <zrd)	 color=RGB(168,0,0);
		}
		if (jrg>zrg)
		{
			if (jrd >zrd)	color=RGB(0,0,168);
			if (jrd==zrd)	color=RGB(192,192,192);
			if (jrd <zrd)	color=RGB(0,168,168);
		}

		g_d.DrawLine(pDC,zrd,zrg,jrd,jrg,color);
		if (i==m_nLeftNo+m_nDrawNum-1)
		{
			if (jrd!=zrd)
				g_d.DrawLine(pDC,jrd-((jrd-zrd)/abs(jrd-zrd))*10,jrg,jrd,jrg,color);
			if (jrg!=zrg) 
				g_d.DrawLine(pDC,jrd,jrg-((jrg-zrg)/abs(jrg-zrg))*10,jrd,jrg,color);
		}
		zrd=jrd;zrg=jrg;
	}
}

COLORREF UFxt::GetColor(AnalyData * pAnaly, int& nWhichBrush)
{ 
	if (fabs(pAnaly->Close-pAnaly->Open)<COMPPREC)
	{
		nWhichBrush = LEVEL_BRUSH;
		return VipColor.LEVELCOLOR;
	}
	if (pAnaly->Close>pAnaly->Open)
	{
		nWhichBrush = KP_BRUSH;
		return VipColor.KPCOLOR;
	}
	nWhichBrush = KN_BRUSH;
	return VipColor.KNCOLOR;
}

void UFxt::DrawSpecialLine(CDC *pDC, int nRegionNo,int nObject)
{
	#define  C_O_H_L(i)	\
	 y1  = GetYPosOfRegion(nRegionNo, pHisDay[i].Close);\
	 y2  = GetYPosOfRegion(nRegionNo, pHisDay[i].Open );\
	 yh  = GetYPosOfRegion(nRegionNo, pHisDay[i].High );\
	 yl  = GetYPosOfRegion(nRegionNo, pHisDay[i].Low  );

	int			w,h,flag,nPara, nIndex;
	int			i, j, x1,x2,x3,y1,y2,yl,yh,nOff = int(m_dWidth*0.5), nWhichBrush;
	double		ww,CurVolumeSum,dtmp, dPerUnit2;
	COLORREF	color;

	w = m_aRegion[nRegionNo].Area.right  - m_aRegion[nRegionNo].Area.left;
	h = m_aRegion[nRegionNo].Area.bottom - m_aRegion[nRegionNo].Area.top;
	dPerUnit2=m_aRegion[nRegionNo].dPerUnitEx/8.0;
	nIndex = m_aRegion[nRegionNo].aIndexInfo[nObject]->nGSIndex;	//由于特殊画线的特殊性,此时nGSIndex表示的是画线类型
	struct AnalyData * pHisDay = m_pObject->pHisDay + m_nLeftNo;
	switch (nIndex)
	{
		case VAL_EQUVOL_LINE:	//等量图
			CurVolumeSum = GetAllVolume(m_nDrawNum);
			ww =(double) (m_aRegion[nRegionNo].Area.right-m_aRegion[nRegionNo].Area.left)/CurVolumeSum*m_nDrawNum/m_nWillNum;
			color = VipColor.KPCOLOR;
			x2 = m_aRegion[nRegionNo].Area.left;
			for (i = 0; i < m_nDrawNum; i++)
			{   
				x1 = x2;
				x2 = x1+pHisDay[i].Volume* ww;
				C_O_H_L(i)
				yh  = GetYPosOfRegion(nRegionNo, pHisDay[i].High );
				g_d.DrawRect(pDC,x1,yh,x2,yl,color);
			}
			break;
		case VAL_EQUK_LINE:		//等量Ｋ线
			CurVolumeSum = GetAllVolume(m_nDrawNum);
			ww =(double) (m_aRegion[nRegionNo].Area.right-m_aRegion[nRegionNo].Area.left)/CurVolumeSum*m_nDrawNum/m_nWillNum;
			x3 = m_aRegion[nRegionNo].Area.left;
			for (i = 0; i < m_nDrawNum; i++)
			{
				x1 = x3;
				x2 = (x1+x3)/2.0;
				x3 = x1+pHisDay[i].Volume* ww;
				C_O_H_L(i)

				color = GetColor(pHisDay+i, nWhichBrush);
				if (color!=VipColor.KNCOLOR)
				{   
					g_d.DrawRect(pDC,x1,y1,x3,y2,VipColor.KPCOLOR);
					g_d.DrawLine(pDC,x2,y1,x2,yh,color);
					g_d.DrawLine(pDC,x2,yl,x2,y2,color);
				}
				else
				{
					if(m_nDrawNum<400)
					g_d.Bar(pDC,x1,y1,x3,y2,nWhichBrush);
					g_d.DrawLine(pDC,x2,yl,x2,yh,color);
				} 
			}
			break;
		case VAL_CAN_LINE:		//压缩图
			CurVolumeSum = 0;
			for (i = 0; i < m_nDrawNum; i++)
			{
				if (pHisDay[i].Volume>CurVolumeSum) 
					CurVolumeSum=pHisDay[i].Volume;
			}
			//注:Find the max volume
			ww = (m_dDistance+m_dWidth)*4.0;
			for (i = 0; i < m_nDrawNum; i++)
			{   
				x1 = m_pPosOfX[i]-nOff;
				dtmp=pHisDay[i].Volume;
				dtmp=dtmp/CurVolumeSum;
				try
				{
					if(dtmp==0)
						throw "Error for zero !";
				}
				catch(...)
				{
					dtmp=1.0;
				}
				//注:The ratio of volume
				dtmp=dtmp*ww;
				x3 =min(m_aRegion[nRegionNo].Area.right, x1+dtmp);
				x2 =min(m_aRegion[nRegionNo].Area.right, (x1+x3)/2);
				C_O_H_L(i)
				color = GetColor(pHisDay+i, nWhichBrush);
				if (color!=VipColor.KNCOLOR)
				{  
					g_d.DrawRect(pDC,x1,y1,x3,y2, color);
					g_d.DrawLine(pDC,x2,y1,x2,yh,color);
					g_d.DrawLine(pDC,x2,yl,x2,y2,color);
				}
				else
				{  
					g_d.Bar(pDC ,x1,y1,x3,y2,nWhichBrush);
					g_d.DrawLine(pDC,x2,yl,x2,yh,color);
				}
			}
			break;
			//圈叉图处理时不是用相对与区域的坐标进行计算的,而是相对与第0个区域进行计算的
		case VAL_OX_LINE:		//圈叉图
			nPara=2;
			//由于开始已经使用m_nLeftNo做过偏移,所以 i=m_nLeftNo == 0?1:0表示至少要从第一天开始
			for (j=-1,flag=0,i=m_nLeftNo == 0?1:0; i < (m_nDrawNum) && j*8<(w-8); i++)
			{
				if (pHisDay[i].Close>pHisDay[i-1].Close) //if C > C[1]
				{ switch (flag)
				  {
				  case XFLAG: //if 昨日符号是‘×’
					 DrawOX(pDC,j,pHisDay[i-1].High,pHisDay[i].High,XFLAG,dPerUnit2,VipColor.ZBCOLOR[0]);
					 DrawOX(pDC,j,pHisDay[i].Low  ,pHisDay[i-1].Low,XFLAG,dPerUnit2,VipColor.ZBCOLOR[0]);
					 //if H > H[1]    在同一列上 H[1] 至 H 之间画(H-H[1])/BOXSIZE个‘×’
					 //if L < L[1]    在同一列上 L 至 L[1] 之间画(L[1]-L)/BOXSIZE个‘×’
					 break;
				  case 0:
				  case OFLAG://if 昨日符号是‘○’
					 j++;//在下一列上 L 至 H 之间画(H-L)/BOXSIZE个‘×’
					 DrawOX(pDC,j,pHisDay[i].Low,pHisDay[i].High,XFLAG,dPerUnit2,VipColor.ZBCOLOR[0]);
					 flag=XFLAG;
					 break;
				  }
				}
				else
				{
				  switch (flag)
				  {
				  case OFLAG://if 昨日符号是‘○’
					 //if H > H[1]  在同一列上 H[1] 至 H 之间画(H-H[1])/BOXSIZE个‘○’
					 //if L < L[1]  在同一列上 L 至 L[1] 之间画(L[1]-L)/BOXSIZE个‘○’
					 DrawOX(pDC,j,pHisDay[i-1].High,pHisDay[i].High,OFLAG,dPerUnit2,VipColor.ZBCOLOR[0]);
					 DrawOX(pDC,j,pHisDay[i].Low,pHisDay[i-1].Low,OFLAG,dPerUnit2,VipColor.ZBCOLOR[0]);
					 break;
				  case 0:
				  case XFLAG://if 昨日符号是‘×’
					 j++;//在下一列上 L 至 H 之间画(H-L)/BOXSIZE个‘○’
					 DrawOX(pDC,j,pHisDay[i].Low,pHisDay[i].High,OFLAG,dPerUnit2,VipColor.ZBCOLOR[0]);
					 flag=OFLAG;
					 break;
				  }
				}
			}
			break;
			//高低圈叉图处理时不是用相对与区域的坐标进行计算的,而是相对与第0个区域进行计算的
		case VAL_OXHL_LINE:		//高低圈叉图
			nPara=2;
			dtmp=(m_aRegion[nRegionNo].dMaxValEx-m_aRegion[nRegionNo].dMinValEx)*(nPara)/((h+5)/8);
			//由于开始已经使用m_nLeftNo做过偏移,所以 i=m_nLeftNo == 0?1:0表示至少要从第一天开始
			for (j=0,flag=0,i=m_nLeftNo == 0?1:0; i < (m_nDrawNum) && j*8<(w-8); i++)
			{   
				switch (flag)
				{
				case 0:	//if 是第一个单位
				  flag=(pHisDay[i].Close>pHisDay[i-1].Close)?XFLAG:OFLAG;
				  DrawOX(pDC,j,pHisDay[i].Low,pHisDay[i].High,flag,dPerUnit2,VipColor.ZBCOLOR[0]);
				  break;
				case XFLAG://if 昨日符号是‘×’
				  if (pHisDay[i].High>pHisDay[i-1].High) //if H > H[1]
					 //在同一列上 H[1] 至 H 之间画(H-H[1])/BOXSIZE个‘×’
					 DrawOX(pDC,j,pHisDay[i-1].High,pHisDay[i].High,XFLAG,dPerUnit2,VipColor.ZBCOLOR[0]);
				  else
					 {if (pHisDay[i].Low<pHisDay[i-1].Low-dtmp)
					  //if L < L[1] - REVERSAL*BOXSIZE
						{j++;//  在下一列上 L 至 H 之间画(H-L)/BOXSIZE个‘○’
						 DrawOX(pDC,j,pHisDay[i].Low,pHisDay[i].High,OFLAG,dPerUnit2,VipColor.ZBCOLOR[0]);
						 flag=OFLAG;
						}
					 }
				  break;
				case OFLAG://if 昨日符号是‘○’
				  if (pHisDay[i].Low<pHisDay[i-1].Low) //if L < L[1]
					 //在同一列上 L 至 L[1] 之间画(L[1]-L)/BOXSIZE个‘○’
					 DrawOX(pDC,j,pHisDay[i-1].High,pHisDay[i].High,OFLAG,dPerUnit2,VipColor.ZBCOLOR[0]);
				  else
					 {if (pHisDay[i].High>pHisDay[i-1].High+dtmp)     //if H > H[1] + REVERSAL*BOXSIZE
						 {j++;// 在下一列上 L 至 H 之间画(H-L)/BOXSIZE个‘×’
						  DrawOX(pDC,j,pHisDay[i].Low,pHisDay[i].High,XFLAG,dPerUnit2,VipColor.ZBCOLOR[0]);
						  flag=XFLAG;
						 }
					 }
				  break;
				}
			}
			break;
		case VAL_TBL_LINE:		//新三价线
			for (i = 0; i < m_nDrawNum; i++)
			{
				int nWhichBrush;
				GetColor(pHisDay+i, nWhichBrush);
				C_O_H_L(i)
				g_d.Bar(pDC,m_pPosOfX[i]-nOff,yh,m_pPosOfX[i]+nOff,yl,nWhichBrush);
			}
			break;
		case VAL_TOW_LINE:		//宝塔线
			for (i = 0; i < m_nDrawNum; i++)
			{
				if (i < 2) continue;
				x1 = m_pPosOfX[i]-nOff;
				x2 = m_pPosOfX[i]+nOff;
				flag=0;

				if (pHisDay[i].Close  >=pHisDay[i-1].Close) flag+=1;
				if (pHisDay[i-1].Close>=pHisDay[i-2].Close) flag+=10;
				y1  = GetYPosOfRegion(nRegionNo, MAX(pHisDay[i].Close,pHisDay[i-1].Close));
				y2  = GetYPosOfRegion(nRegionNo, MIN(pHisDay[i].Close,pHisDay[i-1].Close));
				yh  = GetYPosOfRegion(nRegionNo, MAX(pHisDay[i-2].Close,pHisDay[i-1].Close));
				yl  = GetYPosOfRegion(nRegionNo, MIN(pHisDay[i-2].Close,pHisDay[i-1].Close));

				switch (flag)
				{
				case YUP_TUP:
					g_d.Bar(pDC,x1,yh,x2,y1,FIX_RED_BRUSH);
					break;
				case YUP_TDOWN:
					if (y2<=yl)
						g_d.Bar(pDC,x1,yh,x2,y2,FIX_RED_BRUSH);
					else
					{
						g_d.Bar(pDC,x1,yl,x2,y2,FIX_GREEN_BRUSH);
						g_d.Bar(pDC,x1,yh,x2,yl,FIX_RED_BRUSH);
					}
					break;
				case YDOWN_TUP:
					if (y1>=yh)
						g_d.Bar(pDC,x1,yl,x2,y1,FIX_GREEN_BRUSH);
					else
					{
						g_d.Bar(pDC,x1,yl,x2,yh,FIX_GREEN_BRUSH);
						g_d.Bar(pDC,x1,yh,x2,y1,FIX_RED_BRUSH);
					}
					break;
				case YDOWN_TDOWN:
					g_d.Bar(pDC,x1,yl,x2,y2,FIX_GREEN_BRUSH);
					break;
				}
			}
			break;
	}
}

