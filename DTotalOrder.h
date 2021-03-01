#ifndef __DDRAE__TMP__DCURTOTALORDER__H__
#define __DDRAE__TMP__DCURTOTALORDER__H__

#include "DDraw.h"
#include "DUnit.h"

class DTotalOrder : public DUnit
{
protected:
	long			m_nDataNum;
	LPNORMALSTAT	m_pData;

	double			m_dMaxVal;
	double			m_dMinVal;
public:
	DTotalOrder();
	~DTotalOrder();
public:
	void SetLimit(double dMax,double dMin,long nType);

	BOOL SetData(void *pData,long nDataNum);
	BOOL Draw(CDC *pDC,long nType);
protected:
	long GetPosFromVal(double dVal,long nType);
};

#endif