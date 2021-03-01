#include "stdafx.h"
#include "DCancelOrder.h"
#include "custompad.h"

DCancelOrder::DCancelOrder()
{
	m_pData			= NULL;
	m_nDataNum		= 0;
	m_dOrderMaxVal	= 0.0;
	m_dOrderMinVal	= 0.0;
	m_dCancelMaxVal	= 0.0;
	m_dCancelMinVal	= 0.0;
}

DCancelOrder::~DCancelOrder()
{
}

void DCancelOrder::SetLimit(double dMax,double dMin,long nType)
{
	switch(nType)
	{
	case ZST_TOTALCANCEL:
		m_dOrderMaxVal	= dMax;
		m_dOrderMinVal	= dMin;
		break;
	case ZST_CANCELSUM:
		m_dCancelMaxVal	= dMax;
		m_dCancelMinVal	= dMin;
		break;
	default:	break;
	}
}

BOOL DCancelOrder::SetData(void *pData,long nDataNum)
{
	m_pData		= (LPNORMALSTAT)pData;
	m_nDataNum	= nDataNum;
	return TRUE;
}

BOOL DCancelOrder::Draw(CDC *pDC,long nType)
{
	if(m_pData==NULL || m_nDataNum<=0 || m_nWillNum<=0 || !g_bSupportDDE) 
		return FALSE;	
	int		i = 0;
	double	dTmp = 0.0;
	long	lPos1,lPos2;
	double	dSum1 = 0.0,dSum2 = 0.0;
	switch(nType)
	{
	case ZST_TOTALCANCEL:
		if(m_dOrderMaxVal-m_dOrderMinVal<1-COMPPREC) return FALSE;
		for(i=m_nLeftNo+1;i<m_nDataNum;i++)
		{
			lPos1	= m_nLeft+long(1.0*m_nWidth*(i+m_nLeftBlank-m_nLeftNo)/m_nWillNum);
			lPos2	= m_nLeft+long(1.0*m_nWidth*(i+m_nLeftBlank+1-m_nLeftNo)/m_nWillNum);
			if(i==1)
			{
				g_d.DrawLine(pDC,lPos1,GetPosFromVal(m_pData[i].fBOrder,nType),lPos2,GetPosFromVal(m_pData[i].fBOrder,nType),VipColor.ZBCOLOR[0]);
				g_d.DrawLine(pDC,lPos1,GetPosFromVal(-1.0*m_pData[i].fSOrder,nType),lPos2,GetPosFromVal(-1.0*m_pData[i].fSOrder,nType),VipColor.ZBCOLOR[1]);
			}
			else
			{
				g_d.DrawLine(pDC,lPos1,GetPosFromVal(m_pData[i-1].fBOrder,nType),lPos2,GetPosFromVal(m_pData[i].fBOrder,nType),VipColor.ZBCOLOR[0]);
				g_d.DrawLine(pDC,lPos1,GetPosFromVal(-1.0*m_pData[i-1].fSOrder,nType),lPos2,GetPosFromVal(-1.0*m_pData[i].fSOrder,nType),VipColor.ZBCOLOR[1]);
			}
		}
		for(i=m_nLeftNo;i<m_nDataNum;i++)
		{
			lPos1	= m_nLeft+long(1.0*m_nWidth*(i+m_nLeftBlank+1-m_nLeftNo)/m_nWillNum);
			g_d.DrawLine(pDC,lPos1,GetPosFromVal(m_pData[i].fBCancel,nType),lPos1,m_nTop+m_nHeight/2,VipColor.KPCOLOR);
			g_d.DrawLine(pDC,lPos1,GetPosFromVal(-1.0*m_pData[i].fSCancel,nType),lPos1,m_nTop+m_nHeight/2,VipColor.KNCOLOR);
		}
		break;
	case ZST_CANCELSUM:
		if(m_dCancelMaxVal-m_dCancelMinVal<1-COMPPREC) return FALSE;
		if(m_nDataNum>m_nLeftNo)
		{
			CPen *oldpen = g_d.SelectPen(pDC,ZB1_PEN);
			dSum2 = m_pData[m_nLeftNo].fBCancel;
			for(i=m_nLeftNo+1;i<m_nDataNum;i++)
			{
				lPos1	= m_nLeft+long(1.0*m_nWidth*(i+m_nLeftBlank-m_nLeftNo)/m_nWillNum);
				lPos2	= m_nLeft+long(1.0*m_nWidth*(i+m_nLeftBlank+1-m_nLeftNo)/m_nWillNum);
				dSum1 = dSum2;
				dSum2 += m_pData[i].fBCancel;
				g_d.DrawLine(pDC,lPos1,GetPosFromVal(dSum1,nType),lPos2,GetPosFromVal(dSum2,nType));
			}
			g_d.SelectPen(pDC,ZB2_PEN);
			dSum2 = m_pData[m_nLeftNo].fSCancel;
			for(i=m_nLeftNo+1;i<m_nDataNum;i++)
			{
				lPos1	= m_nLeft+long(1.0*m_nWidth*(i+m_nLeftBlank-m_nLeftNo)/m_nWillNum);
				lPos2	= m_nLeft+long(1.0*m_nWidth*(i+m_nLeftBlank+1-m_nLeftNo)/m_nWillNum);
				dSum1 = dSum2;
				dSum2 += m_pData[i].fSCancel;
				g_d.DrawLine(pDC,lPos1,GetPosFromVal(dSum1,nType),lPos2,GetPosFromVal(dSum2,nType));
			}
			g_d.RestorePen(pDC,oldpen);
		}
		break;
	}

	return TRUE;
}

long DCancelOrder::GetPosFromVal(double dVal,long nType)
{
	switch(nType)
	{
	case ZST_TOTALCANCEL:
		if(m_dOrderMaxVal-m_dOrderMinVal<1-COMPPREC) return (m_nTop+m_nHeight);
		return long(m_nTop+m_nHeight-1.0*m_nHeight*(dVal-m_dOrderMinVal)/(m_dOrderMaxVal-m_dOrderMinVal));
		break;
	case ZST_CANCELSUM:
		if(m_dCancelMaxVal-m_dCancelMinVal<1-COMPPREC) return (m_nTop+m_nHeight);
		return long(m_nTop+m_nHeight-1.0*m_nHeight*(dVal-m_dCancelMinVal)/(m_dCancelMaxVal-m_dCancelMinVal));
		break;
	default:	break;
	}
	return (m_nTop+m_nHeight);
}
