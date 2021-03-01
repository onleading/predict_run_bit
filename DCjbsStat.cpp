#include "stdafx.h"
#include "DCjbsStat.h"
#include "custompad.h"

DCjbsStat::DCjbsStat()
{
	m_pData		= NULL;
	m_nDataNum	= 0;
	m_dMaxVal	= 0.0;
	m_dMinVal	= 0.0;
}

DCjbsStat::~DCjbsStat()
{
}

void DCjbsStat::SetLimit(double dMax,double dMin,long nType)
{
	switch(nType)
	{
	case ZST_TICKNUM:
		m_dMaxVal	= dMax;
		m_dMinVal	= dMin;
		break;
	case ZST_LARGETICK:
		m_dMaxLarge	= dMax;
		m_dMinLarge	= dMin;
		break;
	default:	break;
	}
}

BOOL DCjbsStat::SetData(void *pData,long nDataNum)
{
	m_pData		= (LPTICKSTAT)pData;
	m_nDataNum	= nDataNum;
	return TRUE;
}

BOOL DCjbsStat::Draw(CDC *pDC,long nType)
{
	if(m_pData==NULL || m_nDataNum<=0 || m_nWillNum<=0 || !g_bSupportDDE) return FALSE;
	
	int		i = 0;
	double	dTmp = 0.0;
	long	lPos1,lPos2;
	double	dSum1 = 0.0,dSum2 = 0.0;
	double	dSum11 = 0.0,dSum22 = 0.0;
	switch(nType)
	{
	case ZST_TICKNUM:
		if(m_dMaxVal-m_dMinVal<1-COMPPREC) return FALSE;
		if(m_nDataNum>m_nLeftNo)
		{
			CPen *oldpen = g_d.SelectPen(pDC,ZB1_PEN);
			//买入总笔数
			dSum2 = m_pData[m_nLeftNo].VolNum[0][0]+m_pData[m_nLeftNo].VolNum[0][2];
			for(i=m_nLeftNo+1;i<m_nDataNum;i++)
			{
				lPos1	= m_nLeft+long(1.0*m_nWidth*(i+m_nLeftBlank-m_nLeftNo)/m_nWillNum);
				lPos2	= m_nLeft+long(1.0*m_nWidth*(i+m_nLeftBlank+1-m_nLeftNo)/m_nWillNum);
				dSum1 = dSum2;
				dSum2 = m_pData[i].VolNum[0][0]+m_pData[i].VolNum[0][2];
				if(dSum2<COMPPREC) dSum2 = dSum1;
				g_d.DrawLine(pDC,lPos1,GetPosFromVal(dSum1,nType),lPos2,GetPosFromVal(dSum2,nType));
			}
			g_d.SelectPen(pDC,ZB2_PEN);
			//卖出总笔数
			dSum2 = m_pData[m_nLeftNo].VolNum[0][1]+m_pData[m_nLeftNo].VolNum[0][3];
			for(i=m_nLeftNo+1;i<m_nDataNum;i++)
			{
				lPos1	= m_nLeft+long(1.0*m_nWidth*(i+m_nLeftBlank-m_nLeftNo)/m_nWillNum);
				lPos2	= m_nLeft+long(1.0*m_nWidth*(i+m_nLeftBlank+1-m_nLeftNo)/m_nWillNum);
				dSum1 = dSum2;
				dSum2 = m_pData[i].VolNum[0][1]+m_pData[i].VolNum[0][3];
				if(dSum2<COMPPREC) dSum2 = dSum1;
				g_d.DrawLine(pDC,lPos1,GetPosFromVal(dSum1,nType),lPos2,GetPosFromVal(dSum2,nType));
			}
			//
			dSum2 = m_pData[m_nLeftNo].VolNum[0][0]+m_pData[m_nLeftNo].VolNum[0][2];
			dSum22 = m_pData[m_nLeftNo].VolNum[0][1]+m_pData[m_nLeftNo].VolNum[0][3];
			for(i=m_nLeftNo,dSum1=0.0;i<m_nDataNum;i++)
			{
				lPos1	= m_nLeft+long(1.0*m_nWidth*(i+m_nLeftBlank-m_nLeftNo)/m_nWillNum);
				lPos2	= m_nLeft+long(1.0*m_nWidth*(i+m_nLeftBlank+1-m_nLeftNo)/m_nWillNum);
				//
				dSum1 = dSum2;
				dSum2 = m_pData[i].VolNum[0][0]+m_pData[i].VolNum[0][2];
				if(dSum2<COMPPREC) dSum2 = dSum1;
				//
				dSum11 = dSum22;
				dSum22 = m_pData[i].VolNum[0][1]+m_pData[i].VolNum[0][3];
				if(dSum22<COMPPREC) dSum22 = dSum11;
				//
				g_d.DrawLine(pDC,lPos1,GetPosFromVal((dSum2-dSum22)+(m_dMaxVal+m_dMinVal)/2,nType),
							 lPos1,m_nTop+m_nHeight/2,(dSum2-dSum22)>0?VipColor.DOWNCOLOR:VipColor.KPCOLOR);
			}
			g_d.RestorePen(pDC,oldpen);
		}
		break;
	case ZST_LARGETICK:
		if(m_dMaxLarge-m_dMinLarge<1-COMPPREC) return FALSE;
		if(m_nDataNum>m_nLeftNo)
		{
			CPen *oldpen = g_d.SelectPen(pDC,ZB1_PEN);
			dSum2 = m_pData[m_nLeftNo].VolNum[1][0];
			for(i=m_nLeftNo+1;i<m_nDataNum;i++)
			{
				lPos1	= m_nLeft+long(1.0*m_nWidth*(i+m_nLeftBlank-m_nLeftNo)/m_nWillNum);
				lPos2	= m_nLeft+long(1.0*m_nWidth*(i+m_nLeftBlank+1-m_nLeftNo)/m_nWillNum);
				dSum1 = dSum2;
				dSum2 = m_pData[i].VolNum[1][0]+m_pData[i].VolNum[1][2];
				if(dSum2<COMPPREC) dSum2 = dSum1;
				g_d.DrawLine(pDC,lPos1,GetPosFromVal(dSum1,nType),lPos2,GetPosFromVal(dSum2,nType));
			}
			g_d.SelectPen(pDC,ZB2_PEN);
			dSum2 = m_pData[m_nLeftNo].VolNum[1][1];
			for(i=m_nLeftNo+1;i<m_nDataNum;i++)
			{
				lPos1	= m_nLeft+long(1.0*m_nWidth*(i+m_nLeftBlank-m_nLeftNo)/m_nWillNum);
				lPos2	= m_nLeft+long(1.0*m_nWidth*(i+m_nLeftBlank+1-m_nLeftNo)/m_nWillNum);
				dSum1 = dSum2;
				dSum2 = m_pData[i].VolNum[1][1]+m_pData[i].VolNum[1][3];
				if(dSum2<COMPPREC) dSum2 = dSum1;
				g_d.DrawLine(pDC,lPos1,GetPosFromVal(dSum1,nType),lPos2,GetPosFromVal(dSum2,nType));
			}
			//
			dSum2 = m_pData[m_nLeftNo].VolNum[1][0];
			dSum22 = m_pData[m_nLeftNo].VolNum[1][1];
			for(i=m_nLeftNo,dSum1=0.0;i<m_nDataNum;i++)
			{
				lPos1	= m_nLeft+long(1.0*m_nWidth*(i+m_nLeftBlank-m_nLeftNo)/m_nWillNum);
				lPos2	= m_nLeft+long(1.0*m_nWidth*(i+m_nLeftBlank+1-m_nLeftNo)/m_nWillNum);
				//
				dSum1 = dSum2;
				dSum2 = m_pData[i].VolNum[1][0]+m_pData[i].VolNum[1][2];
				if(dSum2<COMPPREC) dSum2 = dSum1;
				//
				dSum11 = dSum22;
				dSum22 = m_pData[i].VolNum[1][1]+m_pData[i].VolNum[1][3];
				if(dSum22<COMPPREC) dSum22 = dSum11;
				//
				g_d.DrawLine(pDC,lPos1,GetPosFromVal((dSum2-dSum22)+(m_dMaxLarge+m_dMinLarge)/2,nType),
							 lPos1,m_nTop+m_nHeight/2,(dSum2-dSum22)>0?VipColor.KPCOLOR:VipColor.DOWNCOLOR);
			}
			g_d.RestorePen(pDC,oldpen);
		}
		break;
	default:	
		break;
	}
	return TRUE;
}

long DCjbsStat::GetPosFromVal(double dVal,long nType)
{
	switch(nType)
	{
	case ZST_TICKNUM:
		if(m_dMaxVal-m_dMinVal<1-COMPPREC) return (m_nTop+m_nHeight);
		return long(m_nTop+m_nHeight-1.0*m_nHeight*(dVal-m_dMinVal)/(m_dMaxVal-m_dMinVal));
		break;
	case ZST_LARGETICK:
		if(m_dMaxLarge-m_dMinLarge<1-COMPPREC) return (m_nTop+m_nHeight);
		return long(m_nTop+m_nHeight-1.0*m_nHeight*(dVal-m_dMinLarge)/(m_dMaxLarge-m_dMinLarge));
		break;
	default:	break;
	}
	return (m_nTop+m_nHeight);
}
