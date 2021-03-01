#include "stdafx.h"
#include "DTotalOrder.h"
#include "custompad.h"

DTotalOrder::DTotalOrder()
{
	m_pData		= NULL;
	m_nDataNum	= 0;
	m_dMaxVal	= 0.0;
	m_dMinVal	= 0.0;
}

DTotalOrder::~DTotalOrder()
{
}

void DTotalOrder::SetLimit(double dMax,double dMin,long nType)
{
	m_dMaxVal	= dMax;
	m_dMinVal	= dMin;
}

BOOL DTotalOrder::SetData(void *pData,long nDataNum)
{
	m_pData		= (LPNORMALSTAT)pData;
	m_nDataNum	= nDataNum;
	return TRUE;
}

BOOL DTotalOrder::Draw(CDC *pDC,long nType)
{
	if(m_pData==NULL || m_nDataNum<=0 || m_nWillNum<=0 || m_dMaxVal-m_dMinVal<1-COMPPREC || !g_bSupportDDE) 
		return FALSE;
	int		i = 0;
	double	dTmp = 0.0;
	long	lPos1,lPos2;
	CPen *oldpen = g_d.SelectPen(pDC,ZB1_PEN);
	for(i=m_nLeftNo+1;i<m_nDataNum;i++)
	{
		lPos1	= m_nLeft+long(1.0*m_nWidth*(i+m_nLeftBlank-m_nLeftNo)/m_nWillNum);
		lPos2	= m_nLeft+long(1.0*m_nWidth*(i+m_nLeftBlank+1-m_nLeftNo)/m_nWillNum);
		g_d.DrawLine(pDC,lPos1,GetPosFromVal(m_pData[i-1].TotalBuyv,nType),lPos2,GetPosFromVal(m_pData[i].TotalBuyv,nType));
	}
	g_d.SelectPen(pDC,ZB2_PEN);
	for(i=m_nLeftNo+1;i<m_nDataNum;i++)
	{
		lPos1	= m_nLeft+long(1.0*m_nWidth*(i+m_nLeftBlank-m_nLeftNo)/m_nWillNum);
		lPos2	= m_nLeft+long(1.0*m_nWidth*(i+m_nLeftBlank+1-m_nLeftNo)/m_nWillNum);
		g_d.DrawLine(pDC,lPos1,GetPosFromVal(m_pData[i-1].TotalSellv,nType),lPos2,GetPosFromVal(m_pData[i].TotalSellv,nType));
	}
	for(i=m_nLeftNo;i<m_nDataNum;i++)
	{
		dTmp	= 0.5*m_pData[i].TotalBuyv-0.5*m_pData[i].TotalSellv;
		lPos1	= m_nLeft+long(1.0*m_nWidth*(i+m_nLeftBlank+1-m_nLeftNo)/m_nWillNum);
		g_d.DrawLine(pDC,lPos1,GetPosFromVal(dTmp+m_dMaxVal/2,nType),
						 lPos1,m_nTop+m_nHeight/2,dTmp>0?VipColor.KPCOLOR:VipColor.DOWNCOLOR);
	}
	g_d.RestorePen(pDC,oldpen);
	return TRUE;
}

long DTotalOrder::GetPosFromVal(double dVal,long nType)
{
	if(m_dMaxVal-m_dMinVal<1-COMPPREC) return (m_nTop+m_nHeight);

	return long(m_nTop+m_nHeight-1.0*m_nHeight*(dVal-m_dMinVal)/(m_dMaxVal-m_dMinVal));
}
