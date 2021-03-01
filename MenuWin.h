#ifndef _BIGMENU_H
#define _BIGMENU_H

#include "BaseWin.h"

class CMenuWin : public BaseWin
{
	int     nStartNo;			//起始序号
	int     nMenuNum,nNowNo;	//菜单项个数,当前选中项
	int		nColNum;			//列数

	int		nSaveMenuID,nSaveNowNo;
	int     nNextID;
	int     nDomainID;
	char	lpszTopic[80];

	int		m_nStartx1,m_nStartx2;//第一,二列菜单开始X

	int 	DrawFlag;	//DRAW_PUTBITMAP、DRAW_ALL两种
	CBitmap	tempBitmap;	//作出临时位图
	CRect	m_rcItemRect[50];	//菜单所在区域
public:
	int		m_nMenuID;
private:
	int		GetMaxMenuLen(CDC *pDC);
	void	GetItemRect(CDC *pDC);
	int		IsOnItem(POINT pt);
	void	DrawAdvert(CDC *pDC);
	void	DrawTitleStr(CDC *pDC);
public:
	CMenuWin(CWnd *pParent);
	virtual ~CMenuWin();

	void SetMenuID(int  nMenuNo);
	void SetTopic (char * topic);
	void SetNow   (int  nNowNo);
	void DrawIt();
	void SizeIt();
	int  Handle(UINT message, WPARAM wParam, LPARAM lParam = 0);

	void Process  (void);
};

#endif