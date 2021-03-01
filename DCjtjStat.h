#ifndef __DDRAW__TMP__DCJTJSTAT__H__
#define __DDRAW__TMP__DCJTJSTAT__H__

#include "DDraw.h"
#include "DUnit.h"

class DCjtjStat : public DUnit
{
protected:
	long		m_nDataNum;
	LPTICKSTAT	m_pData;

	double		m_dMaxVal;
	double		m_dMinVal;
	
	double		m_dBigMaxVal;
	double		m_dBigMinVal;
public:
	DCjtjStat();
	~DCjtjStat();
public:
	void SetLimit(double dMax,double dMin,long nType);

	BOOL SetData(void *pData,long nDataNum);
	BOOL Draw(CDC *pDC,long nType);
protected:
	long GetPosFromVal(double dVal,long nType);
};

#endif
