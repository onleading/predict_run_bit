#ifndef UCWCALC_H_
#define UCWCALC_H_

#include "CWTableUnit.h"

struct StringKeyCache
{
	char	lpString[255];
	char	strKey[10][31];
	char	strExclude[10][31];
};

class CCWCalc  
{
public:
	CCWCalc(long nClue);
	virtual ~CCWCalc();

	void	SetConfig(CString strCalcFunc, OutTitle_Cfg *pConfig, short nSetcode=0, char *Code="000001");

	BOOL	ParseCalcString();
	float	GetResult() {return m_fRet;}
private:
	short	ChangeOneKeyword();
	BOOL	ChangeCalcString();

	BOOL	MathParse();
	BOOL	TCalcParse();
private:
	CString			m_strCalcFunc;		//�������ʽ��
	long			m_nClue;			//ԭʼ���ݼ�
	OutTitle_Cfg *	m_pConfig;			//��������

	StringKeyCache	m_KeyCache[10];

	short			m_nSetCode;
	char			m_Code[CODE_SIZE];

	float			m_fRet;
};

#endif
