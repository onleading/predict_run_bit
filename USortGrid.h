// USortGrid.h: interface for the USortGrid class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USORTGRID_H__D348B034_7715_4A87_B8FD_C12A7DA177BE__INCLUDED_)
#define AFX_USORTGRID_H__D348B034_7715_4A87_B8FD_C12A7DA177BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UBase.h"
#include "MyScrollbar.h"

#define	MAX_ROW_NUM		11
#define	MAX_ROW_ITEM	30

typedef struct
{
	int		Num;
	char	SetCode[MAX_ROW_ITEM*(MAX_ROW_NUM-1)];
	char	Code[MAX_ROW_ITEM*(MAX_ROW_NUM-1)][CODE_SIZE];
	float	Now[MAX_ROW_ITEM*(MAX_ROW_NUM-1)];
	float	Other[MAX_ROW_ITEM*(MAX_ROW_NUM-1)];
}SORTRESULT,*LPSORTRESULT;

//81,83ȫ������
class USortGrid : public UBase  
{
	BOOL	InitDPDate();
	void	ProcessSort();
	float	fGetValue(int Type,short GpIndex);
	void	Sort();
	void	DrawOneCell(CDC *pDC,int type);
	int		HitTest(CPoint point);
	void	CalcSomeInfo();
	int		ToNextUnit(int nCurrUnit, int nSkipVal, BOOL bNext);

	void	SetControlPos(CWnd *pWnd, CRect &rect);
	void	RefreshScrollBar();
	
	int		nOptBW;
	CString	m_CalcCode;
	BOOL	m_bAscendSort;
public:
	USortGrid(LPCOMINFO pComInfo);
	virtual ~USortGrid();

	short	GetCurrentGPIndex();
	int		ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
	void	SetSCType(int nType)
	{
		WhichType = nType;
	}
	void	SetCalcCode(CString CalcCode) 
	{
		m_CalcCode = CalcCode; 
	//	ProcessMsg(WM_PAINT, 1);
	}
	void	OnVScrollMessage(WPARAM wParam,LPARAM lParam);

	int		m_nDSFlag;
	short   TypeFlag[MAX_ROW_NUM];
	int		m_nUnitRow, m_nUnitCol;
	int		m_nRowNum;
	int		m_nItemHeight;
	int		WhichType;
	char	TypeStr[MAX_ROW_NUM][25];
	CRect	CellRect[MAX_ROW_NUM];
	
	short	lpnUsedStkIndex[MAXGPNUM];	//ָ�򱾴���ʵ������ʾ�Ĺ�Ʊ�������
	int		nGpNum;
	
	long	SortDate[MAX_ROW_NUM];

	short	SortResult[MAX_ROW_NUM][MAX_ROW_ITEM];			//����������Ĺ�Ʊ����
	SORTRESULT m_ZHBuf[MAX_ROW_NUM];						//���Զ�����������
	
	int		m_nItems;
	int		m_nCurSel;	//0<m_nCurSel<54
	
	BOOL	m_bHasData;
	short	m_nMainID;
	BOOL	m_bRefreshDraw;

	BOOL	m_bNeedHZ;					//�Ƿ���Ҫ���¼������

	CMyScrollBar	*m_pVScrollBar;
	int		nStartRow;
	
};

#endif // !defined(AFX_USortGrid_H__D348B034_7715_4A87_B8FD_C12A7DA177BE__INCLUDED_)
