#ifndef _BIGMENU_H
#define _BIGMENU_H

#include "BaseWin.h"

class CMenuWin : public BaseWin
{
	int     nStartNo;			//��ʼ���
	int     nMenuNum,nNowNo;	//�˵������,��ǰѡ����
	int		nColNum;			//����

	int		nSaveMenuID,nSaveNowNo;
	int     nNextID;
	int     nDomainID;
	char	lpszTopic[80];

	int		m_nStartx1,m_nStartx2;//��һ,���в˵���ʼX

	int 	DrawFlag;	//DRAW_PUTBITMAP��DRAW_ALL����
	CBitmap	tempBitmap;	//������ʱλͼ
	CRect	m_rcItemRect[50];	//�˵���������
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