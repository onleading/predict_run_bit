// JJGrid.h: interface for the JJGrid class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JJGRID_H__41A75EDB_649B_4479_99E3_9453FED651D4__INCLUDED_)
#define AFX_JJGRID_H__41A75EDB_649B_4479_99E3_9453FED651D4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Grid.h"

#define OPENFUND 0
#define CLOSEDFUND 1
#define HBFUND    2
#define HOLDSTOCK 3

#define BASECOL 15
#define HBCOL	11
#define STOCKCOL 7

typedef struct HOLDSTOCKINFO 
{
	char szSymbol[7];
	char szName[9];
	
	short nHoldNum;
	float fHoldAmount;
	float fComRate;
	float fMarketV;
	float fAmountInc;
}HOLDSTOCKINFO,*pHOLDSTOCKINFO;

class JJGrid : public Grid  
{
	pFUNDBASE m_pFundBase;
	pFUNDHOLDSTOCK m_pFundHoldStock;

	vector<HOLDSTOCKINFO> m_vtHoldStock;
	CMapStringToString  m_map;

	int lpnColType[BASECOL];

	int nFundNum;
	int nFHSNum;
	int nFHSTotalNum;
	BOOL bLoadOk;
	char (*lpszItemName)[20];
	float* lpfWidth;
public:
	int nCurFundFlag;
public:
	JJGrid();
	virtual ~JJGrid();
	BOOL	InitGrid(int nflag = -1);
	void	Sort();
	void	DrawGrid(CDC* pDC);
	void	OnSize(UINT nType, int cx, int cy);
	void	InsertSwitch(int nSCType);
	int		ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
	void	SetCurLabel(int n);

	void	OnClick(CDC *pDC,CPoint point,BOOL IsRight);
	void	OnLButtonDown(UINT nFlags, CPoint point);
	void	OnLButtonDbClick(UINT nFlags, CPoint point); 

	void	SelectSomeRow(const char* lpszSymbol);
	char*	GetStockCode(short& setcode);
private:
	int		GetCellLeft(int nCol,int& nWidth);
	void	OnCtrlLeft();
	void	GetSomeSize(CDC* pDC);
	float   fGetSortValue(short xh);
	char*   GetSortString(short xh);
	void	DrawTitle(CDC *pDC);
	void	DrawItem(CDC *pDC,int row,int col);
	void	DrawFundBase(CDC *pDC,int row,int col);
	void	DrawHoldStock(CDC *pDC,int row,int col);
	void	DrawHbFund(CDC *pDC,int row,int col);
	BOOL	GetData();//从文件中读取数据
	char*	GetTopStock(const char* lpszFund,float& fTotalInc);//得到每只基金的首席股票
	void	InsertStockInfo();	//转换为HOLDSTOCKINFO结构的数据，方便显示
};

#endif // !defined(AFX_JJGRID_H__41A75EDB_649B_4479_99E3_9453FED651D4__INCLUDED_)
