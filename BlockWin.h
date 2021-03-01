#ifndef _BLOCKWIN_H
#define _BLOCKWIN_H

#include "BaseWin.h"

class CBlockWin : public BaseWin
{	
	int 	DrawFlag;		//DRAW_PUTBITMAP、DRAW_ALL两种
	CBitmap	tempBitmap;		//作出临时位图
	CScrollBar	*pVScrollBar;
	BOOL	bHasScrollBar;
	
	int		m_nType;
	int		m_nPageNum,m_nCurPage;
	int		m_nItemNum,m_nNowNo;
	int		m_nMaxRow,m_nMaxCol;	//本页可容纳的最大行与最大列
	int		m_nStartItem,m_nEndItem;//本页开始项与最后一项
	int		m_nCurRow,m_nCurCol;	//当前项所处的行与列(相当于本页)
	char	m_pDataBuff[600][10];
	int		m_nColGap;				
private:
	void	DrawTitleStr(CDC *pDC);
	void	CalcSize();
	void	GetBlock();
	void	DrawRect(CDC *pDC,int x,int y,int cx,int cy);
	int		IsOnItem(POINT pt);
	void	AfterChangePage(int nCurPage);
	void	ShowVScrollBar(BOOL bShow);
public:
	CBlockWin(CWnd *pParent);
	virtual ~CBlockWin();

	void SetType(int type);
	void Process();
	void DrawIt();
	void SizeIt();

	int	 GetBlockNo();
	int Handle(UINT message, WPARAM wParam, LPARAM lParam = 0);
};

#endif