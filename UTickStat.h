#ifndef UTickStat_H_
#define UTickStat_H_

//龙虎看盘
#include "Ubase.h"
#include "DDraw.h"

class UTickStat : public UBase
{
	BOOL		m_bEmpty;
	//远程部分用
	BOOL		m_bDataReady;
	//
	CDBFile		*m_pHQDBFile;
	map<long, long, greater<long> > m_DBIndexMap;
	TICKSTAT	m_TickStat;

	BOOL		m_bPile;
	BOOL		m_bVolume;
	BOOL		m_bAll;

	double		m_dValue[6];
	double		m_dTotal;
	
	CBrush		m_Brush[7];
	COLORREF	m_UpColor;
	COLORREF	m_DownColor;
	COLORREF	m_LevelColor;
	COLORREF	m_BrushColor[7];

	int			m_GraphBottom;
	
	BOOL		m_bOnlyDrawGraph;
public:
	UTickStat(LPCOMINFO pComInfo);
	~UTickStat();

public:
	//饼/柱;笔数/数量
	void	SetDrawMode(BOOL bPile);
	void	SetDataType(BOOL bVolume);
public:

	void	ReadData();

	void	TransData();
	void	DrawIt(CDC *pDC);
	void	DrawFrame(CDC *pDC);
	int 	DrawGraph(CDC *pDC);
	void	DrawList(CDC *pDC);
	void	DrawPile(CDC *pDC,CRect rc,double *dValue,int ValueNum,CBrush *pBrush);
	void	DrawCycle(CDC *pDC,CPoint ptCentre,long lRadius,double dStartRate,double *dValue,int ValueNum,double dTotal,CBrush *pBrush);

	int		ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
private:
	void	CheckColor();
	COLORREF GetColor(COLORREF rcBase,double dRate);
	void	DrawListItem(CDC *pDC,long lListStart,int nLine,double dVol,DWORD dwVolNum,double dTotalVal);
	void	DrawListItemAmo(CDC *pDC,long lListStart,int nLine,float fAmo,DWORD dwVolNum,double dTotalVal);
};


#endif
