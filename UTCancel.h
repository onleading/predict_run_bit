#ifndef UTCancel_H_
#define UTCancel_H_

//�ۼƳ�������
#include "UBase.h"

class UTCancel : public UBase
{
	BOOL	m_bEmpty;
	int					 m_nZoomFlag;//0:�޷Ŵ�,1:�ϲ��ַŴ� 2:�²��ַŴ�
	//
	short	m_fltype;
	short	m_setcode;
	//Զ�̲�����
	BOOL	m_bDataReady;
	CRect	m_SellRect[10],m_BuyRect[10];
private:
	void DrawPart1(CDC *pDC,int TotalHeight,int TextH,int XWidth,int XFrom[],int YFrom);
	void DrawPart2(CDC *pDC,int TotalHeight,int TextH,int XWidth,int XFrom[],int YFrom);
public:
	UTCancel(LPCOMINFO pComInfo);
	~UTCancel();

	void ReadData(BOOL bDataReady=TRUE,BOOL bReqData=TRUE);
	void DrawIt(CDC *pDC);

	int ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
};


#endif
