#ifndef __DDRAW__TMP__DCANCELORDER__H__
#define __DDRAW__TMP__DCANCELORDER__H__

#include "DDraw.h"
#include "DUnit.h"

class DCancelOrder : public DUnit
{
protected:
	long			m_nDataNum;
	LPNORMALSTAT	m_pData;

	double			m_dOrderMaxVal;
	double			m_dOrderMinVal;

	double			m_dCancelMaxVal;
	double			m_dCancelMinVal;
public:
	DCancelOrder();
	~DCancelOrder();
public:
	void SetLimit(double dMax,double dMin,long nType);

	BOOL SetData(void *pData,long nDataNum);
	BOOL Draw(CDC *pDC,long nType);
protected:
	long GetPosFromVal(double dVal,long nType);
};

#endif