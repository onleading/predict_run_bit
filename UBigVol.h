#ifndef UBIGVOL_H_
#define UBIGVOL_H_

#include "Ubase.h"

class UBigVol : public UBase
{
	char    BigVol_Code[SH_CODE_LEN+1];
	char	BigVol_SetCode;

	int		m_nCurSel;
	int		m_nRowNum,m_nDrawPos,m_nDrawNum,m_nLastPos;

	int			m_nFlag;	//0:ȫ�� 1:��ѡ�� 2:��ǰ��Ʊ
	int			m_nCurGP;
	long		m_nWarnNum;
	LPWARNATOM	m_pWarnAtom;
public:
	UBigVol(LPCOMINFO pComInfo);
	~UBigVol();
	int  GetStyle() { return m_nStyle; }

	short GetCurrentGPIndex();
	void DrawIt(CDC *pDC);

	int ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
};

#endif