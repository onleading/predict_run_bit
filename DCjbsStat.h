#ifndef __DDRAW__TMP__DCJBSSTAT__H__
#define __DDRAW__TMP__DCJBSSTAT__H__

#include "DDraw.h"
#include "DUnit.h"

class DCjbsStat : public DUnit
{
protected:
	long		m_nDataNum;
	LPTICKSTAT	m_pData;

	double		m_dMaxVal;
	double		m_dMinVal;

	double		m_dMaxLarge;
	double		m_dMinLarge;
public:
	DCjbsStat();
	~DCjbsStat();
public:
	void SetLimit(double dMax,double dMin,long nType);

	BOOL SetData(void *pData,long nDataNum);
	BOOL Draw(CDC *pDC,long nType);
protected:
	long GetPosFromVal(double dVal,long nType);
};

#endif
