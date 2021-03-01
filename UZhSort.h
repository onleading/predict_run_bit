// UZhSort.h: interface for the UZhSort class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UZHSORT_H__D348B034_7715_4A87_B8FD_C12A7DA177BE__INCLUDED_)
#define AFX_UZHSORT_H__D348B034_7715_4A87_B8FD_C12A7DA177BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UBase.h"

#define	MAX_ZHROW_ITEM	20

#define	STYLE_2X2		0
#define	STYLE_3X3		1
#define STYLE_1X5		2

//81,83�ۺ�����
class UZhSort : public UBase  
{
	void	ProcessSort();
	float	fGetValue(int Type,short GpIndex);
	void	Sort(int Type,BOOL bAscendSort);
	void	DrawOneCell(CDC *pDC,int xh,int type);
	int		HitTest(CPoint point);
	void	CalcSomeInfo(int nFlag);
	int		ToNextUnit(int nCurrUnit, int nSkipVal, BOOL bNext);
	
	int		nOptBW;
	//��û�����ùɱ�Ʊ��
	BOOL	m_bHasFL;
public:
	UZhSort(LPCOMINFO pComInfo);
	virtual ~UZhSort();

	void	SetStyle(int nStyle);
	short	GetCurrentGPIndex();
	int		ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
	void	SetShowSort(int **aShowSort);
	void	GetShowSort(int *aShowSort);
	void	GetOptValue(int &nSort, CString &aSort, int &nType);

	short	m_nDSFlag;
	short   TypeFlag[9];
	int		m_nUnitRow, m_nUnitCol;
	int		m_nRowNum;
	int		m_nItemHeight;
	int		m_aShowSort[9];
	int		WhichType;
	char	TypeStr[9][25];
	CRect	CellRect[9];
	
	short	lpnUsedStkIndex[MAXGPNUM];	//ָ�򱾴���ʵ������ʾ�Ĺ�Ʊ�������
	int		nGpNum;
	
	short	SortResult[9][20];			//����������Ĺ�Ʊ����
	ZHRESULT m_ZHBuf[9];				//���Զ�����������
	
	int		m_nItems;
	int		m_nCurSel;
	
	BOOL	m_bHasData;
	short	m_nMainID;
	BOOL	m_bRefreshDraw;
	
	int		m_nStyle;	
};

#endif
