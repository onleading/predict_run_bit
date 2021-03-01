#include "stdafx.h"
#include "DDraw.h"
#include "DUnit.h"

DUnit::DUnit()
{
	m_nLeft		= 0;
	m_nWidth	= 0;
	m_nTop		= 0;
	m_nHeight	= 0;
	m_nWillNum	= 0;
	m_nLeftNo	= 0;
}

DUnit::~DUnit()
{
}

void DUnit::SetRectRgn(long nLeft,long nWidth,long nTop,long nHeight)
{
	if(nWidth<0)	nWidth=0;
	if(nHeight<0)	nHeight=0;
	m_nLeft		= nLeft;
	m_nWidth	= nWidth;
	m_nTop		= nTop;
	m_nHeight	= nHeight;
}

RECT DUnit::GetRect()
{
	CRect rect(m_nLeft,m_nTop,m_nLeft+m_nWidth,m_nTop+m_nHeight);
	return RECT(rect);
}

void DUnit::SetWillNum(long nWillNum,long nLeftBlank,long nLeftNo)
{
	m_nWillNum	= nWillNum;
	m_nLeftBlank= nLeftBlank;
	m_nLeftNo	= nLeftNo;
}

BOOL DUnit::SetData(void *pData,long nDataNum)
{
	return FALSE;
}

BOOL DUnit::Calc()
{
	return FALSE;
}

BOOL DUnit::Draw(CDC *pDC,long nType)
{
	return FALSE;
}

