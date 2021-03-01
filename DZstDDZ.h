#ifndef __DDRAW__TMP__DZSTDDZ__H__
#define __DDRAW__TMP__DZSTDDZ__H__

#include "DDraw.h"
#include "DUnit.h"

class DZstDDZ : public DUnit
{
protected:
	long		m_nDataNum;
	LPTICKSTAT	m_pData;

	double		m_dMaxVal;
	double		m_dMinVal;

	CBrush		m_Brush1,m_Brush2,*m_pOldBrush;
	CPen		m_Pen1,m_Pen2,*m_pOldPen;
public:
	DZstDDZ();
	~DZstDDZ();
public:
	void SetLimit(double dMax,double dMin,long nType);

	BOOL SetData(void *pData,long nDataNum);
	BOOL Draw(CDC *pDC,long nType);
protected:
	long GetPosFromVal(double dVal,long nType);
};

#endif
