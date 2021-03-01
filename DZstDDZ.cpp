#include "stdafx.h"
#include "DZstDDZ.h"
#include "custompad.h"

DZstDDZ::DZstDDZ()
{
	m_pData		= NULL;
	m_nDataNum	= 0;
	m_dMaxVal	= 0.0;
	m_dMinVal	= 0.0;
	
	m_Brush1.CreateSolidBrush(VipColor.KNCOLOR);
	m_Brush2.CreateSolidBrush(VipColor.KPCOLOR);
	m_Pen1.CreatePen(PS_SOLID,1,VipColor.KNCOLOR);
	m_Pen2.CreatePen(PS_SOLID,1,VipColor.KPCOLOR);
}

DZstDDZ::~DZstDDZ()
{
	m_Brush1.DeleteObject();
	m_Brush2.DeleteObject();
	m_Pen1.DeleteObject();
	m_Pen2.DeleteObject();
}

void DZstDDZ::SetLimit(double dMax,double dMin,long nType)
{
	m_dMaxVal	= dMax;
	m_dMinVal	= dMin;
}

BOOL DZstDDZ::SetData(void *pData,long nDataNum)
{
	m_pData		= (LPTICKSTAT)pData;
	m_nDataNum	= nDataNum;
	return TRUE;
}

BOOL DZstDDZ::Draw(CDC *pDC,long nType)
{
	if(m_pData==NULL || m_nDataNum<=0 || m_nWillNum<=0 || !g_bSupportDDE) 
		return FALSE;
	int i;
	int y11,y12,y21,y22,ycross,xcross;
	long	lPos1,lPos2;
	double dCurDDZ1 = 0;
	double dCurDDZ2 = 0;
	//
	double dBuyNum = 0;
	double dSellNum = 0;
	//
	m_pOldPen		= pDC->SelectObject(&m_Pen1);
	m_pOldBrush	= pDC->SelectObject(&m_Brush1);
	if(m_nDataNum>1)
	{
		dBuyNum	= long(m_pData[0].VolNum[0][0])+long(m_pData[0].VolNum[0][2]);
		dSellNum= long(m_pData[0].VolNum[0][1])+long(m_pData[0].VolNum[0][3]);

		if(dBuyNum>=-COMPPREC && dSellNum>=-COMPPREC && dBuyNum+dSellNum>COMPPREC)
			dCurDDZ2 = (dSellNum-dBuyNum)/(dSellNum+dBuyNum);
		else dCurDDZ2 = 0;
		
		y21		= GetPosFromVal(dCurDDZ2*13,ZST_DDZ);
		y22		= GetPosFromVal(dCurDDZ2*17,ZST_DDZ);
		lPos2	= m_nLeft+long(1.0*m_nWidth*(m_nLeftBlank+1-m_nLeftNo)/m_nWillNum);
	}
	for(i=1;i<m_nDataNum;i++)
	{
		dBuyNum	= long(m_pData[i].VolNum[0][0])+long(m_pData[i].VolNum[0][2])-long(m_pData[i-1].VolNum[0][0])-long(m_pData[i-1].VolNum[0][2]);
		dSellNum= long(m_pData[i].VolNum[0][1])+long(m_pData[i].VolNum[0][3])-long(m_pData[i-1].VolNum[0][1])-long(m_pData[i-1].VolNum[0][3]);

		if(dBuyNum>=-COMPPREC && dSellNum>=-COMPPREC && dBuyNum+dSellNum>COMPPREC)
			dCurDDZ2 = (dSellNum-dBuyNum)/(dSellNum+dBuyNum);
		else dCurDDZ2 = 0;
		
		y11		= y21;
		y12		= y22;
		lPos1	= lPos2;
		y21		= GetPosFromVal(dCurDDZ2*13,ZST_DDZ);
		y22		= GetPosFromVal(dCurDDZ2*17,ZST_DDZ);
		lPos2	= m_nLeft+long(1.0*m_nWidth*(i+m_nLeftBlank+1-m_nLeftNo)/m_nWillNum);
		//下面用坐标比较来决定颜色,因为纵坐标大小和数值正好相反,此处颜色处理要注意反相
		if((y11-y12)*(y21-y22)<0)	//反相
		{
			xcross = lPos1+(lPos2-lPos1)*(y11-y12)/(y11+y22-y12-y21);
			ycross = y11+(y21-y11)*(y11-y12)/(y11+y22-y12-y21);
			if(y11-y12>0)
			{
				pDC->SelectObject(&m_Brush2);
				pDC->SelectObject(&m_Pen2);
				//
				pDC->BeginPath();
				pDC->MoveTo(lPos1,y11);
				pDC->LineTo(lPos1,y12);
				pDC->LineTo(xcross,ycross);
				pDC->LineTo(lPos1,y11);
				pDC->EndPath();
				pDC->StrokeAndFillPath();
				//
				pDC->SelectObject(&m_Brush1);
				pDC->SelectObject(&m_Pen1);
				//
				pDC->BeginPath();
				pDC->MoveTo(lPos2,y21);
				pDC->LineTo(lPos2,y22);
				pDC->LineTo(xcross,ycross);
				pDC->LineTo(lPos2,y21);
				pDC->EndPath();
				pDC->StrokeAndFillPath();
			}
			else
			{
				pDC->SelectObject(&m_Brush1);
				pDC->SelectObject(&m_Pen1);
				//
				pDC->BeginPath();
				pDC->MoveTo(lPos1,y11);
				pDC->LineTo(lPos1,y12);
				pDC->LineTo(xcross,ycross);
				pDC->LineTo(lPos1,y11);
				pDC->EndPath();
				pDC->StrokeAndFillPath();
				//
				pDC->SelectObject(&m_Brush2);
				pDC->SelectObject(&m_Pen2);
				//
				pDC->BeginPath();
				pDC->MoveTo(lPos2,y21);
				pDC->LineTo(lPos2,y22);
				pDC->LineTo(xcross,ycross);
				pDC->LineTo(lPos2,y21);
				pDC->EndPath();
				pDC->StrokeAndFillPath();
			}
		}
		else
		{
			if(y11>y12 || y21>y22)
			{
				pDC->SelectObject(&m_Brush2);
				pDC->SelectObject(&m_Pen2);
			}
			else
			{
				pDC->SelectObject(&m_Brush1);
				pDC->SelectObject(&m_Pen1);
			}
			pDC->BeginPath();
			pDC->MoveTo(lPos1,y11);
			pDC->LineTo(lPos1,y12);
			pDC->LineTo(lPos2,y22);
			pDC->LineTo(lPos2,y21);
			pDC->LineTo(lPos1,y11);
			pDC->EndPath();
			pDC->StrokeAndFillPath();
		}
	}
	pDC->SelectObject(m_pOldPen);
	pDC->SelectObject(m_pOldBrush);
	return TRUE;
}

long DZstDDZ::GetPosFromVal(double dVal,long nType)
{
	if(m_dMaxVal-m_dMinVal<1-COMPPREC) return (m_nTop+m_nHeight);
	return long(m_nTop+m_nHeight-1.0*m_nHeight*(dVal-m_dMinVal)/(m_dMaxVal-m_dMinVal));
}
